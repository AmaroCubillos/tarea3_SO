#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_BUFFER 100  
typedef struct {
    int *buffer_harina;
    int *buffer_carne;
    int *buffer_vegetales;
    int *buffer_especias;

    int indice_produccion[4];
    int indice_consumo[4];

    sem_t mutex_mesa;
    sem_t espacios_disponibles[4];
    sem_t ingredientes_disponibles[4];
} Mesa;

typedef struct {
    sem_t batidora;
    sem_t sarten;
} Utensilios;

Mesa mesa_central;
Utensilios utensilios;
int contador_platillos = 0;
int programa_activo = 1;
pthread_mutex_t contador_platillos_mutex;


int total_platillos;
int tamanio_mesa;
