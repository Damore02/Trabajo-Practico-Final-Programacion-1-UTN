#ifndef UI_H
#define UI_H

#define USAR_COLORES 1
#define ANCHO_UI     76

#if USAR_COLORES
    #define CLR_RESET   "\x1b[0m"
    #define CLR_TITULO  "\x1b[1;36m"
    #define CLR_SUBTIT  "\x1b[1;37m"
    #define CLR_OK      "\x1b[1;32m"
    #define CLR_ALERTA  "\x1b[1;33m"
    #define CLR_ERROR   "\x1b[1;31m"
    #define CLR_TENUE   "\x1b[90m"
#else
    #define CLR_RESET   ""
    #define CLR_TITULO  ""
    #define CLR_SUBTIT  ""
    #define CLR_OK      ""
    #define CLR_ALERTA  ""
    #define CLR_ERROR   ""
    #define CLR_TENUE   ""
#endif

/* Utilidades visuales de consola */
void uiLimpiarPantalla(void);
void uiPausar(void);
void uiLinea(char simbolo, int ancho);
void uiMostrarBannerPrincipal(void);
void uiMostrarBarraSesion(const char usuario[], const char rol[]);
void uiTituloSeccion(const char titulo[]);
void uiMensajeExito(const char msg[]);
void uiMensajeAlerta(const char msg[]);
void uiMensajeError(const char msg[]);

/* Utilidades de lectura segura por teclado */
void uiLimpiarBuffer(void);
int uiLeerEnteroEnRango(const char prompt[], int min, int max);
float uiLeerFloatMinimo(const char prompt[], float min);
void uiLeerTexto(const char prompt[], char destino[], int tamMaximo);
int uiConfirmar(const char prompt[]);

#endif
