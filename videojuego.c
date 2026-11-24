#include "videojuego.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Cuenta la cantidad total de registros Videojuego almacenados en el archivo binario */
int videojuegoContarRegistrosArchivo(char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    int bytes = 0;
    int tam = sizeof(Videojuego);

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

/* Imprime una fila resumen del videojuego destacando stock critico */
void videojuegoMostrarResumen(Videojuego v) {
    char colorStock[15];

    if (v.stock == 0) {
        strcpy(colorStock, CLR_ERROR);
    } else if (v.stock <= STOCK_CRITICO) {
        strcpy(colorStock, CLR_ALERTA);
    } else {
        strcpy(colorStock, CLR_OK);
    }

    printf("| %-4d | %-32.32s | %-16.16s | %s%6d%s |\n",
           v.idJuego, v.titulo, v.plataforma, colorStock, v.stock, CLR_RESET);
}

/* Imprime la ficha detallada de un videojuego en tarjeta ASCII con indicadores */
void videojuegoMostrarDetalle(Videojuego v, int mostrarPrecioCosto) {
    char estado[15];
    char colorEstado[15];
    char badgeStock[20];
    char colorStock[15];

    if (v.activo == JUEGO_ACTIVO) {
        strcpy(estado, "[ACTIVO]");
        strcpy(colorEstado, CLR_OK);
    } else {
        strcpy(estado, "[INACTIVO]");
        strcpy(colorEstado, CLR_ERROR);
    }

    if (v.stock == 0) {
        strcpy(badgeStock, "[AGOTADO]");
        strcpy(colorStock, CLR_ERROR);
    } else if (v.stock <= STOCK_CRITICO) {
        strcpy(badgeStock, "[STOCK CRITICO]");
        strcpy(colorStock, CLR_ALERTA);
    } else {
        strcpy(badgeStock, "[DISPONIBLE]");
        strcpy(colorStock, CLR_OK);
    }

    uiAbrirCaja("FICHA DE PRODUCTO - INVENTARIO PIXEL PLUS");
    printf("%s|%s  ID #%04d  |  %s%-57.57s%s  %s|%s\n",
           CLR_TITULO, CLR_RESET, v.idJuego, CLR_SUBTIT, v.titulo, CLR_RESET, CLR_TITULO, CLR_RESET);
    uiSeparadorCaja();
    printf("%s|%s  Plataforma : %-20.20s  Genero : %-26.26s  %s|%s\n",
           CLR_TITULO, CLR_RESET, v.plataforma, v.genero, CLR_TITULO, CLR_RESET);
    printf("%s|%s  Stock      : %s%-4d unid. %-15.15s%s  Estado : %s%-20.20s%s  %s|%s\n",
           CLR_TITULO, CLR_RESET, colorStock, v.stock, badgeStock, CLR_RESET,
           colorEstado, estado, CLR_RESET, CLR_TITULO, CLR_RESET);
    uiSeparadorCaja();

    if (mostrarPrecioCosto) {
        printf("%s|%s  Precio de adquisicion (Admin) : %s$%-37.2f%s  %s|%s\n",
               CLR_TITULO, CLR_RESET, CLR_SUBTIT, v.precioAdquisicion, CLR_RESET, CLR_TITULO, CLR_RESET);
    } else {
        printf("%s|%s  Precio de adquisicion : %s%-46.46s%s  %s|%s\n",
               CLR_TITULO, CLR_RESET, CLR_TENUE, "(Restringido a rol administrativo)", CLR_RESET,
               CLR_TITULO, CLR_RESET);
    }
    uiCerrarCaja();
}

/* Compara dos cadenas sin distinguir mayusculas de minusculas */
int videojuegoTextoIgualSinCase(char a[], char b[]) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (tolower(a[i]) != tolower(b[i])) {
            return 0;
        }
        i++;
    }

    return (a[i] == '\0' && b[i] == '\0');
}

