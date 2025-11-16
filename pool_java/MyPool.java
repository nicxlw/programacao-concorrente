/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Atividade 1 */
/* Codigo: Criando um pool de threads em Java */

import java.util.LinkedList;


//------------------ DOCUMENTAÇÃO DA CLASSE FILATAREFAS ------------------------

/*
 * A classe FilaTarefas implementa um pool de threads simples.
 * 
 * Funcionamento:
 * - Ao ser criada, inicializa 'nThreads' threads que ficam em execução contínua.
 * - Cada thread executa tarefas colocadas em uma fila compartilhada (queue).
 * - A fila armazena objetos Runnable.
 * - O método execute() insere tarefas na fila e acorda uma thread.
 * - As threads ficam esperando enquanto a fila estiver vazia.
 * - Quando o método shutdown() é chamado:
 *      -> Ele sinaliza que não haverá mais novas tarefas.
 *      -> Acorda todas as threads.
 *      -> Aguarda cada thread terminar.
 * 
 * Sincronização:
 * - A fila é protegida com synchronized(queue).
 * - A comunicação é feita por wait() e notify().
 * 
 * Em resumo, esta classe implementa o comportamento básico de um executor:
 *   - Submeter tarefas
 *   - Threads consumindo tarefas da fila
 *   - Encerramento ordenado
 */

class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    // Enfileira uma nova tarefa para execução
    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify(); // acorda uma thread
        }
    }
    
    // Encerra o pool de threads
    public void shutdown() {
        synchronized(queue) {
            this.shutdown = true;
            queue.notifyAll(); // acorda TODAS as threads
        }
        for (int i = 0; i < nThreads; i++) {
            try { threads[i].join(); } 
            catch (InterruptedException e) { return; }
        }
    }

    // Classe interna que representa cada thread do pool
    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty()) return; // fila vazia E shutdown = true → terminar thread
             r = queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}

// Tarefa simples: imprime uma mensagem
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   public void run() {
      System.out.println(msg);
   }
}


//-------------------------------------------------------------------
//------------------ IMPLEMENTAÇÃO DA TAREFA PRIMO ------------------
/*
 * Recebe um número inteiro positivo e imprime se ele é primo.
 */
class Primo implements Runnable {
    private final long n;

    public Primo(long n) {
        this.n = n;
    }

    private boolean ehPrimo(long n) {
        if (n <= 1) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;

        long limite = (long)Math.sqrt(n) + 1;

        for (long i = 3; i <= limite; i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }

    public void run() {
        boolean primo = ehPrimo(n);
        if (primo)
            System.out.println("Número " + n + " é primo.");
        else
            System.out.println("Número " + n + " NÃO é primo.");
    }
}
//-------------------------------------------------------------------


// Classe da aplicação (método main)
class MyPool {
    private static final int NTHREADS = 10;


    public static void main (String[] args) {
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      for (int i = 0; i < 25; i++) {

        // ---- PARA TESTAR A PRIMEIRA TAREFA (Hello) ----
        // final String m = "Hello da tarefa " + i;
        // Runnable hello = new Hello(m);
        // pool.execute(hello);

        // ---- PARA TESTAR A TAREFA DE PRIMO ----
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      pool.shutdown();
      System.out.println("Terminou");
   }
}
