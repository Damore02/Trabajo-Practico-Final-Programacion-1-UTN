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

/* Funciones base de Transaccion */
int transaccionContarRegistrosArchivo(char nombreArchivo[]);
void transaccionMostrarResumen(Transaccion t);
void transaccionMostrarDetalle(Transaccion t);

/* Motor de ventas, carga dinamica con realloc y reportes recursivos */
int transaccionGenerarNuevoId(char nombreArchivo[]);
void transaccionCargarTodasRealloc(char nombreArchivo[], Transaccion **arr, int *validos, int *dim);
void transaccionMostrarResumenRec(Transaccion arr[], int validos, int pos);
int transaccionBuscarPorIdRec(Transaccion arr[], int validos, int idBuscado, int pos);
int transaccionRegistrarVenta(char archiTrans[], char archiJuegos[],
                              char archiPersonas[], Transaccion nuevaVenta);
float transaccionSumarRecaudacionMesRec(Transaccion arr[], int validos, int pos, int mes, int anio);
float transaccionSumarGananciaMesRec(Transaccion arr[], int validos, int pos, int mes, int anio);
Transaccion transaccionBuscarMayorGananciaRec(Transaccion arr[], int validos, int pos);

/* Interfaz interactiva de Ventas y Reportes */
void transaccionRegistrarVentaInteractiva(char archiTrans[], char archiJuegos[],
                                          char archiPersonas[], char dniEmpleadoActivo[]);
void transaccionListarResumenInteractivo(char archiTrans[]);
void transaccionVerDetalleInteractivo(char archiTrans[]);
void transaccionReporteRecaudacionMensual(char archiTrans[]);
void transaccionReporteMayorGanancia(char archiTrans[]);
void transaccionMenuVentas(char archiTrans[], char archiJuegos[],
                           char archiPersonas[], char dniEmpleadoActivo[]);
void transaccionMenuReportes(char archiTrans[], char archiJuegos[], int esAdmin);

#endif
