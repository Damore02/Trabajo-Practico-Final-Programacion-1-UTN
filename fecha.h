#ifndef FECHA_H
#define FECHA_H

typedef struct stFecha {
    int dia;
    int mes;
    int anio;
} Fecha;


//Prototipos
Fecha cargarFecha();
int validarFecha(Fecha f);
void mostrarFecha(Fecha f)
int esAnioBisiesto(int anio)

#endif
