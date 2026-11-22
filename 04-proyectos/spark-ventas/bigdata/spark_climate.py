"""
ENTREGABLE 3 - PARTE B: Apache Spark (Big Data)
Procesamiento distribuido de datos climaticos (fecha,estacion,temperatura)

Implementa:
  1. Promedio por estacion
  2. Promedio mensual (global)
  3. Top-N estaciones mas calientes
  4. Deteccion de anomalias (temperatura fuera de rango tipico, por estacion)

Ejecucion (desde la laptop MASTER, con el cluster ya levantado):
  bin\\spark-submit --master spark://IP_MASTER:7077 ^
      --conf spark.driver.port=7001 ^
      --conf spark.driver.blockManager.port=7002 ^
      --conf spark.blockManager.port=7003 ^
      --conf spark.driver.host=IP_MASTER ^
      --executor-memory 2G --total-executor-cores 4 ^
      C:\\bigdata\\spark_climate.py file:///C:/bigdata/datos/clima_total.csv 10

NOTA IMPORTANTE (Windows, cluster de 2+ maquinas):
Los resultados se guardan con pandas (.toPandas().to_csv(...)) en vez de
.write.csv() de Spark, porque .write.csv() con file:// requiere que TODAS
las maquinas vean el mismo disco fisico -- en un cluster real de Windows con
2 laptops distintas eso no aplica (cada una tiene su propio C:). Como estos
resultados son pequenios (miles de filas, no millones), el driver los junta
en memoria y los escribe el mismo directamente en su propio disco local.
"""

import os
import sys
import time

from pyspark.sql import SparkSession
from pyspark.sql import functions as F
from pyspark.sql.types import StructType, StructField, IntegerType, StringType, DoubleType


def main():
    if len(sys.argv) < 2:
        print("Uso: spark-submit spark_climate.py <ruta_csv> [top_n]")
        sys.exit(1)

    input_path = sys.argv[1]
    top_n = int(sys.argv[2]) if len(sys.argv) > 2 else 10

    spark = (
        SparkSession.builder
        .appName("ClimaDistribuido-Spark-Entregable3")
        .getOrCreate()
    )
    sc = spark.sparkContext
    sc.setLogLevel("WARN")

    print(f"Cluster Spark -> master: {sc.master} | executors disponibles: "
          f"{sc.defaultParallelism}")

    schema = StructType([
        StructField("fecha", IntegerType(), True),
        StructField("estacion", StringType(), True),
        StructField("temperatura", DoubleType(), True),
    ])

    t0 = time.time()

    df = spark.read.csv(input_path, header=True, schema=schema)
    df = df.dropna(subset=["fecha", "estacion", "temperatura"])

    df = (
        df.withColumn("anio", (F.col("fecha") / 10000).cast("int"))
          .withColumn("mes", ((F.col("fecha") / 100) % 100).cast("int"))
    )

    df.cache()
    total = df.count()
    print(f"Registros leidos: {total}")

    # ============ 1. Promedio por estacion ============
    prom_estacion = (
        df.groupBy("estacion")
          .agg(
              F.avg("temperatura").alias("promedio"),
              F.max("temperatura").alias("maximo"),
              F.min("temperatura").alias("minimo"),
              F.count("*").alias("conteo"),
          )
          .orderBy("estacion")
    )

    # ============ 2. Promedio mensual (global) ============
    prom_mensual = (
        df.groupBy("mes")
          .agg(
              F.avg("temperatura").alias("promedio_mensual"),
              F.count("*").alias("conteo"),
          )
          .orderBy("mes")
    )

    # ============ 3. Top-N estaciones mas calientes ============
    top_estaciones = (
        prom_estacion
        .filter(F.col("conteo") >= 5)
        .orderBy(F.col("promedio").desc())
        .limit(top_n)
    )

    # ============ 4. Deteccion de anomalias ============
    stats = df.groupBy("estacion").agg(
        F.mean("temperatura").alias("media_est"),
        F.stddev("temperatura").alias("std_est"),
    )

    df_join = df.join(F.broadcast(stats), on="estacion", how="left")

    anomalias = (
        df_join.withColumn(
            "es_anomalia",
            F.when(
                (F.col("std_est").isNotNull()) & (F.col("std_est") > 0) &
                (
                    (F.col("temperatura") > F.col("media_est") + 3 * F.col("std_est")) |
                    (F.col("temperatura") < F.col("media_est") - 3 * F.col("std_est"))
                ),
                True,
            ).otherwise(False),
        )
        .filter(F.col("es_anomalia") == True)  # noqa: E712
        .select("fecha", "estacion", "temperatura", "media_est", "std_est")
    )

    n_anomalias = anomalias.count()
    n_estaciones = prom_estacion.count()

    t1 = time.time()

    print("\n===== RESULTADOS SPARK =====")
    print(f"Registros totales:      {total}")
    print(f"Estaciones distintas:   {n_estaciones}")
    print(f"Anomalias detectadas:   {n_anomalias}")
    print(f"Tiempo de procesamiento: {t1 - t0:.2f} s")

    print(f"\nTop {top_n} estaciones mas calientes:")
    top_estaciones.show(top_n, truncate=False)

    print("Promedio mensual global:")
    prom_mensual.show(12, truncate=False)

    # ============ Guardar resultados (via collect() + csv nativo, solo en disco del driver) ============
    # NOTA: se evita toPandas() a propósito porque PySpark 3.5.x internamente
    # importa "distutils", modulo eliminado del stdlib desde Python 3.12+.
    # Con Python 3.14 esto revienta con ModuleNotFoundError. collect() + csv
    # nativo no depende de pandas ni de distutils, así que es más robusto aquí.
    import csv as csv_module

    os.makedirs("salida_spark", exist_ok=True)

    def guardar_csv(dataframe, ruta):
        filas = dataframe.collect()
        if not filas:
            columnas = dataframe.columns
        else:
            columnas = filas[0].asDict().keys()
        with open(ruta, "w", newline="", encoding="utf-8") as f:
            writer = csv_module.writer(f)
            writer.writerow(columnas)
            for fila in filas:
                writer.writerow(fila)

    guardar_csv(prom_estacion, "salida_spark/promedio_por_estacion.csv")
    guardar_csv(prom_mensual, "salida_spark/promedio_mensual.csv")
    guardar_csv(top_estaciones, "salida_spark/top_estaciones.csv")
    guardar_csv(anomalias, "salida_spark/anomalias.csv")

    with open("salida_spark/resumen_spark.txt", "w") as f:
        f.write(f"registros={total}\n")
        f.write(f"estaciones={n_estaciones}\n")
        f.write(f"anomalias={n_anomalias}\n")
        f.write(f"tiempo_seg={t1 - t0:.2f}\n")
        f.write(f"executors_default_parallelism={sc.defaultParallelism}\n")

    print("\nResultados guardados en la carpeta 'salida_spark/'")

    spark.stop()


if __name__ == "__main__":
    main()