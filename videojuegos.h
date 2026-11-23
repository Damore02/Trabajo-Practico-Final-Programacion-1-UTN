#ifndef VIDEOJUEGO_H_INCLUDED
#define VIDEOJUEGO_H_INCLUDED

typedef struct stVideojuego
{
    int idJuego; // ID único generado automáticamente o ingresado.
    char titulo[50];
    char plataforma[20]; // Ejemplo: "PS5", "Xbox Series X", "PC", "Switch".
    char genero[20];
    int stock; // Cantidad de unidades en inventario.
    float precioAdquisicion;
} Videojuego;

void administrarVideojuegos(const char *nombreArchivo);
void listarVideojuegos(const char *nombreArchivo);
void buscarPorID(const char *nombreArchivo);
void registrarVenta (const char *nombreArchivo);
void stockCritico (const char *nombreArchivo);

#endif // VIDEOJUEGO_H_INCLUDED
