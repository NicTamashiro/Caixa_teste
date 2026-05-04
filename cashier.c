#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_DENOMINACOES 13
#define FATOR 100

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

int ler_inteiro(const char *prompt){
    int v;
    int valido;
    do{
        printf("%s", prompt);
        valido = (scanf("%d", &v) == 1 && v>= 0);
        if(!valido) {
            printf("  Entrada invalida.\n\n");
            while(getchar() != '\n');   // limpar buffer
        }
    } while (!valido);
    return v;
}

void aguardar(){
    printf("\n  Pressione ENTER para continuar...");
    while (getchar() != '\n');  // descarta o \n velho do scanf
    while (getchar() != '\n');  // espera o usuario apertar Enter
}

void cadastrar(Caixa *c) {
    limpar_tela();
    cabecalho("CADASTRAR NOTAS E MOEDAS");
    printf("\n  Informe a quantidade de cada denominacao:\n\n");
    for (int i = 0; i < NUM_DENOMINACOES; i++){
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "  %s -> qtd: ", NOMES[i]); // guardar o texto numa variável para passar ele como argumento para a função ler_inteiro()
        c->quantidade[i] = ler_inteiro(prompt); // salva a quantidade digitada diretamente na caixa original
    }
    printf("\n  Caixa atualizado com sucesso!\n");
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