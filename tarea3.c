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
int tamano_mesa;

void inicializar_mesa(int tamano) {
    mesa_central.buffer_harina = (int *)malloc(tamano * sizeof(int));
    mesa_central.buffer_carne = (int *)malloc(tamano * sizeof(int));
    mesa_central.buffer_vegetales = (int *)malloc(tamano * sizeof(int));
    mesa_central.buffer_especias = (int *)malloc(tamano * sizeof(int));

    for (int i = 0; i < 4; i++) {
        mesa_central.indice_produccion[i] = 0;
        mesa_central.indice_consumo[i] = 0;
        sem_init(&mesa_central.espacios_disponibles[i], 0, tamano);
        sem_init(&mesa_central.ingredientes_disponibles[i], 0, 0);
    }

    sem_init(&mesa_central.mutex_mesa, 0, 1);
}

void inicializar_utensilios() {
    sem_init(&utensilios.batidora, 0, 1);
    sem_init(&utensilios.sarten, 0, 1);
}

void incrementar_contador_platillos() {
    pthread_mutex_lock(&contador_platillos_mutex);
    contador_platillos++;
    printf("platillo completado. Total: %d\n", contador_platillos);
    pthread_mutex_unlock(&contador_platillos_mutex);
}

void *productor(void *arg) {
    int tipo_ingrediente = *(int *)arg;

    while (programa_activo) {
        sem_wait(&mesa_central.espacios_disponibles[tipo_ingrediente]);
        sem_wait(&mesa_central.mutex_mesa);

        if (!programa_activo) {
            sem_post(&mesa_central.mutex_mesa);
            sem_post(&mesa_central.espacios_disponibles[tipo_ingrediente]);
            break;
        }

        
        int *buffer;
        switch (tipo_ingrediente) {
            case 0: buffer = mesa_central.buffer_harina; break;
            case 1: buffer = mesa_central.buffer_carne; break;
            case 2: buffer = mesa_central.buffer_vegetales; break;
            case 3: buffer = mesa_central.buffer_especias; break;
        }

        int idx = mesa_central.indice_produccion[tipo_ingrediente];
        buffer[idx] = 1; 
        mesa_central.indice_produccion[tipo_ingrediente] = (idx + 1) % tamano_mesa;

        printf("Productor de ingrediente %d produjo\n", tipo_ingrediente);

        sem_post(&mesa_central.mutex_mesa);
        sem_post(&mesa_central.ingredientes_disponibles[tipo_ingrediente]);
    }
    return NULL;
}