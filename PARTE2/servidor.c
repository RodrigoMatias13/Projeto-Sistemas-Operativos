/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 2 de Sistemas Operativos 2023/2024, Enunciado Versão 3+
 **
 ** Aluno: Nº: 123294      Nome: Rodrigo Matias
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo: Este módulo é dividido pelo servidor e pelo servidor dedicado, no primeiro vamos criamos um ficheiro onde vai dar para ler as informações do cliente e depois passando para o servidor dedicado onde se vai tratar do Check-in e do clienteRequest.
 **
 **
 ******************************************************************************/

// #define SO_HIDE_DEBUG                // Uncomment this line to hide all @DEBUG statements
#include "common.h"

/*** Variáveis Globais ***/
CheckIn clientRequest; // Variável que tem o pedido enviado do Cliente para o Servidor

/**
 * @brief Processamento do processo Servidor e dos processos Servidor Dedicado
 *        "os alunos não deverão alterar a função main(), apenas compreender o que faz.
 *         Deverão, sim, completar as funções seguintes à main(), nos locais onde está claramente assinalado
 *         '// Substituir este comentário pelo código da função a ser implementado pelo aluno' "
 */
int main () {
    // S1
    checkExistsDB_S1(FILE_DATABASE);
    // S2
    createFifo_S2(FILE_REQUESTS);
    // S3
    triggerSignals_S3(FILE_REQUESTS);

    int indexClient;       // Índice do cliente que fez o pedido ao servidor/servidor dedicado na BD

    // S4: CICLO1
    while (TRUE) {
        // S4
        clientRequest = readRequest_S4(FILE_REQUESTS); // S4: "Se houver erro (...) clientRequest.nif == -1"
        if (clientRequest.nif < 0)   // S4: "Se houver erro na abertura do FIFO ou na leitura do mesmo, (...)"
            continue;                // S4: "(...) e recomeça o Ciclo1 neste mesmo passo S4, lendo um novo pedido"

        // S5
        int pidServidorDedicado = createServidorDedicado_S5();
        if (pidServidorDedicado > 0) // S5: "o processo Servidor (pai) (...)"
            continue;                // S5: "(...) recomeça o Ciclo1 no passo S4 (ou seja, volta a aguardar novo pedido)"
        // S5: "o Servidor Dedicado (que tem o PID pidServidorDedicado) segue para o passo SD9"

        // SD9
        triggerSignals_SD9();
        // SD10
        CheckIn itemBD;
        indexClient = searchClientDB_SD10(clientRequest, FILE_DATABASE, &itemBD);
        // SD11
        checkinClientDB_SD11(&clientRequest, FILE_DATABASE, indexClient, itemBD);
        // SD12
        sendAckCheckIn_SD12(clientRequest.pidCliente);
        // SD13
        closeSessionDB_SD13(clientRequest, FILE_DATABASE, indexClient);
        so_exit_on_error(-1, "ERRO: O servidor dedicado nunca devia chegar a este ponto");
    }
}

/**
 *  "O módulo Servidor é responsável pelo processamento do check-in dos passageiros. 
 *   Está dividido em duas partes, um Servidor (pai) e zero ou mais Servidores Dedicados (filhos).
 *   Este módulo realiza as seguintes tarefas:"
 */

/**
 * @brief S1     Ler a descrição da tarefa S1 no enunciado
 * @param nameDB O nome da base de dados (i.e., FILE_DATABASE)
 */
void checkExistsDB_S1 (char *nameDB) {
    so_debug("< [@param nameDB:%s]", nameDB);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    struct stat st;

    // Ver se o arquivo existe
    if (stat(nameDB, &st) == -1) {
        so_error("S1", "");
        exit(0);
    }

    // Ver se temos as permissões
    if (!(st.st_mode & S_IRUSR) || !(st.st_mode & S_IWUSR)) {
        so_error("S1", "");
        exit(1);
    }else {
        so_success("S1", "");
    }
    so_debug(">");
}

/**
 * @brief S2       Ler a descrição da tarefa S2 no enunciado
 * @param nameFifo O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 */
void createFifo_S2 (char *nameFifo) {
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if (remove(nameFifo) == -1 && errno != ENOENT) {
        so_error("S2", "");
        exit(0);
    }

    // Cria arquivo
    if (mkfifo(nameFifo, 0666) == -1) {
        so_error("S2", "");
        exit(1);
    }else {
        so_success("S2", "");
    }
    so_debug(">");
}

/**
 * @brief S3   Ler a descrição da tarefa S3 no enunciado
 */
void triggerSignals_S3 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // SIGINT
    if (signal(SIGINT, trataSinalSIGINT_S6) == SIG_ERR) {
        so_error("S3", "");
        deleteFifoAndExit_S7();
    } else {
        so_success("S3", "");
    }

    // SIGCHLD
    if (signal(SIGCHLD, trataSinalSIGCHLD_S8) == SIG_ERR) {
        so_error("S3", "");
        deleteFifoAndExit_S7();
    } else {
        so_success("S3", "");
    }
    
    so_debug(">");
}

/**
 * @brief S4       O CICLO1 já está a ser feito na função main(). Ler a descrição da tarefa S4 no enunciado
 * @param nameFifo O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 * @return CheckIn Elemento com os dados preenchidos. Se nif=-1, significa que o elemento é inválido
 */
