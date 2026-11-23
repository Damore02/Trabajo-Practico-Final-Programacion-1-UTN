#include "videojuegos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void administrarVideojuegos(const char *nombreArchivo)
{
    FILE *archivo = fopen(nombreArchivo, "r+b");
    if (archivo==NULL)
    {
        FILE *archivo = fopen (nombreArchivo, "wb");
        if (archivo == NULL)
        {
            printf("Error: No se pudo abrir o crear el archivo '%s'.\n", nombreArchivo);
            return;
        }
        fclose(archivo);
        archivo = fopen(nombreArchivo, "r+b");
    }
    int idJuegoBuscado;
    printf ("Ingrese el ID del juego que quiera modificar, si esta no existe, se creara uno nuevo: ");
    scanf ("%d", &idJuegoBuscado);

    Videojuego v;
    int encontrado=0;
    long posicionRegistro=0;

    while (fread(&v, sizeof(Videojuego), 1, archivo) == 1)
    {
        if (v.idJuego == idJuegoBuscado)
        {
            encontrado=1;
            posicionRegistro = ftell(archivo) - sizeof(Videojuego);
            break;
        }
    }
    if (encontrado==1)
    {
        printf ("El videojuego ya esta registrado.\n");
        printf ("\n");
        printf ("==================================\n");
        printf ("\n");
        printf ("Titulo: %s", v.titulo);
        printf ("Plataforma: %s", v.plataforma);
        printf ("Genero: %s", v.genero);
        printf ("Stock: %d", v.stock);
        printf ("Precio de adquisicion: %.2f", v.precioAdquisicion);
        printf ("\n");
        printf ("==================================\n");
        printf ("\n");

        v.idJuego = idJuegoBuscado;

        printf ("Nuevo Titulo: ");
        while (getchar()!= '\n');
        fgets(v.titulo, sizeof(v.titulo), stdin);
        v.titulo[strcspn(v.titulo, "\n")] = 0;
        printf("Nueva Plataforma: ");
        fgets(v.plataforma, sizeof(v.plataforma), stdin);
        v.plataforma[strcspn(v.plataforma, "\n")] = 0;
        printf("Nuevo Genero: ");
        fgets(v.genero, sizeof(v.genero), stdin);
        v.genero[strcspn(v.genero, "\n")] = 0;
        printf("Nuevo Stock: ");
        scanf("%d", &v.stock);
        printf("Nuevo Precio de Adquisicion: ");
        scanf("%f", &v.precioAdquisicion);
        fseek(archivo, posicionRegistro, SEEK_SET);
        fwrite(&v, sizeof(Videojuego), 1, archivo);
        fclose(archivo);
        printf("Videojuego modificado exitosamente!\n");
    }

    else
    {
        fclose(archivo);
        archivo = fopen(nombreArchivo, "ab");
        if (archivo == NULL)
        {
            printf("Error al abrir el archivo para agregar el nuevo registro.\n");
            return;
        }
        v.idJuego = idJuegoBuscado;
        printf ("El videojuego no existe en la base de datos. Ingrese los datos para crear un nuevo videojuego:\n");
        printf("Titulo: ");
        while (getchar() != '\n'); // Limpiar búfer antes de fgets
        fgets(v.titulo, sizeof(v.titulo), stdin);
        v.titulo[strcspn(v.titulo, "\n")] = 0;
        printf("Plataforma: ");
        fgets(v.plataforma, sizeof(v.plataforma), stdin);
        v.plataforma[strcspn(v.plataforma, "\n")] = 0;
        printf("Genero: ");
        fgets(v.genero, sizeof(v.genero), stdin);
        v.genero[strcspn(v.genero, "\n")] = 0;
        printf("Stock inicial: ");
        scanf("%d", &v.stock);
        printf("Precio de Adquisicion: ");
        scanf("%f", &v.precioAdquisicion);
        fwrite(&v, sizeof(Videojuego), 1, archivo);
        fclose(archivo);
        printf("Nuevo videojuego agregado exitosamente!\n");
    }
    return;
}

void listarVideojuegos(const char *nombreArchivo)
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    if (archivo==NULL)
    {
        printf ("Ha ocurrido un error al leer el archivo.\n");
        return;
    }
    Videojuego v;
    int contador=0;

    while (fread(&v, sizeof(Videojuego), 1, archivo))
    {
        printf ("\n");
        printf ("ID: %d\n", v.idJuego);
        printf ("Titulo: %s\n", v.titulo);
        printf ("Plataforma: %s\n", v.plataforma);
        printf ("Genero: %s\n", v.genero);
        printf ("Stock: %d\n", v.stock);
        printf ("Precio de adquisicion: %.2f\n", v.precioAdquisicion);
        printf ("\n");
        printf ("====================================================\n");
        contador++;
    }
    printf ("\n");
    if (contador==0)
    {
        printf ("El registro esta vacio.\n");
    }
    else
    {
        printf ("Total de videojuegos en la base de datos: %d\n", contador);
    }
    fclose (archivo);
    archivo=NULL;
}

