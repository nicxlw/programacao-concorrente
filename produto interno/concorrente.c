#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    float *a;
    float *b;
    long int start;
    long int end;
    double parcial;
} ThreadData;

void* calcula_parcial(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    double soma = 0.0;
    for (long int i = data->start; i < data->end; i++) {
        soma += data->a[i] * data->b[i];
    }
    data->parcial = soma;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <num_threads> <arquivo>\n", argv[0]);
        return 1;
    }
    
    int T = atoi(argv[1]);
    char *filename = argv[2];
    
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Erro ao abrir arquivo");
        return 1;
    }
    
    long int N;
    fread(&N, sizeof(long int), 1, fp);
    
    float *a = malloc(N * sizeof(float));
    float *b = malloc(N * sizeof(float));
    float produto_sequencial;
    
    fread(a, sizeof(float), N, fp);
    fread(b, sizeof(float), N, fp);
    fread(&produto_sequencial, sizeof(float), 1, fp);
    fclose(fp);
    
    pthread_t threads[T];
    ThreadData dados[T];
    
    long int bloco = N / T;
    
    clock_t inicio = clock();
    for (int i = 0; i < T; i++) {
        dados[i].a = a;
        dados[i].b = b;
        dados[i].start = i * bloco;
        dados[i].end = (i == T-1) ? N : (i+1) * bloco;
        dados[i].parcial = 0.0;
        pthread_create(&threads[i], NULL, calcula_parcial, &dados[i]);
    }
    
    double produto_concorrente = 0.0;
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        produto_concorrente += dados[i].parcial;
    }
    clock_t fim = clock();
    
    double erro_relativo = (produto_sequencial - produto_concorrente) / produto_sequencial;
    double tempo_exec = (double)(fim - inicio) / CLOCKS_PER_SEC;
    
    printf("Produto interno (concorrente): %.6f\n", produto_concorrente);
    printf("Erro relativo: %.10f\n", erro_relativo);
    printf("Tempo execucao: %.6f s\n", tempo_exec);
    
    free(a);
    free(b);
    return 0;
}
