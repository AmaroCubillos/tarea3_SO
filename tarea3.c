#include "tarea3s.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>

// estructura buffer y configuración global
Config config;
BufferC *buffer_harina; // buffer circular para ingrediente harina
BufferC *buffer_carne; // buffer circular para ingrediente carne
BufferC *buffer_vegetales; // buffer circular para ingrediente vegetales
BufferC *buffer_especias; // buffer circular para ingrediente especias
sem_t batidora, sarten; // semáforos para controlar acceso a utensilios
int preparados = 0; // contador de platos preparados
pthread_mutex_t mutex_platos; // mutex para proteger el contador de platos

// función para crear un buffer circular con sincronización
BufferC* crear_buffer(int tamano) {
    BufferC *buffer = malloc(sizeof(BufferC));
    buffer->buffer = calloc(tamano, sizeof(int));
    buffer->in = 0;
    buffer->out = 0;

    // inicializa semáforos para control de producción y consumo
    sem_init(&buffer->libres, 0, tamano);
    sem_init(&buffer->utilizados, 0, 0);
    pthread_mutex_init(&buffer->mutex, NULL); // mutex para exclusión mutua
    return buffer;
}

// función para liberar recursos del buffer
void liberar_buffer(BufferC *buffer) {
    free(buffer->buffer);
    sem_destroy(&buffer->libres);
    sem_destroy(&buffer->utilizados);
    pthread_mutex_destroy(&buffer->mutex);
    free(buffer);
}

// función que ejecutan los hilos productores
void *productor(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    BufferC *buffer = args->buffer;

    while (1) {
        // espera si el buffer está lleno
        sem_wait(&buffer->libres);
        pthread_mutex_lock(&buffer->mutex);

        // verifica si ya se alcanzó el total de platos requeridos
        if (preparados >= config.total) {
            pthread_mutex_unlock(&buffer->mutex);
            sem_post(&buffer->libres);
            break;
        }

        // produce un ingrediente
        buffer->buffer[buffer->in] = 1;
        buffer->in = (buffer->in + 1) % config.tam_buffer;
        printf("productor %s número %d: produjo ingrediente\n", args->tipo, args->id);

        pthread_mutex_unlock(&buffer->mutex);
        // señala que hay un nuevo ingrediente disponible
        sem_post(&buffer->utilizados);
    }
    return NULL;
}

// función para verificar si se alcanzó el total de platos
int verificar_termino() {
    pthread_mutex_lock(&mutex_platos);
    if (preparados >= config.total) {
        pthread_mutex_unlock(&mutex_platos);
        return 1;
    }
    preparados++;
    pthread_mutex_unlock(&mutex_platos);
    return 0;
}

// función genérica para los chefs que preparan platos
void *chef(void *arg, BufferC *buffer1, BufferC *buffer2, BufferC *buffer3, sem_t *utensilio, const char *desc) {
    ThreadArgs *args = (ThreadArgs *)arg;

    while (1) {
        // verifica si ya se completaron todos los platos
        if (verificar_termino()) break;

        // Toma el utensilio necesario, batidora o sartén
        sem_wait(utensilio);
        printf("chef %s número %d: tomó el %s\n", desc, args->id, desc);

        // espera por los ingredientes necesarios
        if (buffer1) sem_wait(&buffer1->utilizados);
        if (buffer2) sem_wait(&buffer2->utilizados);
        if (buffer3) sem_wait(&buffer3->utilizados);

        // toma los ingredientes de los buffers
        // bloquea los buffers para modificarlos
        if (buffer1) pthread_mutex_lock(&buffer1->mutex);
        if (buffer2) pthread_mutex_lock(&buffer2->mutex);
        if (buffer3) pthread_mutex_lock(&buffer3->mutex);

        // actualiza los índices de consumo
        if (buffer1) buffer1->out = (buffer1->out + 1) % config.tam_buffer;
        if (buffer2) buffer2->out = (buffer2->out + 1) % config.tam_buffer;
        if (buffer3) buffer3->out = (buffer3->out + 1) % config.tam_buffer;

        printf("chef %s número %d: preparando plato\n", desc, args->id);

        // libera los mutex de los buffers
        if (buffer1) pthread_mutex_unlock(&buffer1->mutex);
        if (buffer2) pthread_mutex_unlock(&buffer2->mutex);
        if (buffer3) pthread_mutex_unlock(&buffer3->mutex);

        // señala que hay espacios libres en los buffers
        if (buffer1) sem_post(&buffer1->libres);
        if (buffer2) sem_post(&buffer2->libres);
        if (buffer3) sem_post(&buffer3->libres);

        // libera el utensilio
        sem_post(utensilio);
    }
    return NULL;
}

