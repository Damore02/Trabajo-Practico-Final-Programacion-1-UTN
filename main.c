#include <stdio.h>
#include "ui.h"
#include "seed.h"
#include "usuario.h"
#include "persona.h"
#include "videojuego.h"
#include "transaccion.h"

void mostrarMenuPrincipal(Usuario sesionActiva);

/* Imprime las opciones del menu principal */

int main(void) {
    Usuario sesionActiva;
    int opcion;
    int esAdmin;

    seedInicializarArchivosSiNoExisten();

    uiLimpiarPantalla();
    uiMostrarBannerPrincipal();

    if (!usuarioLogin(ARCHIVO_USUARIOS, &sesionActiva)) {
        uiPausar();
        return 0;
    }

    uiPausar();
    esAdmin = usuarioEsAdmin(sesionActiva);

    do {
        mostrarMenuPrincipal(sesionActiva);
        opcion = uiLeerEnteroEnRango("  Seleccione una opcion > ", 0, 6);

        switch (opcion) {
            case 1:
                videojuegoMenuGestion(ARCHIVO_VIDEOJUEGOS,
                                      sesionActiva.usuario,
                                      sesionActiva.persona.rol,
                                      esAdmin);
                break;
            case 2:
                personaMenuGestion(ARCHIVO_PERSONAS,
                                   sesionActiva.usuario,
                                   sesionActiva.persona.rol);
                break;
            case 3:
                transaccionMenuVentas(ARCHIVO_TRANSACCIONES,
                                      ARCHIVO_VIDEOJUEGOS,
                                      ARCHIVO_PERSONAS,
                                      sesionActiva.persona.dni);
                break;
            case 4:
                transaccionMenuReportes(ARCHIVO_TRANSACCIONES,
                                        ARCHIVO_VIDEOJUEGOS);
                break;
            case 5:
                if (esAdmin) {
                    usuarioAltaInteractiva(ARCHIVO_USUARIOS, ARCHIVO_PERSONAS);
                } else {
                    uiMensajeError("Acceso restringido. Solo administradores pueden crear usuarios.");
                }
                uiPausar();
                break;
            case 6:
                uiMensajeAlerta("Modulo Extra de Promocion disponible en Fase 4.");
                uiPausar();
                break;
            case 0:
                uiMensajeExito("Sesion finalizada correctamente. Hasta pronto.");
                break;
            default:
                uiMensajeError("Opcion no reconocida.");
                uiPausar();
                break;
        }
    } while (opcion != 0);

    return 0;
}

void mostrarMenuPrincipal(Usuario sesionActiva) {
    uiLimpiarPantalla();
    uiMostrarBannerPrincipal();
    uiMostrarBarraSesion(sesionActiva.usuario, sesionActiva.persona.rol);
    uiTituloSeccion("MENU PRINCIPAL - PIXEL PLUS");

    printf("  %s[1]%s Gestion de Inventario (Videojuegos)\n", CLR_SUBTIT, CLR_RESET);
    printf("  %s[2]%s Gestion de Personas (Clientes y Empleados)\n", CLR_SUBTIT, CLR_RESET);
    printf("  %s[3]%s Ventas y Transacciones\n", CLR_SUBTIT, CLR_RESET);
    printf("  %s[4]%s Reportes Gerenciales (Recaudacion / Mayor Ganancia / Stock Critico)\n",
           CLR_SUBTIT, CLR_RESET);
    printf("  %s[5]%s Gestion de Usuarios (Solo Admin)\n", CLR_SUBTIT, CLR_RESET);
    printf("  %s[6]%s Modulo Extra de Promocion\n", CLR_SUBTIT, CLR_RESET);
    printf("  %s[0]%s Cerrar Sesion y Salir\n", CLR_SUBTIT, CLR_RESET);
    printf("%s", CLR_TENUE);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
}


