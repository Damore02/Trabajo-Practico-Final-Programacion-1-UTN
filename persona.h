#ifndef PERSONA_H
#define PERSONA_H

#define ARCHIVO_PERSONAS  "personas.bin"
#define ROL_ADMIN         "admin"
#define ROL_EMPLEADO      "empleado"
#define ROL_CLIENTE       "cliente"
#define PERSONA_ACTIVA    1
#define PERSONA_INACTIVA  0
#define BLOQUE_PERSONAS   5

typedef struct stPersona {
    char dni[15];
    char nombre[50];
    char telefono[20];
    char direccion[30];
    char rol[15];
    int activo;
} Persona;

/* Funciones base de Persona (Fase 1) */
long personaContarRegistrosArchivo(const char nombreArchivo[]);
void personaMostrarResumen(Persona p);
void personaMostrarDetalle(Persona p);

/* ABM, busqueda y carga dinamica con realloc (Fase 2) */
int personaBuscarPorDni(const char nombreArchivo[], const char dniBuscado[], Persona *encontrada);
int personaAgregar(const char nombreArchivo[], Persona nueva);
int personaModificar(const char nombreArchivo[], const char dniBuscado[], Persona datosActualizados);
int personaBajaLogica(const char nombreArchivo[], const char dniBuscado[]);
void personaCargarTodasRealloc(const char nombreArchivo[], Persona **arr, int *validos, int *dim);
void personaMenuGestion(const char nombreArchivo[], const char usuarioActivo[], const char rolActivo[]);

#endif
