#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constantes
#define TAM_NOME 30
#define TAM_COR 10

// Struct Territorio
typedef struct {
    char nome[TAM_NOME];
    char cor[TAM_COR];
    int tropas;
} Territorio;

// Struct Jogador (guarda nome, cor e ponteiro para missão alocada dinamicamente)
typedef struct {
    char nome[TAM_NOME];
    char cor[TAM_COR];
    char *missao; // alocada dinamicamente
} Jogador;

// ---------- Prototipos ----------
void limparBufferEntrada(void);
void exibirMapa(Territorio *mapa, int tamanho);
void atacar(Territorio *atacante, Territorio *defensor);
void atribuirMissao(char *destino, char *missoes[], int totalMissoes);
int verificarMissao(const char *missao, Jogador *jogador, Territorio *mapa, int tamanho);
void exibirMissao(const char *missao);
void liberarMemoria(Territorio *mapa, Jogador *jogadores, int numJogadores);

// ---------- Utilitários ----------
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Exibe mapa (lista de territórios)
void exibirMapa(Territorio *mapa, int tamanho) {
    printf("\n--- MAPA ATUAL ---\n");
    for (int i = 0; i < tamanho; ++i) {
        printf("%2d: %s | Cor: %s | Tropas: %d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// Atribui uma missão aleatória (copia a string para 'destino' - destino deve ter espaço alocado)
void atribuirMissao(char *destino, char *missoes[], int totalMissoes) {
    int idx = rand() % totalMissoes;
    strcpy(destino, missoes[idx]);
}

// Exibe a missão (usada uma vez no início)
void exibirMissao(const char *missao) {
    printf("  >>> Sua missão: %s\n", missao);
}

// Função de ataque (usa 1 dado para cada lado, 1..6).
// Regras implementadas:
// - valida tropa mínima no chamador (assumimos que chamador já validou ataque a inimigo).
// - rola dados; se atacante vence, defensor perde 1 tropa; se defensor ficar <=0,
//   o defensor muda de cor para a do atacante e recebe metade (inteira) das tropas do atacante.
//   o atacante perde a metade transferida (garantindo pelo menos 1 no território atacante).
// - se defensor vence ou empata, atacante perde 1 tropa.
void atacar(Territorio *atacante, Territorio *defensor) {
    if (atacante->tropas <= 1) { // exige que atacante deixe pelo menos 1
        printf("[ERRO] Tropas insuficientes para atacar (é preciso ter mais de 1 tropa no território atacante).\n");
        return;
    }
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("[ERRO] Não é permitido atacar território da mesma cor.\n");
        return;
    }
    printf("\nBatalha: %s (%s - %d tropas) ATACA %s (%s - %d tropas)\n",
           atacante->nome, atacante->cor, atacante->tropas,
           defensor->nome, defensor->cor, defensor->tropas);

    int dadoAt = rand() % 6 + 1;
    int dadoDf = rand() % 6 + 1;
    printf("  Dado atacante: %d | Dado defensor: %d\n", dadoAt, dadoDf);

    if (dadoAt > dadoDf) {
        printf("  Resultado: Atacante venceu!\n");
        // defender perde 1 tropa inicialmente
        defensor->tropas -= 1;
        if (defensor->tropas <= 0) {
            // Atacante conquista: transfere metade das tropas do atacante (inteiro)
            int transferencia = atacante->tropas / 2;
            if (transferencia < 1) transferencia = 1; // ao menos 1 troop
            defensor->tropas = transferencia;
            atacante->tropas -= transferencia;
            if (atacante->tropas < 1) atacante->tropas = 1;
            // transfere cor
            strncpy(defensor->cor, atacante->cor, TAM_COR-1);
            defensor->cor[TAM_COR-1] = '\0';
            printf("  Território %s conquistado! Agora pertence a %s com %d tropas.\n",
                   defensor->nome, defensor->cor, defensor->tropas);
        } else {
            printf("  Defensor perdeu 1 tropa (restam %d).\n", defensor->tropas);
        }
    } else {
        printf("  Resultado: Defensor resistiu!\n");
        atacante->tropas -= 1;
        if (atacante->tropas < 1) atacante->tropas = 1;
        printf("  Atacante perde 1 tropa (restam %d).\n", atacante->tropas);
    }
}

// ---------- Verificação de Missões ----------
// A função identifica alguns padrões de missão simples e verifica a condição.
// Retorna 1 se cumprida, 0 caso contrário.
//
// Missões implementadas (exemplos):
// - "Conquistar 3 territorios seguidos"  -> verifica se existe sequência de 3 territórios consecutivos da cor do jogador.
// - "Eliminar todas as tropas da cor <cor>" -> verifica se soma de tropas da cor alvo é zero.
// - "Conquistar 4 territorios" -> verifica se jogador possui >=4 territórios (independe de consecutivos).
// - "Ter o maior exército" -> verifica se a soma de tropas do jogador é maior que de qualquer outra cor.
// - "Controlar todos os territorios de cor <cor>" -> verifica se jogador controla todos territórios que antes eram de cor X (esta missão exige que jogador tenha a cor X; para simplicidade trata como: controla todos os territórios que tenham cor X originalmente NÃO implementado histórico -> então esta missão será tratada como 'possuir todos territorios que atualmente tenham cor alvo' -> se jogador cor == alvo então trivially true; we keep simple)
int verificarMissao(const char *missao, Jogador *jogador, Territorio *mapa, int tamanho) {
    // Se missão nula, não cumprida
    if (!missao || !jogador || !mapa) return 0;

    // 1) Conquistar 3 territorios seguidos
    if (strstr(missao, "3 territorios seguidos") != NULL) {
        int consec = 0;
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0) {
                consec++;
                if (consec >= 3) return 1;
            } else {
                consec = 0;
            }
        }
        return 0;
    }

    // 2) Eliminar todas as tropas da cor <alguma cor>
    if (strstr(missao, "Eliminar todas as tropas da cor") != NULL) {
        // extrair a cor alvo da string (assume formato "Eliminar todas as tropas da cor <nome>")
        const char *ptr = strstr(missao, "cor");
        if (ptr) {
            // pula "cor" e possível espaço
            ptr += 3;
            while (*ptr == ' ') ptr++;
            char corAlvo[TAM_COR];
            strncpy(corAlvo, ptr, TAM_COR-1);
            corAlvo[TAM_COR-1] = '\0';
            // soma tropas da corAlvo
            int soma = 0;
            for (int i = 0; i < tamanho; ++i) {
                if (strcasecmp(mapa[i].cor, corAlvo) == 0) soma += mapa[i].tropas;
            }
            return (soma == 0) ? 1 : 0;
        }
    }

    // 3) Conquistar 4 territorios
    if (strstr(missao, "Conquistar 4 territorios") != NULL) {
        int conta = 0;
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0) conta++;
        }
        return (conta >= 4) ? 1 : 0;
    }

    // 4) Ter o maior exército (mais tropas que qualquer outra cor)
    if (strstr(missao, "Ter o maior exército") != NULL) {
        // soma tropas da cor do jogador
        int somaJog = 0;
        // soma tropas por cor (simplesmente compare com outras cores encontradas)
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0) somaJog += mapa[i].tropas;
        }
        // para cada cor diferente calcule soma e compare
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, jogador->cor) != 0) {
                int somaOutra = 0;
                for (int j = 0; j < tamanho; ++j)
                    if (strcmp(mapa[j].cor, mapa[i].cor) == 0) somaOutra += mapa[j].tropas;
                if (somaOutra >= somaJog) return 0; // existe cor com >= tropas
            }
        }
        return 1;
    }

    // 5) Controlar todos os territorios de cor <X>
    if (strstr(missao, "Controlar todos os territorios de cor") != NULL) {
        const char *ptr = strstr(missao, "cor");
        if (ptr) {
            ptr += 3;
            while (*ptr == ' ') ptr++;
            char corAlvo[TAM_COR];
            strncpy(corAlvo, ptr, TAM_COR-1);
            corAlvo[TAM_COR-1] = '\0';
            // verifica se todos os territórios que atualmente têm cor corAlvo pertencem ao jogador
            for (int i = 0; i < tamanho; ++i) {
                if (strcasecmp(mapa[i].cor, corAlvo) == 0) {
                    // se existe tal território e corAlvo != jogador->cor então missão não cumprida
                    if (strcmp(mapa[i].cor, jogador->cor) != 0) return 0;
                }
            }
            // se não encontrou territórios com corAlvo, considerar missão não cumprida
            // (ou poderia devolver true dependendo da definição). Vamos considerar: se não existe, não cumprida.
            // Mais razoável: se o jogador tem a cor alvo em absolutamente todos eles -> true.
            // Para simplicidade retornamos 0 se não houver territórios alvo.
            int existe = 0;
            for (int i = 0; i < tamanho; ++i) if (strcasecmp(mapa[i].cor, corAlvo) == 0) existe = 1;
            return existe ? 1 : 0;
        }
    }

    // Missão não reconhecida: marca não cumprida
    return 0;
}