// funciones específicas para cada chef
void *chef_X(void *arg) {
    // chef X usa harina y carne, necesita batidora
    return chef(arg, buffer_harina, buffer_carne, NULL, &batidora, "X");
}

void *chef_Y(void *arg) {
    // chef Y usa vegetales y especias, necesita sartén
    return chef(arg, buffer_vegetales, buffer_especias, NULL, &sarten, "Y");
}

void *chef_Z(void *arg) {
    // chef Z usa harina, vegetales y especias, necesita batidora
    return chef(arg, buffer_harina, buffer_vegetales, buffer_especias, &batidora, "Z");
}

void indefinido() {
    printf("uso: ./programa -b <tamaño_buffer> -h <num_harina> -c <num_carne> -v <num_vegetales> -e <num_especias> -x <num_chef_x> -y <num_chef_y> -z <num_chef_z> -i <total>\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "b:h:c:v:e:x:y:z:i:")) != -1) {
        switch (opt) {
            case 'b': config.tam_buffer = atoi(optarg); break;
            case 'h': config.num_harina = atoi(optarg); break;
            case 'c': config.num_carne = atoi(optarg); break;
            case 'v': config.num_vegetales = atoi(optarg); break;
            case 'e': config.num_especias = atoi(optarg); break;
            case 'x': config.num_chef_x = atoi(optarg); break;
            case 'y': config.num_chef_y = atoi(optarg); break;
            case 'z': config.num_chef_z = atoi(optarg); break;
            case 'i': config.total = atoi(optarg); break;
            default: indefinido();
        }
    }

    if (config.tam_buffer <= 0 || config.num_harina <= 0 || config.num_carne <= 0 ||
        config.num_vegetales <= 0 || config.num_especias <= 0 || config.num_chef_x <= 0 ||
        config.num_chef_y <= 0 || config.num_chef_z <= 0 || config.total <= 0) {
        printf("error\n");
        indefinido();
    }

    sem_init(&batidora, 0, 1);
    sem_init(&sarten, 0, 1);
    pthread_mutex_init(&mutex_platos, NULL);

    buffer_harina = crear_buffer(config.tam_buffer);
    buffer_carne = crear_buffer(config.tam_buffer);
    buffer_vegetales = crear_buffer(config.tam_buffer);
    buffer_especias = crear_buffer(config.tam_buffer);

    pthread_t threads[config.num_harina + config.num_carne + config.num_vegetales + config.num_especias +
                      config.num_chef_x + config.num_chef_y + config.num_chef_z];

    int thread_index = 0;

    for (int i = 0; i < config.num_harina; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->id = i;
        args->tipo = "harina";
        args->buffer = buffer_harina;
        pthread_create(&threads[thread_index++], NULL, productor, args);
    }
    for (int i = 0; i < config.num_carne; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->id = i;
        args->tipo = "carne";
        args->buffer = buffer_carne;
        pthread_create(&threads[thread_index++], NULL, productor, args);
    }
    for (int i = 0; i < config.num_vegetales; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->id = i;
        args->tipo = "vegetales";
        args->buffer = buffer_vegetales;
        pthread_create(&threads[thread_index++], NULL, productor, args);
    }
    for (int i = 0; i < config.num_especias; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->id = i;
        args->tipo = "especias";
        args->buffer = buffer_especias;
        pthread_create(&threads[thread_index++], NULL, productor, args);
    }
    for (int i = 0; i < config.num_chef_x; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->id = i;
        pthread_create(&threads[thread_index++], NULL, chef_X, args);
    }
    for (int i = 0; i < config.num_chef_y; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->id = i;
        pthread_create(&threads[thread_index++], NULL, chef_Y, args);
    }
    for (int i = 0; i < config.num_chef_z; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->id = i;
        pthread_create(&threads[thread_index++], NULL, chef_Z, args);
    }

    for (int i = 0; i < thread_index; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nse prepararon todos los %d platos\n", config.total);

    liberar_buffer(buffer_harina);
    liberar_buffer(buffer_carne);
    liberar_buffer(buffer_vegetales);
    liberar_buffer(buffer_especias);

    sem_destroy(&batidora);
    sem_destroy(&sarten);
    pthread_mutex_destroy(&mutex_platos);

    return 0;
}
