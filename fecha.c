#include "fecha.h"
#include "ui.h"
#include <stdio.h>

/* Determina si un anio es bisiesto para validar febrero */
static int fechaEsBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

/* Retorna la cantidad maxima de dias validos para un mes y anio */
static int fechaDiasDelMes(int mes, int anio) {
    int diasPorMes[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (mes < 1 || mes > 12) {
        return 0;
    }

    if (mes == 2 && fechaEsBisiesto(anio)) {
        return 29;
    }

    return diasPorMes[mes - 1];
}

/* Verifica si la fecha posee rangos calendario validos */
int fechaEsValida(Fecha f) {
    int maxDias;

    if (f.anio < 2020 || f.anio > 2035) {
        return 0;
    }

    if (f.mes < 1 || f.mes > 12) {
        return 0;
    }

    maxDias = fechaDiasDelMes(f.mes, f.anio);
    return (f.dia >= 1 && f.dia <= maxDias);
}

/* Solicita dia, mes y anio por teclado hasta obtener una fecha valida */
Fecha fechaCargar(const char prompt[]) {
    Fecha f;
    int valida = 0;

    printf("%s\n", prompt);
    while (!valida) {
        f.dia = uiLeerEnteroEnRango("  Dia (1-31): ", 1, 31);
        f.mes = uiLeerEnteroEnRango("  Mes (1-12): ", 1, 12);
        f.anio = uiLeerEnteroEnRango("  Anio (2020-2035): ", 2020, 2035);

        if (fechaEsValida(f)) {
            valida = 1;
        } else {
            uiMensajeError("La fecha ingresada no existe en el calendario. Intente nuevamente.");
        }
    }

    return f;
}

/* Imprime una fecha con formato DD/MM/AAAA */
void fechaMostrar(Fecha f) {
    printf("%02d/%02d/%04d", f.dia, f.mes, f.anio);
}

/* Retorna 1 si la fecha pertenece al mes y anio indicados, 0 en caso contrario */
int fechaCoincideMesAnio(Fecha f, int mes, int anio) {
    return (f.mes == mes && f.anio == anio);
}
