#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_DENOMINACOES 13

const int VALORES[NUM_DENOMINACOES] = {
    20000, 10000, 5000, 2000, 1000, 500, 200, 100, 50, 25, 10, 5, 1
};

const char *NOMES[NUM_DENOMINACOES] = {
    "R$ 200,00", "R$ 100,00", "R$  50,00", "R$  20,00",
    "R$  10,00", "R$   5,00", "R$   2,00", "R$   1,00",
    "R$   0,50", "R$   0,25", "R$   0,10", "R$   0,05", "R$   0,01"
};

// Cria um tipo novo chamado Caixa.
// Sem typedef, precisaria escrever 'struct Caixa' toda vez que fosse usar.
typedef struct{
    int quantidade[NUM_DENOMINACOES];
} Caixa;

void limpar_tela(void) {
#ifdef _WIN32
    system("cls");  // Windows
#else
    system("clear");    // Mac / Linux
#endif
}

void linha(char c, int n){
    for (int i = 0; i < n; i++) putchar(c);
    putchar('\n');
}

void cabecalho(const char *titulo){
    linha('=', 35);
    printf("    %s\n", titulo);
    linha('=', 35);
}

void limpar_buffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

int ler_valor_centavos(const char *prompt){
    double v;
    int valido = 0;
    do{
        printf("%s", prompt);
        valido = (scanf("%lf", &v) == 1 && v >= 0);
        limpar_buffer();
        if(!valido){
            printf("    Valor invalido. Tente novamente.\n");
        }
    } while(!valido);
    return (int)(v * 100.0 + 0.5);
}

int ler_inteiro(const char *prompt){
    int v;
    int valido;
    do{
        printf("%s", prompt);
        valido = (scanf("%d", &v) == 1 && v>= 0);
        limpar_buffer();
        if(!valido) {
            printf("  Entrada invalida.\n\n");
        }
    } while (!valido);
    return v;
}

void aguardar(){
    printf("\n  Pressione ENTER para continuar...\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  // descarta o \n do scanf
    while ((c = getchar()) != '\n' && c != EOF);  // espera o Enter do usuário
}

long long saldo_centavos(const Caixa *c){
    long long total = 0;
    for (int i = 0; i < NUM_DENOMINACOES; i++){
        total += (long long)c->quantidade[i] * VALORES[i];
    }
    return total;
}

void cadastrar(Caixa *c) {
    limpar_tela();
    cabecalho("CADASTRAR NOTAS E MOEDAS");
    printf("\n  Informe a quantidade de cada denominacao:\n\n");
    for (int i = 0; i < NUM_DENOMINACOES; i++){
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "  %s -> qtd: ", NOMES[i]); // guardar o texto numa variável para passar ele como argumento para a função ler_inteiro()
        c->quantidade[i] = ler_inteiro(prompt); // salva a quantidade digitada diretamente na caixa original, nao apenas uma copia
    }
    printf("\n  Caixa atualizado com sucesso!\n");
    aguardar();
}

void ver_caixa(const Caixa *c){
    limpar_tela();
    cabecalho("EXTRATO DO CAIXA");
    printf("\n  %-12s  %-8s  %s\n", "Denominacao", "Qtd", "Subtotal");  
    linha('-', 55);
    for (int i = 0; i < NUM_DENOMINACOES; i++){
        long long sub = (long long)c->quantidade[i] * VALORES[i];
        printf("  %-12s  %-8d  R$ %8.2f\n", NOMES[i], c->quantidade[i], sub / 100.0);
    }
    linha('-', 55);
    printf("  %-22s  R$ %8.2f\n", "TOTAL EM CAIXA:", saldo_centavos(c) / 100.0);
    linha('=', 55);
    aguardar();
}

int decompor(Caixa *c, int troco_cts, int usados[NUM_DENOMINACOES], int simulacao){
    int temp[NUM_DENOMINACOES];
    memcpy(temp, c->quantidade, sizeof(temp));  // copia tudo isso pro array temp

    int restante = troco_cts;
    memset(usados, 0, NUM_DENOMINACOES * sizeof(int));  // preencher tudo com 0

    for(int i = 0; i < NUM_DENOMINACOES && restante > 0; i++){
        if (VALORES[i] > restante) continue;
        int qtd_necessaria = restante / VALORES[i];
        // Usa o menor valor entre o necessário para o troco e o disponível no caixa
        int qtd_usar = (qtd_necessaria < temp[i]) ? qtd_necessaria : temp[i];
        usados[i] = qtd_usar;              // guarda quantas notas/moedas desse valor foram usadas no troco
        temp[i] -= qtd_usar;              // diminui do estoque temporário a quantidade que foi usada
        restante -= qtd_usar * VALORES[i]; // reduz do troco o valor já pago com essas notas/moedas
    }

    if (restante != 0) return 0; // nao conseguiu dar o troco

    if (!simulacao) {
        memcpy(c->quantidade, temp, sizeof(temp));  //Copia o resultado final para o caixa real
    }

    return 1;
}

void simular_venda(Caixa *c){
    limpar_tela();
    cabecalho("SIMULAR VENDA E TROCO");

    int compra = ler_valor_centavos("\n  Valor da compra (R$): ");
    int pago = ler_valor_centavos("  Valor pago pelo cliente (R$): ");

    if(pago < compra){
        printf("\n  Valor insuficiente! O cliente pagou menos do que o total.\n");
        aguardar();
        return;
    }

    int troco = pago - compra;
    printf("\n  Troco a devolver: R$ %.2f\n", troco / 100.0);

    if(troco == 0){
        printf("   Sem troco. Venda concluida.\n");
        aguardar();
        return;
    }

    int usados[NUM_DENOMINACOES];
    if(!decompor(c,troco, usados, 0)){
        printf("\n  Caixa sem notas/moedas suficientes para o troco.\n");
        aguardar();
        return;
    }

    printf("\n  Composicao do troco:\n");
    linha('-', 40);
    for(int i = 0; i < NUM_DENOMINACOES; i++){
        if(usados[i] > 0){
            printf("  %s  x %d\n", NOMES[i], usados[i]);
        }
    }
    linha('-', 40);
    printf("\n  Venda e troco processados com sucesso!\n");
    printf("  Caixa atualizado.\n");
    aguardar();
}

int menu(){
    limpar_tela();
    cabecalho("SISTEMA DE GESTAO DE CAIXA");
    printf("\n");
    printf("  [1]  Cadastrar Notas/Moedas\n");
    printf("  [2]  Ver Caixa\n");
    printf("  [3]  Venda e Troco\n");
    printf("  [0]  Sair\n");
    printf("\n");
    return ler_inteiro("  Escolha uma opcao: ");
}

int main(){

    Caixa caixa;
    memset(&caixa, 0, sizeof(caixa));   // apaga o lixo, tudo vira 0

    int opcao;
    do{
        opcao = menu();
        switch(opcao) {
            case 1:
                cadastrar(&caixa);
                break;
            case 2: 
                ver_caixa(&caixa);     
                break;
            case 3: 
                simular_venda(&caixa); 
                break;
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