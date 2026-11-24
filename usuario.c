#include "usuario.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>

/* Cuenta la cantidad total de registros Usuario almacenados en el archivo binario */
long usuarioContarRegistrosArchivo(const char nombreArchivo[]) {
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

    return bytes / (long)sizeof(Usuario);
}

/* Retorna 1 si el usuario posee rol administrativo, 0 en caso contrario */
int usuarioEsAdmin(Usuario u) {
    return (strcmp(u.persona.rol, ROL_ADMIN) == 0);
}

/* Imprime una fila resumen del usuario sin exponer la contrasenia */
void usuarioMostrarResumen(Usuario u) {
    const char *estado = (u.activo == USUARIO_ACTIVO) ? "Activo" : "Inactivo";
    const char *colorEstado = (u.activo == USUARIO_ACTIVO) ? CLR_OK : CLR_ERROR;

    printf("| %-14.14s | %-24.24s | %-10.10s | %s%-10.10s%s |\n",
           u.usuario, u.persona.nombre, u.persona.rol, colorEstado, estado, CLR_RESET);
}

/* Cuenta la cantidad de usuarios activos existentes en el archivo binario */
static int usuarioContarActivos(const char nombreArchivo[]) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Usuario aux;
    int activos = 0;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Usuario), 1, archi) == 1) {
        if (aux.activo == USUARIO_ACTIVO) {
            activos++;
        }
    }

    fclose(archi);
    return activos;
}

