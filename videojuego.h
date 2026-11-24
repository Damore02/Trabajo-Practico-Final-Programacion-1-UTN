#ifndef VIDEOJUEGO_H
#define VIDEOJUEGO_H

#define ARCHIVO_VIDEOJUEGOS  "videojuegos.bin"
#define STOCK_CRITICO        5
#define JUEGO_ACTIVO         1
#define JUEGO_INACTIVO       0

typedef struct stVideojuego {
    int idJuego;
    char titulo[50];
    char plataforma[20];
    char genero[20];
    int stock;
    float precioAdquisicion;
    int activo;
} Videojuego;

/* Funciones base de Videojuego */
int videojuegoContarRegistrosArchivo(char nombreArchivo[]);
void videojuegoMostrarResumen(Videojuego v);
void videojuegoMostrarDetalle(Videojuego v, int mostrarPrecioCosto);

/* Auxiliares y ABM */
int videojuegoTextoIgualSinCase(char a[], char b[]);
int videojuegoExisteIdEnArchivo(char nombreArchivo[], int idBuscado);
int videojuegoContarActivos(char nombreArchivo[]);
int videojuegoGenerarNuevoId(char nombreArchivo[]);
int videojuegoBuscarPorId(char nombreArchivo[], int idBuscado, Videojuego *encontrado);
int videojuegoAgregar(char nombreArchivo[], Videojuego nuevo);
int videojuegoModificar(char nombreArchivo[], int idBuscado, Videojuego actualizado);
int videojuegoBajaLogica(char nombreArchivo[], int idBuscado);
int videojuegoDescontarStock(char nombreArchivo[], int idJuego, int cantidadVendida);

/* Arreglos dinamicos, recursividad y stock critico */
Videojuego* videojuegoCargarActivos(char nombreArchivo[], int *validos);
void videojuegoMostrarArregloRec(Videojuego arr[], int validos, int pos);
int videojuegoContarPorPlataforma(char nombreArchivo[], char plataforma[]);
Videojuego* videojuegoFiltrarPorPlataforma(char nombreArchivo[], char plataforma[], int *validos);
int videojuegoContarStockCritico(char nombreArchivo[]);
Videojuego* videojuegoCargarStockCritico(char nombreArchivo[], int *validos);
void videojuegoOrdenarPorStockCreciente(Videojuego arr[], int validos);
void videojuegoReporteStockCritico(char nombreArchivo[]);

/* Interfaz interactiva de Videojuego */
void videojuegoSeleccionarPlataforma(char destinoPlataforma[]);
void videojuegoAltaInteractiva(char nombreArchivo[]);
void videojuegoModificarInteractiva(char nombreArchivo[], int esAdmin);
void videojuegoBajaInteractiva(char nombreArchivo[], int esAdmin);
void videojuegoListarInteractivo(char nombreArchivo[]);
void videojuegoFiltrarPlataformaInteractivo(char nombreArchivo[], int esAdmin);
void videojuegoVerDetalleInteractivo(char nombreArchivo[], int esAdmin);
void videojuegoMenuGestion(char nombreArchivo[], char usuarioActivo[], char rolActivo[], int esAdmin);

#endif
