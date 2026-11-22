#define _CRT_SECURE_NO_WARNINGS

#include <mpi.h>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* imagenRGB = nullptr;

    //--Proceso maestro carga la imagen

    if (rank == 0)
    {
        imagenRGB = stbi_load(
            "lion.jpg",
            &width,
            &height,
            &channels,
            3); // Fuerza RGB

        if (!imagenRGB)
        {
            cout << "Error cargando imagen" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        cout << "Imagen cargada correctamente" << endl;
        cout << "Width    = " << width << endl;
        cout << "Height   = " << height << endl;
        cout << "Channels = 3" << endl;
    }

    //--Difundir dimensiones
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Verificar divisibilidad
    /*if (height % size != 0)
    {
        if (rank == 0)
        {
            cout << "El numero de filas debe ser divisible "
                << "entre el numero de procesos."
                << endl;
        }

        MPI_Finalize();
        return 0;
    }*/
    //--Distribución por filas
   /* int filasLocales = height / size;
    int pixelesLocales = filasLocales * width;
    int bytesLocalesRGB = pixelesLocales * 3;

    vector<unsigned char> bloqueRGB(bytesLocalesRGB);*/
    int filasBase = height / size;
    int resto = height % size;

    int filasLocales = filasBase + (rank < resto ? 1 : 0);

    int pixelesLocales = filasLocales * width;
    int bytesLocalesRGB = pixelesLocales * 3;

    vector<unsigned char> bloqueRGB(bytesLocalesRGB);
    //--Scatter RGB
    vector<int> sendCountsRGB;
    vector<int> displsRGB;

    vector<int> sendCountsGray;
    vector<int> displsGray;

    if (rank == 0)
    {
        sendCountsRGB.resize(size);
        displsRGB.resize(size);

        sendCountsGray.resize(size);
        displsGray.resize(size);

        int offsetRGB = 0;
        int offsetGray = 0;

        for (int i = 0; i < size; i++)
        {
            int filas = filasBase + (i < resto ? 1 : 0);

            sendCountsRGB[i] = filas * width * 3;
            displsRGB[i] = offsetRGB;

            sendCountsGray[i] = filas * width;
            displsGray[i] = offsetGray;

            offsetRGB += sendCountsRGB[i];
            offsetGray += sendCountsGray[i];
        }
    }
    /*
    MPI_Scatter(
        imagenRGB,
        bytesLocalesRGB,
        MPI_UNSIGNED_CHAR,
        bloqueRGB.data(),
        bytesLocalesRGB,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );*/
    MPI_Scatterv(
        imagenRGB,
        sendCountsRGB.data(),
        displsRGB.data(),
        MPI_UNSIGNED_CHAR,
        bloqueRGB.data(),
        bytesLocalesRGB,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );
    //--Conversión local a gris
    vector<unsigned char> bloqueGray(pixelesLocales);

    for (int i = 0; i < pixelesLocales; i++)
    {
        //--convertir a gris cada pixel
        unsigned char r = bloqueRGB[3 * i];
        unsigned char g = bloqueRGB[3 * i + 1];
        unsigned char b = bloqueRGB[3 * i + 2];

        bloqueGray[i] = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
    }

    //--Imagen resultado
    unsigned char* imagenGray = nullptr;

    if (rank == 0)
    {
        imagenGray = new unsigned char[width * height];
    }

    //--recolectar imagen gris

    /*MPI_Gather(
        bloqueGray.data(),
        pixelesLocales,
        MPI_UNSIGNED_CHAR,
        imagenGray,
        pixelesLocales,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );*/
    MPI_Gatherv(
        bloqueGray.data(),
        pixelesLocales,
        MPI_UNSIGNED_CHAR,
        imagenGray,
        sendCountsGray.data(),
        displsGray.data(),
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );
    // Guardar imagen

    if (rank == 0)
    {
        stbi_write_png(
            "imagen_gris.png",
            width,
            height,
            1,
            imagenGray,
            width
        );

        cout << "\nImagen guardada: imagen_gris.png" << endl;
        stbi_image_free(imagenRGB);
        delete[] imagenGray;
    }

    MPI_Finalize();

    return 0;
}