/* Verifica si un idJuego ya existe en el archivo (activo o inactivo) */
int videojuegoExisteIdEnArchivo(char nombreArchivo[], int idBuscado) {
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
int videojuegoContarActivos(char nombreArchivo[]) {
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
int videojuegoGenerarNuevoId(char nombreArchivo[]) {
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
int videojuegoBuscarPorId(char nombreArchivo[], int idBuscado, Videojuego *encontrado) {
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
int videojuegoAgregar(char nombreArchivo[], Videojuego nuevo) {
    FILE *archi;
    int escritos = 0;

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
int videojuegoModificar(char nombreArchivo[], int idBuscado, Videojuego actualizado) {
    FILE *archi = fopen(nombreArchivo, "r+b");
    Videojuego aux;
    int tam = sizeof(Videojuego);

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO && aux.idJuego == idBuscado) {
            actualizado.idJuego = aux.idJuego;
            actualizado.activo = JUEGO_ACTIVO;
            fseek(archi, -tam, SEEK_CUR);
            fwrite(&actualizado, sizeof(Videojuego), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Realiza la baja logica (activo = 0) de un videojuego activo en modo r+b */
int videojuegoBajaLogica(char nombreArchivo[], int idBuscado) {
    FILE *archi = fopen(nombreArchivo, "r+b");
    Videojuego aux;
    int tam = sizeof(Videojuego);

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Videojuego), 1, archi) == 1) {
        if (aux.activo == JUEGO_ACTIVO && aux.idJuego == idBuscado) {
            aux.activo = JUEGO_INACTIVO;
            fseek(archi, -tam, SEEK_CUR);
            fwrite(&aux, sizeof(Videojuego), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Descuenta stock de un juego activo en r+b (puente para registro de ventas) */
int videojuegoDescontarStock(char nombreArchivo[], int idJuego, int cantidadVendida) {
    FILE *archi;
    Videojuego aux;
    int tam = sizeof(Videojuego);

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
            fseek(archi, -tam, SEEK_CUR);
            fwrite(&aux, sizeof(Videojuego), 1, archi);
            fclose(archi);
            return 1;
        }
    }

    fclose(archi);
    return 0;
}

/* Carga todos los videojuegos activos en un arreglo dinamico de tamanio exacto */
Videojuego* videojuegoCargarActivos(char nombreArchivo[], int *validos) {
    FILE *archi;
    Videojuego *arr = NULL;
    Videojuego aux;
    int cantActivos = videojuegoContarActivos(nombreArchivo);
    int i = 0;

    *validos = 0;
    if (cantActivos == 0) {
        return NULL;
    }

    arr = (Videojuego *)malloc(cantActivos * sizeof(Videojuego));
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
void videojuegoMostrarArregloRec(Videojuego arr[], int validos, int pos) {
    if (pos >= validos) {
        return;
    }

    videojuegoMostrarResumen(arr[pos]);
    videojuegoMostrarArregloRec(arr, validos, pos + 1);
}

/* Cuenta cuantos videojuegos activos pertenecen a una plataforma especifica (Req. 3f) */
int videojuegoContarPorPlataforma(char nombreArchivo[], char plataforma[]) {
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
Videojuego* videojuegoFiltrarPorPlataforma(char nombreArchivo[], char plataforma[], int *validos) {
    FILE *archi;
    Videojuego *arr = NULL;
    Videojuego aux;
    int cant = videojuegoContarPorPlataforma(nombreArchivo, plataforma);
    int i = 0;

    *validos = 0;
    if (cant == 0) {
        return NULL;
    }

    arr = (Videojuego *)malloc(cant * sizeof(Videojuego));
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

/* Selector rapido de plataforma con opcion personalizada */
void videojuegoSeleccionarPlataforma(char destinoPlataforma[]) {
    int op;

    printf("  Seleccione la plataforma:\n");
    printf("    %s[1]%s PS5            %s[3]%s PC\n", CLR_SUBTIT, CLR_RESET, CLR_SUBTIT, CLR_RESET);
    printf("    %s[2]%s Xbox Series X  %s[4]%s Switch\n", CLR_SUBTIT, CLR_RESET, CLR_SUBTIT, CLR_RESET);
    printf("    %s[5]%s Otra plataforma (escribir manualmente)\n", CLR_SUBTIT, CLR_RESET);
    op = uiLeerEnteroEnRango("  Plataforma > ", 1, 5);

    if (op == 1) {
        strcpy(destinoPlataforma, "PS5");
    } else if (op == 2) {
        strcpy(destinoPlataforma, "Xbox Series X");
    } else if (op == 3) {
        strcpy(destinoPlataforma, "PC");
    } else if (op == 4) {
        strcpy(destinoPlataforma, "Switch");
    } else {
        uiLeerTexto("  Nombre de plataforma: ", destinoPlataforma, 20);
    }
}

/* Flujo interactivo guiado con vista previa para alta de videojuego */
void videojuegoAltaInteractiva(char nombreArchivo[]) {
    Videojuego nuevo;
    int idSugerido = videojuegoGenerarNuevoId(nombreArchivo);

    uiAbrirCaja("ALTA DE NUEVO VIDEOJUEGO - FORMULARIO GUIADO");
    printf("  %sPaso 1/6 - Identificador Unico%s\n", CLR_TITULO, CLR_RESET);
    printf("  ID sugerido automaticamente: %s#%04d%s\n", CLR_SUBTIT, idSugerido, CLR_RESET);

    if (uiConfirmar("Desea utilizar el ID automatico sugerido?")) {
        nuevo.idJuego = idSugerido;
    } else {
        nuevo.idJuego = uiLeerEnteroEnRango("  Ingrese ID unico del videojuego (1-99999): ", 1, 99999);
        if (videojuegoExisteIdEnArchivo(nombreArchivo, nuevo.idJuego)) {
            uiMensajeError("Ya existe un videojuego registrado con ese ID.");
            return;
        }
    }

    printf("\n  %sPaso 2/6 - Titulo del Producto%s\n", CLR_TITULO, CLR_RESET);
    uiLeerTexto("  Titulo             : ", nuevo.titulo, 50);

    printf("\n  %sPaso 3/6 - Plataforma%s\n", CLR_TITULO, CLR_RESET);
    videojuegoSeleccionarPlataforma(nuevo.plataforma);

    printf("\n  %sPaso 4/6 - Genero%s\n", CLR_TITULO, CLR_RESET);
    uiLeerTexto("  Genero (ej Accion) : ", nuevo.genero, 20);

    printf("\n  %sPaso 5/6 - Stock Inicial%s\n", CLR_TITULO, CLR_RESET);
    nuevo.stock = uiLeerEnteroEnRango("  Unidades en stock  : ", 0, 100000);

    printf("\n  %sPaso 6/6 - Costo de Adquisicion%s\n", CLR_TITULO, CLR_RESET);
    nuevo.precioAdquisicion = uiLeerFloatMinimo("  Precio adquisicion : $", 0.0f);
    nuevo.activo = JUEGO_ACTIVO;

    printf("\n  %sVISTA PREVIA DEL PRODUCTO ANTES DE GUARDAR:%s", CLR_ALERTA, CLR_RESET);
    videojuegoMostrarDetalle(nuevo, 1);

    if (uiConfirmar("Confirma el alta de este videojuego en inventario?")) {
        if (videojuegoAgregar(nombreArchivo, nuevo)) {
            uiMensajeExito("Videojuego agregado correctamente al inventario.");
        } else {
            uiMensajeError("No se pudo agregar el videojuego.");
        }
    } else {
        uiMensajeAlerta("Alta cancelada por el usuario.");
    }
}

/* Flujo interactivo con menu por campo y vista en vivo para modificar un juego  */
void videojuegoModificarInteractiva(char nombreArchivo[], int esAdmin) {
    int idBuscado;
    Videojuego actual;
    int opcion;
    int maxOpcion;
    int huboCambios = 0;

    uiTituloSeccion("MODIFICAR VIDEOJUEGO / ACTUALIZAR STOCK");
    idBuscado = uiLeerEnteroEnRango("  Ingrese ID del videojuego a modificar: ", 1, 99999);

    if (!videojuegoBuscarPorId(nombreArchivo, idBuscado, &actual)) {
        uiMensajeError("No se encontro ningun videojuego activo con ese ID.");
        return;
    }

    maxOpcion = esAdmin ? 6 : 5;

    do {
        uiLimpiarPantalla();
        uiMostrarBannerPrincipal();
        printf("\n  %sEDITOR INTERACTIVO DE PRODUCTO (ID #%04d)%s", CLR_SUBTIT, actual.idJuego, CLR_RESET);
        videojuegoMostrarDetalle(actual, esAdmin);

        printf("\n%s", CLR_TITULO);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);
        printf("  %s[1]%s Editar Titulo          %s[4]%s Actualizar Stock\n",
               CLR_SUBTIT, CLR_RESET, CLR_SUBTIT, CLR_RESET);
        printf("  %s[2]%s Cambiar Plataforma     ", CLR_SUBTIT, CLR_RESET);
        if (esAdmin) {
            printf("%s[5]%s Editar Precio Adquisicion\n", CLR_SUBTIT, CLR_RESET);
            printf("  %s[3]%s Editar Genero          %s[6]%s %sGuardar Cambios%s\n",
                   CLR_SUBTIT, CLR_RESET, CLR_SUBTIT, CLR_RESET, CLR_OK, CLR_RESET);
        } else {
            printf("%s[5]%s %sGuardar Cambios%s\n", CLR_SUBTIT, CLR_RESET, CLR_OK, CLR_RESET);
            printf("  %s[3]%s Editar Genero\n", CLR_SUBTIT, CLR_RESET);
        }
        printf("  %s[0]%s Cancelar sin Guardar\n", CLR_SUBTIT, CLR_RESET);
        printf("%s", CLR_TENUE);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);

        opcion = uiLeerEnteroEnRango("  Que campo desea modificar? > ", 0, maxOpcion);

        if (opcion == 1) {
            uiLeerTexto("  Nuevo titulo : ", actual.titulo, 50);
            huboCambios = 1;
        } else if (opcion == 2) {
            videojuegoSeleccionarPlataforma(actual.plataforma);
            huboCambios = 1;
        } else if (opcion == 3) {
            uiLeerTexto("  Nuevo genero : ", actual.genero, 20);
            huboCambios = 1;
        } else if (opcion == 4) {
            actual.stock = uiLeerEnteroEnRango("  Nuevo stock  : ", 0, 100000);
            huboCambios = 1;
        } else if (esAdmin && opcion == 5) {
            actual.precioAdquisicion = uiLeerFloatMinimo("  Nuevo costo  : $", 0.0f);
            huboCambios = 1;
        } else if (opcion == maxOpcion) {
            if (huboCambios) {
                if (videojuegoModificar(nombreArchivo, idBuscado, actual)) {
                    uiMensajeExito("Videojuego actualizado correctamente en videojuegos.bin.");
                } else {
                    uiMensajeError("No se pudo actualizar el videojuego.");
                }
            } else {
                uiMensajeAlerta("No se realizaron cambios sobre el videojuego.");
            }
            opcion = 0;
        } else if (opcion == 0 && huboCambios) {
            uiMensajeAlerta("Cambios descartados. No se modifico videojuegos.bin.");
        }
    } while (opcion != 0);
}

/* Flujo interactivo para baja logica de videojuego */
void videojuegoBajaInteractiva(char nombreArchivo[], int esAdmin) {
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

/* Muestra el listado general de videojuegos usando recursividad*/
void videojuegoListarInteractivo(char nombreArchivo[]) {
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

/* Filtra videojuegos por plataforma usando malloc de tamanio justo */
void videojuegoFiltrarPlataformaInteractivo(char nombreArchivo[], int esAdmin) {
    char plataforma[20];
    Videojuego *filtrados = NULL;
    int validos = 0;
    int i;

    uiTituloSeccion("BUSCAR VIDEOJUEGOS POR PLATAFORMA");
    uiLeerTexto("  Plataforma (PS5 / PC / Switch / Xbox Series X): ", plataforma, 20);

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

/* Muestra la ficha detallada de un videojuego aplicando control de rol  */
void videojuegoVerDetalleInteractivo(char nombreArchivo[], int esAdmin) {
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

/* Submenu interactivo de gestion de inventario con control de permisos por rol */
void videojuegoMenuGestion(char nombreArchivo[], char usuarioActivo[], char rolActivo[], int esAdmin) {
    int opcion;

    do {
        uiLimpiarPantalla();
        uiMostrarBannerPrincipal();
        uiMostrarBarraSesion(usuarioActivo, rolActivo);
        uiTituloSeccion("GESTION DE INVENTARIO (VIDEOJUEGOS)");

        if (esAdmin) {
            printf("  %s[1]%s Agregar Videojuego\n", CLR_SUBTIT, CLR_RESET);
            printf("  %s[2]%s Modificar Videojuego / Actualizar Stock\n", CLR_SUBTIT, CLR_RESET);
            printf("  %s[3]%s Baja Logica de Videojuego\n", CLR_SUBTIT, CLR_RESET);
        } else {
            printf("  %s[1] Agregar Videojuego (Solo Admin)%s\n", CLR_TENUE, CLR_RESET);
            printf("  %s[2] Modificar Videojuego / Actualizar Stock (Solo Admin)%s\n", CLR_TENUE, CLR_RESET);
            printf("  %s[3] Baja Logica de Videojuego (Solo Admin)%s\n", CLR_TENUE, CLR_RESET);
        }
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
                if (esAdmin) {
                    videojuegoAltaInteractiva(nombreArchivo);
                } else {
                    uiMensajeError("Acceso restringido. Solo administradores pueden agregar videojuegos.");
                }
                uiPausar();
                break;
            case 2:
                if (esAdmin) {
                    videojuegoModificarInteractiva(nombreArchivo, esAdmin);
                } else {
                    uiMensajeError("Acceso restringido. Solo administradores pueden modificar el inventario.");
                }
                uiPausar();
                break;
            case 3:
                if (esAdmin) {
                    videojuegoBajaInteractiva(nombreArchivo, esAdmin);
                } else {
                    uiMensajeError("Acceso restringido. Solo administradores pueden dar de baja videojuegos.");
                }
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

/* Cuenta cuantos videojuegos activos poseen stock <= STOCK_CRITICO  */
int videojuegoContarStockCritico(char nombreArchivo[]) {
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

/* Carga en arreglo dinamico de tamanio justo los juegos activos en stock critico */
Videojuego* videojuegoCargarStockCritico(char nombreArchivo[], int *validos) {
    FILE *archi;
    Videojuego *arr = NULL;
    Videojuego aux;
    int cant = videojuegoContarStockCritico(nombreArchivo);
    int i = 0;

    *validos = 0;
    if (cant == 0) {
        return NULL;
    }

    arr = (Videojuego *)malloc(cant * sizeof(Videojuego));
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

/* Coordina la carga exacta, ordenamiento y muestra del reporte de stock critico  */
void videojuegoReporteStockCritico(char nombreArchivo[]) {
    Videojuego *criticos = NULL;
    int validos = 0;
    int i;
    char colorStock[15];
    char etiqueta[15];

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
            strcpy(colorStock, CLR_ERROR);
            strcpy(etiqueta, "AGOTADO");
        } else {
            strcpy(colorStock, CLR_ALERTA);
            strcpy(etiqueta, "CRITICO");
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