void buscarPorID(const char *nombreArchivo)
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    if (archivo==NULL)
    {
        printf ("Ha ocurrido un error al leer el archivo.\n");
        return;
    }
    Videojuego v;
    int idJuegoBuscado;
    printf ("Ingrese la ID del videojuego que desea buscar: ");
    scanf ("%d", &idJuegoBuscado);

    while (fread(&v, sizeof(Videojuego), 1, archivo)==1)
    {
        if (idJuegoBuscado==v.idJuego)
        {
            printf ("\n");
            printf ("ID: %d\n", v.idJuego);
            printf ("Titulo: %s\n", v.titulo);
            printf ("Plataforma: %s\n", v.plataforma);
            printf ("Genero: %s\n", v.genero);
            printf ("Stock: %d\n", v.stock);
            printf ("Precio de adquisicion: %.2f\n", v.precioAdquisicion);
            printf ("\n");
            return;
        }
    }
    printf ("El videojuego no existe.\n");
    return;
}

void registrarVenta (const char *nombreArchivo)
{
    FILE *archivo = fopen(nombreArchivo, "r+b");
    if (archivo==NULL)
    {
        FILE *archivo = fopen (nombreArchivo, "wb");
        if (archivo == NULL)
        {
            printf("Error: No se pudo abrir o crear el archivo '%s'.\n", nombreArchivo);
            return;
        }
        fclose(archivo);
        archivo = fopen(nombreArchivo, "r+b");

    }
    int cantidadVendida=0;
    int idBuscada;
    printf ("Cuantas copias se han vendido?: ");
    scanf ("%d", &cantidadVendida);
    if (!(cantidadVendida>=1))
        {
            printf ("Error: La cantidad debe ser mayor a 0.\n");
            return;
        }
    printf ("Ingrese la fecha de la transaccion: ");
    printf ("\n");
    printf ("Ingrese la id del juego vendido: ");
    scanf ("%d", &idBuscada);
    Videojuego v;
    int encontrado = 0;
    long posicionRegistro = 0;
    while (fread(&v, sizeof(Videojuego), 1, archivo) == 1)
    {
        if (v.idJuego == idBuscada)
        {
            encontrado = 1;
            posicionRegistro = ftell(archivo) - sizeof(Videojuego);
            break;
        }
    }
    if (!encontrado)
    {
        printf("Error: El videojuego no existe en la base de datos.\n");
        fclose(archivo);
        return;
    }
    if (v.stock <= 0)
    {
        printf("Error: '%s' se encuentra agotado en este momento. (Stock: 0).\n", v.titulo);
        fclose(archivo);
        return;
    }
    if (cantidadVendida > v.stock)
    {
        printf("Error: Stock insuficiente.\n");
        fclose(archivo);
        return;
    }
    v.stock -= cantidadVendida;
    fseek(archivo, posicionRegistro, SEEK_SET);
    fwrite(&v, sizeof(Videojuego), 1, archivo);
    fclose(archivo);
    printf ("Venta registrada con exito.\n");
    return;
}

void stockCritico (const char *nombreArchivo)
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL)
    {
        printf("El archivo de videojuegos aun no existe o esta vacio.\n");
        return;
    }
    fseek(archivo, 0, SEEK_END);
    long bytesTotales = ftell(archivo);
    int totalRegistros = bytesTotales / sizeof(Videojuego);

    if (totalRegistros <= 0)
    {
        printf("El inventario esta vacio.\n");
        fclose(archivo);
        return;
    }
    rewind(archivo);
    Videojuego *lista = (Videojuego *)malloc(totalRegistros * sizeof(Videojuego));
    if (lista == NULL)
    {
        printf("Error: No se pudo reservar memoria dinamica.\n");
        fclose(archivo);
        return;
    }

    int leidos = fread(lista, sizeof(Videojuego), totalRegistros, archivo);
    fclose(archivo);

    for (int i = 0; i < leidos - 1; i++)
    {
        for (int j = 0; j < leidos - i - 1; j++)
        {
            if (lista[j].stock > lista[j + 1].stock)
            {
                Videojuego aux = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = aux;
            }
        }
    }

    int criticosEncontrados = 0;
    for (int i = 0; i < leidos; i++)
    {
        if (lista[i].stock <= 5)
        {
            printf ("\n");
            printf ("ID: %d\n", lista[i].idJuego); // Usamos lista[i] en lugar de 'v'
            printf ("Titulo: %s\n", lista[i].titulo);
            printf ("Plataforma: %s\n", lista[i].plataforma);
            printf ("Genero: %s\n", lista[i].genero);
            printf ("Stock: %d\n", lista[i].stock);
            printf ("Precio de adquisicion: %.2f\n", lista[i].precioAdquisicion);
            printf ("====================================================\n");
            criticosEncontrados++;
        }
    }
    if (criticosEncontrados == 0)
    {
        printf("No hay videojuegos con stock critico en este momento.\n");
    }
    else
    {
        printf("Total de titulos en estado critico: %d\n", criticosEncontrados);
    }
    free(lista);
    lista = NULL;

}
