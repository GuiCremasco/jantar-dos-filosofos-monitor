#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

struct monitor {
    enum {PENSATIVO, FAMINTO, COMENDO} estado[5];
    sem_t mutex;
    sem_t semFilosofo[5];
    int nFilosofo[5];
};

#define ESQUERDA ((nFilosofo + 4) % 5)
#define DIREITA ((nFilosofo + 1) % 5)

struct monitor MONITOR;
pthread_t threadFilosofo[5];

void pausarExecucao() {
    sleep(1);
}

void teste(int nFilosofo) {
    if(MONITOR.estado[ESQUERDA] != COMENDO && MONITOR.estado[nFilosofo] == FAMINTO && MONITOR.estado[DIREITA] != COMENDO) {
        MONITOR.estado[nFilosofo] = COMENDO;
        pausarExecucao();
        printf("Filosofo %d pegou os hashis %d e %d\n", nFilosofo+1, ESQUERDA+1, nFilosofo+1);
        printf("Filosofo %d esta comendo\n", nFilosofo+1);
        sem_post(&MONITOR.semFilosofo[nFilosofo]);
    }
}

void pegaHashi(int nFilosofo) {
    sem_wait(&MONITOR.mutex);
    MONITOR.estado[nFilosofo] = FAMINTO;
    printf("Filosofo %d esta faminto\n", nFilosofo+1);
    teste(nFilosofo);
    sem_post(&MONITOR.mutex);
    sem_wait(&MONITOR.semFilosofo[nFilosofo]);
    pausarExecucao();
}

void largaHashi(int nFilosofo) {
    sem_wait(&MONITOR.mutex);
    MONITOR.estado[nFilosofo] = PENSATIVO;
    printf("Filosofo %d largou os hashis %d e %d\n", nFilosofo+1, ESQUERDA+1, nFilosofo+1);
    printf("Filosofo %d voltou a filosofar, satisfeito e feliz :)\n", nFilosofo+1);
    teste(ESQUERDA);
    teste(DIREITA);
    sem_post(&MONITOR.mutex);
}

void *filosofo(void *num) {
    int *f = num;
    pausarExecucao();
    pegaHashi(*f);
    pausarExecucao();
    largaHashi(*f);
}

int inicializarMonitor() {
    int check = 1;
    for(int i = 0; i < 5; i++) {
        MONITOR.nFilosofo[i] = i;
    }

    if(sem_init(&MONITOR.mutex, 0, 1) == 0) {
        for(int i = 0; i < 5; i++) {
            sem_init(&MONITOR.semFilosofo[i], 0, 0);
        }
        check = 0;
    } else {
        printf("Erro ao inicializar os semaforos\n");
    }

    return check;
}

void inicializarThreads() {
    for(int i = 0; i < 5; i++) {
        if(pthread_create(&threadFilosofo[i], NULL, filosofo, &MONITOR.nFilosofo[i])) {
            printf("Falha ao criar a thread %d\n", i+1);
        }
        else {
            printf("Filosofo %d esta filosofando\n", i+1);
        }
    }
}

void finalizarMonitor() {
    sem_destroy(&MONITOR.mutex);
    for(int i = 0; i < 5; i++) {
        sem_destroy(&MONITOR.semFilosofo[i]);
    }
}

void finalizarThreads() {
    for(int i = 0; i < 5; i++) {
        pthread_join(threadFilosofo[i], NULL);
    }
}

int main() {
    if(inicializarMonitor() == 0) {
        inicializarThreads();
        finalizarThreads();
        finalizarMonitor();
    }
}