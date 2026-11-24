#include "videojuego.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Cuenta la cantidad total de registros Videojuego almacenados en el archivo binario */
long videojuegoContarRegistrosArchivo(const char nombreArchivo[]) {
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

    return bytes / (long)sizeof(Videojuego);
}

/* Imprime una fila resumen del videojuego destacando stock critico */
void videojuegoMostrarResumen(Videojuego v) {
    const char *colorStock = CLR_OK;

    if (v.stock == 0) {
        colorStock = CLR_ERROR;
    } else if (v.stock <= STOCK_CRITICO) {
        colorStock = CLR_ALERTA;
    }

    printf("| %-4d | %-32.32s | %-16.16s | %s%6d%s |\n",
           v.idJuego, v.titulo, v.plataforma, colorStock, v.stock, CLR_RESET);
}

/* Imprime la ficha detallada de un videojuego controlando la visibilidad del costo */
void videojuegoMostrarDetalle(Videojuego v, int mostrarPrecioCosto) {
    const char *estado = (v.activo == JUEGO_ACTIVO) ? "Activo" : "Inactivo";
    const char *colorEstado = (v.activo == JUEGO_ACTIVO) ? CLR_OK : CLR_ERROR;
    const char *colorStock = (v.stock <= STOCK_CRITICO) ? CLR_ALERTA : CLR_OK;

    if (v.stock == 0) {
        colorStock = CLR_ERROR;
    }

    printf("%s", CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("  %sDETALLE DE VIDEOJUEGO%s\n", CLR_SUBTIT, CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
    printf("  ID Juego           : %d\n", v.idJuego);
    printf("  Titulo             : %s\n", v.titulo);
    printf("  Plataforma         : %s\n", v.plataforma);
    printf("  Genero             : %s\n", v.genero);
    printf("  Stock disponible   : %s%d unidades%s\n", colorStock, v.stock, CLR_RESET);
    if (mostrarPrecioCosto) {
        printf("  Precio adquisicion : $%.2f\n", v.precioAdquisicion);
    } else {
        printf("  Precio adquisicion : %s(Restringido a rol administrativo)%s\n",
               CLR_TENUE, CLR_RESET);
    }
    printf("  Estado             : %s%s%s\n", colorEstado, estado, CLR_RESET);
    printf("%s", CLR_TENUE);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
}

/* Compara dos cadenas sin distinguir mayusculas de minusculas */
static int videojuegoTextoIgualSinCase(const char a[], const char b[]) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return 0;
        }
        i++;
    }

    return (a[i] == '\0' && b[i] == '\0');
}

/* Verifica si un idJuego ya existe en el archivo (activo o inactivo) */
static int videojuegoExisteIdEnArchivo(const char nombreArchivo[], int idBuscado) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Videojuego aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.idJuego == idBuscado) {
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Cuenta la cantidad de videojuegos activos en el archivo */
static int videojuegoContarActivos(const char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Videojuego aux;
    int cant = 0;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO) {
            cant++;
        }
    }

    fclose(archi);
    return cant;
}

/* Genera el proximo ID unico analizando el maximo ID registrado en disco */
int videojuegoGenerarNuevoId(const char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Videojuego aux;
    int maxId = 0;

    if (archi == NULL) {
        return 1;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.idJuego > maxId) {
            maxId = aux.idJuego;
        }
    }

    fclose(archi);
    return maxId + 1;
}