// Libera memória de jogadores (missões alocadas) e mapa
void liberarMemoria(Territorio *mapa, Jogador *jogadores, int numJogadores) {
    if (mapa) free(mapa);
    if (jogadores) {
        for (int i = 0; i < numJogadores; ++i) {
            if (jogadores[i].missao) free(jogadores[i].missao);
        }
        free(jogadores);
    }
}

// -------------------- MAIN --------------------
int main(void) {
    srand((unsigned) time(NULL));

    int qtdTerritorios;
    printf("Quantos territorios deseja cadastrar? ");
    if (scanf("%d", &qtdTerritorios) != 1 || qtdTerritorios <= 0) {
        printf("Entrada invalida.\n");
        return 1;
    }
    limparBufferEntrada();

    // aloca mapa
    Territorio *mapa = (Territorio*) calloc(qtdTerritorios, sizeof(Territorio));
    if (!mapa) {
        printf("Erro de alocacao.\n");
        return 1;
    }

    // cadastra territórios
    for (int i = 0; i < qtdTerritorios; ++i) {
        printf("\n--- Territorio %d ---\n", i);
        printf("Nome: ");
        fgets(mapa[i].nome, TAM_NOME, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Cor do exército: ");
        fgets(mapa[i].cor, TAM_COR, stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';

        printf("Tropas: ");
        scanf("%d", &mapa[i].tropas);
        limparBufferEntrada();
    }

    // jogadores
    int numJogadores;
    printf("\nQuantos jogadores? (2 a 4 recomendado) ");
    if (scanf("%d", &numJogadores) != 1 || numJogadores <= 0) {
        printf("Entrada invalida.\n");
        free(mapa);
        return 1;
    }
    limparBufferEntrada();

    Jogador *jogadores = (Jogador*) calloc(numJogadores, sizeof(Jogador));
    if (!jogadores) {
        printf("Erro de alocacao de jogadores.\n");
        free(mapa);
        return 1;
    }

    // vetor de missoes pre-definidas (pelo menos 5)
    char *missoesPredefinidas[] = {
        "Conquistar 3 territorios seguidos",
        "Eliminar todas as tropas da cor vermelha",
        "Conquistar 4 territorios",
        "Ter o maior exército",
        "Controlar todos os territorios de cor azul"
    };
    int totalMissoes = sizeof(missoesPredefinidas) / sizeof(missoesPredefinidas[0]);

    // cadastra jogadores e atribui missão (aloca espaço dinamicamente para cada missão)
    for (int p = 0; p < numJogadores; ++p) {
        printf("\n--- Jogador %d ---\n", p+1);
        printf("Nome do jogador: ");
        fgets(jogadores[p].nome, TAM_NOME, stdin);
        jogadores[p].nome[strcspn(jogadores[p].nome, "\n")] = '\0';

        printf("Cor do jogador (ex: vermelho, azul): ");
        fgets(jogadores[p].cor, TAM_COR, stdin);
        jogadores[p].cor[strcspn(jogadores[p].cor, "\n")] = '\0';

        // aloca 100 bytes para a missão deste jogador (suficiente para as missoes simples)
        jogadores[p].missao = (char*) malloc(100 * sizeof(char));
        if (!jogadores[p].missao) {
            printf("Erro de alocacao de missao.\n");
            liberarMemoria(mapa, jogadores, numJogadores);
            return 1;
        }
        atribuirMissao(jogadores[p].missao, missoesPredefinidas, totalMissoes);
        printf("Missao atribuida ao jogador %s (sera mostrada agora):\n", jogadores[p].nome);
        exibirMissao(jogadores[p].missao);
    }

    // exibe mapa inicial
    exibirMapa(mapa, qtdTerritorios);

    // loop principal de ataques: após cada ataque verificamos as missoes
    char continuar = 's';
    while ((continuar == 's' || continuar == 'S')) {
        exibirMapa(mapa, qtdTerritorios);
        printf("\nEscolha indice do territorio atacante: ");
        int idxAtk, idxDef;
        if (scanf("%d", &idxAtk) != 1) break;
        printf("Escolha indice do territorio defensor: ");
        if (scanf("%d", &idxDef) != 1) break;
        limparBufferEntrada();

        if (idxAtk < 0 || idxAtk >= qtdTerritorios || idxDef < 0 || idxDef >= qtdTerritorios) {
            printf("[ERRO] Indices invalidos.\n");
            printf("Deseja continuar? (s/n): ");
            scanf(" %c", &continuar);
            limparBufferEntrada();
            continue;
        }

        // valida que atacante e defensor sejam de cores diferentes
        if (strcmp(mapa[idxAtk].cor, mapa[idxDef].cor) == 0) {
            printf("[ERRO] Nao e permitido atacar mesmo dono (mesma cor).\n");
        } else {
            atacar(&mapa[idxAtk], &mapa[idxDef]);
        }

        // verifica missões para cada jogador silenciosamente
        for (int p = 0; p < numJogadores; ++p) {
            if (verificarMissao(jogadores[p].missao, &jogadores[p], mapa, qtdTerritorios)) {
                printf("\n####################################################\n");
                printf("O JOGADOR %s (cor %s) CUMPRIU A MISSÃO: %s\n", jogadores[p].nome, jogadores[p].cor, jogadores[p].missao);
                printf(">>> VENCEDOR!\n");
                printf("####################################################\n");
                // encerra o jogo liberando memoria
                liberarMemoria(mapa, jogadores, numJogadores);
                return 0;
            }
        }

        printf("\nDeseja realizar outro ataque? (s/n): ");
        scanf(" %c", &continuar);
        limparBufferEntrada();
    }

    printf("\nFim do jogo sem missões cumpridas. Obrigado por jogar!\n");

    liberarMemoria(mapa, jogadores, numJogadores);
    return 0;
}

// Função utilitária para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.
