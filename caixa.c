#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_DENOMINACOES 13
#define FATOR 100  /* Trabalhar com centavos para evitar erro de ponto flutuante */

static const int VALORES[NUM_DENOMINACOES] = {
    20000, 10000, 5000, 2000, 1000, 500, 200, 100, 50, 25, 10, 5, 1
};

static const char *NOMES[NUM_DENOMINACOES] = {
    "R$ 200,00", "R$ 100,00", "R$  50,00", "R$  20,00",
    "R$  10,00", "R$   5,00", "R$   2,00", "R$   1,00",
    "R$   0,50", "R$   0,25", "R$   0,10", "R$   0,05", "R$   0,01"
};

typedef struct {
    int quantidade[NUM_DENOMINACOES];
} Caixa;

/* ─── Utilitários ─────────────────────────────────────────────────────────── */

void limpar_tela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void linha(char c, int n) {
    for (int i = 0; i < n; i++) putchar(c);
    putchar('\n');
}

void cabecalho(const char *titulo) {
    linha('=', 55);
    printf("    %s\n", titulo);
    linha('=', 55);
}

/* Lê um valor monetário em reais e converte para centavos */
int ler_valor_centavos(const char *prompt) {
    double v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lf", &v) == 1 && v >= 0) {
            /* Arredondamento seguro para centavos */
            return (int)(v * 100.0 + 0.5);
        }
        printf("    Valor invalido. Tente novamente.\n");
        while (getchar() != '\n');
    }
}

int ler_inteiro(const char *prompt) {
    int v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &v) == 1 && v >= 0) return v;
        printf("  Entrada invalida.\n\n");
        while (getchar() != '\n');
    }
}

void aguardar(void) {
    printf("\n  Pressione ENTER para continuar...");
    while (getchar() != '\n');
    while (getchar() != '\n');
}

/* ─── Saldo total ─────────────────────────────────────────────────────────── */

long long saldo_centavos(const Caixa *c) {
    long long total = 0;
    for (int i = 0; i < NUM_DENOMINACOES; i++)
        total += (long long)c->quantidade[i] * VALORES[i];
    return total;
}

/* ─── Funcionalidade 1: Cadastrar notas/moedas ────────────────────────────── */

void cadastrar(Caixa *c) {
    limpar_tela();
    cabecalho("CADASTRAR NOTAS E MOEDAS");
    printf("\n  Informe a quantidade de cada denominacao:\n\n");
    for (int i = 0; i < NUM_DENOMINACOES; i++) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "  %s  -> qtd: ", NOMES[i]);
        c->quantidade[i] = ler_inteiro(prompt);
    }
    printf("\n  Caixa atualizado com sucesso!\n");
    aguardar();
}

/* ─── Funcionalidade 2: Ver caixa ─────────────────────────────────────────── */

void ver_caixa(const Caixa *c) {
    limpar_tela();
    cabecalho("EXTRATO DO CAIXA");
    printf("\n  %-12s  %-8s  %s\n", "Denominacao", "Qtd", "Subtotal");
    linha('-', 55);
    for (int i = 0; i < NUM_DENOMINACOES; i++) {
        long long sub = (long long)c->quantidade[i] * VALORES[i];
        printf("  %-12s  %-8d  R$ %8.2f\n",
               NOMES[i], c->quantidade[i], sub / 100.0);
    }
    linha('-', 55);
    printf("  %-22s  R$ %8.2f\n", "TOTAL EM CAIXA:",
           saldo_centavos(c) / 100.0);
    linha('=', 55);
    aguardar();
}

/* ─── Algoritmo de decomposição (greedy) ──────────────────────────────────── */

/*
 * Tenta compor 'troco_cts' (em centavos) usando as cédulas/moedas disponíveis.
 * Preenche 'usados[]' com a quantidade retirada de cada denominação.
 * Retorna 1 se conseguiu compor exatamente, 0 caso contrário.
 * Se simulacao==1, não altera o caixa (só testa); se 0, deduz do estoque.
 */
int decompor(Caixa *c, int troco_cts, int usados[NUM_DENOMINACOES], int simulacao) {
    /* Cópia temporária para não alterar em caso de falha */
    int temp[NUM_DENOMINACOES];
    memcpy(temp, c->quantidade, sizeof(temp));

    int restante = troco_cts;
    memset(usados, 0, NUM_DENOMINACOES * sizeof(int));

    for (int i = 0; i < NUM_DENOMINACOES && restante > 0; i++) {
        if (VALORES[i] > restante) continue;
        int qtd_necessaria = restante / VALORES[i];
        int qtd_usar = (qtd_necessaria < temp[i]) ? qtd_necessaria : temp[i];
        usados[i] = qtd_usar;
        temp[i]  -= qtd_usar;
        restante -= qtd_usar * VALORES[i];
    }

    if (restante != 0) return 0; /* Não foi possível */

    if (!simulacao) {
        memcpy(c->quantidade, temp, sizeof(temp));
    }
    return 1;
}

/* ─── Funcionalidade 3: Simular venda e troco ─────────────────────────────── */

void simular_venda(Caixa *c) {
    limpar_tela();
    cabecalho("SIMULAR VENDA E TROCO");

    int compra  = ler_valor_centavos("\n  Valor da compra  (R$): ");
    int pago    = ler_valor_centavos("  Valor pago pelo cliente (R$): ");

    if (pago < compra) {
        printf("\n  Valor insuficiente! O cliente pagou menos do que o total.\n");
        aguardar();
        return;
    }

    int troco = pago - compra;
    printf("\n  Troco a devolver: R$ %.2f\n", troco / 100.0);

    if (troco == 0) {
        /* Atualiza o caixa com o valor recebido */
        printf("\n   Sem troco. Venda concluida.\n");
        aguardar();
        return;
    }

    /* Verifica se o caixa comporta o troco */
    int usados[NUM_DENOMINACOES];
    if (!decompor(c, troco, usados, 0)) {
        printf("\n  Caixa sem notas/moedas suficientes para o troco.\n");
        aguardar();
        return;
    }

    /* Mostra a decomposição */
    printf("\n  Composicao do troco:\n");
    linha('-', 40);
    for (int i = 0; i < NUM_DENOMINACOES; i++) {
        if (usados[i] > 0)
            printf("  %s  x %d\n", NOMES[i], usados[i]);
    }
    linha('-', 40);
    printf("\n  Venda e troco processados com sucesso!\n");
    printf("  Caixa atualizado.\n");
    aguardar();
}


/* ─── Menu principal ──────────────────────────────────────────────────────── */

int menu(void) {
    limpar_tela();
    cabecalho("SISTEMA DE GESTAO DE CAIXA  v1.0");
    printf("\n");
    printf("  [1]  Cadastrar Notas/Moedas\n");
    printf("  [2]  Ver Caixa\n");
    printf("  [3]  Simular Venda e Troco\n");
    printf("  [0]  Sair\n");
    printf("\n");
    return ler_inteiro("  Escolha uma opcao: ");
}

int main(void) {
    Caixa caixa;
    memset(&caixa, 0, sizeof(caixa));

    int opcao;
    do {
        opcao = menu();
        switch (opcao) {
            case 1: cadastrar(&caixa);     break;
            case 2: ver_caixa(&caixa);     break;
            case 3: simular_venda(&caixa); break;
            case 0:
                limpar_tela();
                printf("Encerrando o sistema. Ate logo!\n\n");
                break;
            default:
                printf("    Opcao invalida.\n");
                aguardar();
        }
    } while (opcao != 0);

    return 0;
}