CheckIn readRequest_S4 (char *nameFifo) {
    CheckIn request;
    request.nif = -1;   // Por omissão retorna erro
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    FILE *file = fopen(nameFifo, "r");
    if (file == NULL) {
        so_error("S4","");
        deleteFifoAndExit_S7();
        return request;
    }

    if(fscanf(file, "%d %s %d", &request.nif, &request.senha, &request.pidCliente) != 3) {
        so_error("S4.1","");
        fclose(file);
        deleteFifoAndExit_S7();
        return request;
    }

    if(request.nif <= 0 || request.pidCliente <= 0) {
        so_error("S4.1","");
        fclose(file);
        deleteFifoAndExit_S7();
        return request;
    } 

    fclose(file);
    so_success("S4.1","%d %s %d", request.nif, request.senha, request.pidCliente);

    so_debug("> [@return nif:%d, senha:%s, pidCliente:%d]", request.nif, request.senha, request.pidCliente);
    return request;
}

/**
 * @brief S5   Ler a descrição da tarefa S5 no enunciado
 * @return int PID do processo filho, se for o processo Servidor (pai),
 *             0 se for o processo Servidor Dedicado (filho), ou -1 em caso de erro.
 */
int createServidorDedicado_S5 () {
    int pid_filho = -1;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    pid_filho = fork();

    if (pid_filho < 0) {
        so_error("S5", "");
        deleteFifoAndExit_S7();
    } else if (pid_filho == 0) {
        triggerSignals_SD9();
        pid_filho = 0;
    } else {
        so_success("S5","Servidor: Iniciei SD %d", pid_filho);
    }

    so_debug("> [@return:%d]", pid_filho);
    return pid_filho;
}

/**
 * @brief S6            Ler a descrição das tarefas S6 e S7 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGINT_S6 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    so_success("S6","Servidor: Start Shutdown");

    FILE *file = fopen("bd_passageiros.dat", "rb");
    if (file == NULL) {
        so_error("S6.1","");
        deleteFifoAndExit_S7();
    }else {
        so_success("S6.1","");
        
        CheckIn passageiro;
        while (fread(&passageiro, sizeof(CheckIn), 1, file) == 1) {
            if (passageiro.pidServidorDedicado > 0) {
                kill(passageiro.pidServidorDedicado, SIGUSR2);
                so_success("S6.3","Servidor: Shutdown SD %d", passageiro.pidServidorDedicado);
            }
        }
        fclose(file);
        so_success("S6.2","");
    }
    deleteFifoAndExit_S7();

    so_debug(">");
}

/**
 * @brief S7 Ler a descrição da tarefa S7 no enunciado
 */
void deleteFifoAndExit_S7 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if (unlink("server.fifo") == -1) {
        so_error("S7", "");
    }else {
        so_success("S7", "Servidor: End Shutdown");
    }
    
    so_debug(">");
    exit(0);
}

/**
 * @brief S8            Ler a descrição da tarefa S8 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGCHLD_S8 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno


    so_debug(">");
}

/**
 * @brief SD9  Ler a descrição da tarefa SD9 no enunciado
 */
void triggerSignals_SD9 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno


    so_debug(">");
}

/**
 * @brief SD10    Ler a descrição da tarefa SD10 no enunciado
 * @param request O pedido do cliente
 * @param nameDB  O nome da base de dados
 * @param itemDB  O endereço de estrutura CheckIn a ser preenchida nesta função com o elemento da BD
 * @return int    Em caso de sucesso, retorna o índice de itemDB no ficheiro nameDB.
 */
int searchClientDB_SD10 (CheckIn request, char *nameDB, CheckIn *itemDB) {
    int indexClient = 0;    // SD10:"inicia uma variável indexClient a 0"
    so_debug("< [@param request.nif:%d, request.senha:%s, nameDB:%s, itemDB:%p]", request.nif,
                                                                    request.senha, nameDB, itemDB);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    

    so_debug("> [@return:%d, nome:%s, nrVoo:%s]", indexClient, itemDB->nome, itemDB->nrVoo);
    return indexClient;
}

/**
 * @brief SD11        Ler a descrição da tarefa SD11 no enunciado
 * @param request     O endereço do pedido do cliente (endereço é necessário pois será alterado)
 * @param nameDB      O nome da base de dados
 * @param indexClient O índica na base de dados do elemento correspondente ao cliente
 * @param itemDB      O elemento da BD correspondente ao cliente
 */
void checkinClientDB_SD11 (CheckIn *request, char *nameDB, int indexClient, CheckIn itemDB) {
    so_debug("< [@param request:%p, nameDB:%s, indexClient:%d, itemDB.pidServidorDedicado:%d]",
                                    request, nameDB, indexClient, itemDB.pidServidorDedicado);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno


    so_debug("> [nome:%s, nrVoo:%s, pidServidorDedicado:%d]", request->nome,
                                                request->nrVoo, request->pidServidorDedicado);
}

/**
 * @brief SD12       Ler a descrição da tarefa SD12 no enunciado
 * @param pidCliente PID (Process ID) do processo Cliente
 */
void sendAckCheckIn_SD12 (int pidCliente) {
    so_debug("< [@param pidCliente:%d]", pidCliente);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno


    so_debug(">");
}

/**
 * @brief SD13          Ler a descrição da tarefa SD13 no enunciado
 * @param clientRequest O endereço do pedido do cliente
 * @param nameDB        O nome da base de dados
 * @param indexClient   O índica na base de dados do elemento correspondente ao cliente
 */
void closeSessionDB_SD13 (CheckIn clientRequest, char *nameDB, int indexClient) {
    so_debug("< [@param clientRequest:%p, nameDB:%s, indexClient:%d]", &clientRequest, nameDB,
                                                                                    indexClient);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno


    so_debug("> [pidCliente:%d, pidServidorDedicado:%d]", clientRequest.pidCliente, 
                                                          clientRequest.pidServidorDedicado);
}

/**
 * @brief SD14          Ler a descrição da tarefa SD14 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR2_SD14 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_success("SD14","SD: Recebi pedido do Servidor para terminar");
    exit(0);

    so_debug(">");
}