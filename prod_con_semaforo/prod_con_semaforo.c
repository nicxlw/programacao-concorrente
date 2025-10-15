#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <unistd.h>

/*
Nicole Cardozo dos Santos
DRE: 123174984
Data: 14/10/2025
Programação Concorrente - Exercício 3

Descrição: Implementa uma variação do padrão produtor/consumidor usando threads e semáforos.
A thread produtora gera N números consecutivos (uma sequência 1, 2, ..., N) e, a cada iteração,
preenche todo o buffer de tamanho M (aguardando ele ficar totalmente vazio antes). 
Várias threads consumidoras retiram os números, um por vez, verificam se são primos e contabilizam seus resultados.
O número de threads consumidoras foi definido como 4.
O programa recebe como entrada os valores de N e M.
Ao final, o programa exibe:
(i) Total de números primos encontrados
(ii) A thread consumidora vencedora (que encontrou mais primos)
*/

int *Buffer;             // Buffer compartilhado
int N, M;                // N = total de números, M = tamanho do buffer
int in = 0, out = 0;     // Índices de inserção e remoção
int totalConsumido = 0;  // Contagem total de itens consumidos
int totalPrimos = 0;     // Contagem total de primos encontrados
int numConsumidores = 4; // Número de threads consumidoras

// Semáforos
sem_t slotCheio, slotVazio; // Quantidade de slots cheios/vazios
sem_t mutexBuffer;          // Protege acesso ao buffer (binário)
sem_t mutexPrimos;          // Protege contagem total de primos (binário)
sem_t mutexConsumo;         // Protege contagem total consumida (binário)


// Função do enunciado para testar primalidade
int ehPrimo(long long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(n); i += 2)
        if (n % i == 0) return 0;
    return 1;
}

void Insere(int item) {
    sem_wait(&slotVazio);       // Espera slot vazio
    sem_wait(&mutexBuffer);    // Entra na seção crítica

    Buffer[in] = item;
    in = (in + 1) % M;

    sem_post(&mutexBuffer);    // Sai da seção crítica
    sem_post(&slotCheio);      // Sinaliza que há item cheio
}

int Retira(void) {
    int item;

    sem_wait(&slotCheio); // Espera ter item cheio
    sem_wait(&mutexBuffer); // Entra na seção crítica

    item = Buffer[out];
    out = (out + 1) % M; // Buffer circular

    sem_post(&mutexBuffer); // Sai da seção crítica
    sem_post(&slotVazio); // Libera um espaço vazio

    return item;
}

void *Produtora(void *arg) {
    int i = 1;
    while (i <= N) { // Roda enquanto houverem números a serem inseridos
    
        for (int k = 0; k < M; k++) { // Espera o buffer ter M posições vazias
            sem_wait(&slotVazio);
        }

        sem_wait(&mutexBuffer);
        printf("\nProdutora enchendo o buffer...\n");

        // Insere M números ou o restante que falta
        for (int j = 0; j < M && i <= N; j++, i++) { // Insere valores inteiros sequenciais
            Buffer[j] = i;
        }

        in = 0; // reinicia índice de inserção
        sem_post(&mutexBuffer);

        // Libera M slots cheios (permite consumidores retirarem)
        for (int k = 0; k < M && (i - M + k) <= N; k++) {
            sem_post(&slotCheio);
        }
    }

    pthread_exit(NULL);
}


void *Consumidora(void *arg) {
    int id = *(int *)arg;
    int primosLocal = 0;

    while (1) {
        sem_wait(&mutexConsumo);
        if (totalConsumido >= N) { // Para quando os N números foram consumidos
            sem_post(&mutexConsumo);
            break;
        }
        totalConsumido++;
        sem_post(&mutexConsumo);

        int item = Retira();
        
        // Calculo das métricas
        if (ehPrimo(item)) {
            primosLocal++;

            sem_wait(&mutexPrimos);
            totalPrimos++;
            sem_post(&mutexPrimos);
        }
    }

    printf("\nConsumidora %d encontrou %d primos.\n", id, primosLocal);
    pthread_exit((void *)(long)primosLocal);
}


int main() {
    printf("Digite N (quantidade total de números): ");
    scanf("%d", &N);
    printf("Digite M (tamanho do buffer): ");
    scanf("%d", &M);

    Buffer = (int *)malloc(M * sizeof(int));

    sem_init(&slotCheio, 0, 0);
    sem_init(&slotVazio, 0, M);
    sem_init(&mutexBuffer, 0, 1);
    sem_init(&mutexPrimos, 0, 1);
    sem_init(&mutexConsumo, 0, 1);

    pthread_t thProdutor;
    pthread_t thConsumidores[numConsumidores];
    int ids[numConsumidores];

    pthread_create(&thProdutor, NULL, Produtora, NULL);
    for (int i = 0; i < numConsumidores; i++) {
        ids[i] = i + 1;
        pthread_create(&thConsumidores[i], NULL, Consumidora, &ids[i]);
    }

    pthread_join(thProdutor, NULL);

    int maior = 0, vencedora = -1;
    for (int i = 0; i < numConsumidores; i++) {
        void *ret;
        pthread_join(thConsumidores[i], &ret);
        int primosThread = (int)(long)ret;
        if (primosThread > maior) {
            maior = primosThread;
            vencedora = i + 1;
        }
    }

    printf("\n----------------------------------\n");
    printf("Total de números primos: %d\n", totalPrimos);
    printf("Thread vencedora: %d (com %d primos)\n", vencedora, maior);
    printf("----------------------------------\n");

    sem_destroy(&slotCheio);
    sem_destroy(&slotVazio);
    sem_destroy(&mutexBuffer);
    sem_destroy(&mutexPrimos);
    sem_destroy(&mutexConsumo);
    free(Buffer);

    return 0;
}
