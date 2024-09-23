#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define max 50
#define maxNome 60
#define maxSigla 10

typedef struct Pessoa Pessoa;
struct Pessoa {
    char nome[maxNome]; //nome da pessoa
    int idade; //idade da pessoa 
};

typedef struct Professor Professor;
struct Professor {
    Pessoa pes; //assim professor tem nome e idade
    int codigo; //código do professor
    char depto[maxSigla]; //departamento onde trabalha
};

typedef struct Aluno Aluno;
struct Aluno {
    Pessoa pes; //assim aluno tem nome e idade
    int matricula; //número de matrícula do aluno 
    int ano; //ano no curso. 
    char depto[maxSigla]; //departamento onde estuda
};

typedef struct TG TG;
struct TG {
    int codigo;
    int autor; //equivalente à matrícula do aluno autor
    int orientador; //equivalente ao código do professor
    char titulo[maxNome]; //título do TG 
    int qtdeVotos; //somatório dos votos recebidos
};

typedef struct Eleitor Eleitor;
struct Eleitor {
    char cpf[15];
    bool votou; //true se já votou; false se não votou
    int codigoTG;
};

Aluno formandos[max]; //vetor de alunos
int qtdeFormandos; //qtde de alunos no vetor

TG listaPIs[max]; //vetor de TGs
int qtdeTGs; //qtde de TGs no vetor

Professor docentes[max]; //vetor de professores
int qtdeDocentes; //qtde de professores no vetor

Eleitor comissao[max]; //vetor de eleitores
int qtdeEleitores; //qtde de eleitores no vetor


int verificaAluno(int codigo, const char *departamento);
int verificaProfessor(int codigo);
bool verificarCPF(const char *cpf);
void menuDois();

void lerAluno(){
    FILE *file;

    // Abre o arquivo para leitura
    file = fopen("aluno.txt", "r");
    if (file == NULL) {
        perror("Arquivo aluno.txt faltando");
        return;
    }

    // Lê a quantidade de alunos
    fscanf(file, "%d", &qtdeFormandos);
    
    // Lê as informações dos alunos
    for (int i = 0; i < qtdeFormandos; i++) {
        fscanf(file, "%d %d %s %d %[^\n]", 
               &formandos[i].matricula, 
               &formandos[i].ano, 
               formandos[i].depto, 
               &formandos[i].pes.idade, 
               formandos[i].pes.nome);
    }

    // Fecha o arquivo
    fclose(file);
}

void lerProfessor() {
    FILE *file;

    // Abre o arquivo para leitura
    file = fopen("professor.txt", "r");
    if (file == NULL) {
        perror("Arquivo professor.txt faltando");
        return;
    }

    // Lê a quantidade de professores
    fscanf(file, "%d", &qtdeDocentes);
    
    // Lê as informações dos professores
    for (int i = 0; i < qtdeDocentes; i++) {
        fscanf(file, "%d %s %d %[^\n]", 
               &docentes[i].codigo, 
               docentes[i].depto, 
               &docentes[i].pes.idade, 
               docentes[i].pes.nome);
    }

    // Fecha o arquivo
    fclose(file);
}

