#include "transaccion.h"
#include "videojuego.h"
#include "persona.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Cuenta la cantidad total de registros Transaccion almacenados en el archivo binario */
long transaccionContarRegistrosArchivo(const char nombreArchivo[]) {
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

    return bytes / (long)sizeof(Transaccion);
}

/* Imprime una fila resumen de una transaccion */
void transaccionMostrarResumen(Transaccion t) {
    float montoTotal = t.precioUnitarioVenta * (float)t.cantidadVendida;

    printf("| %-5d | ", t.idTransaccion);
    fechaMostrar(t.fecha);
    printf(" | %-8d | %6d | $%10.2f | $%10.2f |\n",
           t.idJuegoVendido, t.cantidadVendida, montoTotal, t.gananciaTotal);
}

/* Imprime la ficha completa de una transaccion incluyendo ganancia */
void transaccionMostrarDetalle(Transaccion t) {
    float montoTotal = t.precioUnitarioVenta * (float)t.cantidadVendida;

    printf("%s", CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("  %sDETALLE DE TRANSACCION #%d%s\n", CLR_SUBTIT, t.idTransaccion, CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
    printf("  ID Transaccion        : %d\n", t.idTransaccion);
    printf("  Fecha                 : ");
    fechaMostrar(t.fecha);
    printf("\n");
    printf("  ID Videojuego vendido : %d\n", t.idJuegoVendido);
    printf("  Cantidad vendida      : %d\n", t.cantidadVendida);
    printf("  Precio unitario venta : $%.2f\n", t.precioUnitarioVenta);
    printf("  Total facturado       : $%.2f\n", montoTotal);
    printf("  Ganancia total        : %s$%.2f%s\n", CLR_OK, t.gananciaTotal, CLR_RESET);
    printf("  DNI Cliente           : %s\n", t.dniCliente);
    printf("  DNI Empleado          : %s\n", t.dniEmpleado);
    printf("%s", CLR_TENUE);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
}

/* Genera el proximo idTransaccion unico a partir del maximo ID almacenado en disco */
int transaccionGenerarNuevoId(const char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Transaccion aux;
    int maxId = 0;

    if (archi == NULL) {
        return 1;
    }

    while (fread(&aux, sizeof(Transaccion), 1, archi) == 1) {
        if (aux.idTransaccion > maxId) {
            maxId = aux.idTransaccion;
        }
    }

    fclose(archi);
    return maxId + 1;
}

/* Carga todas las transacciones usando realloc por bloques y puntero doble (Req. 3h) */
void transaccionCargarTodasRealloc(const char nombreArchivo[], Transaccion **arr, int *validos, int *dim) {
    FILE *archi;
    Transaccion aux;
    Transaccion *temp;
    int nuevaDim;

    *arr = NULL;
    *validos = 0;
    *dim = 0;

    archi = fopen(nombreArchivo, "rb");
    if (archi == NULL) {
        return;
    }

    *arr = (Transaccion *)malloc((size_t)BLOQUE_TRANSACCIONES * sizeof(Transaccion));
    if (*arr == NULL) {
        fclose(archi);
        uiMensajeError("No se pudo asignar memoria dinamica para transacciones.");
        return;
    }
    *dim = BLOQUE_TRANSACCIONES;

    while (fread(&aux, sizeof(Transaccion), 1, archi) == 1) {
        if (*validos == *dim) {
            nuevaDim = *dim + BLOQUE_TRANSACCIONES;
            temp = (Transaccion *)realloc(*arr, (size_t)nuevaDim * sizeof(Transaccion));
            if (temp == NULL) {
                uiMensajeError("Error al ampliar memoria con realloc para transacciones.");
                fclose(archi);
                return;
            }
            *arr = temp;
            *dim = nuevaDim;
        }
        (*arr)[*validos] = aux;
        (*validos)++;
    }

    fclose(archi);
}

/* Muestra recursivamente el resumen de todas las transacciones (Req. 3h) */
void transaccionMostrarResumenRec(const Transaccion arr[], int validos, int pos) {
    if (pos >= validos) {
        return;
    }

    transaccionMostrarResumen(arr[pos]);
    transaccionMostrarResumenRec(arr, validos, pos + 1);
}

/* Busqueda recursiva por idTransaccion; retorna indice o -1 sin imprimir (Req. 3i) */
int transaccionBuscarPorIdRec(const Transaccion arr[], int validos, int idBuscado, int pos) {
    if (pos >= validos) {
        return -1;
    }

    if (arr[pos].idTransaccion == idBuscado) {
        return pos;
    }

    return transaccionBuscarPorIdRec(arr, validos, idBuscado, pos + 1);
}

/* Registra una venta validando integridad en 3 archivos y descontando stock (Req. 3j) */
int transaccionRegistrarVenta(const char archiTrans[], const char archiJuegos[],
                              const char archiPersonas[], Transaccion nuevaVenta) {
    Videojuego juego;
    Persona cliente;
    Persona empleado;
    FILE *archi;
    size_t escritos;

    if (nuevaVenta.cantidadVendida <= 0 || nuevaVenta.precioUnitarioVenta <= 0.0f) {
        return 0;
    }

    if (!fechaEsValida(nuevaVenta.fecha)) {
        return 0;
    }

    if (!videojuegoBuscarPorId(archiJuegos, nuevaVenta.idJuegoVendido, &juego)) {
        return 0;
    }

    if (juego.stock < nuevaVenta.cantidadVendida) {
        return 0;
    }

    if (!personaBuscarPorDni(archiPersonas, nuevaVenta.dniCliente, &cliente) ||
        strcmp(cliente.rol, ROL_CLIENTE) != 0) {
        return 0;
    }

    if (!personaBuscarPorDni(archiPersonas, nuevaVenta.dniEmpleado, &empleado) ||
        (strcmp(empleado.rol, ROL_EMPLEADO) != 0 && strcmp(empleado.rol, ROL_ADMIN) != 0)) {
        return 0;
    }

    nuevaVenta.idTransaccion = transaccionGenerarNuevoId(archiTrans);
    nuevaVenta.gananciaTotal = (nuevaVenta.precioUnitarioVenta - juego.precioAdquisicion)
                               * (float)nuevaVenta.cantidadVendida;

    if (!videojuegoDescontarStock(archiJuegos, nuevaVenta.idJuegoVendido, nuevaVenta.cantidadVendida)) {
        return 0;
    }

    archi = fopen(archiTrans, "ab");
    if (archi == NULL) {
        videojuegoModificar(archiJuegos, juego.idJuego, juego);
        return 0;
    }

    escritos = fwrite(&nuevaVenta, sizeof(Transaccion), 1, archi);
    fclose(archi);

    if (escritos != 1) {
        videojuegoModificar(archiJuegos, juego.idJuego, juego);
        return 0;
    }

    return 1;
}

/* Suma recursivamente la recaudacion total (facturacion) de un mes y anio (Req. 3k) */
float transaccionSumarRecaudacionMesRec(const Transaccion arr[], int validos, int pos, int mes, int anio) {
    float montoActual = 0.0f;

    if (pos >= validos) {
        return 0.0f;
    }

    if (fechaCoincideMesAnio(arr[pos].fecha, mes, anio)) {
        montoActual = arr[pos].precioUnitarioVenta * (float)arr[pos].cantidadVendida;
    }

    return montoActual + transaccionSumarRecaudacionMesRec(arr, validos, pos + 1, mes, anio);
}

/* Suma recursivamente la ganancia neta de un mes y anio (Auxiliar de Req. 3k) */
float transaccionSumarGananciaMesRec(const Transaccion arr[], int validos, int pos, int mes, int anio) {
    float gananciaActual = 0.0f;

    if (pos >= validos) {
        return 0.0f;
    }

    if (fechaCoincideMesAnio(arr[pos].fecha, mes, anio)) {
        gananciaActual = arr[pos].gananciaTotal;
    }

    return gananciaActual + transaccionSumarGananciaMesRec(arr, validos, pos + 1, mes, anio);
}

/* Busca recursivamente la transaccion con mayor gananciaTotal (Precondicion: validos > 0) */
Transaccion transaccionBuscarMayorGananciaRec(const Transaccion arr[], int validos, int pos) {
    Transaccion mejorResto;

    if (pos == validos - 1) {
        return arr[pos];
    }

    mejorResto = transaccionBuscarMayorGananciaRec(arr, validos, pos + 1);
    if (arr[pos].gananciaTotal > mejorResto.gananciaTotal) {
        return arr[pos];
    }

    return mejorResto;
}

/* Flujo interactivo para registrar una nueva venta con descuento en cascada (Req. 3j) */
static void transaccionRegistrarVentaInteractiva(const char archiTrans[], const char archiJuegos[],
                                                 const char archiPersonas[], const char dniEmpleadoActivo[]) {
    Transaccion nueva;
    Videojuego juego;
    Persona cliente;
    Persona empleado;
    int idEstimado;
    int stockRestante;

    uiTituloSeccion("REGISTRAR NUEVA VENTA");

    nueva.idJuegoVendido = uiLeerEnteroEnRango("  ID del videojuego a vender: ", 1, 99999);
    if (!videojuegoBuscarPorId(archiJuegos, nueva.idJuegoVendido, &juego)) {
        uiMensajeError("No existe un videojuego activo con ese ID.");
        return;
    }

    if (juego.stock <= 0) {
        uiMensajeError("Sin stock disponible para este videojuego. No se puede vender.");
        return;
    }

    printf("  Juego seleccionado : %s (%s) | Stock disponible: %d\n",
           juego.titulo, juego.plataforma, juego.stock);

    nueva.cantidadVendida = uiLeerEnteroEnRango("  Cantidad a vender         : ", 1, juego.stock);

    uiLeerTexto("  DNI del cliente comprador : ", nueva.dniCliente, (int)sizeof(nueva.dniCliente));
    if (!personaBuscarPorDni(archiPersonas, nueva.dniCliente, &cliente)) {
        uiMensajeError("No existe ninguna persona activa registrada con ese DNI.");
        return;
    }
    if (strcmp(cliente.rol, ROL_CLIENTE) != 0) {
        uiMensajeError("El DNI ingresado no corresponde a un cliente.");
        return;
    }
    printf("  Cliente validado   : %s\n", cliente.nombre);

    strncpy(nueva.dniEmpleado, dniEmpleadoActivo, sizeof(nueva.dniEmpleado) - 1);
    nueva.dniEmpleado[sizeof(nueva.dniEmpleado) - 1] = '\0';
    if (!personaBuscarPorDni(archiPersonas, nueva.dniEmpleado, &empleado)) {
        uiMensajeError("El empleado autenticado no se encuentra activo en personas.bin.");
        return;
    }

    nueva.fecha = fechaCargar("  Fecha de la venta:");
    nueva.precioUnitarioVenta = uiLeerFloatMinimo("  Precio unitario de venta ($): ", 0.01f);

    if (nueva.precioUnitarioVenta < juego.precioAdquisicion) {
        uiMensajeAlerta("El precio de venta es inferior al costo de adquisicion (venta a perdida).");
        if (!uiConfirmar("Desea confirmar igualmente esta venta?")) {
            uiMensajeAlerta("Venta cancelada por el usuario.");
            return;
        }
    }

    idEstimado = transaccionGenerarNuevoId(archiTrans);
    nueva.idTransaccion = idEstimado;
    nueva.gananciaTotal = (nueva.precioUnitarioVenta - juego.precioAdquisicion)
                          * (float)nueva.cantidadVendida;

    if (transaccionRegistrarVenta(archiTrans, archiJuegos, archiPersonas, nueva)) {
        uiMensajeExito("Venta registrada y stock actualizado en cascada correctamente.");
        transaccionMostrarDetalle(nueva);

        stockRestante = juego.stock - nueva.cantidadVendida;
        if (stockRestante <= STOCK_CRITICO) {
            printf("%s[ALERTA] Atencion: '%s' quedo en stock critico (%d unidades).%s\n",
                   CLR_ALERTA, juego.titulo, stockRestante, CLR_RESET);
        }
    } else {
        uiMensajeError("No se pudo registrar la venta. Verifique los datos ingresados.");
    }
}

/* Muestra el listado resumen de todas las transacciones usando realloc y recursividad (Req. 3h) */
static void transaccionListarResumenInteractivo(const char archiTrans[]) {
    Transaccion *lista = NULL;
    int validos = 0;
    int dim = 0;

    uiTituloSeccion("LISTADO RESUMEN DE TRANSACCIONES");
    transaccionCargarTodasRealloc(archiTrans, &lista, &validos, &dim);

    if (lista == NULL || validos == 0) {
        uiMensajeAlerta("No hay transacciones registradas todavia.");
        if (lista != NULL) {
            free(lista);
            lista = NULL;
        }
        return;
    }

    printf("%s", CLR_TITULO);
    printf("+-------+------------+----------+--------+-------------+-------------+\n");
    printf("| ID    | Fecha      | ID Juego |  Cant. | Total Venta | Ganancia    |\n");
    printf("+-------+------------+----------+--------+-------------+-------------+\n");
    printf("%s", CLR_RESET);

    transaccionMostrarResumenRec(lista, validos, 0);

    printf("%s", CLR_TITULO);
    printf("+-------+------------+----------+--------+-------------+-------------+\n");
    printf("%s", CLR_RESET);
    printf("  Total registradas: %d (Capacidad dinamica reservada: %d)\n", validos, dim);

    free(lista);
    lista = NULL;
}

/* Consulta el detalle de una transaccion por ID usando busqueda recursiva (Req. 3i) */
static void transaccionVerDetalleInteractivo(const char archiTrans[]) {
    Transaccion *lista = NULL;
    int validos = 0;
    int dim = 0;
    int idBuscado;
    int pos;

    uiTituloSeccion("CONSULTAR DETALLE DE TRANSACCION");
    transaccionCargarTodasRealloc(archiTrans, &lista, &validos, &dim);

    if (lista == NULL || validos == 0) {
        uiMensajeAlerta("No hay transacciones registradas para consultar.");
        if (lista != NULL) {
            free(lista);
            lista = NULL;
        }
        return;
    }

    idBuscado = uiLeerEnteroEnRango("  Ingrese ID de transaccion a consultar: ", 1, 99999);
    pos = transaccionBuscarPorIdRec(lista, validos, idBuscado, 0);

    if (pos != -1) {
        transaccionMostrarDetalle(lista[pos]);
    } else {
        uiMensajeError("No se encontro la transaccion consultada.");
    }

    free(lista);
    lista = NULL;
}

/* Ejecuta el reporte recursivo de recaudacion y ganancia mensual (Req. 3k) */
static void transaccionReporteRecaudacionMensual(const char archiTrans[]) {
    Transaccion *lista = NULL;
    int validos = 0;
    int dim = 0;
    int mes;
    int anio;
    float totalRecaudado;
    float totalGanancia;

    uiTituloSeccion("REPORTE DE RECAUDACION MENSUAL");
    mes = uiLeerEnteroEnRango("  Mes a consultar (1-12)     : ", 1, 12);
    anio = uiLeerEnteroEnRango("  Anio a consultar (2020-2100): ", 2020, 2100);

    transaccionCargarTodasRealloc(archiTrans, &lista, &validos, &dim);

    if (lista == NULL || validos == 0) {
        totalRecaudado = 0.0f;
        totalGanancia = 0.0f;
    } else {
        totalRecaudado = transaccionSumarRecaudacionMesRec(lista, validos, 0, mes, anio);
        totalGanancia = transaccionSumarGananciaMesRec(lista, validos, 0, mes, anio);
    }

    printf("\n%s", CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("  %sPERIODO CONSULTADO: %02d/%04d%s\n", CLR_SUBTIT, mes, anio, CLR_TITULO);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);
    printf("  Total facturado (recaudacion) : %s$%.2f%s\n", CLR_OK, totalRecaudado, CLR_RESET);
    printf("  Ganancia neta del periodo     : %s$%.2f%s\n", CLR_SUBTIT, totalGanancia, CLR_RESET);
    printf("%s", CLR_TENUE);
    uiLinea('-', ANCHO_UI);
    printf("%s", CLR_RESET);

    if (totalRecaudado == 0.0f) {
        uiMensajeAlerta("No se registraron ventas en el periodo indicado.");
    }

    if (lista != NULL) {
        free(lista);
        lista = NULL;
    }
}

/* Ejecuta el reporte recursivo de la venta con mayor ganancia (Req. 3l) */
static void transaccionReporteMayorGanancia(const char archiTrans[]) {
    Transaccion *lista = NULL;
    int validos = 0;
    int dim = 0;
    Transaccion mejorVenta;

    uiTituloSeccion("REPORTE DE VENTA CON MAYOR GANANCIA");
    transaccionCargarTodasRealloc(archiTrans, &lista, &validos, &dim);

    if (lista == NULL || validos == 0) {
        uiMensajeAlerta("No hay transacciones registradas para calcular la mayor ganancia.");
        if (lista != NULL) {
            free(lista);
            lista = NULL;
        }
        return;
    }

    mejorVenta = transaccionBuscarMayorGananciaRec(lista, validos, 0);
    uiMensajeExito("Venta de mayor ganancia historica identificada:");
    transaccionMostrarDetalle(mejorVenta);

    free(lista);
    lista = NULL;
}

/* Submenu interactivo de Ventas y Transacciones (Reqs. 3h, 3i, 3j) */
void transaccionMenuVentas(const char archiTrans[], const char archiJuegos[],
                           const char archiPersonas[], const char dniEmpleadoActivo[]) {
    int opcion;

    do {
        uiLimpiarPantalla();
        uiMostrarBannerPrincipal();
        uiTituloSeccion("VENTAS Y TRANSACCIONES");
        printf("  %sEmpleado operador (DNI): %s%s\n", CLR_TENUE, dniEmpleadoActivo, CLR_RESET);
        printf("%s", CLR_TENUE);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);

        printf("  %s[1]%s Registrar Nueva Venta\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[2]%s Ver Listado Resumen de Transacciones\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[3]%s Consultar Detalle de Transaccion por ID\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[0]%s Volver al Menu Principal\n", CLR_SUBTIT, CLR_RESET);
        printf("%s", CLR_TENUE);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);

        opcion = uiLeerEnteroEnRango("  Seleccione una opcion > ", 0, 3);

        switch (opcion) {
            case 1:
                transaccionRegistrarVentaInteractiva(archiTrans, archiJuegos, archiPersonas, dniEmpleadoActivo);
                uiPausar();
                break;
            case 2:
                transaccionListarResumenInteractivo(archiTrans);
                uiPausar();
                break;
            case 3:
                transaccionVerDetalleInteractivo(archiTrans);
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

/* Submenu interactivo de Reportes Gerenciales (Reqs. 3k, 3l, 3m) */
void transaccionMenuReportes(const char archiTrans[], const char archiJuegos[]) {
    int opcion;

    do {
        uiLimpiarPantalla();
        uiMostrarBannerPrincipal();
        uiTituloSeccion("REPORTES GERENCIALES - PIXEL PLUS");

        printf("  %s[1]%s Reporte de Recaudacion Mensual\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[2]%s Reporte de Venta con Mayor Ganancia\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[3]%s Reporte de Stock Critico (Orden Creciente)\n", CLR_SUBTIT, CLR_RESET);
        printf("  %s[0]%s Volver al Menu Principal\n", CLR_SUBTIT, CLR_RESET);
        printf("%s", CLR_TENUE);
        uiLinea('-', ANCHO_UI);
        printf("%s", CLR_RESET);

        opcion = uiLeerEnteroEnRango("  Seleccione una opcion > ", 0, 3);

        switch (opcion) {
            case 1:
                transaccionReporteRecaudacionMensual(archiTrans);
                uiPausar();
                break;
            case 2:
                transaccionReporteMayorGanancia(archiTrans);
                uiPausar();
                break;
            case 3:
                videojuegoReporteStockCritico(archiJuegos);
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
