#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Limpia la pantalla encapsulando la llamada de sistema en un unico punto */
void uiLimpiarPantalla(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Descarta cualquier residuo pendiente en el buffer de entrada estandar */
void uiLimpiarBuffer(void) {
    int c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

/* Detiene la ejecucion hasta que el usuario presione ENTER */
void uiPausar(void) {
    printf("\n%sPresione ENTER para continuar...%s", CLR_TENUE, CLR_RESET);
    uiLimpiarBuffer();
}

/* Imprime una linea horizontal del ancho solicitado */
void uiLinea(char simbolo, int ancho) {
    int i;
    for (i = 0; i < ancho; i++) {
        putchar(simbolo);
    }
    putchar('\n');
}

/* Muestra el logotipo ASCII principal de Pixel Plus */
void uiMostrarBannerPrincipal(void) {
    printf("%s", CLR_TITULO);
    uiLinea('=', ANCHO_UI);
    printf("     ____  _____  __   __  _____  _       ____   _      _   _   ____\n");
    printf("    |  _ \\|_   _| \\ \\ / / | ____|| |     |  _ \\ | |    | | | | / ___|\n");
    printf("    | |_) | | |    \\ V /  |  _|  | |     | |_) || |    | | | | \\___ \\\n");
    printf("    |  __/  | |    / . \\  | |___ | |___  |  __/ | |___ | |_| |  ___) |\n");
    printf("    |_|    |___|  /_/ \\_\\ |_____||_____| |_|    |_____| \\___/  |____/\n");
    printf("%s", CLR_SUBTIT);
    printf("                 SISTEMA DE GESTION INTEGRAL DE VIDEOJUEGOS\n");
    printf("%s", CLR_TITULO);
    uiLinea('=', ANCHO_UI);
    printf("%s", CLR_RESET);
}

/* Muestra la barra fija superior con los datos de la sesion activa */
void uiMostrarBarraSesion(const char usuario[], const char rol[]) {
    printf("%s Usuario activo: %-20.20s Rol: %-14.14s Estado: %sConectado%s\n",
           CLR_SUBTIT, usuario, rol, CLR_OK, CLR_RESET);
    printf("%s", CLR_TENUE);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
}

/* Muestra un encabezado encuadrado homogeneo para cada pantalla */
void uiTituloSeccion(const char titulo[]) {
    printf("\n%s", CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("  %s%s%s\n", CLR_SUBTIT, titulo, CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
}

/* Muestra un mensaje de confirmacion en verde */
void uiMensajeExito(const char msg[]) {
    printf("%s[OK] %s%s\n", CLR_OK, msg, CLR_RESET);
}

/* Muestra una alerta de negocio en amarillo */
void uiMensajeAlerta(const char msg[]) {
    printf("%s[ALERTA] %s%s\n", CLR_ALERTA, msg, CLR_RESET);
}

/* Muestra un error de validacion o ejecucion en rojo */
void uiMensajeError(const char msg[]) {
    printf("%s[ERROR] %s%s\n", CLR_ERROR, msg, CLR_RESET);
}

/* Lee y valida un numero entero dentro de [min, max] */
int uiLeerEnteroEnRango(const char prompt[], int min, int max) {
    int valor = 0;
    int leido = 0;
    int valido = 0;
    int siguiente = 0;

    while (!valido) {
        printf("%s", prompt);
        leido = scanf("%d", &valor);

        if (leido == EOF) {
            return min;
        }

        if (leido != 1) {
            uiLimpiarBuffer();
            uiMensajeError("Entrada invalida. Debe ingresar un numero entero.");
        } else {
            siguiente = getchar();
            while (siguiente == ' ' || siguiente == '\t') {
                siguiente = getchar();
            }

            if (siguiente != '\n' && siguiente != EOF) {
                uiLimpiarBuffer();
                uiMensajeError("Entrada invalida. Ingrese solo digitos enteros.");
            } else if (valor < min || valor > max) {
                printf("%s[ERROR] El valor debe estar entre %d y %d.%s\n",
                       CLR_ERROR, min, max, CLR_RESET);
            } else {
                valido = 1;
            }
        }
    }

    return valor;
}

/* Lee y valida un numero decimal mayor o igual a min */
float uiLeerFloatMinimo(const char prompt[], float min) {
    float valor = 0.0f;
    int leido = 0;
    int valido = 0;
    int siguiente = 0;

    while (!valido) {
        printf("%s", prompt);
        leido = scanf("%f", &valor);

        if (leido == EOF) {
            return min;
        }

        if (leido != 1) {
            uiLimpiarBuffer();
            uiMensajeError("Entrada invalida. Debe ingresar un valor numerico.");
        } else {
            siguiente = getchar();
            while (siguiente == ' ' || siguiente == '\t') {
                siguiente = getchar();
            }

            if (siguiente != '\n' && siguiente != EOF) {
                uiLimpiarBuffer();
                uiMensajeError("Entrada invalida. Ingrese solo un numero valido.");
            } else if (valor < min) {
                printf("%s[ERROR] El valor debe ser mayor o igual a %.2f.%s\n",
                       CLR_ERROR, min, CLR_RESET);
            } else {
                valido = 1;
            }
        }
    }

    return valor;
}

/* Lee una cadena con espacios respetando tamMaximo - 1 sin gets ni fflush */
void uiLeerTexto(const char prompt[], char destino[], int tamMaximo) {
    char formato[32];
    int leido = 0;

    if (tamMaximo <= 1) {
        if (tamMaximo == 1) {
            destino[0] = '\0';
        }
        return;
    }

    sprintf(formato, " %%%d[^\n]", tamMaximo - 1);

    do {
        printf("%s", prompt);
        leido = scanf(formato, destino);
        if (leido == EOF) {
            destino[0] = '\0';
            return;
        }
        uiLimpiarBuffer();
        if (leido != 1 || destino[0] == '\0') {
            uiMensajeError("El texto no puede estar vacio.");
        }
    } while (leido != 1 || destino[0] == '\0');
}

/* Solicita una confirmacion binaria (s/n) y retorna 1 (si) o 0 (no) */
int uiConfirmar(const char prompt[]) {
    char opcion = '\0';
    int leido = 0;
    int siguiente = 0;
    int resultado = -1;

    while (resultado == -1) {
        printf("%s (s/n): ", prompt);
        leido = scanf(" %c", &opcion);

        if (leido == EOF) {
            return 0;
        }

        siguiente = getchar();
        while (siguiente == ' ' || siguiente == '\t') {
            siguiente = getchar();
        }

        if (siguiente != '\n' && siguiente != EOF) {
            uiLimpiarBuffer();
            uiMensajeError("Responda unicamente con 's' o 'n'.");
        } else {
            opcion = (char)tolower((unsigned char)opcion);
            if (opcion == 's') {
                resultado = 1;
            } else if (opcion == 'n') {
                resultado = 0;
            } else {
                uiMensajeError("Opcion invalida. Ingrese 's' para si o 'n' para no.");
            }
        }
    }

    return resultado;
}
