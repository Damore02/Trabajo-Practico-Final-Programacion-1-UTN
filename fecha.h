#ifndef FECHA_H
#define FECHA_H

typedef struct stFecha {
    int dia;
    int mes;
    int anio;
} Fecha;

/* Carga, validacion, visualizacion y comparacion de fechas */
Fecha fechaCargar(const char prompt[]);
int fechaEsValida(Fecha f);
void fechaMostrar(Fecha f);
int fechaCoincideMesAnio(Fecha f, int mes, int anio);

#endif
