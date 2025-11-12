#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Definicoes Globais ---
#define CAPACIDADE_MOCHILA 10
#define MAX_NOME 100

// Define a estrutura de um componente
typedef struct {
    char nome[MAX_NOME];
} Componente;

// --- Prototipos das Funcoes ---
void limparBufferEntrada(void);
void exibirMenu(int numItens, int estaOrdenado);
void adicionarComponente(Componente mochila[], int *numItens, int *estaOrdenado);
void descartarComponente(Componente mochila[], int *numItens);
void listarComponentes(const Componente mochila[], int numItens);
void organizarMochila(Componente mochila[], int numItens, int *estaOrdenado);
void buscarComponente(const Componente mochila[], int numItens, int estaOrdenado);
int compararComponentes(const void *a, const void *b); // Para o qsort

// --- Funcao Principal ---
int main() {
    Componente mochila[CAPACIDADE_MOCHILA];
    int numItens = 0;
    int estaOrdenado = 0; // 0 = NAO ORDENADO, 1 = ORDENADO
    int opcao;

    do {
        exibirMenu(numItens, estaOrdenado);
        
        // Le a opcao e valida a entrada
        if (scanf("%d", &opcao) != 1) {
            printf("\n*** Entrada invalida! Por favor, digite um numero. ***\n");
            limparBufferEntrada();
            opcao = -1; // Define uma opcao invalida para continuar o loop
            continue;
        }
        limparBufferEntrada(); // Limpa o '\n' deixado pelo scanf

        switch (opcao) {
            case 1:
                adicionarComponente(mochila, &numItens, &estaOrdenado);
                break;
            case 2:
                descartarComponente(mochila, &numItens);
                break;
            case 3:
                listarComponentes(mochila, numItens);
                break;
            case 4:
                organizarMochila(mochila, numItens, &estaOrdenado);
                break;
            case 5:
                buscarComponente(mochila, numItens, estaOrdenado);
                break;
            case 0:
                printf("\n>>> ATIVANDO TORRE DE FUGA... ADEUS, ILHA! <<<\n");
                break;
            default:
                printf("\n*** Opcao invalida! Tente novamente. ***\n");
        }

    } while (opcao != 0);

    return 0;
}

// --- Implementacao das Funcoes ---


void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void exibirMenu(int numItens, int estaOrdenado) {
    printf("\n\n--- PLANO DE FUGA - CODIGO DA ILHA (NIVEL MESTRE) ---\n");
    printf("Itens na Mochila: %d/%d\n", numItens, CAPACIDADE_MOCHILA);
    printf("Status da Ordenacao por Nome: %s\n", estaOrdenado ? "ORDENADO" : "NAO ORDENADO");
    printf("---------------------------------------------------\n");
    printf("1. Adicionar Componente\n");
    printf("2. Descartar Componente\n");
    printf("3. Listar Componentes (Inventario)\n");
    printf("4. Organizar Mochila (Ordenar Componentes)\n");
    printf("5. Busca Binaria por Componente-Chave (por nome)\n");
    printf("0. ATIVAR TORRE DE FUGA (Sair)\n");
    printf("---------------------------------------------------\n");
    printf("Escolha sua acao: ");
}


void adicionarComponente(Componente mochila[], int *numItens, int *estaOrdenado) {
    if (*numItens >= CAPACIDADE_MOCHILA) {
        printf("\n*** Mochila cheia! Impossivel adicionar mais componentes. ***\n");
        return;
    }

    printf("Digite o nome do componente a adicionar: ");
    fgets(mochila[*numItens].nome, MAX_NOME, stdin);
    // Remove o '\n' do final da string, se existir
    mochila[*numItens].nome[strcspn(mochila[*numItens].nome, "\n")] = 0;

    printf("Componente '%s' adicionado.\n", mochila[*numItens].nome);
    (*numItens)++;
    
    // Adicionar um item sempre quebra a ordenacao
    *estaOrdenado = 0;
}


