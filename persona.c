#include "persona.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Cuenta la cantidad total de registros Persona almacenados en el archivo binario */
long personaContarRegistrosArchivo(const char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    long bytes = 0;

    if (archi == NULL) {
        return 0;
    }

    fseek(archi, 0, SEEK_END);
    bytes = ftell(archi);
    fclose(archi);

    if (bytes <= 0) {
        return 0;
    }

    return bytes / (long)sizeof(Persona);
}

/* Imprime una fila resumen alineada para tablas de personas */
void personaMostrarResumen(Persona p) {
    const char *estado = (p.activo == PERSONA_ACTIVA) ? "Activo" : "Inactivo";
    const char *colorEstado = (p.activo == PERSONA_ACTIVA) ? CLR_OK : CLR_ERROR;

    printf("| %-14.14s | %-28.28s | %-11.11s | %s%-10.10s%s |\n",
           p.dni, p.nombre, p.rol, colorEstado, estado, CLR_RESET);
}

/* Imprime la ficha completa de una persona */
void personaMostrarDetalle(Persona p) {
    const char *estado = (p.activo == PERSONA_ACTIVA) ? "Activo" : "Inactivo";
    const char *colorEstado = (p.activo == PERSONA_ACTIVA) ? CLR_OK : CLR_ERROR;

    printf("%s", CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("  %sDETALLE DE PERSONA%s\n", CLR_SUBTIT, CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
    printf("  DNI       : %s\n", p.dni);
    printf("  Nombre    : %s\n", p.nombre);
    printf("  Telefono  : %s\n", p.telefono);
    printf("  Direccion : %s\n", p.direccion);
    printf("  Rol       : %s\n", p.rol);
    printf("  Estado    : %s%s%s\n", colorEstado, estado, CLR_RESET);
    printf("%s", CLR_TENUE);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
}

/* Busca una persona activa por DNI; si encontrada != NULL copia el registro */
int personaBuscarPorDni(const char nombreArchivo[], const char dniBuscado[], Persona *encontrada) {
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
static int personaExisteDniEnArchivo(const char nombreArchivo[], const char dniBuscado[]) {
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
int personaAgregar(const char nombreArchivo[], Persona nueva) {
    FILE *archi;
    size_t escritos;

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
int personaModificar(const char nombreArchivo[], const char dniBuscado[], Persona datosActualizados) {
    FILE *archi = fopen(nombreArchivo, "r+b");
    Persona aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Persona), 1, archi) == 1) {
        if (aux.activo == PERSONA_ACTIVA && strcmp(aux.dni, dniBuscado) == 0) {
            strcpy(datosActualizados.dni, aux.dni);
            datosActualizados.activo = PERSONA_ACTIVA;
            fseek(archi, -(long)sizeof(Persona), SEEK_CUR);
            fwrite(&datosActualizados, sizeof(Persona), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Realiza la baja logica (activo = 0) de una persona activa en modo r+b */
int personaBajaLogica(const char nombreArchivo[], const char dniBuscado[]) {
    FILE *archi = fopen(nombreArchivo, "r+b");
    Persona aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Persona), 1, archi) == 1) {
        if (aux.activo == PERSONA_ACTIVA && strcmp(aux.dni, dniBuscado) == 0) {
            aux.activo = PERSONA_INACTIVA;
            fseek(archi, -(long)sizeof(Persona), SEEK_CUR);
            fwrite(&aux, sizeof(Persona), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Carga todas las personas activas en un arreglo dinamico usando realloc y puntero doble */
void personaCargarTodasRealloc(const char nombreArchivo[], Persona **arr, int *validos, int *dim) {
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

    *arr = (Persona *)malloc((size_t)BLOQUE_PERSONAS * sizeof(Persona));
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
                temp = (Persona *)realloc(*arr, (size_t)nuevaDim * sizeof(Persona));
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
static void personaSeleccionarRol(char destinoRol[]) {
    int op;

    printf("  Seleccione el rol:\n");
    printf("    [1] Cliente (%s)\n", ROL_CLIENTE);
    printf("    [2] Empleado (%s)\n", ROL_EMPLEADO);
    printf("    [3] Administrador (%s)\n", ROL_ADMIN);
    op = uiLeerEnteroEnRango("  Rol > ", 1, 3);

    if (op == 1) {
        strcpy(destinoRol, ROL_CLIENTE);
    } else if (op == 2) {
        strcpy(destinoRol, ROL_EMPLEADO);
    } else {
        strcpy(destinoRol, ROL_ADMIN);
    }
}

/* Flujo interactivo para alta de nueva persona (Req. 3b) */
static void personaAltaInteractiva(const char nombreArchivo[]) {
    Persona nueva;

    uiTituloSeccion("ALTA DE PERSONA");
    uiLeerTexto("  DNI (sin puntos): ", nueva.dni, (int)sizeof(nueva.dni));

    if (personaExisteDniEnArchivo(nombreArchivo, nueva.dni)) {
        uiMensajeError("Ya existe una persona registrada con ese DNI.");
        return;
    }

    uiLeerTexto("  Nombre completo : ", nueva.nombre, (int)sizeof(nueva.nombre));
    uiLeerTexto("  Telefono        : ", nueva.telefono, (int)sizeof(nueva.telefono));
    uiLeerTexto("  Direccion       : ", nueva.direccion, (int)sizeof(nueva.direccion));
    personaSeleccionarRol(nueva.rol);
    nueva.activo = PERSONA_ACTIVA;

    if (personaAgregar(nombreArchivo, nueva)) {
        uiMensajeExito("Persona registrada correctamente en personas.bin.");
    } else {
        uiMensajeError("No se pudo registrar la persona.");
    }
}

/* Flujo interactivo para modificar una persona existente (Req. 3b) */
static void personaModificarInteractiva(const char nombreArchivo[]) {
    char dniBuscado[15];
    Persona actual;

    uiTituloSeccion("MODIFICAR PERSONA");
    uiLeerTexto("  Ingrese DNI de la persona a modificar: ", dniBuscado, (int)sizeof(dniBuscado));

    if (!personaBuscarPorDni(nombreArchivo, dniBuscado, &actual)) {
        uiMensajeError("No se encontro ninguna persona activa con ese DNI.");
        return;
    }

    personaMostrarDetalle(actual);

    if (uiConfirmar("Desea modificar el nombre?")) {
        uiLeerTexto("  Nuevo nombre   : ", actual.nombre, (int)sizeof(actual.nombre));
    }
    if (uiConfirmar("Desea modificar el telefono?")) {
        uiLeerTexto("  Nuevo telefono : ", actual.telefono, (int)sizeof(actual.telefono));
    }
    if (uiConfirmar("Desea modificar la direccion?")) {
        uiLeerTexto("  Nueva direccion: ", actual.direccion, (int)sizeof(actual.direccion));
    }
    if (uiConfirmar("Desea modificar el rol?")) {
        personaSeleccionarRol(actual.rol);
    }

    if (personaModificar(nombreArchivo, dniBuscado, actual)) {
        uiMensajeExito("Datos de la persona actualizados correctamente.");
    } else {
        uiMensajeError("No se pudieron actualizar los datos.");
    }
}

/* Flujo interactivo para baja logica de persona */
static void personaBajaInteractiva(const char nombreArchivo[]) {
    char dniBuscado[15];
    Persona actual;

    uiTituloSeccion("BAJA LOGICA DE PERSONA");
    uiLeerTexto("  Ingrese DNI de la persona a dar de baja: ", dniBuscado, (int)sizeof(dniBuscado));

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
static void personaListarInteractivo(const char nombreArchivo[]) {
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
static void personaVerDetalleInteractivo(const char nombreArchivo[]) {
    char dniBuscado[15];
    Persona encontrada;

    uiTituloSeccion("BUSCAR PERSONA POR DNI");
    uiLeerTexto("  Ingrese DNI a consultar: ", dniBuscado, (int)sizeof(dniBuscado));

    if (personaBuscarPorDni(nombreArchivo, dniBuscado, &encontrada)) {
        personaMostrarDetalle(encontrada);
    } else {
        uiMensajeError("No se encontro ninguna persona activa con ese DNI.");
    }
}

/* Submenu interactivo de gestion de personas */
void personaMenuGestion(const char nombreArchivo[], const char usuarioActivo[], const char rolActivo[]) {
    int opcion;

    do {
        uiLimpiarPantalla();
        uiMostrarBannerPrincipal();
        uiMostrarBarraSesion(usuarioActivo, rolActivo);
        uiTituloSeccion("GESTION DE PERSONAS (CLIENTES Y EMPLEADOS)");

        printf("  %s[1]%s Agregar Persona\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[2]%s Modificar Persona\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[3]%s Baja Logica de Persona\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[4]%s Ver Listado de Personas (Resumen)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[5]%s Ver Info Detallada por DNI\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[0]%s Volver al Menu Principal\n", CLR_SUBTIT, CLR_RESET);
        printf("%s", CLR_TENUE);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);

        opcion = uiLeerEnteroEnRango("  Seleccione una opcion > ", 0, 5);

        switch (opcion) {
            case 1:
                personaAltaInteractiva(nombreArchivo);
                uiPausar();
                break;
            case 2:
                personaModificarInteractiva(nombreArchivo);
                uiPausar();
                break;
            case 3:
                personaBajaInteractiva(nombreArchivo);
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