/* Busca un usuario activo por nombre de usuario exacto */
int usuarioBuscarPorNombre(const char nombreArchivo[], const char nombreUsuario[], Usuario *encontrado) {
    FILE *archi = fopen(nombreArchivo, "rb");
    Usuario aux;

    if (archi == NULL) {
        return 0;
    }

    while (fread(&aux, sizeof(Usuario), 1, archi) == 1) {
        if (aux.activo == USUARIO_ACTIVO && strcmp(aux.usuario, nombreUsuario) == 0) {
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

/* Crea un administrador inicial cuando no existen usuarios activos en disco */
static int usuarioCrearAdminInicial(const char nombreArchivo[]) {
    FILE *archi;
    Usuario admin;
    size_t escritos;

    uiMensajeAlerta("No existen usuarios activos registrados en el sistema.");
    uiTituloSeccion("CONFIGURACION INICIAL - ALTA DE ADMINISTRADOR");

    uiLeerTexto("  Nombre de usuario : ", admin.usuario, (int)sizeof(admin.usuario));
    uiLeerTexto("  Contrasenia       : ", admin.contrasenia, (int)sizeof(admin.contrasenia));
    uiLeerTexto("  DNI               : ", admin.persona.dni, (int)sizeof(admin.persona.dni));
    uiLeerTexto("  Nombre completo   : ", admin.persona.nombre, (int)sizeof(admin.persona.nombre));
    uiLeerTexto("  Telefono          : ", admin.persona.telefono, (int)sizeof(admin.persona.telefono));
    uiLeerTexto("  Direccion         : ", admin.persona.direccion, (int)sizeof(admin.persona.direccion));
    strcpy(admin.persona.rol, ROL_ADMIN);
    admin.persona.activo = PERSONA_ACTIVA;
    admin.activo = USUARIO_ACTIVO;

    personaAgregar(ARCHIVO_PERSONAS, admin.persona);

    archi = fopen(nombreArchivo, "ab");
    if (archi == NULL) {
        uiMensajeError("No se pudo abrir el archivo de usuarios para escritura.");
        return 0;
    }

    escritos = fwrite(&admin, sizeof(Usuario), 1, archi);
    fclose(archi);

    if (escritos == 1) {
        uiMensajeExito("Usuario administrador inicial creado correctamente.");
        return 1;
    }

    return 0;
}

/* Valida credenciales contra usuarios.bin con un maximo de MAX_INTENTOS_LOGIN */
int usuarioLogin(const char nombreArchivo[], Usuario *sesionActiva) {
    int intentosRestantes = MAX_INTENTOS_LOGIN;
    int autenticado = 0;
    char userIngresado[20];
    char passIngresada[20];
    Usuario encontrado;

    if (usuarioContarActivos(nombreArchivo) == 0) {
        if (!usuarioCrearAdminInicial(nombreArchivo)) {
            return 0;
        }
    }

    uiTituloSeccion("INICIO DE SESION - PIXEL PLUS");

    while (intentosRestantes > 0 && !autenticado) {
        uiLeerTexto("  Usuario     : ", userIngresado, (int)sizeof(userIngresado));
        uiLeerTexto("  Contrasenia : ", passIngresada, (int)sizeof(passIngresada));

        if (usuarioBuscarPorNombre(nombreArchivo, userIngresado, &encontrado) &&
            strcmp(encontrado.contrasenia, passIngresada) == 0) {
            if (sesionActiva != NULL) {
                *sesionActiva = encontrado;
            }
            autenticado = 1;
            uiMensajeExito("Autenticacion exitosa. Bienvenido a Pixel Plus.");
        } else {
            intentosRestantes--;
            if (intentosRestantes > 0) {
                printf("%s[ALERTA] Credenciales incorrectas. Intentos restantes: %d%s\n",
                       CLR_ALERTA, intentosRestantes, CLR_RESET);
            }
        }
    }

    if (!autenticado) {
        uiMensajeError("Acceso denegado. Se agotaron los intentos.");
    }

    return autenticado;
}

/* Registra un nuevo usuario vinculado a un empleado o administrador activo */
int usuarioAgregar(const char archiUsuarios[], const char archiPersonas[], Usuario nuevo) {
    FILE *archi;
    Persona personaAsociada;
    size_t escritos;

    if (usuarioBuscarPorNombre(archiUsuarios, nuevo.usuario, NULL)) {
        return 0;
    }

    if (!personaBuscarPorDni(archiPersonas, nuevo.persona.dni, &personaAsociada)) {
        return 0;
    }

    if (strcmp(personaAsociada.rol, ROL_EMPLEADO) != 0 &&
        strcmp(personaAsociada.rol, ROL_ADMIN) != 0) {
        return 0;
    }

    nuevo.persona = personaAsociada;
    nuevo.activo = USUARIO_ACTIVO;

    archi = fopen(archiUsuarios, "ab");
    if (archi == NULL) {
        return 0;
    }

    escritos = fwrite(&nuevo, sizeof(Usuario), 1, archi);
    fclose(archi);

    return (escritos == 1);
}

/* Flujo interactivo para dar de alta un nuevo usuario del sistema (solo admin) */
void usuarioAltaInteractiva(const char archiUsuarios[], const char archiPersonas[]) {
    Usuario nuevo;
    Persona personaAsociada;

    uiTituloSeccion("ALTA DE USUARIO DEL SISTEMA (SOLO ADMIN)");
    uiLeerTexto("  DNI del empleado/admin registrado: ", nuevo.persona.dni, (int)sizeof(nuevo.persona.dni));

    if (!personaBuscarPorDni(archiPersonas, nuevo.persona.dni, &personaAsociada)) {
        uiMensajeError("No existe una persona activa con ese DNI en personas.bin.");
        return;
    }

    if (strcmp(personaAsociada.rol, ROL_EMPLEADO) != 0 &&
        strcmp(personaAsociada.rol, ROL_ADMIN) != 0) {
        uiMensajeError("Un cliente no puede poseer usuario del sistema. Debe ser empleado o admin.");
        return;
    }

    personaMostrarDetalle(personaAsociada);
    uiLeerTexto("  Nuevo nombre de usuario: ", nuevo.usuario, (int)sizeof(nuevo.usuario));

    if (usuarioBuscarPorNombre(archiUsuarios, nuevo.usuario, NULL)) {
        uiMensajeError("Ese nombre de usuario ya se encuentra registrado.");
        return;
    }

    uiLeerTexto("  Contrasenia            : ", nuevo.contrasenia, (int)sizeof(nuevo.contrasenia));
    nuevo.persona = personaAsociada;
    nuevo.activo = USUARIO_ACTIVO;

    if (usuarioAgregar(archiUsuarios, archiPersonas, nuevo)) {
        uiMensajeExito("Usuario registrado correctamente en usuarios.bin.");
    } else {
        uiMensajeError("No se pudo registrar el usuario.");
    }
}
