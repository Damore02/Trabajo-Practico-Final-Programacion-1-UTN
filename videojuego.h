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

/* Funciones base de Videojuego (Fase 1) */
long videojuegoContarRegistrosArchivo(const char nombreArchivo[]);
void videojuegoMostrarResumen(Videojuego v);
void videojuegoMostrarDetalle(Videojuego v, int mostrarPrecioCosto);

/* ABM, consultas dinamicas, recursividad y puente de stock (Fase 2) */
int videojuegoGenerarNuevoId(const char nombreArchivo[]);
int videojuegoBuscarPorId(const char nombreArchivo[], int idBuscado, Videojuego *encontrado);
int videojuegoAgregar(const char nombreArchivo[], Videojuego nuevo);
int videojuegoModificar(const char nombreArchivo[], int idBuscado, Videojuego actualizado);
int videojuegoBajaLogica(const char nombreArchivo[], int idBuscado);
int videojuegoDescontarStock(const char nombreArchivo[], int idJuego, int cantidadVendida);
Videojuego* videojuegoCargarActivos(const char nombreArchivo[], int *validos);
void videojuegoMostrarArregloRec(const Videojuego arr[], int validos, int pos);
int videojuegoContarPorPlataforma(const char nombreArchivo[], const char plataforma[]);
Videojuego* videojuegoFiltrarPorPlataforma(const char nombreArchivo[], const char plataforma[], int *validos);
void videojuegoMenuGestion(const char nombreArchivo[], const char usuarioActivo[], const char rolActivo[], int esAdmin);

/* Reporte de Stock Critico con malloc justo y ordenamiento creciente (Fase 3 - Req. 3m) */
int videojuegoContarStockCritico(const char nombreArchivo[]);
Videojuego* videojuegoCargarStockCritico(const char nombreArchivo[], int *validos);
void videojuegoOrdenarPorStockCreciente(Videojuego arr[], int validos);
void videojuegoReporteStockCritico(const char nombreArchivo[]);

#endif
