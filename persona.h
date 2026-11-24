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

/* Funciones base de Persona */
int personaContarRegistrosArchivo(char nombreArchivo[]);
void personaMostrarResumen(Persona p);
void personaMostrarDetalle(Persona p);

/* ABM, busqueda y carga dinamica con realloc */
int personaExisteDniEnArchivo(char nombreArchivo[], char dniBuscado[]);
int personaBuscarPorDni(char nombreArchivo[], char dniBuscado[], Persona *encontrada);
int personaAgregar(char nombreArchivo[], Persona nueva);
int personaModificar(char nombreArchivo[], char dniBuscado[], Persona datosActualizados);
int personaBajaLogica(char nombreArchivo[], char dniBuscado[]);
void personaCargarTodasRealloc(char nombreArchivo[], Persona **arr, int *validos, int *dim);

/* Interfaz interactiva de Persona */
void personaSeleccionarRol(char destinoRol[]);
void personaAltaInteractiva(char nombreArchivo[], int esAdmin);
void personaModificarInteractiva(char nombreArchivo[], int esAdmin);
void personaBajaInteractiva(char nombreArchivo[]);
void personaListarInteractivo(char nombreArchivo[]);
void personaVerDetalleInteractivo(char nombreArchivo[]);
void personaMenuGestion(char nombreArchivo[], char usuarioActivo[], char rolActivo[], int esAdmin);

#endif