void descartarComponente(Componente mochila[], int *numItens) {
    if (*numItens == 0) {
        printf("\n*** Mochila vazia! Nao ha nada para descartar. ***\n");
        return;
    }

    char nomeBusca[MAX_NOME];
    printf("Digite o nome do componente a descartar: ");
    fgets(nomeBusca, MAX_NOME, stdin);
    nomeBusca[strcspn(nomeBusca, "\n")] = 0; // Remove o '\n'

    int indiceEncontrado = -1;
    // Busca linear pelo item
    for (int i = 0; i < *numItens; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) {
            indiceEncontrado = i;
            break;
        }
    }

    if (indiceEncontrado == -1) {
        printf("\n*** Componente '%s' nao encontrado na mochila. ***\n", nomeBusca);
    } else {
        // "Remove" o item sobrescrevendo-o com os itens seguintes
        for (int i = indiceEncontrado; i < *numItens - 1; i++) {
            mochila[i] = mochila[i + 1]; // Copia o struct inteiro
        }
        (*numItens)--;
        printf("Componente '%s' descartado com sucesso.\n", nomeBusca);
        // Nota: A ordem relativa dos itens restantes nao muda,
        // entao o status 'estaOrdenado' nao precisa ser alterado.
    }
}


void listarComponentes(const Componente mochila[], int numItens) {
    if (numItens == 0) {
        printf("\n--- INVENTARIO (VAZIO) ---\n");
        return;
    }

    printf("\n--- INVENTARIO (%d/%d) ---\n", numItens, CAPACIDADE_MOCHILA);
    for (int i = 0; i < numItens; i++) {
        printf("  %d: %s\n", i + 1, mochila[i].nome);
    }
    printf("---------------------------\n");
}


int compararComponentes(const void *a, const void *b) {
    Componente *compA = (Componente *)a;
    Componente *compB = (Componente *)b;
    return strcmp(compA->nome, compB->nome);
}


void organizarMochila(Componente mochila[], int numItens, int *estaOrdenado) {
    if (numItens < 2) {
        printf("\nNao ha itens suficientes para organizar.\n");
        *estaOrdenado = 1; // Lista vazia ou com 1 item ja esta ordenada
        return;
    }

    // Usa a funcao qsort da biblioteca <stdlib.h>
    qsort(mochila, numItens, sizeof(Componente), compararComponentes);

    *estaOrdenado = 1;
    printf("\n*** Mochila organizada por nome. ***\n");
}


void buscarComponente(const Componente mochila[], int numItens, int estaOrdenado) {
    if (!estaOrdenado) {
        printf("\n*** ERRO: A mochila precisa ser organizada (Opcao 4) antes de usar a Busca Binaria. ***\n");
        return;
    }

    if (numItens == 0) {
        printf("\n*** Mochila vazia! Nao ha nada para buscar. ***\n");
        return;
    }

    char nomeBusca[MAX_NOME];
    printf("Digite o nome do Componente-Chave a buscar: ");
    fgets(nomeBusca, MAX_NOME, stdin);
    nomeBusca[strcspn(nomeBusca, "\n")] = 0; // Remove o '\n'

    // Implementacao da Busca Binaria
    int baixo = 0;
    int alto = numItens - 1;
    int encontrado = 0;

    while (baixo <= alto) {
        int meio = baixo + (alto - baixo) / 2;
        int comp = strcmp(nomeBusca, mochila[meio].nome);

        if (comp == 0) { // Encontrou
            printf("\n>>> Componente-Chave '%s' ENCONTRADO no indice %d. <<<\n", nomeBusca, meio);
            encontrado = 1;
            break;
        } else if (comp > 0) { // O nome buscado vem *depois* do nome do meio
            baixo = meio + 1;
        } else { // O nome buscado vem *antes* do nome do meio
            alto = meio - 1;
        }
    }

    if (!encontrado) {
        printf("\n>>> Componente-Chave '%s' NAO ENCONTRADO. <<<\n", nomeBusca);
    }
}
