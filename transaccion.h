#ifndef TRANSACCION_H
#define TRANSACCION_H

#include "fecha.h"

#define ARCHIVO_TRANSACCIONES  "transacciones.bin"
#define BLOQUE_TRANSACCIONES   5

typedef struct stTransaccion {
    int idTransaccion;
    Fecha fecha;
    int idJuegoVendido;
    int cantidadVendida;
    float precioUnitarioVenta;
    float gananciaTotal;
    char dniCliente[15];
    char dniEmpleado[15];
} Transaccion;

/* Funciones base de Transaccion (Fase 1) */
long transaccionContarRegistrosArchivo(const char nombreArchivo[]);
void transaccionMostrarResumen(Transaccion t);
void transaccionMostrarDetalle(Transaccion t);

/* Motor de ventas, carga dinamica con realloc y reportes recursivos (Fase 3) */
int transaccionGenerarNuevoId(const char nombreArchivo[]);
void transaccionCargarTodasRealloc(const char nombreArchivo[], Transaccion **arr, int *validos, int *dim);
void transaccionMostrarResumenRec(const Transaccion arr[], int validos, int pos);
int transaccionBuscarPorIdRec(const Transaccion arr[], int validos, int idBuscado, int pos);
int transaccionRegistrarVenta(const char archiTrans[], const char archiJuegos[],
                              const char archiPersonas[], Transaccion nuevaVenta);
float transaccionSumarRecaudacionMesRec(const Transaccion arr[], int validos, int pos, int mes, int anio);
float transaccionSumarGananciaMesRec(const Transaccion arr[], int validos, int pos, int mes, int anio);
Transaccion transaccionBuscarMayorGananciaRec(const Transaccion arr[], int validos, int pos);

/* Submenus interactivos conectados desde main.c */
void transaccionMenuVentas(const char archiTrans[], const char archiJuegos[],
                           const char archiPersonas[], const char dniEmpleadoActivo[]);
void transaccionMenuReportes(const char archiTrans[], const char archiJuegos[]);

#endif
