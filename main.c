#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "seed.h"
#include "usuario.h"
#include "persona.h"
#include "videojuego.h"
#include "transaccion.h"

void mostrarMenuPrincipal(Usuario sesionActiva, int esAdmin, int esCliente);

/* Imprime las opciones del menu principal adaptadas al rol activo */
void mostrarMenuPrincipal(Usuario sesionActiva, int esAdmin, int esCliente) {
    uiLimpiarPantalla();
    uiMostrarBannerPrincipal();
    uiMostrarBarraSesion(sesionActiva.usuario, sesionActiva.persona.rol);
    uiTituloSeccion("MENU PRINCIPAL - PIXEL PLUS");

    if (esCliente) {
        printf("  %s[1]%s Catalogo de Videojuegos (Solo Consulta)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[2] Gestion de Personas (Sin acceso)%s\n", CLR_TENUE, CLR_RESET);
        printf("  %s[3] Ventas y Transacciones (Sin acceso)%s\n", CLR_TENUE, CLR_RESET);
        printf("  %s[4] Reportes Gerenciales (Sin acceso)%s\n", CLR_TENUE, CLR_RESET);
        printf("  %s[5] Gestion de Usuarios (Solo Admin)%s\n", CLR_TENUE, CLR_RESET);
        printf("  %s[6] Modulo Extra de Promocion (Sin acceso)%s\n", CLR_TENUE, CLR_RESET);
    } else if (esAdmin) {
        printf("  %s[1]%s Gestion de Inventario (Videojuegos)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[2]%s Gestion de Personas (Clientes y Empleados)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[3]%s Ventas y Transacciones\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[4]%s Reportes Gerenciales (Recaudacion / Mayor Ganancia / Stock Critico)\n",
               CLR_SUBTIT, CLR_RESET);
        printf("  %s[5]%s Gestion de Usuarios (Solo Admin)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[6]%s Modulo Extra de Promocion\n", CLR_SUBTIT, CLR_RESET);
    } else {
        printf("  %s[1]%s Consulta de Inventario (Videojuegos)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[2]%s Gestion de Clientes y Consultas\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[3]%s Ventas y Transacciones\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[4]%s Reportes Operativos (Stock Critico)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[5] Gestion de Usuarios (Solo Admin)%s\n", CLR_TENUE, CLR_RESET);
        printf("  %s[6]%s Modulo Extra de Promocion\n", CLR_SUBTIT, CLR_RESET);
    }

    printf("  %s[0]%s Cerrar Sesion y Salir\n", CLR_SUBTIT, CLR_RESET);
    printf("%s", CLR_TENUE);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
}

int main(void) {
    Usuario sesionActiva;
    int opcion;
    int esAdmin;
    int esCliente;

    seedInicializarArchivosSiNoExisten();

    uiLimpiarPantalla();
    uiMostrarBannerPrincipal();

    if (!usuarioLogin(ARCHIVO_USUARIOS, &sesionActiva)) {
        uiPausar();
        return 0;
    }

    uiPausar();
    esAdmin = usuarioEsAdmin(sesionActiva);
    esCliente = (strcmp(sesionActiva.persona.rol, ROL_CLIENTE) == 0);

    do {
        mostrarMenuPrincipal(sesionActiva, esAdmin, esCliente);
        opcion = uiLeerEnteroEnRango("  Seleccione una opcion > ", 0, 6);

        switch (opcion) {
            case 1:
                videojuegoMenuGestion(ARCHIVO_VIDEOJUEGOS,
                                      sesionActiva.usuario,
                                      sesionActiva.persona.rol,
                                      esAdmin);
                break;
            case 2:
                if (esCliente) {
                    uiMensajeError("Acceso restringido. Un cliente no puede acceder a la gestion de personas.");
                    uiPausar();
                } else {
                    personaMenuGestion(ARCHIVO_PERSONAS,
                                       sesionActiva.usuario,
                                       sesionActiva.persona.rol,
                                       esAdmin);
                }
                break;
            case 3:
                if (esCliente) {
                    uiMensajeError("Acceso restringido. Un cliente no puede registrar ni consultar ventas.");
                    uiPausar();
                } else {
                    transaccionMenuVentas(ARCHIVO_TRANSACCIONES,
                                          ARCHIVO_VIDEOJUEGOS,
                                          ARCHIVO_PERSONAS,
                                          sesionActiva.persona.dni);
                }
                break;
            case 4:
                if (esCliente) {
                    uiMensajeError("Acceso restringido. Un cliente no puede acceder a reportes.");
                    uiPausar();
                } else {
                    transaccionMenuReportes(ARCHIVO_TRANSACCIONES,
                                            ARCHIVO_VIDEOJUEGOS,
                                            esAdmin);
                }
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
                if (esCliente) {
                    uiMensajeError("Acceso restringido para rol cliente.");
                } else {
                    uiMensajeAlerta("Modulo Extra de Promocion disponible en Fase 4.");
                }
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
