/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 2 de Sistemas Operativos 2023/2024, Enunciado Versão 3+
 **
 ** Aluno: Nº: 123294      Nome: Rodrigo Matias
 ** Nome do Módulo: cliente.c
 ** Descrição/Explicação do Módulo: Neste módulo interagimos com o cliente, vamos buscar a informação ao servidor(ver se exite ou não) passando para as informações do cliente para em seguida realizarmos a request do cliente.
 **
 **
 ******************************************************************************/

// #define SO_HIDE_DEBUG                // Uncomment this line to hide all @DEBUG statements
#include "common.h"

/**
 * @brief Processamento do processo Cliente
 *        "os alunos não deverão alterar a função main(), apenas compreender o que faz.
 *         Deverão, sim, completar as funções seguintes à main(), nos locais onde está claramente assinalado
 *         '// Substituir este comentário pelo código da função a ser implementado pelo aluno' "
 */
int main () {
    // C1
    checkExistsFifoServidor_C1(FILE_REQUESTS);
    // C2
    triggerSignals_C2();
    // C3 + C4
    CheckIn clientRequest = getDadosPedidoUtilizador_C3_C4();
    // C5
    writeRequest_C5(clientRequest, FILE_REQUESTS);
    // C6
    configureTimer_C6(MAX_ESPERA);
    // C7
    waitForEvents_C7();
    so_exit_on_error(-1, "ERRO: O cliente nunca devia chegar a este ponto");
}

/**
 *  "O módulo Cliente é responsável pela interação com o utilizador.
 *   Após o login do utilizador, este poderá realizar atividades durante o tempo da sessão.
 *   Assim, definem-se as seguintes tarefas a desenvolver:"
 */

/**
 * @brief C1       Ler a descrição da tarefa C1 no enunciado
 * @param nameFifo Nome do FIFO servidor (i.e., FILE_REQUESTS)
 */
void checkExistsFifoServidor_C1 (char *nameFifo) {
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    struct stat st;
    if (stat(nameFifo, &st) < 0) {
        so_error("C1","");
        exit(0);
    }

    if (!S_ISFIFO(st.st_mode)) {
        so_error("C1","");
        exit(1);
    }

    so_success("C1","");
    so_debug(">");
}

/**
 * @brief C2   Ler a descrição da tarefa C2 no enunciado
 */
void triggerSignals_C2 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if (signal(SIGUSR1, trataSinalSIGUSR1_C8) == SIG_ERR || signal(SIGHUP, trataSinalSIGHUP_C9) == SIG_ERR || signal(SIGINT, trataSinalSIGINT_C10) == SIG_ERR || signal(SIGALRM, trataSinalSIGALRM_C11) == SIG_ERR) {
        so_error("C2","");
        exit(0);
    } else {
        so_success("C2","");
    }
    
    so_debug(">");
}

/**
 * @brief C3+C4    Ler a descrição das tarefas C3 e C4 no enunciado
 * @return CheckIn Elemento com os dados preenchidos. Se nif=-1, significa que o elemento é inválido
 */
CheckIn getDadosPedidoUtilizador_C3_C4 () {
    CheckIn request;
    request.nif = -1;   // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    printf("Introduza o seu NIF: ");
    int nif;
    if (scanf("%d", &nif) != 1 || nif < 0 || nif > 999999999) {
        so_error("C3","NIF inválido\n");
        exit(0);
    }
    request.nif = nif;

    char password[100];
    printf("Introduza a sua password: ");
    scanf("%s", password);

    request.pidCliente = getpid();
    strcpy(request.senha, password);

    char success_message[100];
    snprintf(success_message, sizeof(success_message), "%d %s %d", request.nif, request.senha, request.pidCliente);
    so_success("C4","%d %s %d", request.nif, request.senha, request.pidCliente);

    so_debug("> [@return nif:%d, senha:%s, pidCliente:%d]", request.nif, request.senha, request.pidCliente);
    return request;
}

/**
 * @brief C5       Ler a descrição da tarefa C5 no enunciado
 * @param request  Elemento com os dados a enviar
 * @param nameFifo O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 */
void writeRequest_C5 (CheckIn request, char *nameFifo) {
    so_debug("< [@param request.nif:%d, request.senha:%s, request.pidCliente:%d, nameFifo:%s]",
                                        request.nif, request.senha, request.pidCliente, nameFifo);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    FILE *fifo = fopen(nameFifo, "w");

    if (fifo == NULL) {
        so_error("C5","");
        exit(0);
    }

    fprintf(fifo, "%d\n", request.nif);
    fprintf(fifo, "%s\n", request.senha);
    fprintf(fifo, "%d\n", request.pidCliente);

    if (fclose(fifo) != 0) {
        so_error("C5","");
        exit(1);
    }
    so_success("C5","");

    so_debug(">");
}

/**
 * @brief C6          Ler a descrição da tarefa C6 no enunciado
 * @param tempoEspera o tempo em segundos que queremos pedir para marcar o timer do SO (i.e., MAX_ESPERA)
 */
void configureTimer_C6 (int tempoEspera) {
    so_debug("< [@param tempoEspera:%d]", tempoEspera);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    alarm(tempoEspera);
    so_success("C6","Espera resposta em %d segundos", tempoEspera);

    so_debug(">");
}

/**
 * @brief C7 Ler a descrição da tarefa C7 no enunciado
 */
void waitForEvents_C7 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    pause();

    so_debug(">");
}

/**
 * @brief C8            Ler a descrição da tarefa C8 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR1_C8 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if (sinalRecebido == SIGUSR1) {
        so_success("C8","Check-in concluído com sucesso");
        exit(0);
    }

    so_debug(">");
}

/**
 * @brief C9            Ler a descrição da tarefa C9 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGHUP_C9 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if (sinalRecebido == SIGHUP) {
        so_success("C9","Check-in concluído sem sucesso");
        exit(1);
    }

    so_debug(">");
}

/**
 * @brief C10           Ler a descrição da tarefa C10 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGINT_C10 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
        if (sinalRecebido == SIGINT) {
        so_success("C10","Cliente: Shutdown");
        exit(0);
    }

    so_debug(">");
}

/**
 * @brief C11           Ler a descrição da tarefa C11 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGALRM_C11 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_error("C11","Cliente: Timeout");
    exit(0);

    so_debug(">");
}