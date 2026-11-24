#include "seed.h"
#include "usuario.h"
#include "persona.h"
#include "videojuego.h"
#include "transaccion.h"
#include <stdio.h>

/* Verifica si un archivo binario ya existe en disco sin modificarlo */
int seedArchivoExiste(char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");

    if (archi != NULL) {
        fclose(archi);
        return 1;
    }

    return 0;
}

/* Crea personas.bin con 7 registros para probar realloc y bajas logicas */
void seedCrearPersonas(void) {
    FILE *archi;
    Persona lote[7] = {
        {"30111222", "Martina Gomez",   "2235111222", "Av. Colon 1420",     ROL_ADMIN,    PERSONA_ACTIVA},
        {"35444555", "Lucas Fernandez", "2235444555", "San Martin 2310",    ROL_EMPLEADO, PERSONA_ACTIVA},
        {"40123456", "Sofia Alvarez",   "2236001122", "Belgrano 3100",      ROL_CLIENTE,  PERSONA_ACTIVA},
        {"38987654", "Tomas Pereyra",   "2236334455", "Luro 4520",          ROL_CLIENTE,  PERSONA_ACTIVA},
        {"42555666", "Valentina Ruiz",  "2236778899", "Independencia 1890", ROL_CLIENTE,  PERSONA_ACTIVA},
        {"36222333", "Mateo Silva",     "2235998877", "Guemes 2740",        ROL_CLIENTE,  PERSONA_ACTIVA},
        {"33777888", "Diego Morales",   "2235667788", "Rivadavia 3900",     ROL_CLIENTE,  PERSONA_INACTIVA}
    };

    if (seedArchivoExiste(ARCHIVO_PERSONAS)) {
        return;
    }

    archi = fopen(ARCHIVO_PERSONAS, "wb");
    if (archi != NULL) {
        fwrite(lote, sizeof(Persona), 7, archi);
        fclose(archi);
    }
}

/* Crea usuarios.bin con 1 administrador y 1 empleado */
void seedCrearUsuarios(void) {
    FILE *archi;
    Usuario lote[2] = {
        {
            "admin",
            "admin123",
            {"30111222", "Martina Gomez", "2235111222", "Av. Colon 1420", ROL_ADMIN, PERSONA_ACTIVA},
            USUARIO_ACTIVO
        },
        {
            "vendedor",
            "venta123",
            {"35444555", "Lucas Fernandez", "2235444555", "San Martin 2310", ROL_EMPLEADO, PERSONA_ACTIVA},
            USUARIO_ACTIVO
        }
    };

    if (seedArchivoExiste(ARCHIVO_USUARIOS)) {
        return;
    }

    archi = fopen(ARCHIVO_USUARIOS, "wb");
    if (archi != NULL) {
        fwrite(lote, sizeof(Usuario), 2, archi);
        fclose(archi);
    }
}

/* Crea videojuegos.bin con 8 registros cubriendo plataformas, stock critico, stock 0 e inactivo */
void seedCrearVideojuegos(void) {
    FILE *archi;
    Videojuego lote[8] = {
        {1, "God of War Ragnarok",        "PS5",           "Accion",       12, 42000.00f, JUEGO_ACTIVO},
        {2, "Elden Ring",                 "PC",            "RPG",           4, 35000.00f, JUEGO_ACTIVO},
        {3, "Zelda Tears of the Kingdom", "Switch",        "Aventura",      1, 48000.00f, JUEGO_ACTIVO},
        {4, "Halo Infinite",              "Xbox Series X", "Shooter",       3, 30000.00f, JUEGO_ACTIVO},
        {5, "Spider-Man 2",               "PS5",           "Accion",        9, 45000.00f, JUEGO_ACTIVO},
        {6, "Hollow Knight",              "PC",            "Metroidvania",  0, 12000.00f, JUEGO_ACTIVO},
        {7, "Mario Kart 8 Deluxe",        "Switch",        "Carreras",     14, 38000.00f, JUEGO_ACTIVO},
        {8, "Cyberpunk 2077",             "PS5",           "RPG",           2, 28000.00f, JUEGO_INACTIVO}
    };

    if (seedArchivoExiste(ARCHIVO_VIDEOJUEGOS)) {
        return;
    }

    archi = fopen(ARCHIVO_VIDEOJUEGOS, "wb");
    if (archi != NULL) {
        fwrite(lote, sizeof(Videojuego), 8, archi);
        fclose(archi);
    }
}

/* Crea transacciones.bin con 4 ventas coherentes en distintos meses y anios */
void seedCrearTransacciones(void) {
    FILE *archi;
    Transaccion lote[4] = {
        {1, {10, 11, 2026}, 1, 2, 65000.00f, 46000.00f, "40123456", "35444555"},
        {2, {18, 11, 2026}, 3, 1, 72000.00f, 24000.00f, "38987654", "30111222"},
        {3, {25, 10, 2026}, 5, 3, 68000.00f, 69000.00f, "42555666", "35444555"},
        {4, {14, 11, 2025}, 2, 1, 52000.00f, 17000.00f, "36222333", "30111222"}
    };

    if (seedArchivoExiste(ARCHIVO_TRANSACCIONES)) {
        return;
    }

    archi = fopen(ARCHIVO_TRANSACCIONES, "wb");
    if (archi != NULL) {
        fwrite(lote, sizeof(Transaccion), 4, archi);
        fclose(archi);
    }
}

/* Inicializa los 4 archivos binarios base respetando archivos ya existentes */
void seedInicializarArchivosSiNoExisten(void) {
    seedCrearPersonas();
    seedCrearUsuarios();
    seedCrearVideojuegos();
    seedCrearTransacciones();
}
