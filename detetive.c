#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ============================================
// DEFINIÇÕES E CONSTANTES
// ============================================

#define TAMANHO_HASH 13  // Tamanho da tabela hash (número primo)
#define MAX_SUSPEITOS 5
#define MAX_PISTAS 20

// ============================================
// ESTRUTURAS DE DADOS
// ============================================

// Estrutura para um cômodo da mansão (árvore binária)
typedef struct Sala {
    char nome[30];              // Nome do cômodo
    char pista[100];            // Pista encontrada neste cômodo
    struct Sala *esquerda;      // Cômodo à esquerda
    struct Sala *direita;       // Cômodo à direita
} Sala;

// Estrutura para uma pista (nó da BST)
typedef struct PistaBST {
    char descricao[100];        // Descrição da pista
    struct PistaBST *esquerda;  // Subárvore esquerda
    struct PistaBST *direita;   // Subárvore direita
} PistaBST;

// Estrutura para entrada na tabela hash
typedef struct EntradaHash {
    char pista[100];            // Chave: descrição da pista
    char suspeito[30];          // Valor: nome do suspeito
    struct EntradaHash *proximo; // Encadeamento para colisões
} EntradaHash;

// Estrutura para tabela hash
typedef struct {
    EntradaHash *tabela[TAMANHO_HASH]; // Array de listas encadeadas
} TabelaHash;

// ============================================
// PROTÓTIPOS DAS FUNÇÕES
// ============================================

// Funções para a mansão (árvore binária)
Sala* criarSala(const char* nome, const char* pista);
void construirMansao(Sala** raiz);
void explorarMansao(Sala* salaAtual, PistaBST** raizPistas, TabelaHash* tabela);
void liberarMansao(Sala* raiz);

// Funções para árvore BST de pistas
PistaBST* criarNoPista(const char* descricao);
PistaBST* inserirPista(PistaBST* raiz, const char* descricao);
void listarPistasOrdenadas(PistaBST* raiz);
void liberarPistas(PistaBST* raiz);

// Funções para tabela hash
unsigned int hash(const char* chave);
void inicializarHash(TabelaHash* tabela);
void inserirNaHash(TabelaHash* tabela, const char* pista, const char* suspeito);
const char* encontrarSuspeito(TabelaHash* tabela, const char* pista);
void liberarHash(TabelaHash* tabela);

// Funções de jogo
void coletarPista(Sala* sala, PistaBST** raizPistas, TabelaHash* tabela);
int contarPistasPorSuspeito(TabelaHash* tabela, const char* suspeito, PistaBST* raizPistas);
void verificarSuspeitoFinal(TabelaHash* tabela, PistaBST* raizPistas);

// Funções auxiliares
void limparBuffer();
void exibirTitulo();
void exibirInstrucoes();
void toLowerString(char* str);

// ============================================
// IMPLEMENTAÇÃO DAS FUNÇÕES
// ============================================

// ========== FUNÇÕES PARA A MANSÃO ==========

/**
 * @brief Cria dinamicamente um novo cômodo da mansão
 * @param nome Nome do cômodo
 * @param pista Pista associada ao cômodo
 * @return Ponteiro para a sala criada
 */
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    
    strcpy(novaSala->nome, nome);
    strcpy(novaSala->pista, pista);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    
    return novaSala;
}

/**
 * @brief Constrói a mansão com cômodos e pistas pré-definidas
 * @param raiz Ponteiro para a raiz da árvore da mansão
 */
void construirMansao(Sala** raiz) {
    // Criação dos cômodos com suas pistas
    *raiz = criarSala("Entrada", 
        "Uma carta rasgada com a assinatura 'A' parcialmente visível");
    
    (*raiz)->esquerda = criarSala("Biblioteca", 
        "Um livro sobre venenos com a página 13 marcada");
    (*raiz)->direita = criarSala("Sala de Estar", 
        "Um copo quebrado com resíduos de um líquido roxo");
    
    (*raiz)->esquerda->esquerda = criarSala("Escritório", 
        "Extrato bancário mostrando uma grande transferência para 'Carlos'");
    (*raiz)->esquerda->direita = criarSala("Jardim de Inverno", 
        "Uma luva de jardinagem manchada de terra vermelha");
    
    (*raiz)->direita->esquerda = criarSala("Cozinha", 
        "Uma faca de chef ausente do estojo");
    (*raiz)->direita->direita = criarSala("Quarto Principal", 
        "Um diário com a entrada do dia do crime apagada");
    
    (*raiz)->esquerda->esquerda->esquerda = criarSala("Porão", 
        "Uma caixa com documentos antigos sobre herança familiar");
    (*raiz)->esquerda->esquerda->direita = criarSala("Varanda", 
        "Pegadas de sapato masculino tamanho 42 na lama");
    
    printf("✅ Mansão construída com 10 cômodos misteriosos!\n");
}