/* Busca un videojuego activo por idJuego; si encontrado != NULL copia el registro */
int videojuegoBuscarPorId(const char nombreArchivo[], int idBuscado, Videojuego *encontrado) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Videojuego aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO && aux.idJuego == idBuscado) {
            if (encontrado != NULL) {
                *encontrado = aux;
            }
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Da de alta un videojuego validando unicidad de idJuego */
int videojuegoAgregar(const char nombreArchivo[], Videojuego nuevo) {
    FILE *archi;
    size_t escritos;

    if (nuevo.idJuego <= 0 || videojuegoExisteIdEnArchivo(nombreArchivo, nuevo.idJuego)) {
        return 0;
    }

    archi = fopen(nombreArchivo, "ab");
    if (archi == NULL) {
        return 0;
    }

    nuevo.activo = JUEGO_ACTIVO;
    escritos = fwrite(&nuevo, sizeof(Videojuego), 1, archi);
    fclose(archi);

    return (escritos == 1);
}

/* Modifica los datos y/o stock de un videojuego activo in-place con r+b y fseek */
int videojuegoModificar(const char nombreArchivo[], int idBuscado, Videojuego actualizado) {
    FILE *archi = fopen(nombreArchivo, "r+b");
    Videojuego aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO && aux.idJuego == idBuscado) {
            actualizado.idJuego = aux.idJuego;
            actualizado.activo = JUEGO_ACTIVO;
            fseek(archi, -(long)sizeof(Videojuego), SEEK_CUR);
            fwrite(&actualizado, sizeof(Videojuego), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Realiza la baja logica (activo = 0) de un videojuego activo en modo r+b */
int videojuegoBajaLogica(const char nombreArchivo[], int idBuscado) {
    FILE *archi = fopen(nombreArchivo, "r+b");
    Videojuego aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO && aux.idJuego == idBuscado) {
            aux.activo = JUEGO_INACTIVO;
            fseek(archi, -(long)sizeof(Videojuego), SEEK_CUR);
            fwrite(&aux, sizeof(Videojuego), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Descuenta stock de un juego activo en r+b (puente para registro de ventas Fase 3) */
int videojuegoDescontarStock(const char nombreArchivo[], int idJuego, int cantidadVendida) {
    FILE *archi;
    Videojuego aux;

    if (cantidadVendida <= 0) {
        return 0;
    }

    archi = fopen(nombreArchivo, "r+b");
    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO && aux.idJuego == idJuego) {
            if (aux.stock < cantidadVendida) {
                fclose(archi);
                return 0;
            }
            aux.stock -= cantidadVendida;
            fseek(archi, -(long)sizeof(Videojuego), SEEK_CUR);
            fwrite(&aux, sizeof(Videojuego), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Carga todos los videojuegos activos en un arreglo dinamico de tamanio exacto */
Videojuego* videojuegoCargarActivos(const char nombreArchivo[], int *validos) {
    FILE *archi;
    Videojuego *arr = NULL;
    Videojuego aux;
    int cantActivos = videojuegoContarActivos(nombreArchivo);
    int i = 0;

    *validos = 0;
    if (cantActivos == 0) {
        return NULL;
    }

    arr = (Videojuego *)malloc((size_t)cantActivos * sizeof(Videojuego));
    if (arr == NULL) {
        uiMensajeError("No se pudo asignar memoria dinamica para videojuegos.");
        return NULL;
    }

    archi = fopen(nombreArchivo, "rb");
    if (archi == NULL) {
        free(arr);
        return NULL;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1 && i < cantActivos) {
        if (aux.activo == JUEGO_ACTIVO) {
            arr[i] = aux;
            i++;
        }
    }

    fclose(archi);
    *validos = i;
    return arr;
}

/* Muestra recursivamente el arreglo de videojuegos activos (Req. 3e) */
void videojuegoMostrarArregloRec(const Videojuego arr[], int validos, int pos) {
    if (pos >= validos) {
        return;
    }

    videojuegoMostrarResumen(arr[pos]);
    videojuegoMostrarArregloRec(arr, validos, pos + 1);
}

/* Cuenta cuantos videojuegos activos pertenecen a una plataforma especifica (Req. 3f) */
int videojuegoContarPorPlataforma(const char nombreArchivo[], const char plataforma[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Videojuego aux;
    int cant = 0;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO &&
            videojuegoTextoIgualSinCase(aux.plataforma, plataforma)) {
            cant++;
        }
    }

    fclose(archi);
    return cant;
}

/* Carga en arreglo dinamico de tamanio justo los juegos activos de una plataforma (Req. 3f) */
Videojuego* videojuegoFiltrarPorPlataforma(const char nombreArchivo[], const char plataforma[], int *validos) {
    FILE *archi;
    Videojuego *arr = NULL;
    Videojuego aux;
    int cant = videojuegoContarPorPlataforma(nombreArchivo, plataforma);
    int i = 0;

    *validos = 0;
    if (cant == 0) {
        return NULL;
    }

    arr = (Videojuego *)malloc((size_t)cant * sizeof(Videojuego));
    if (arr == NULL) {
        uiMensajeError("No se pudo asignar memoria dinamica para el filtro por plataforma.");
        return NULL;
    }

    archi = fopen(nombreArchivo, "rb");
    if (archi == NULL) {
        free(arr);
        return NULL;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1 && i < cant) {
        if (aux.activo == JUEGO_ACTIVO &&
            videojuegoTextoIgualSinCase(aux.plataforma, plataforma)) {
            arr[i] = aux;
            i++;
        }
    }

    fclose(archi);
    *validos = i;
    return arr;
}

/* Flujo interactivo para alta de videojuego (Req. 3a) */
static void videojuegoAltaInteractiva(const char nombreArchivo[]) {
    Videojuego nuevo;
    int idSugerido = videojuegoGenerarNuevoId(nombreArchivo);

    uiTituloSeccion("ALTA DE VIDEOJUEGO");
    printf("  ID sugerido automaticamente: %d\n", idSugerido);

    if (uiConfirmar("Desea utilizar el ID automatico sugerido?")) {
        nuevo.idJuego = idSugerido;
    } else {
        nuevo.idJuego = uiLeerEnteroEnRango("  Ingrese ID unico del videojuego (1-99999): ", 1, 99999);
        if (videojuegoExisteIdEnArchivo(nombreArchivo, nuevo.idJuego)) {
            uiMensajeError("Ya existe un videojuego registrado con ese ID.");
            return;
        }
    }

    uiLeerTexto("  Titulo             : ", nuevo.titulo, (int)sizeof(nuevo.titulo));
    uiLeerTexto("  Plataforma (ej PS5): ", nuevo.plataforma, (int)sizeof(nuevo.plataforma));
    uiLeerTexto("  Genero             : ", nuevo.genero, (int)sizeof(nuevo.genero));
    nuevo.stock = uiLeerEnteroEnRango("  Stock inicial      : ", 0, 100000);
    nuevo.precioAdquisicion = uiLeerFloatMinimo("  Precio adquisicion : $", 0.0f);
    nuevo.activo = JUEGO_ACTIVO;

    if (videojuegoAgregar(nombreArchivo, nuevo)) {
        uiMensajeExito("Videojuego agregado correctamente al inventario.");
    } else {
        uiMensajeError("No se pudo agregar el videojuego.");
    }
}

/* Flujo interactivo para modificar un videojuego y actualizar stock (Req. 3a) */
static void videojuegoModificarInteractiva(const char nombreArchivo[], int esAdmin) {
    int idBuscado;
    Videojuego actual;

    uiTituloSeccion("MODIFICAR VIDEOJUEGO / ACTUALIZAR STOCK");
    idBuscado = uiLeerEnteroEnRango("  Ingrese ID del videojuego a modificar: ", 1, 99999);

    if (!videojuegoBuscarPorId(nombreArchivo, idBuscado, &actual)) {
        uiMensajeError("No se encontro ningun videojuego activo con ese ID.");
        return;
    }

    videojuegoMostrarDetalle(actual, esAdmin);

    if (uiConfirmar("Desea modificar el titulo?")) {
        uiLeerTexto("  Nuevo titulo     : ", actual.titulo, (int)sizeof(actual.titulo));
    }
    if (uiConfirmar("Desea modificar la plataforma?")) {
        uiLeerTexto("  Nueva plataforma : ", actual.plataforma, (int)sizeof(actual.plataforma));
    }
    if (uiConfirmar("Desea modificar el genero?")) {
        uiLeerTexto("  Nuevo genero     : ", actual.genero, (int)sizeof(actual.genero));
    }
    if (uiConfirmar("Desea actualizar el stock?")) {
        actual.stock = uiLeerEnteroEnRango("  Nuevo stock      : ", 0, 100000);
    }
    if (esAdmin && uiConfirmar("Desea modificar el precio de adquisicion?")) {
        actual.precioAdquisicion = uiLeerFloatMinimo("  Nuevo costo      : $", 0.0f);
    }

    if (videojuegoModificar(nombreArchivo, idBuscado, actual)) {
        uiMensajeExito("Videojuego actualizado correctamente en videojuegos.bin.");
    } else {
        uiMensajeError("No se pudo actualizar el videojuego.");
    }
}

/* Flujo interactivo para baja logica de videojuego */
static void videojuegoBajaInteractiva(const char nombreArchivo[], int esAdmin) {
    int idBuscado;
    Videojuego actual;

    uiTituloSeccion("BAJA LOGICA DE VIDEOJUEGO");
    idBuscado = uiLeerEnteroEnRango("  Ingrese ID del videojuego a dar de baja: ", 1, 99999);

    if (!videojuegoBuscarPorId(nombreArchivo, idBuscado, &actual)) {
        uiMensajeError("No se encontro ningun videojuego activo con ese ID.");
        return;
    }

    videojuegoMostrarDetalle(actual, esAdmin);
    if (uiConfirmar("Confirma la baja logica de este videojuego?")) {
        if (videojuegoBajaLogica(nombreArchivo, idBuscado)) {
            uiMensajeExito("Videojuego dado de baja logicamente.");
        } else {
            uiMensajeError("No se pudo completar la baja logica.");
        }
    } else {
        uiMensajeAlerta("Operacion cancelada por el usuario.");
    }
}

/* Muestra el listado general de videojuegos usando recursividad (Req. 3e) */
static void videojuegoListarInteractivo(const char nombreArchivo[]) {
    Videojuego *lista = NULL;
    int validos = 0;

    uiTituloSeccion("LISTADO GENERAL DE VIDEOJUEGOS (ACTIVOS)");
    lista = videojuegoCargarActivos(nombreArchivo, &validos);

    if (lista == NULL || validos == 0) {
        uiMensajeAlerta("No hay videojuegos activos registrados en inventario.");
        return;
    }

    printf("%s", CLR_TITULO);
    printf("+------+----------------------------------+------------------+--------+\n");
    printf("| ID   | Titulo                           | Plataforma       |  Stock |\n");
    printf("+------+----------------------------------+------------------+--------+\n");
    printf("%s", CLR_RESET);

    videojuegoMostrarArregloRec(lista, validos, 0);

    printf("%s", CLR_TITULO);
    printf("+------+----------------------------------+------------------+--------+\n");
    printf("%s", CLR_RESET);
    printf("  Total de titulos activos: %d\n", validos);

    free(lista);
    lista = NULL;
}

/* Filtra videojuegos por plataforma usando malloc de tamanio justo (Req. 3f) */
static void videojuegoFiltrarPlataformaInteractivo(const char nombreArchivo[], int esAdmin) {
    char plataforma[20];
    Videojuego *filtrados = NULL;
    int validos = 0;
    int i;

    uiTituloSeccion("BUSCAR VIDEOJUEGOS POR PLATAFORMA");
    uiLeerTexto("  Plataforma (PS5 / PC / Switch / Xbox Series X): ",
                plataforma, (int)sizeof(plataforma));

    filtrados = videojuegoFiltrarPorPlataforma(nombreArchivo, plataforma, &validos);

    if (filtrados == NULL || validos == 0) {
        uiMensajeAlerta("No se encontraron videojuegos activos para esa plataforma.");
        return;
    }

    printf("%s", CLR_TITULO);
    if (esAdmin) {
        printf("+------+----------------------------+--------------+--------+-------------+\n");
        printf("| ID   | Titulo                     | Genero       |  Stock | Costo Adq.  |\n");
        printf("+------+----------------------------+--------------+--------+-------------+\n");
    } else {
        printf("+------+----------------------------------+------------------+--------+\n");
        printf("| ID   | Titulo                           | Plataforma       |  Stock |\n");
        printf("+------+----------------------------------+------------------+--------+\n");
    }
    printf("%s", CLR_RESET);

    for (i = 0; i < validos; i++) {
        if (esAdmin) {
            printf("| %-4d | %-26.26s | %-12.12s | %6d | $%10.2f |\n",
                   filtrados[i].idJuego, filtrados[i].titulo,
                   filtrados[i].genero, filtrados[i].stock,
                   filtrados[i].precioAdquisicion);
        } else {
            videojuegoMostrarResumen(filtrados[i]);
        }
    }

    printf("%s", CLR_TITULO);
    if (esAdmin) {
        printf("+------+----------------------------+--------------+--------+-------------+\n");
    } else {
        printf("+------+----------------------------------+------------------+--------+\n");
    }
    printf("%s", CLR_RESET);
    printf("  Encontrados para '%s': %d (memoria exacta reservada)\n", plataforma, validos);

    free(filtrados);
    filtrados = NULL;
}

/* Muestra la ficha detallada de un videojuego aplicando control de rol (Req. 3g) */
static void videojuegoVerDetalleInteractivo(const char nombreArchivo[], int esAdmin) {
    int idBuscado;
    Videojuego encontrado;

    uiTituloSeccion("INFO DETALLADA DE VIDEOJUEGO");
    idBuscado = uiLeerEnteroEnRango("  Ingrese ID del videojuego a consultar: ", 1, 99999);

    if (videojuegoBuscarPorId(nombreArchivo, idBuscado, &encontrado)) {
        videojuegoMostrarDetalle(encontrado, esAdmin);
    } else {
        uiMensajeError("No se encontro ningun videojuego activo con ese ID.");
    }
}

/* Submenu interactivo de gestion de inventario de videojuegos */
void videojuegoMenuGestion(const char nombreArchivo[], const char usuarioActivo[], const char rolActivo[], int esAdmin) {
    int opcion;

    do {
        uiLimpiarPantalla();
        uiMostrarBannerPrincipal();
        uiMostrarBarraSesion(usuarioActivo, rolActivo);
        uiTituloSeccion("GESTION DE INVENTARIO (VIDEOJUEGOS)");

        printf("  %s[1]%s Agregar Videojuego\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[2]%s Modificar Videojuego / Actualizar Stock\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[3]%s Baja Logica de Videojuego\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[4]%s Ver Listado General de Videojuegos\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[5]%s Ver Videojuegos por Plataforma\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[6]%s Ver Info Detallada por ID\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[0]%s Volver al Menu Principal\n", CLR_SUBTIT, CLR_RESET);
        printf("%s", CLR_TENUE);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);

        opcion = uiLeerEnteroEnRango("  Seleccione una opcion > ", 0, 6);

        switch (opcion) {
            case 1:
                videojuegoAltaInteractiva(nombreArchivo);
                uiPausar();
                break;
            case 2:
                videojuegoModificarInteractiva(nombreArchivo, esAdmin);
                uiPausar();
                break;
            case 3:
                videojuegoBajaInteractiva(nombreArchivo, esAdmin);
                uiPausar();
                break;
            case 4:
                videojuegoListarInteractivo(nombreArchivo);
                uiPausar();
                break;
            case 5:
                videojuegoFiltrarPlataformaInteractivo(nombreArchivo, esAdmin);
                uiPausar();
                break;
            case 6:
                videojuegoVerDetalleInteractivo(nombreArchivo, esAdmin);
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

/* Cuenta cuantos videojuegos activos poseen stock <= STOCK_CRITICO (Req. 3m) */
int videojuegoContarStockCritico(const char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Videojuego aux;
    int cant = 0;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO && aux.stock <= STOCK_CRITICO) {
            cant++;
        }
    }

    fclose(archi);
    return cant;
}

/* Carga en arreglo dinamico de tamanio justo los juegos activos en stock critico (Req. 3m) */
Videojuego* videojuegoCargarStockCritico(const char nombreArchivo[], int *validos) {
    FILE *archi;
    Videojuego *arr = NULL;
    Videojuego aux;
    int cant = videojuegoContarStockCritico(nombreArchivo);
    int i = 0;

    *validos = 0;
    if (cant == 0) {
        return NULL;
    }

    arr = (Videojuego *)malloc((size_t)cant * sizeof(Videojuego));
    if (arr == NULL) {
        uiMensajeError("No se pudo asignar memoria dinamica para el reporte de stock critico.");
        return NULL;
    }

    archi = fopen(nombreArchivo, "rb");
    if (archi == NULL) {
        free(arr);
        return NULL;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1 && i < cant) {
        if (aux.activo == JUEGO_ACTIVO && aux.stock <= STOCK_CRITICO) {
            arr[i] = aux;
            i++;
        }
    }

    fclose(archi);
    *validos = i;
    return arr;
}

/* Ordena un arreglo de videojuegos por stock creciente moviendo estructuras completas */
void videojuegoOrdenarPorStockCreciente(Videojuego arr[], int validos) {
    int i;
    int j;
    int posMenor;
    Videojuego aux;

    for (i = 0; i < validos - 1; i++) {
        posMenor = i;
        for (j = i + 1; j < validos; j++) {
            if (arr[j].stock < arr[posMenor].stock) {
                posMenor = j;
            }
        }

        if (posMenor != i) {
            aux = arr[i];
            arr[i] = arr[posMenor];
            arr[posMenor] = aux;
        }
    }
}

/* Coordina la carga exacta, ordenamiento y muestra del reporte de stock critico (Req. 3m) */
void videojuegoReporteStockCritico(const char nombreArchivo[]) {
    Videojuego *criticos = NULL;
    int validos = 0;
    int i;
    const char *colorStock;
    const char *etiqueta;

    uiTituloSeccion("REPORTE DE STOCK CRITICO (STOCK <= 5, ORDEN CRECIENTE)");
    criticos = videojuegoCargarStockCritico(nombreArchivo, &validos);

    if (criticos == NULL || validos == 0) {
        uiMensajeExito("No hay videojuegos en estado de stock critico.");
        if (criticos != NULL) {
            free(criticos);
            criticos = NULL;
        }
        return;
    }

    videojuegoOrdenarPorStockCreciente(criticos, validos);

    printf("%s", CLR_TITULO);
    printf("+------+----------------------------+----------------+--------+-----------+\n");
    printf("| ID   | Titulo                     | Plataforma     |  Stock | Nivel     |\n");
    printf("+------+----------------------------+----------------+--------+-----------+\n");
    printf("%s", CLR_RESET);

    for (i = 0; i < validos; i++) {
        if (criticos[i].stock == 0) {
            colorStock = CLR_ERROR;
            etiqueta = "AGOTADO";
        } else {
            colorStock = CLR_ALERTA;
            etiqueta = "CRITICO";
        }

        printf("| %-4d | %-26.26s | %-14.14s | %s%6d%s | %s%-9.9s%s |\n",
               criticos[i].idJuego,
               criticos[i].titulo,
               criticos[i].plataforma,
               colorStock, criticos[i].stock, CLR_RESET,
               colorStock, etiqueta, CLR_RESET);
    }

    printf("%s", CLR_TITULO);
    printf("+------+----------------------------+----------------+--------+-----------+\n");
    printf("%s", CLR_RESET);
    printf("  Total de videojuegos en stock critico: %d (memoria exacta reservada)\n", validos);

    free(criticos);
    criticos = NULL;
}
