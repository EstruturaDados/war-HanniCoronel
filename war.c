#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constantes Globais
#define TAM_NOME 30
#define TAM_COR 10

// Definição da Estrutura (struct)
// Representa um território no sistema
struct Territorio {
    char nome[TAM_NOME];   // Nome do território
    char cor[TAM_COR];     // Cor do exército
    int tropas;            // Quantidade de tropas
};
typedef struct Territorio Territorio;

// Função para limpar o buffer de entrada
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Função que simula um ataque entre dois territórios
void atacar(Territorio* atacante, Territorio* defensor) {
    if (atacante->tropas < 1) {
        printf("\n[ERRO] O território atacante não possui tropas suficientes!\n");
        return;
    }
    if (defensor->tropas < 1) {
        printf("\n[ERRO] O território defensor não possui tropas!\n");
        return;
    }

    printf("\n=== BATALHA ENTRE %s (Atacante) E %s (Defensor) ===\n", atacante->nome, defensor->nome);

    // Rolagem de dados (1 a 6)
    int dadoAtacante = rand() % 6 + 1;
    int dadoDefensor = rand() % 6 + 1;

    printf("Dado Atacante (%s): %d\n", atacante->nome, dadoAtacante);
    printf("Dado Defensor (%s): %d\n", defensor->nome, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("Resultado: O atacante venceu a batalha!\n");

        // Defensor perde 1 tropa
        defensor->tropas--;

        // Se o defensor perdeu todas as tropas, atacante assume o território
        if (defensor->tropas <= 0) {
            defensor->tropas = atacante->tropas - 1; // transfere parte das tropas
            atacante->tropas = 1; // atacante mantém pelo menos 1 tropa no território original
            strcpy(defensor->cor, atacante->cor); // muda dono
            printf(">>> O território %s agora pertence ao exército %s!\n", defensor->nome, defensor->cor);
        }
    } else {
        printf("Resultado: O defensor resistiu ao ataque!\n");
        atacante->tropas--; // atacante perde 1 tropa
    }
}

// Função principal
int main() {
    srand(time(NULL)); // inicializa números aleatórios

    int qtdTerritorios, i;
    Territorio* territorios;

    printf("===================================\n");
    printf("     SISTEMA DE TERRITÓRIOS - WAR\n");
    printf("===================================\n");

    // Pergunta quantos territórios cadastrar
    printf("\nDigite a quantidade de territórios a cadastrar: ");
    scanf("%d", &qtdTerritorios);
    limparBufferEntrada();

    // Alocação dinâmica
    territorios = (Territorio*) calloc(qtdTerritorios, sizeof(Territorio));
    if (territorios == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    // Cadastro dos territórios
    for(i = 0; i < qtdTerritorios; i++) {
        printf("\n--- Cadastro do Território %d ---\n", i + 1);

        printf("Digite o nome do território: ");
        fgets(territorios[i].nome, TAM_NOME, stdin);
        territorios[i].nome[strcspn(territorios[i].nome, "\n")] = '\0';

        printf("Digite a cor do exército: ");
        fgets(territorios[i].cor, TAM_COR, stdin);
        territorios[i].cor[strcspn(territorios[i].cor, "\n")] = '\0';

        printf("Digite a quantidade de tropas: ");
        scanf("%d", &territorios[i].tropas);
        limparBufferEntrada();
    }

    // Exibição dos dados cadastrados
    printf("\n===================================\n");
    printf("   LISTA DE TERRITÓRIOS CADASTRADOS\n");
    printf("===================================\n");
    for(i = 0; i < qtdTerritorios; i++) {
        printf("\nTerritório %d\n", i + 1);
        printf("Nome: %s\n", territorios[i].nome);
        printf("Cor do Exército: %s\n", territorios[i].cor);
        printf("Tropas: %d\n", territorios[i].tropas);
        printf("-----------------------------\n");
    }

    // Loop de ataque
    char opcao;
    do {
        int atk, def;

        printf("\n=== MENU DE ATAQUE ===\n");
        for (i = 0; i < qtdTerritorios; i++) {
            printf("%d - %s (Cor: %s, Tropas: %d)\n", i, territorios[i].nome, territorios[i].cor, territorios[i].tropas);
        }

        printf("\nEscolha o índice do território atacante: ");
        scanf("%d", &atk);
        printf("Escolha o índice do território defensor: ");
        scanf("%d", &def);
        limparBufferEntrada();

        if (atk >= 0 && atk < qtdTerritorios && def >= 0 && def < qtdTerritorios && atk != def) {
            atacar(&territorios[atk], &territorios[def]);
        } else {
            printf("[ERRO] Índices inválidos!\n");
        }

        // Exibir atualização
        printf("\n--- ESTADO ATUAL DOS TERRITÓRIOS ---\n");
        for(i = 0; i < qtdTerritorios; i++) {
            printf("%d - %s | Cor: %s | Tropas: %d\n", i, territorios[i].nome, territorios[i].cor, territorios[i].tropas);
        }

        printf("\nDeseja realizar outro ataque? (s/n): ");
        scanf(" %c", &opcao);
        limparBufferEntrada();

    } while(opcao == 's' || opcao == 'S');

    printf("\n=== FIM DO JOGO ===\n");

    free(territorios); // libera memória
   
    return 0;
}