/**
 * @brief Explora a mansão de forma interativa
 * @param salaAtual Cômodo atual do jogador
 * @param raizPistas Raiz da árvore BST de pistas
 * @param tabela Tabela hash de associações pista-suspeito
 */
void explorarMansao(Sala* salaAtual, PistaBST** raizPistas, TabelaHash* tabela) {
    char escolha;
    
    do {
        printf("\n══════════════════════════════════════════════════════\n");
        printf("📍 VOCÊ ESTÁ NA/NO: %s\n", salaAtual->nome);
        printf("══════════════════════════════════════════════════════\n");
        
        printf("\nPistas nesta sala:\n");
        printf("🔍 %s\n", salaAtual->pista);
        
        printf("\nO que deseja fazer?\n");
        printf("[C] Coletar esta pista\n");
        
        if (salaAtual->esquerda != NULL) {
            printf("[E] Ir para %s (esquerda)\n", salaAtual->esquerda->nome);
        }
        
        if (salaAtual->direita != NULL) {
            printf("[D] Ir para %s (direita)\n", salaAtual->direita->nome);
        }
        
        printf("[S] Sair da mansão e acusar suspeito\n");
        printf("\nEscolha: ");
        
        scanf(" %c", &escolha);
        limparBuffer();
        escolha = toupper(escolha);
        
        switch (escolha) {
            case 'C':
                coletarPista(salaAtual, raizPistas, tabela);
                break;
                
            case 'E':
                if (salaAtual->esquerda != NULL) {
                    salaAtual = salaAtual->esquerda;
                } else {
                    printf("❌ Não há cômodo à esquerda!\n");
                }
                break;
                
            case 'D':
                if (salaAtual->direita != NULL) {
                    salaAtual = salaAtual->direita;
                } else {
                    printf("❌ Não há cômodo à direita!\n");
                }
                break;
                
            case 'S':
                printf("\n🚪 Saindo da mansão...\n");
                return;
                
            default:
                printf("❌ Opção inválida! Use C, E, D ou S.\n");
        }
        
    } while (1);
}

/**
 * @brief Libera a memória alocada para a mansão
 * @param raiz Raiz da árvore da mansão
 */
void liberarMansao(Sala* raiz) {
    if (raiz == NULL) return;
    
    liberarMansao(raiz->esquerda);
    liberarMansao(raiz->direita);
    free(raiz);
}

// ========== FUNÇÕES PARA ÁRVORE BST DE PISTAS ==========

/**
 * @brief Cria um novo nó para a árvore BST de pistas
 * @param descricao Descrição da pista
 * @return Ponteiro para o nó criado
 */
