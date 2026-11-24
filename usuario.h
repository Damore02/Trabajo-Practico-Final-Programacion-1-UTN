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

/* Funciones base de Usuario */
int usuarioContarRegistrosArchivo(char nombreArchivo[]);
int usuarioContarActivos(char nombreArchivo[]);
int usuarioEsAdmin(Usuario u);
void usuarioMostrarResumen(Usuario u);

/* Autenticacion, busqueda y alta de usuarios */
int usuarioCrearAdminInicial(char nombreArchivo[]);
int usuarioLogin(char nombreArchivo[], Usuario *sesionActiva);
int usuarioBuscarPorNombre(char nombreArchivo[], char nombreUsuario[], Usuario *encontrado);
int usuarioAgregar(char archiUsuarios[], char archiPersonas[], Usuario nuevo);
void usuarioAltaInteractiva(char archiUsuarios[], char archiPersonas[]);

#endif
