#ifndef SEED_H
#define SEED_H

/* Genera los 4 archivos binarios iniciales unicamente si no existen en disco */
int seedArchivoExiste(char nombreArchivo[]);
void seedCrearPersonas(void);
void seedCrearUsuarios(void);
void seedCrearVideojuegos(void);
void seedCrearTransacciones(void);
void seedInicializarArchivosSiNoExisten(void);

#endif