PistaBST* criarNoPista(const char* descricao) {
    PistaBST* novoNo = (PistaBST*)malloc(sizeof(PistaBST));
    if (novoNo == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    
    strcpy(novoNo->descricao, descricao);
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    
    return novoNo;
}

/**
 * @brief Insere uma pista na árvore BST em ordem alfabética
 * @param raiz Raiz da árvore BST
 * @param descricao Descrição da pista a ser inserida
 * @return Raiz atualizada da árvore
 */
PistaBST* inserirPista(PistaBST* raiz, const char* descricao) {
    if (raiz == NULL) {
        return criarNoPista(descricao);
    }
    
    // Compara as descrições para decidir onde inserir
    int comparacao = strcmp(descricao, raiz->descricao);
    
    if (comparacao < 0) {
        // Inserir na subárvore esquerda
        raiz->esquerda = inserirPista(raiz->esquerda, descricao);
    } else if (comparacao > 0) {
        // Inserir na subárvore direita
        raiz->direita = inserirPista(raiz->direita, descricao);
    } else {
        // Pista duplicada (não insere)
        printf("⚠️  Pista já coletada!\n");
    }
    
    return raiz;
}

/**
 * @brief Lista todas as pistas coletadas em ordem alfabética (in-order traversal)
 * @param raiz Raiz da árvore BST de pistas
 */
void listarPistasOrdenadas(PistaBST* raiz) {
    if (raiz == NULL) return;
    
    listarPistasOrdenadas(raiz->esquerda);
    printf("• %s\n", raiz->descricao);
    listarPistasOrdenadas(raiz->direita);
}

/**
 * @brief Libera a memória alocada para a árvore BST de pistas
 * @param raiz Raiz da árvore BST
 */
void liberarPistas(PistaBST* raiz) {
    if (raiz == NULL) return;
    
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

// ========== FUNÇÕES PARA TABELA HASH ==========

/**
 * @brief Função hash que converte uma string em um índice
 * @param chave String a ser hasheada
 * @return Índice na tabela hash
 */
unsigned int hash(const char* chave) {
    unsigned int valor = 0;
    
    for (int i = 0; chave[i] != '\0'; i++) {
        valor = (valor * 31 + chave[i]) % TAMANHO_HASH;
    }
    
    return valor;
}

/**
 * @brief Inicializa a tabela hash
 * @param tabela Ponteiro para a tabela hash
 */
void inicializarHash(TabelaHash* tabela) {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabela->tabela[i] = NULL;
    }
}

/**
 * @brief Insere uma associação pista-suspeito na tabela hash
 * @param tabela Ponteiro para a tabela hash
 * @param pista Chave: descrição da pista
 * @param suspeito Valor: nome do suspeito
 */
void inserirNaHash(TabelaHash* tabela, const char* pista, const char* suspeito) {
    unsigned int indice = hash(pista);
    
    // Cria nova entrada
    EntradaHash* novaEntrada = (EntradaHash*)malloc(sizeof(EntradaHash));
    if (novaEntrada == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    
    strcpy(novaEntrada->pista, pista);
    strcpy(novaEntrada->suspeito, suspeito);
    novaEntrada->proximo = NULL;
    
    // Verifica se há colisão
    if (tabela->tabela[indice] == NULL) {
        // Primeira entrada neste índice
        tabela->tabela[indice] = novaEntrada;
    } else {
        // Encadeamento: insere no início da lista
        novaEntrada->proximo = tabela->tabela[indice];
        tabela->tabela[indice] = novaEntrada;
    }
}

/**
 * @brief Busca um suspeito associado a uma pista na tabela hash
 * @param tabela Ponteiro para a tabela hash
 * @param pista Pista a ser buscada
 * @return Nome do suspeito ou NULL se não encontrado
 */
const char* encontrarSuspeito(TabelaHash* tabela, const char* pista) {
    unsigned int indice = hash(pista);
    EntradaHash* atual = tabela->tabela[indice];
    
    // Percorre a lista encadeada neste índice
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    
    return NULL; // Pista não encontrada
}

/**
 * @brief Libera a memória alocada para a tabela hash
 * @param tabela Ponteiro para a tabela hash
 */
void liberarHash(TabelaHash* tabela) {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        EntradaHash* atual = tabela->tabela[i];
        while (atual != NULL) {
            EntradaHash* proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
        tabela->tabela[i] = NULL;
    }
}

// ========== FUNÇÕES DE JOGO ==========

/**
 * @brief Coleta uma pista da sala atual e a adiciona às estruturas
 * @param sala Sala atual do jogador
 * @param raizPistas Raiz da árvore BST de pistas
 * @param tabela Tabela hash de associações
 */
void coletarPista(Sala* sala, PistaBST** raizPistas, TabelaHash* tabela) {
    printf("\n🔎 COLETANDO PISTA...\n");
    printf("Pista coletada: %s\n", sala->pista);
    
    // Insere na árvore BST
    *raizPistas = inserirPista(*raizPistas, sala->pista);
    
    printf("✅ Pista adicionada ao seu caderno de investigação!\n");
    
    // Conta pistas coletadas
    static int contadorPistas = 0;
    contadorPistas++;
    printf("Pistas coletadas até agora: %d\n", contadorPistas);
}

/**
 * @brief Conta quantas pistas apontam para um determinado suspeito
 * @param tabela Tabela hash de associações
 * @param suspeito Suspeito a ser verificado
 * @param raizPistas Raiz da árvore BST de pistas
 * @return Número de pistas que apontam para o suspeito
 */
int contarPistasPorSuspeito(TabelaHash* tabela, const char* suspeito, PistaBST* raizPistas) {
    if (raizPistas == NULL) return 0;
    
    int contagem = 0;
    
    // Função auxiliar recursiva para percorrer a BST
    void contarRecursivo(PistaBST* no) {
        if (no == NULL) return;
        
        // Verifica a subárvore esquerda
        contarRecursivo(no->esquerda);
        
        // Verifica o nó atual
        const char* suspeitoDaPista = encontrarSuspeito(tabela, no->descricao);
        if (suspeitoDaPista != NULL && strcmp(suspeitoDaPista, suspeito) == 0) {
            contagem++;
        }
        
        // Verifica a subárvore direita
        contarRecursivo(no->direita);
    }
    
    contarRecursivo(raizPistas);
    return contagem;
}

/**
 * @brief Conduz à fase final de julgamento e verifica a acusação
 * @param tabela Tabela hash de associações
 * @param raizPistas Raiz da árvore BST de pistas
 */
void verificarSuspeitoFinal(TabelaHash* tabela, PistaBST* raizPistas) {
    printf("\n══════════════════════════════════════════════════════\n");
    printf("                    FASE FINAL                       \n");
    printf("                 JULGAMENTO DO CRIME                 \n");
    printf("══════════════════════════════════════════════════════\n");
    
    // Lista as pistas coletadas
    printf("\n📋 PISTAS COLETADAS (em ordem alfabética):\n");
    printf("══════════════════════════════════════════════════════\n");
    
    if (raizPistas == NULL) {
        printf("❌ Nenhuma pista coletada! Você não tem evidências suficientes.\n");
        printf("💀 RESULTADO: Caso arquivado por falta de provas.\n");
        return;
    }
    
    listarPistasOrdenadas(raizPistas);
    
    // Lista os suspeitos possíveis
    printf("\n🔎 SUSPEITOS POSSÍVEIS:\n");
    printf("1. Ana - A herdeira desfavorecida\n");
    printf("2. Bruno - O mordomo leal\n");
    printf("3. Carlos - O sócio traído\n");
    printf("4. Diana - A rival de negócios\n");
    printf("5. Eduardo - O filho pródigo\n");
    
    // Solicita a acusação do jogador
    printf("\n🕵️  QUEM É O CULPADO?\n");
    printf("Digite o nome do suspeito (exatamente como listado): ");
    
    char acusacao[30];
    fgets(acusacao, sizeof(acusacao), stdin);
    acusacao[strcspn(acusacao, "\n")] = '\0'; // Remove newline
    
    // Converte para minúsculas para comparação case-insensitive
    toLowerString(acusacao);
    
    // Conta pistas que apontam para o suspeito acusado
    int pistasParaSuspeito = contarPistasPorSuspeito(tabela, acusacao, raizPistas);
    
    printf("\n══════════════════════════════════════════════════════\n");
    printf("                    VEREDICTO                         \n");
    printf("══════════════════════════════════════════════════════\n");
    
    printf("Suspeito acusado: %s\n", acusacao);
    printf("Pistas que apontam para %s: %d\n", acusacao, pistasParaSuspeito);
    
    // Determina o resultado baseado nas pistas
    if (pistasParaSuspeito >= 3) {
        printf("\n🎉 RESULTADO: ACUSAÇÃO SUSTENTADA!\n");
        printf("✅ O júri aceitou as evidências contra %s!\n", acusacao);
        printf("🏆 DETETIVE BEM-SUCEDIDO! Caso resolvido com maestria!\n");
    } else if (pistasParaSuspeito >= 2) {
        printf("\n⚠️  RESULTADO: ACUSAÇÃO PARCIALMENTE SUSTENTADA\n");
        printf("📋 Há algumas evidências, mas não são conclusivas.\n");
        printf("💼 O caso permanece em investigação.\n");
    } else if (pistasParaSuspeito == 1) {
        printf("\n❌ RESULTADO: ACUSAÇÃO FRACA\n");
        printf("📉 Evidências insuficientes para sustentar a acusação.\n");
        printf("🔍 Continue investigando!\n");
    } else {
        printf("\n💀 RESULTADO: ACUSAÇÃO INFUNDADA\n");
        printf("🚫 Nenhuma pista aponta para %s!\n", acusacao);
        printf("😞 O verdadeiro culpado escapou da justiça...\n");
    }
    
    // Revela o culpado real (para fins educacionais)
    printf("\n🤫 SEGREDO DO DESENVOLVEDOR:\n");
    printf("O culpado real do caso é 'Carlos', com 4 pistas contra ele.\n");
    printf("Para resolver o caso, colete pelo menos 3 pistas!\n");
}

// ========== FUNÇÕES AUXILIARES ==========

/**
 * @brief Limpa o buffer de entrada
 */
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Exibe o título do jogo
 */
void exibirTitulo() {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║              DETECTIVE QUEST: O MISTÉRIO            ║\n");
    printf("║               DA MANSÃO BLACKWOOD                  ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
}

/**
 * @brief Exibe as instruções do jogo
 */
void exibirInstrucoes() {
    printf("\n📖 INSTRUÇÕES DO JOGO:\n");
    printf("══════════════════════════════════════════════════════\n");
    printf("• Explore a mansão coletando pistas em cada cômodo\n");
    printf("• Use [E] para ir à esquerda, [D] para direita\n");
    printf("• Use [C] para coletar a pista do cômodo atual\n");
    printf("• Use [S] para sair e fazer sua acusação final\n");
    printf("• Você precisa de pelo menos 3 pistas contra um suspeito\n");
    printf("  para sustentar uma acusação!\n");
    printf("══════════════════════════════════════════════════════\n");
}

/**
 * @brief Converte uma string para minúsculas
 * @param str String a ser convertida
 */
void toLowerString(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// ============================================
// FUNÇÃO PRINCIPAL
// ============================================

int main() {
    // Estruturas principais do jogo
    Sala* mansao = NULL;
    PistaBST* pistasColetadas = NULL;
    TabelaHash tabelaHash;
    
    // Configuração inicial
    exibirTitulo();
    exibirInstrucoes();
    
    printf("\n🔨 CONSTRUINDO O CENÁRIO DO CRIME...\n");
    
    // 1. Constrói a mansão (árvore binária)
    construirMansao(&mansao);
    
    // 2. Inicializa a tabela hash
    inicializarHash(&tabelaHash);
    
    // 3. Preenche a tabela hash com associações pista-suspeito
    printf("\n🔗 ESTABELECENDO CONEXÕES ENTRE PISTAS E SUSPEITOS...\n");
    
    // Associações pré-definidas (pista → suspeito)
    inserirNaHash(&tabelaHash, 
        "Uma carta rasgada com a assinatura 'A' parcialmente visível", 
        "ana");
    inserirNaHash(&tabelaHash, 
        "Um livro sobre venenos com a página 13 marcada", 
        "diana");
    inserirNaHash(&tabelaHash, 
        "Um copo quebrado com resíduos de um líquido roxo", 
        "bruno");
    inserirNaHash(&tabelaHash, 
        "Extrato bancário mostrando uma grande transferência para 'Carlos'", 
        "carlos");
    inserirNaHash(&tabelaHash, 
        "Uma luva de jardinagem manchada de terra vermelha", 
        "bruno");
    inserirNaHash(&tabelaHash, 
        "Uma faca de chef ausente do estojo", 
        "carlos");
    inserirNaHash(&tabelaHash, 
        "Um diário com a entrada do dia do crime apagada", 
        "eduardo");
    inserirNaHash(&tabelaHash, 
        "Uma caixa com documentos antigos sobre herança familiar", 
        "ana");
    inserirNaHash(&tabelaHash, 
        "Pegadas de sapato masculino tamanho 42 na lama", 
        "carlos");
    inserirNaHash(&tabelaHash, 
        "Uma chave enferrujada com a inscrição 'Porão Secreto'", 
        "carlos");
    
    printf("✅ Conexões estabelecidas! Tabela hash pronta.\n");
    
    printf("\n🚪 ADENTRANDO A MANSÃO BLACKWOOD...\n");
    printf("O corpo foi encontrado na biblioteca. Boa sorte, detetive!\n");
    
    // 4. Inicia a exploração da mansão
    explorarMansao(mansao, &pistasColetadas, &tabelaHash);
    
    // 5. Fase final de julgamento
    verificarSuspeitoFinal(&tabelaHash, pistasColetadas);
    
    // 6. Limpeza de memória
    printf("\n🧹 FINALIZANDO O JOGO E LIBERANDO RECURSOS...\n");
    liberarMansao(mansao);
    liberarPistas(pistasColetadas);
    liberarHash(&tabelaHash);
    
    printf("\n🎬 FIM DO DETECTIVE QUEST!\n");
    printf("Obrigado por jogar! 🕵️‍♂️\n\n");
    
    return 0;
}