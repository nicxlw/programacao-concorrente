#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0;
long int ultimo_valor = 0; // valor congelado para impressão
pthread_mutex_t mutex;
pthread_cond_t cond_imprime;
pthread_cond_t cond_soma;
int imprimindo = 0;

void *ExecutaTarefa (void *arg) {
    long int id = (long int) arg;
    printf("Thread : %ld esta executando...\n", id);

    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);

        soma++;

        if (soma % 1000 == 0) {
            ultimo_valor = soma; // congela o valor
            imprimindo = 1;
            pthread_cond_signal(&cond_imprime);
            while (imprimindo == 1) {
                pthread_cond_wait(&cond_soma, &mutex);
            }
        }

        pthread_mutex_unlock(&mutex);
    }

    printf("Thread : %ld terminou!\n", id);
    pthread_exit(NULL);
}

void *extra (void *args) {
    printf("Extra : esta executando...\n");
    int nthreads = *(int *)args;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (imprimindo == 0) {
            pthread_cond_wait(&cond_imprime, &mutex);
        }

        printf("soma = %ld\n", ultimo_valor); // usa valor congelado

        imprimindo = 0;
        pthread_cond_broadcast(&cond_soma);

        // Condição de saída
        if (soma >= 100000 * nthreads) { 
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_unlock(&mutex);
    }

    printf("Extra : terminou!\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t *tid;
    int nthreads;

    if(argc < 2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    tid = (pthread_t*) malloc(sizeof(pthread_t) * (nthreads + 1));
    if(tid == NULL) {puts("ERRO--malloc"); return 2;}

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_imprime, NULL);
    pthread_cond_init(&cond_soma, NULL);

    for (long int t = 0; t < nthreads; t++) {
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    if (pthread_create(&tid[nthreads], NULL, extra, &nthreads)) {
        printf("--ERRO: pthread_create()\n"); exit(-1);
    }

    for (int t = 0; t < nthreads + 1; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    } 

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_imprime);
    pthread_cond_destroy(&cond_soma);

    printf("Valor final de 'soma' = %ld\n", soma);
    return 0;
}
