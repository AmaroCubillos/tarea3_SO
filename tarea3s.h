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
    int total;
} Config;

typedef struct {
    int *buffer;
    int in;
    int out;
    sem_t libres;
    sem_t utilizados;
    pthread_mutex_t mutex;
} BufferC;

typedef struct {
    int id;
    char *tipo;
    BufferC *buffer;
} ThreadArgs;

extern Config config;
extern BufferC *buffer_harina;
extern BufferC *buffer_carne;
extern BufferC *buffer_vegetales;
extern BufferC *buffer_especias;
extern sem_t batidora, sarten;
extern int preparados;
extern pthread_mutex_t mutex_platos;

BufferC* crear_buffer(int tamano);
void liberar_buffer(BufferC *buffer);

void *productor(void *arg);
void *chef_X(void *arg);
void *chef_Y(void *arg);
void *chef_Z(void *arg);

int verificar_termino();

void indefinido();

#endif