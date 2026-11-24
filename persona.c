#include "persona.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Cuenta la cantidad total de registros Persona almacenados en el archivo binario */
int personaContarRegistrosArchivo(char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    int bytes = 0;
    int tam = sizeof(Persona);

    if (archi == NULL) {
        return 0;
    }

    fseek(archi, 0, SEEK_END);
    bytes = ftell(archi);
    fclose(archi);

    if (bytes <= 0) {
        return 0;
    }

    return bytes / tam;
}

/* Imprime una fila resumen alineada para tablas de personas */
void personaMostrarResumen(Persona p) {
    char estado[15];
    char colorEstado[15];

    if (p.activo == PERSONA_ACTIVA) {
        strcpy(estado, "Activo");
        strcpy(colorEstado, CLR_OK);
    } else {
        strcpy(estado, "Inactivo");
        strcpy(colorEstado, CLR_ERROR);
    }

    printf("| %-14.14s | %-28.28s | %-11.11s | %s%-10.10s%s |\n",
           p.dni, p.nombre, p.rol, colorEstado, estado, CLR_RESET);
}

/* Imprime la ficha completa de una persona dentro de una tarjeta ASCII */
void personaMostrarDetalle(Persona p) {
    char estado[15];
    char colorEstado[15];

    if (p.activo == PERSONA_ACTIVA) {
        strcpy(estado, "[ACTIVO]");
        strcpy(colorEstado, CLR_OK);
    } else {
        strcpy(estado, "[INACTIVO]");
        strcpy(colorEstado, CLR_ERROR);
    }

    uiAbrirCaja("FICHA DETALLADA DE PERSONA");
    printf("%s|%s  Nombre    : %s%-58.58s%s  %s|%s\n",
           CLR_TITULO, CLR_RESET, CLR_SUBTIT, p.nombre, CLR_RESET, CLR_TITULO, CLR_RESET);
    uiSeparadorCaja();
    printf("%s|%s  DNI       : %-24.24s  Rol    : %-23.23s  %s|%s\n",
           CLR_TITULO, CLR_RESET, p.dni, p.rol, CLR_TITULO, CLR_RESET);
    printf("%s|%s  Telefono  : %-24.24s  Estado : %s%-23.23s%s  %s|%s\n",
           CLR_TITULO, CLR_RESET, p.telefono, colorEstado, estado, CLR_RESET, CLR_TITULO, CLR_RESET);
    printf("%s|%s  Direccion : %-58.58s  %s|%s\n",
           CLR_TITULO, CLR_RESET, p.direccion, CLR_TITULO, CLR_RESET);
    uiCerrarCaja();
}

