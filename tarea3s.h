#ifndef TAREA_H
#define TAREA_H

#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int tam_buffer;
    int num_harina;
    int num_carne;
    int num_vegetales;
    int num_especias;
    int num_chef_x;
    int num_chef_y;
    int num_chef_z;
    int total_platos;
} Config;

typedef struct {
    int *buffer;
    int in;
    int out;
    sem_t espacios_libres;
    sem_t espacios_utilizados;
    pthread_mutex_t mutex;
} BufferCircular;

typedef struct {
    int id;
    char *tipo;
    BufferCircular *buffer;
} ThreadArgs;

extern Config config;
extern BufferCircular *buffer_harina;
extern BufferCircular *buffer_carne;
extern BufferCircular *buffer_vegetales;
extern BufferCircular *buffer_especias;
extern sem_t batidora, sarten;
extern int platos_preparados;
extern pthread_mutex_t mutex_platos;

BufferCircular* crear_buffer(int tamano);
void liberar_buffer(BufferCircular *buffer);

void *productor(void *arg);
void *chef_X(void *arg);
void *chef_Y(void *arg);
void *chef_Z(void *arg);

int verificar_termino();

void mostrar_ayuda();

#endif