#include <stdio.h>
#include "fecha.h"
#include <time.h>

Fecha cargarFecha(){
    Fecha f;

    do{
        printf("Cargar el dia: ");
        scanf("%d", &f.dia);

        printf("ingresar Mes: ");
        scanf("%d", &f.mes);

        printf("Ingresar anio: ");
        scanf("%d", &f.anio);

        if (validarFecha(f) == 0) {
                printf("Fecha invalida, intente nuevamente.\n\n");
        }

    }while(validarFecha(f) == 0);


    return f;
}



int validarFecha(Fecha f) {
    int diaValido = 0;
    int mesValido = 0;
    int anioValido = 0;
    int maxDias = 31;

    // Validar anio
    if (f.anio >= 2000 && f.anio <= 2050) {
        anioValido = 1;
    }

    // Validar mes
    if (f.mes >= 1 && f.mes <= 12) {
        mesValido = 1;

        // Ajustar el maximo de dias segun el mes
        if (f.mes == 4 || f.mes == 6 || f.mes == 9 || f.mes == 11) {
            maxDias = 30;
        } else if (f.mes == 2) {
            if (esAnioBisiesto(f.anio)) {
                maxDias = 29;
            } else {
                maxDias = 28;
            }
        }
    }

    // Validar dia usando el maximo correspondiente a ese mes
    if (f.dia > 0 && f.dia <= maxDias) {
        diaValido = 1;
    }

    // Validacion integral
    if (diaValido && mesValido && anioValido) {
        return 1;
    }

    return 0;
}



int esAnioBisiesto(int anio){
    if((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)){
        return 1;
    }

    return 0;
}



void mostrarFecha(Fecha f) {
    printf("%02d/%02d/%04d", f.dia, f.mes, f.anio);
}


Fecha obtenerFechaActual(){
    Fecha f;
    time_t t = time(NULL);
    struct tm *tiempoLocal = localtime(&t);

    f.dia = tiempoLocal->tm_mday;
    f.mes = tiempoLocal->tm_mon + 1;
    f.anio = tiempoLocal->tm_year + 1900;

    return f;
}
