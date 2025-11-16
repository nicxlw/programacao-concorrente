/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Atividade 3 - Contar primos com Futures */

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;

class PrimoCallable implements Callable<Integer> {
    private final int inicio;
    private final int fim;

    PrimoCallable(int inicio, int fim) {
        this.inicio = inicio;
        this.fim = fim;
    }

    // Verifica se um número é primo
    private boolean ehPrimo(int n) {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        for (int i = 3; i * i <= n; i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }

    // tarefa executada pela thread
    public Integer call() {
        int count = 0;
        for (int num = inicio; num <= fim; num++) {
            if (ehPrimo(num)) count++;
        }
        return count;
    }
}

public class FutureHello {

    private static final int N = 1000000;   // limite superior
    private static final int NTHREADS = 10;    // tamanho do pool

    public static void main(String[] args) {

        ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
        List<Future<Integer>> list = new ArrayList<>();

        int bloco = N / NTHREADS;

        // divide em intervalos iguais
        for (int i = 0; i < NTHREADS; i++) {
            int inicio = i * bloco + 1;
            int fim = (i == NTHREADS - 1) ? N : (i + 1) * bloco;

            Callable<Integer> worker = new PrimoCallable(inicio, fim);
            Future<Integer> submit = executor.submit(worker);
            list.add(submit);
        }

        // soma resultados
        int totalPrimos = 0;
        for (Future<Integer> future : list) {
            try {
                totalPrimos += future.get();
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }
        }

        System.out.println("Quantidade total de números primos entre 1 e " + N + ": " + totalPrimos);

        executor.shutdown();
    }
}
