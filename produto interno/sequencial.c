#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    long int N = 100; // tamanho do vetor
    float *a, *b, produto = 0.0;
    FILE *fp;
    
    srand(time(NULL));
    
    a = malloc(N * sizeof(float));
    b = malloc(N * sizeof(float));
    
    // Preenche vetores com valores aleatórios
    for (long int i = 0; i < N; i++) {
        a[i] = (float)rand() / RAND_MAX;
        b[i] = (float)rand() / RAND_MAX;
    }
    
    // Calcula produto interno
    for (long int i = 0; i < N; i++) {
        produto += a[i] * b[i];
    }
    
    // Salva no arquivo binário
    fp = fopen("dados.bin", "wb");
    fwrite(&N, sizeof(long int), 1, fp);
    fwrite(a, sizeof(float), N, fp);
    fwrite(b, sizeof(float), N, fp);
    fwrite(&produto, sizeof(float), 1, fp);
    fclose(fp);
    
    printf("Produto interno (sequencial): %.6f\n", produto);
    
    free(a);
    free(b);
    return 0;
}
