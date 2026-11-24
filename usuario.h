#ifndef USUARIO_H
#define USUARIO_H

#include "persona.h"

#define ARCHIVO_USUARIOS    "usuarios.bin"
#define MAX_INTENTOS_LOGIN  3
#define USUARIO_ACTIVO      1
#define USUARIO_INACTIVO    0

typedef struct stUsuario {
    char usuario[20];
    char contrasenia[20];
    Persona persona;
    int activo;
} Usuario;

/* Funciones base de Usuario (Fase 1) */
long usuarioContarRegistrosArchivo(const char nombreArchivo[]);
int usuarioEsAdmin(Usuario u);
void usuarioMostrarResumen(Usuario u);

/* Autenticacion, busqueda y alta de usuarios (Fase 2) */
int usuarioLogin(const char nombreArchivo[], Usuario *sesionActiva);
int usuarioBuscarPorNombre(const char nombreArchivo[], const char nombreUsuario[], Usuario *encontrado);
int usuarioAgregar(const char archiUsuarios[], const char archiPersonas[], Usuario nuevo);
void usuarioAltaInteractiva(const char archiUsuarios[], const char archiPersonas[]);

#endif