int lerPI(const char *depto, int totalPIs){
    FILE *file;
    char nomeArquivo[maxNome];
    int qtdTGsArquivo = 0;
    snprintf(nomeArquivo, sizeof(nomeArquivo), "PI_%s.txt", depto);

    // Abre o arquivo para leitura
    file = fopen(nomeArquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        perror("Erro");
        exit(EXIT_FAILURE);
    }

    // Lê a quantidade de pis
    fscanf(file, "%d", &qtdTGsArquivo);
    qtdeTGs = qtdeTGs + qtdTGsArquivo;
    
    // Lê as informações dos pis
    for (int i = totalPIs; i < qtdTGsArquivo + totalPIs; i++) {

        fscanf(file, "%d %d %d %[^\n]", 
               &listaPIs[i].codigo, 
               &listaPIs[i].autor, 
               &listaPIs[i].orientador, 
               listaPIs[i].titulo);

        // Verificaçao do aluno
        // código não encontrado
        int resultadoAluno = verificaAluno(listaPIs[i].autor, depto);
        if (resultadoAluno == -1) {
            printf("Erro no arquivo %s: Aluno com codigo %d nao encontrado.\n", nomeArquivo, listaPIs[i].autor);
            fclose(file);
            exit(EXIT_FAILURE);

        } else if (resultadoAluno == 0) { // aluno com departamento errado
            printf("Erro no arquivo %s: Aluno com codigo %d nao pertence ao departamento correto.\n", nomeArquivo, listaPIs[i].autor);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // Verificaçao do professor
        if (!verificaProfessor(listaPIs[i].orientador)) {
            printf("Erro no arquivo %s: Professor com codigo %d nao encontrado.\n", nomeArquivo, listaPIs[i].orientador);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    // Fecha o arquivo
    fclose(file);
    return totalPIs + 1;
}

void lerPis(){
    int totalPIs = 0;    // Total de PIs lidos de todos os arquivos

    // PIs
    totalPIs += lerPI("LOG", totalPIs);
    totalPIs += lerPI("AMS", totalPIs);
    totalPIs += lerPI("ADS", totalPIs);
    totalPIs += lerPI("DSM", totalPIs);
    totalPIs += lerPI("CEX", totalPIs);
    totalPIs += lerPI("EMP", totalPIs);
    totalPIs += lerPI("POL", totalPIs);
}

void lerComissao() {
    FILE *file;

    // Abre o arquivo para leitura
    file = fopen("comissao.txt", "r");
    if (file == NULL) {
        perror("Arquivo comissao.txt faltando");
        return;
    }

    // Lê a quantidade de professores
    fscanf(file, "%d", &qtdeEleitores);

    // Lê as informações dos professores
    for (int i = 0; i < qtdeEleitores; i++) {
        fscanf(file, "%s", comissao[i].cpf);

        if (!verificarCPF(comissao[i].cpf)) {
            printf("Erro no arquivo comissao.txt: CPF %s invalido\n", comissao[i].cpf);
            return;
        }
    }

    // Fecha o arquivo
    fclose(file);
}

void mostrarAluno(){
    // Exibe os alunos
    for (int i = 0; i < qtdeFormandos; i++) {
        printf("Aluno %d:\n", i + 1);
        printf("Codigo: %d\n", formandos[i].matricula);
        printf("Ano: %d\n", formandos[i].ano);
        printf("Departamento: %s\n", formandos[i].depto);
        printf("Idade: %d\n", formandos[i].pes.idade);
        printf("Nome: %s\n\n", formandos[i].pes.nome);
    }
}

void mostrarProfessor(){
    // Exibe os professores
    for (int i = 0; i < qtdeDocentes; i++) {
        printf("Professor %d:\n", i + 1);
        printf("Codigo: %d\n", docentes[i].codigo);
        printf("Departamento: %s\n", docentes[i].depto);
        printf("Idade: %d\n", docentes[i].pes.idade);
        printf("Nome: %s\n\n", docentes[i].pes.nome);
    }
}

void mostrarPI() {
   // Exibe os PIS
    for (int i = 0; i < qtdeTGs; i++) {
        printf("PI %d:\n", i + 1);
        printf("Codigo: %d\n", listaPIs[i].codigo);
        printf("Autor: %d\n", listaPIs[i].autor);
        printf("Orientador: %d\n", listaPIs[i].orientador);
        printf("Titulo: %s\n", listaPIs[i].titulo);
        printf("N de Votos: %d\n\n", listaPIs[i].qtdeVotos);
    }
}

void mostrarComissao() {
   // Exibe os PIS
    for (int i = 0; i < qtdeEleitores; i++) {
        printf("Comissao %d:\n", i + 1);
        printf("CPF: %s\n", comissao[i].cpf);
        printf("Votou: %d\n", comissao[i].votou);
        printf("TG: %d\n\n", comissao[i].codigoTG);
    }
}



// Funçao para verificar se o aluno existe
int verificaAluno(int codigo, const char *departamento) {
    for (int i = 0; i < qtdeFormandos; i++) {
        if (formandos[i].matricula == codigo) {
            if (strcmp(formandos[i].depto, departamento) == 0) {
                return 1; // Aluno encontrado e departamento correto
            }
            return 0; // Aluno encontrado, mas departamento errado
        }
    }
    return -1; // Aluno nao encontrado
}

// Funçao para verificar se o professor existe
int verificaProfessor(int codigo) {
    for (int i = 0; i < qtdeDocentes; i++) {
        if (docentes[i].codigo == codigo) {
            return 1; // Professor encontrado
        }
    }
    return 0; // Professor nao encontrado
}


// int verificarCPF (char cpf[15])
// {
//     //ATENCAO: Nao altere nada de cpf (variavel de entrada), pois 
//     //você estará alterando a string original também
//     int valido = 1;
//     // int num = 0;
//     //faltam outras condições nesse IF. Esse é só um exemplo
//     if(isdigit(cpf[0])==0 || cpf[3]!='.')
//     valido = 0;
//     // char aux[2]; //declaro uma string auxiliar
//     // aux[0] = cpf[0]; //quero saber o respectivo de '1'
//     // aux[1] = '\0'; //finalizo a string auxiliar
//     // num = atoi(aux); //atoi recebe string e retorna 1
//     return valido;
// }


bool verificarCPF(const char *cpf) {
    int i, j, digito1 = 0, digito2 = 0;
    int numeros[11];
    int count = 0;

    // Verifica se o CPF tem 14 caracteres no formato "000.000.000-00"
    if (strlen(cpf) != 14) return false;

    // Extrai os números do CPF ignorando pontos e hífen
    for (i = 0; i < 14; i++) {
        if (isdigit(cpf[i])) {
            numeros[count] = cpf[i] - '0';
            count++;
        } else if (cpf[i] != '.' && cpf[i] != '-') {
            return false; // Formato inválido
        }
    }

    // Verifica se foram extraídos exatamente 11 números
    if (count != 11) return false;

    // Verifica se todos os números são iguais, o que invalidaria o CPF
    bool todos_iguais = true;
    for (i = 1; i < 11; i++) {
        if (numeros[i] != numeros[0]) {
            todos_iguais = false;
            break;
        }
    }
    if (todos_iguais) return false;

    // Calcula o primeiro dígito verificador
    for (i = 0, j = 10; i < 9; i++, j--) {
        digito1 += numeros[i] * j;
    }
    digito1 = 11 - (digito1 % 11);
    if (digito1 >= 10) digito1 = 0;

    // Calcula o segundo dígito verificador
    for (i = 0, j = 11; i < 10; i++, j--) {
        digito2 += numeros[i] * j;
    }
    digito2 = 11 - (digito2 % 11);
    if (digito2 >= 10) digito2 = 0;

    // Verifica se os dígitos verificadores estão corretos
    return digito1 == numeros[9] && digito2 == numeros[10];
}

// void verificarComissao(){
//     FILE *file;
//     char eleitores[MAX][15];
//     int quantidadeEleitores;

//     // Abre o arquivo para leitura
//     file = fopen("comissao.txt", "r");
//     if (file == NULL) {
//         perror("Erro ao abrir o arquivo");
//         return;
//     }

//     // Lê a quantidade de eleitores
//     fscanf(file, "%d", &quantidadeEleitores);
//     for (int i = 0; i < quantidadeEleitores; i++) {
//         fscanf(file, "%s", eleitores[i]);
//     }

//     // Fecha o arquivo
//     fclose(file);

//     // Verifica os CPFs
//     for (int i = 0; i < quantidadeEleitores; i++) {
//         if (!verificarCPF(eleitores[i])) {
//             printf("CPF %s invalido\n", eleitores[i]);
//             return;
//         }
//     }

//     printf("Todos os CPFs sao validos.\n");
// }

void menuUm() {
    char opcao;

    do {
        // Exibe o menu
        printf("MENU1:\n");
        printf("a) Iniciar nova votacao\n");
        printf("b) Continuar votacao gravada\n");
        printf("Escolha uma opcao: ");
        
        // Lê a opção do usuário e converte para minúscula
        scanf(" %c", &opcao);
        opcao = tolower(opcao);

        // Verifica se a opção é válida
        if (opcao != 'a' && opcao != 'b') {
            printf("Opcao invalida! Tente novamente.\n\n");
        }
    } while (opcao != 'a' && opcao != 'b');

    // Respostas para cada opção
    if (opcao == 'a') {
        menuDois();
        // printf("Iniciando nova votacao...\n");
        // // Aqui você pode adicionar o código para iniciar a votação
    } else if (opcao == 'b') {
        printf("Continuando votacao gravada...\n");
        // Aqui você pode adicionar o código para continuar a votação gravada
    }
}

void menuDois(){
    char opcao;

    do {
        // Exibe o menu
        printf("MENU2:\n");
        printf("a) Entrar com voto\n");
        printf("b) Suspender votação\n");
        printf("c) Concluir votação\n");
        printf("Escolha uma opcao: ");
        
        // Lê a opção do usuário e converte para minúscula
        scanf(" %c", &opcao);
        opcao = tolower(opcao);

    } while (opcao != 'a' && opcao != 'b'  && opcao != 'c');
}

void menuTres(){
    char opcao;

    do {
        // Exibe o menu
        printf("MENU2:\n");
        printf("a) Entrar com voto\n");
        printf("b) Suspender votação\n");
        printf("c) Concluir votação\n");
        printf("Escolha uma opcao: ");
        
        // Lê a opção do usuário e converte para minúscula
        scanf(" %c", &opcao);
        opcao = tolower(opcao);

    } while (opcao != 'a' && opcao != 'b'  && opcao != 'c');
}


int main() {
//    carregarDados();
    // mostrarProfessor();
    // mostrarPI();

    lerProfessor();
    lerAluno();

    lerPis();
    mostrarPI();

    lerComissao();
    // mostrarComissao();

    // char opcao;
    // do {
    //     printf("\nMENU 1:\n");
    //     printf("a) Iniciar nova votação\n");
    //     printf("b) Continuar votação gravada\n");
    //     printf("Escolha uma opção: ");
    //     opcao = getchar();
    //     while (getchar() != '\n'); // Limpar buffer

    //     opcao = tolower(opcao);
    //     switch (opcao) {
    //         case 'a':
    //             iniciarVotacao();
    //             break;
    //         case 'b':
    //             // carregarVotosSalvos("parcial.txt");
    //             iniciarVotacao();
    //             break;
    //         default:
    //             printf("Opção inválida. Tente novamente.\n");
    //     }
    // } while (1);

    return 0;
}


// int main() {
//     // lerAluno();
//     // lerProfessor();
//     // lerPI("PI_AMS.txt");
//     // mostrarPI("PI_AMS.txt");

//     menuUm();
//     // verificarComissao();

//     // mostrarAluno();
//     // mostrarProfessor();
//     return 1;
// }