/* Busca una persona activa por DNI; si encontrada != NULL copia el registro */
int personaBuscarPorDni(char nombreArchivo[], char dniBuscado[], Persona *encontrada) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Persona aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Persona), 1, archi) == 1) {
        if (aux.activo == PERSONA_ACTIVA && strcmp(aux.dni, dniBuscado) == 0) {
            if (encontrada != NULL) {
                *encontrada = aux;
            }
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Verifica si un DNI ya existe en el archivo (activo o inactivo) para proteger unicidad */
int personaExisteDniEnArchivo(char nombreArchivo[], char dniBuscado[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Persona aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Persona), 1, archi) == 1) {
        if (strcmp(aux.dni, dniBuscado) == 0) {
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Da de alta una persona validando unicidad de DNI */
int personaAgregar(char nombreArchivo[], Persona nueva) {
    FILE *archi;
    int escritos = 0;

    if (personaBuscarPorDni(nombreArchivo, nueva.dni, NULL) ||
        personaExisteDniEnArchivo(nombreArchivo, nueva.dni)) {
        return 0;
    }

    archi = fopen(nombreArchivo, "ab");
    if (archi == NULL) {
        return 0;
    }

    nueva.activo = PERSONA_ACTIVA;
    escritos = fwrite(&nueva, sizeof(Persona), 1, archi);
    fclose(archi);

    return (escritos == 1);
}

/* Modifica los datos de una persona activa in-place usando modo r+b y fseek */
int personaModificar(char nombreArchivo[], char dniBuscado[], Persona datosActualizados) {
    FILE *archi = fopen(nombreArchivo, "r+b");
    Persona aux;
    int tam = sizeof(Persona);

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Persona), 1, archi) == 1) {
        if (aux.activo == PERSONA_ACTIVA && strcmp(aux.dni, dniBuscado) == 0) {
            strcpy(datosActualizados.dni, aux.dni);
            datosActualizados.activo = PERSONA_ACTIVA;
            fseek(archi, -tam, SEEK_CUR);
            fwrite(&datosActualizados, sizeof(Persona), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Realiza la baja logica (activo = 0) de una persona activa en modo r+b */
int personaBajaLogica(char nombreArchivo[], char dniBuscado[]) {
    FILE *archi = fopen(nombreArchivo, "r+b");
    Persona aux;
    int tam = sizeof(Persona);

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Persona), 1, archi) == 1) {
        if (aux.activo == PERSONA_ACTIVA && strcmp(aux.dni, dniBuscado) == 0) {
            aux.activo = PERSONA_INACTIVA;
            fseek(archi, -tam, SEEK_CUR);
            fwrite(&aux, sizeof(Persona), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Carga todas las personas activas en un arreglo dinamico usando realloc y puntero doble */
void personaCargarTodasRealloc(char nombreArchivo[], Persona **arr, int *validos, int *dim) {
    FILE *archi;
    Persona aux;
    Persona *temp;
    int nuevaDim;

    *arr = NULL;
    *validos = 0;
    *dim = 0;

    archi = fopen(nombreArchivo, "rb");
    if (archi == NULL) {
        return;
    }

    *arr = (Persona *)malloc(BLOQUE_PERSONAS * sizeof(Persona));
    if (*arr == NULL) {
        fclose(archi);
        uiMensajeError("No se pudo asignar memoria dinamica para personas.");
        return;
    }
    *dim = BLOQUE_PERSONAS;

    while (fread(&aux, sizeof(Persona), 1, archi) == 1) {
        if (aux.activo == PERSONA_ACTIVA) {
            if (*validos == *dim) {
                nuevaDim = *dim + BLOQUE_PERSONAS;
                temp = (Persona *)realloc(*arr, nuevaDim * sizeof(Persona));
                if (temp == NULL) {
                    uiMensajeError("Error al ampliar memoria con realloc para personas.");
                    fclose(archi);
                    return;
                }
                *arr = temp;
                *dim = nuevaDim;
            }
            (*arr)[*validos] = aux;
            (*validos)++;
        }
    }

    fclose(archi);
}

/* Permite seleccionar un rol estandarizado evitando errores de escritura */
void personaSeleccionarRol(char destinoRol[]) {
    int op;

    printf("  Seleccione el rol:\n");
    printf("    %s[1]%s Cliente (%s)\n", CLR_SUBTIT, CLR_RESET, ROL_CLIENTE);
    printf("    %s[2]%s Empleado (%s)\n", CLR_SUBTIT, CLR_RESET, ROL_EMPLEADO);
    printf("    %s[3]%s Administrador (%s)\n", CLR_SUBTIT, CLR_RESET, ROL_ADMIN);
    op = uiLeerEnteroEnRango("  Rol > ", 1, 3);

    if (op == 1) {
        strcpy(destinoRol, ROL_CLIENTE);
    } else if (op == 2) {
        strcpy(destinoRol, ROL_EMPLEADO);
    } else {
        strcpy(destinoRol, ROL_ADMIN);
    }
}

/* Flujo interactivo guiado con vista previa para alta de nueva persona (Req. 3b) */
void personaAltaInteractiva(char nombreArchivo[], int esAdmin) {
    Persona nueva;

    uiAbrirCaja("ALTA DE NUEVA PERSONA - FORMULARIO GUIADO");
    printf("  %sPaso 1/5 - Identificacion%s\n", CLR_TITULO, CLR_RESET);
    uiLeerTexto("  DNI (sin puntos) : ", nueva.dni, 15);

    if (personaExisteDniEnArchivo(nombreArchivo, nueva.dni)) {
        uiMensajeError("Ya existe una persona registrada con ese DNI.");
        return;
    }

    printf("\n  %sPaso 2/5 - Datos Personales%s\n", CLR_TITULO, CLR_RESET);
    uiLeerTexto("  Nombre completo  : ", nueva.nombre, 50);

    printf("\n  %sPaso 3/5 - Contacto%s\n", CLR_TITULO, CLR_RESET);
    uiLeerTexto("  Telefono         : ", nueva.telefono, 20);

    printf("\n  %sPaso 4/5 - Domicilio%s\n", CLR_TITULO, CLR_RESET);
    uiLeerTexto("  Direccion        : ", nueva.direccion, 30);

    printf("\n  %sPaso 5/5 - Rol en Pixel Plus%s\n", CLR_TITULO, CLR_RESET);
    if (esAdmin) {
        personaSeleccionarRol(nueva.rol);
    } else {
        strcpy(nueva.rol, ROL_CLIENTE);
        printf("  Rol asignado automaticamente: %s%s%s (solo admin crea empleados/admins)\n",
               CLR_SUBTIT, ROL_CLIENTE, CLR_RESET);
    }
    nueva.activo = PERSONA_ACTIVA;

    printf("\n  %sVISTA PREVIA ANTES DE GUARDAR:%s", CLR_ALERTA, CLR_RESET);
    personaMostrarDetalle(nueva);

    if (uiConfirmar("Confirma el alta de esta persona en personas.bin?")) {
        if (personaAgregar(nombreArchivo, nueva)) {
            uiMensajeExito("Persona registrada correctamente en personas.bin.");
        } else {
            uiMensajeError("No se pudo registrar la persona.");
        }
    } else {
        uiMensajeAlerta("Alta cancelada por el usuario.");
    }
}

/* Flujo interactivo con menu por campo para modificar una persona (Req. 3b) */
void personaModificarInteractiva(char nombreArchivo[], int esAdmin) {
    char dniBuscado[15];
    Persona actual;
    int opcion;
    int maxOpcion;
    int huboCambios = 0;

    uiTituloSeccion("MODIFICAR PERSONA");
    uiLeerTexto("  Ingrese DNI de la persona a modificar: ", dniBuscado, 15);

    if (!personaBuscarPorDni(nombreArchivo, dniBuscado, &actual)) {
        uiMensajeError("No se encontro ninguna persona activa con ese DNI.");
        return;
    }

    if (!esAdmin && strcmp(actual.rol, ROL_CLIENTE) != 0) {
        uiMensajeError("Acceso restringido. Un empleado solo puede modificar datos de clientes.");
        return;
    }

    maxOpcion = esAdmin ? 5 : 4;

    do {
        uiLimpiarPantalla();
        uiMostrarBannerPrincipal();
        printf("\n  %sEDITOR INTERACTIVO DE PERSONA (DNI: %s)%s", CLR_SUBTIT, actual.dni, CLR_RESET);
        personaMostrarDetalle(actual);

        printf("\n%s", CLR_TITULO);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);
        printf("  %s[1]%s Editar Nombre         %s[3]%s Editar Direccion\n",
               CLR_SUBTIT, CLR_RESET, CLR_SUBTIT, CLR_RESET);
        if (esAdmin) {
            printf("  %s[2]%s Editar Telefono       %s[4]%s Cambiar Rol\n",
                   CLR_SUBTIT, CLR_RESET, CLR_SUBTIT, CLR_RESET);
            printf("  %s[5]%s %sGuardar Cambios%s       %s[0]%s Cancelar sin Guardar\n",
                   CLR_SUBTIT, CLR_RESET, CLR_OK, CLR_RESET, CLR_SUBTIT, CLR_RESET);
        } else {
            printf("  %s[2]%s Editar Telefono       %s[4]%s %sGuardar Cambios%s\n",
                   CLR_SUBTIT, CLR_RESET, CLR_SUBTIT, CLR_RESET, CLR_OK, CLR_RESET);
            printf("  %s[0]%s Cancelar sin Guardar\n", CLR_SUBTIT, CLR_RESET);
        }
        printf("%s", CLR_TENUE);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);

        opcion = uiLeerEnteroEnRango("  Que campo desea editar? > ", 0, maxOpcion);

        if (opcion == 1) {
            uiLeerTexto("  Nuevo nombre    : ", actual.nombre, 50);
            huboCambios = 1;
        } else if (opcion == 2) {
            uiLeerTexto("  Nuevo telefono  : ", actual.telefono, 20);
            huboCambios = 1;
        } else if (opcion == 3) {
            uiLeerTexto("  Nueva direccion : ", actual.direccion, 30);
            huboCambios = 1;
        } else if (esAdmin && opcion == 4) {
            personaSeleccionarRol(actual.rol);
            huboCambios = 1;
        } else if (opcion == maxOpcion) {
            if (huboCambios) {
                if (personaModificar(nombreArchivo, dniBuscado, actual)) {
                    uiMensajeExito("Datos de la persona actualizados correctamente.");
                } else {
                    uiMensajeError("No se pudieron actualizar los datos.");
                }
            } else {
                uiMensajeAlerta("No se realizaron cambios sobre la persona.");
            }
            opcion = 0;
        } else if (opcion == 0 && huboCambios) {
            uiMensajeAlerta("Cambios descartados. No se modifico personas.bin.");
        }
    } while (opcion != 0);
}

/* Flujo interactivo para baja logica de persona */
void personaBajaInteractiva(char nombreArchivo[]) {
    char dniBuscado[15];
    Persona actual;

    uiTituloSeccion("BAJA LOGICA DE PERSONA");
    uiLeerTexto("  Ingrese DNI de la persona a dar de baja: ", dniBuscado, 15);

    if (!personaBuscarPorDni(nombreArchivo, dniBuscado, &actual)) {
        uiMensajeError("No se encontro ninguna persona activa con ese DNI.");
        return;
    }

    personaMostrarDetalle(actual);
    if (uiConfirmar("Confirma la baja logica de esta persona?")) {
        if (personaBajaLogica(nombreArchivo, dniBuscado)) {
            uiMensajeExito("Persona dada de baja logicamente.");
        } else {
            uiMensajeError("No se pudo procesar la baja logica.");
        }
    } else {
        uiMensajeAlerta("Operacion cancelada por el usuario.");
    }
}

/* Muestra el listado resumen de personas cargadas dinamicamente con realloc (Req. 3c) */
void personaListarInteractivo(char nombreArchivo[]) {
    Persona *lista = NULL;
    int validos = 0;
    int dim = 0;
    int i;

    uiTituloSeccion("LISTADO DE PERSONAS ACTIVAS");
    personaCargarTodasRealloc(nombreArchivo, &lista, &validos, &dim);

    if (lista == NULL || validos == 0) {
        uiMensajeAlerta("No hay personas activas registradas.");
        if (lista != NULL) {
            free(lista);
            lista = NULL;
        }
        return;
    }

    printf("%s", CLR_TITULO);
    printf("+----------------+------------------------------+-------------+------------+\n");
    printf("| DNI            | Nombre                       | Rol         | Estado     |\n");
    printf("+----------------+------------------------------+-------------+------------+\n");
    printf("%s", CLR_RESET);

    for (i = 0; i < validos; i++) {
        personaMostrarResumen(lista[i]);
    }

    printf("%s", CLR_TITULO);
    printf("+----------------+------------------------------+-------------+------------+\n");
    printf("%s", CLR_RESET);
    printf("  Total activas: %d (Capacidad dinamica reservada: %d)\n", validos, dim);

    free(lista);
    lista = NULL;
}

/* Consulta detallada por DNI (Req. 3d) */
void personaVerDetalleInteractivo(char nombreArchivo[]) {
    char dniBuscado[15];
    Persona encontrada;

    uiTituloSeccion("BUSCAR PERSONA POR DNI");
    uiLeerTexto("  Ingrese DNI a consultar: ", dniBuscado, 15);

    if (personaBuscarPorDni(nombreArchivo, dniBuscado, &encontrada)) {
        personaMostrarDetalle(encontrada);
    } else {
        uiMensajeError("No se encontro ninguna persona activa con ese DNI.");
    }
}

/* Submenu interactivo de gestion de personas con control de permisos por rol */
void personaMenuGestion(char nombreArchivo[], char usuarioActivo[], char rolActivo[], int esAdmin) {
    int opcion;

    do {
        uiLimpiarPantalla();
        uiMostrarBannerPrincipal();
        uiMostrarBarraSesion(usuarioActivo, rolActivo);
        uiTituloSeccion("GESTION DE PERSONAS (CLIENTES Y EMPLEADOS)");

        if (esAdmin) {
            printf("  %s[1]%s Agregar Persona (Cliente / Empleado / Admin)\n", CLR_SUBTIT, CLR_RESET);
            printf("  %s[2]%s Modificar Persona\n", CLR_SUBTIT, CLR_RESET);
            printf("  %s[3]%s Baja Logica de Persona\n", CLR_SUBTIT, CLR_RESET);
        } else {
            printf("  %s[1]%s Agregar Cliente\n", CLR_SUBTIT, CLR_RESET);
            printf("  %s[2]%s Modificar Cliente\n", CLR_SUBTIT, CLR_RESET);
            printf("  %s[3] Baja Logica de Persona (Solo Admin)%s\n", CLR_TENUE, CLR_RESET);
        }
        printf("  %s[4]%s Ver Listado de Personas (Resumen)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[5]%s Ver Info Detallada por DNI\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[0]%s Volver al Menu Principal\n", CLR_SUBTIT, CLR_RESET);
        printf("%s", CLR_TENUE);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);

        opcion = uiLeerEnteroEnRango("  Seleccione una opcion > ", 0, 5);

        switch (opcion) {
            case 1:
                personaAltaInteractiva(nombreArchivo, esAdmin);
                uiPausar();
                break;
            case 2:
                personaModificarInteractiva(nombreArchivo, esAdmin);
                uiPausar();
                break;
            case 3:
                if (esAdmin) {
                    personaBajaInteractiva(nombreArchivo);
                } else {
                    uiMensajeError("Acceso restringido. Solo administradores pueden dar de baja personas.");
                }
                uiPausar();
                break;
            case 4:
                personaListarInteractivo(nombreArchivo);
                uiPausar();
                break;
            case 5:
                personaVerDetalleInteractivo(nombreArchivo);
                uiPausar();
                break;
            case 0:
                break;
            default:
                uiMensajeError("Opcion no reconocida.");
                uiPausar();
                break;
        }
    } while (opcion != 0);
}
