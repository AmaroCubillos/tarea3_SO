#include "tarea3s.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>

Config config;
BufferCircular *buffer_harina;
BufferCircular *buffer_carne;
BufferCircular *buffer_vegetales;
BufferCircular *buffer_especias;
sem_t batidora, sarten;
int platos_preparados = 0;
pthread_mutex_t mutex_platos;

BufferCircular* crear_buffer(int tamano) {
    BufferCircular *buffer = malloc(sizeof(BufferCircular));
    buffer->buffer = calloc(tamano, sizeof(int));
    buffer->in = 0;
    buffer->out = 0;
    sem_init(&buffer->espacios_libres, 0, tamano);
    sem_init(&buffer->espacios_utilizados, 0, 0);
    pthread_mutex_init(&buffer->mutex, NULL);
    return buffer;
}

void liberar_buffer(BufferCircular *buffer) {
    free(buffer->buffer);
    sem_destroy(&buffer->espacios_libres);
    sem_destroy(&buffer->espacios_utilizados);
    pthread_mutex_destroy(&buffer->mutex);
    free(buffer);
}

void *productor(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    BufferCircular *buffer = args->buffer;

    while (1) {
        sem_wait(&buffer->espacios_libres);
        pthread_mutex_lock(&buffer->mutex);

        if (platos_preparados >= config.total_platos) {
            pthread_mutex_unlock(&buffer->mutex);
            sem_post(&buffer->espacios_libres);
            break;
        }

        buffer->buffer[buffer->in] = 1;
        buffer->in = (buffer->in + 1) % config.tam_buffer;
        printf("Productor %s %d: produjo ingrediente\n", args->tipo, args->id);

        pthread_mutex_unlock(&buffer->mutex);
        sem_post(&buffer->espacios_utilizados);
    }
    return NULL;
}

int verificar_termino() {
    pthread_mutex_lock(&mutex_platos);
    if (platos_preparados >= config.total_platos) {
        pthread_mutex_unlock(&mutex_platos);
        return 1;
    }
    platos_preparados++;
    pthread_mutex_unlock(&mutex_platos);
    return 0;
}
