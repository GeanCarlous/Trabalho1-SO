#include <stdio.h>
#include <stdlib.h>
#include <math.h>  // Para cálculos matemáticos, como pow()

#define MAX_TASKS 10  // Definição do número máximo de tarefas

// Estrutura que representa uma Tarefa
typedef struct {
    int periodo;          // Período da tarefa (P)
    int execucao;         // Tempo de execução no pior caso (C)
    int deadline;         // Deadline da tarefa (D)
    int tempo_restante;   // Tempo restante para completar a tarefa
    int proximo_deadline; // Próximo deadline da tarefa
} Tarefa;

Tarefa tarefas[MAX_TASKS];  // Array de tarefas
int numero_tarefas = 0;     // Contador de tarefas

// Função para carregar tarefas a partir de um arquivo
void carregar_tarefas(const char *nome_arquivo) {
    FILE *arquivo = fopen("teste.txt", "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    // Ignora a primeira linha do arquivo (títulos das colunas)
    fscanf(arquivo, "%*s %*s %*s");

    // Lê os dados das tarefas do arquivo
    while (fscanf(arquivo, "%d %d %d", &tarefas[numero_tarefas].periodo, 
                                         &tarefas[numero_tarefas].execucao, 
                                         &tarefas[numero_tarefas].deadline) == 3) {
        tarefas[numero_tarefas].tempo_restante = tarefas[numero_tarefas].execucao;
        tarefas[numero_tarefas].proximo_deadline = tarefas[numero_tarefas].deadline;
        numero_tarefas++;
    }

    fclose(arquivo);
}

// Função para testar a escalonabilidade usando Rate Monotonic (RM)
int teste_escalonabilidade_rm() {
    double utilizacao = 0.0;

    // Calcula a utilização do processador
    for (int i = 0; i < numero_tarefas; i++) {
        utilizacao += (double)tarefas[i].execucao / tarefas[i].periodo;
    }

    // Calcula o limite de utilizabilidade para o número de tarefas
    double limite = numero_tarefas * (pow(2, 1.0 / numero_tarefas) - 1);

    // Verifica se o sistema é escalonável
    return utilizacao <= limite;
}

// Função que implementa o escalonamento Rate Monotonic (RM)
void escalonador_rm(int tempo_simulacao) {
    printf("Escalonamento Rate Monotonic (RM):\n");

    for (int t = 0; t < tempo_simulacao; t++) {
        int tarefa_selecionada = -1;
        int menor_periodo = __INT_MAX__;

        // Seleciona a tarefa com o menor período (prioridade mais alta)
        for (int i = 0; i < numero_tarefas; i++) {
            if (tarefas[i].tempo_restante > 0 && tarefas[i].periodo < menor_periodo) {
                menor_periodo = tarefas[i].periodo;
                tarefa_selecionada = i;
            }
        }

        // Executa a tarefa selecionada, se houver
        if (tarefa_selecionada != -1) {
            printf("Tempo %d: Executando tarefa %d\n", t, tarefa_selecionada + 1);
            tarefas[tarefa_selecionada].tempo_restante--;

            // Se a tarefa for concluída, reinicia seu tempo de execução
            if (tarefas[tarefa_selecionada].tempo_restante == 0) {
                tarefas[tarefa_selecionada].tempo_restante = tarefas[tarefa_selecionada].execucao;
                tarefas[tarefa_selecionada].proximo_deadline += tarefas[tarefa_selecionada].periodo;
            }
        } else {
            printf("Tempo %d: Nenhuma tarefa executando\n", t);
        }
    }
}

// Função que implementa o escalonamento Earliest Deadline First (EDF)
void escalonador_edf(int tempo_simulacao) {
    printf("Escalonamento Earliest Deadline First (EDF):\n");

    for (int t = 0; t < tempo_simulacao; t++) {
        int tarefa_selecionada = -1;
        int menor_deadline = __INT_MAX__;

        // Seleciona a tarefa com o menor deadline
        for (int i = 0; i < numero_tarefas; i++) {
            if (tarefas[i].tempo_restante > 0 && tarefas[i].proximo_deadline < menor_deadline) {
                menor_deadline = tarefas[i].proximo_deadline;
                tarefa_selecionada = i;
            }
        }

        // Executa a tarefa selecionada, se houver
        if (tarefa_selecionada != -1) {
            printf("Tempo %d: Executando tarefa %d\n", t, tarefa_selecionada + 1);
            tarefas[tarefa_selecionada].tempo_restante--;

            // Se a tarefa for concluída, reinicia seu tempo de execução
            if (tarefas[tarefa_selecionada].tempo_restante == 0) {
                tarefas[tarefa_selecionada].tempo_restante = tarefas[tarefa_selecionada].execucao;
                tarefas[tarefa_selecionada].proximo_deadline += tarefas[tarefa_selecionada].periodo;
            }
        } else {
            printf("Tempo %d: Nenhuma tarefa executando\n", t);
        }
    }
}

int main() {
    // Carrega as tarefas a partir de um arquivo .txt
    carregar_tarefas("tarefas.txt");

    // Verifica se o conjunto de tarefas é escalonável pelo RM
    if (!teste_escalonabilidade_rm()) {
        printf("As tarefas nao sao escalonaveis pelo RM.\n");
        return EXIT_FAILURE;
    }

    int tempo_simulacao = 20; // Tempo de simulação, ajustável conforme necessário

    // Executa e exibe os resultados do escalonamento RM
    escalonador_rm(tempo_simulacao);
    printf("\n");

    // Executa e exibe os resultados do escalonamento EDF
    escalonador_edf(tempo_simulacao);

    return 0;
}
