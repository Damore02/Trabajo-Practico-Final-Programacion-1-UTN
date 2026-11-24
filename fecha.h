#ifndef FECHA_H
#define FECHA_H

typedef struct stFecha {
    int dia;
    int mes;
    int anio;
} Fecha;

/* Carga, validacion, visualizacion y comparacion de fechas */
int fechaEsBisiesto(int anio);
int fechaDiasDelMes(int mes, int anio);
Fecha fechaCargar(char prompt[]);
int fechaEsValida(Fecha f);
void fechaMostrar(Fecha f);
int fechaCoincideMesAnio(Fecha f, int mes, int anio);

#endif
