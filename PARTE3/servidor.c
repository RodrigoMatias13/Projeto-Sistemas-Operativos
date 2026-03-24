/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos 2023/2024, Enunciado Versão 1+
 **
 ** Aluno: Nº:  123294     Nome: Rodrigo Matias
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo: Este módulo é dividido em duas partes,na primeira parte vamos fazer a gestão dos voos, primeiro vamos tratar do servidor (pai) onde vamos criar a  Message Queue (MSG), também vamos criar grupo de semáforos (SEM), vamos armar os sinais e criar um processo filho (fork) Servidor Dedicado que é a segunda parte desde módulo, nesta parte começamos por armar os sinais, passamos em seguida a percorrer a database e atualizamos a mesma, depois passamos para a escolha do lugar pelo cliente e a sua confirmação e por fim o encerramento do Servidor Dedicado. 
 **
 **
 ******************************************************************************/

// #define SO_HIDE_DEBUG                // Uncomment this line to hide all @DEBUG statements
#include "defines.h"

/*** Variáveis Globais ***/
int shmId;                              // Variável que tem o ID da Shared Memory
int msgId;                              // Variável que tem o ID da Message Queue
int semId;                              // Variável que tem o ID do Grupo de Semáforos
MsgContent clientRequest;               // Variável que serve para as mensagens trocadas entre Cliente e Servidor
DadosServidor *database = NULL;         // Variável que vai ficar com UM POINTER PARA a memória partilhada
int indexClient = -1;                   // Índice do passageiro que fez o pedido ao servidor/servidor dedicado na BD
int indexFlight = -1;                   // Índice do voo do passageiro que fez o pedido ao servidor/servidor dedicado na BD
int nrServidoresDedicados = 0;          // Número de servidores dedicados (só faz sentido no processo Servidor)

/**
 * @brief Processamento do processo Servidor e dos processos Servidor Dedicado
 *        "os alunos não deverão alterar a função main(), apenas compreender o que faz.
 *         Deverão, sim, completar as funções seguintes à main(), nos locais onde está claramente assinalado
 *         '// Substituir este comentário pelo código da função a ser implementado pelo aluno' "
 */
int main () {
    // S1
    shmId = initShm_S1();
    if (RETURN_ERROR == shmId) terminateServidor_S7();
    // S2
    msgId = initMsg_S2();
    if (RETURN_ERROR == msgId) terminateServidor_S7();
    // S3
    semId = initSem_S3();
    if (RETURN_ERROR == semId) terminateServidor_S7();
    // S4
    if (RETURN_ERROR == triggerSignals_S4()) terminateServidor_S7();

    // S5: CICLO1
    while (TRUE) {
        // S5
        int result = readRequest_S5();
        if (CICLO1_CONTINUE == result) // S5: "Se receber um sinal (...) retorna o valor CICLO1_CONTINUE"
            continue;                  // S5: "para que main() recomece automaticamente o CICLO1 no passo S5"
        if (RETURN_ERROR == result) terminateServidor_S7();
        // S6
        int pidServidorDedicado = createServidorDedicado_S6();
        if (pidServidorDedicado > 0)   // S6: "o processo Servidor (pai) (...) retorna um valor > 0"
            continue;                  // S6: "(...) recomeça o Ciclo1 no passo S4 (ou seja, volta a aguardar novo pedido)"
        if (RETURN_ERROR == pidServidorDedicado) terminateServidor_S7();
        // S6: "o Servidor Dedicado (...) retorna 0 para que main() siga automaticamente para o passo SD10

        // SD10
        if (RETURN_ERROR == triggerSignals_SD10()) terminateServidorDedicado_SD18();
        // SD11
        indexClient = searchClientDB_SD11();
        int erroValidacoes = TRUE;
        if (RETURN_ERROR != indexClient) {
            // SD12: "Se o passo SD11 concluiu com sucesso: (...)"
            indexFlight = searchFlightDB_SD12();
            if (RETURN_ERROR != indexFlight) {
                // SD13: "Se os passos SD11 e SD12 tiveram sucesso, (...)"
                if (!updateClientDB_SD13())
                    erroValidacoes = FALSE; // erroValidacoes = "houve qualquer erro nas validações dos passos SD11, SD12, ou SD13"
            }
        }
        // SD14: CICLO5
        int escolheuLugarDisponivel = FALSE;
        while (!escolheuLugarDisponivel) {
            // SD14.1: erroValidacoes = "houve qualquer erro nas validações dos passos SD11, SD12, ou SD13"
            if (RETURN_ERROR == sendResponseClient_SD14(erroValidacoes)) terminateServidorDedicado_SD18();
            if (erroValidacoes)
                terminateServidorDedicado_SD18();

            // SD15: "Se os pontos anteriores tiveram sucesso, (...)"
            if (RETURN_ERROR == readResponseClient_SD15()) terminateServidorDedicado_SD18();
            // SD16
            if (RETURN_ERROR == updateFlightDB_SD16())  // SD16: "Se lugarEscolhido no pedido NÃO estiver disponível (...) retorna erro (-1)"
                continue;                               // SD16: "para que main() recomece o CICLO5 em SD14"
            else
                escolheuLugarDisponivel = TRUE;
        }
        sendConfirmationClient_SD17();
        terminateServidorDedicado_SD18();
    }
}

/**
 *  "O módulo Servidor é responsável pelo processamento do check-in dos passageiros.
 *   Está dividido em duas partes, um Servidor (pai) e zero ou mais Servidores Dedicados (filhos).
 *   Este módulo realiza as seguintes tarefas:"
 */

/**
 * @brief S1: Ler a descrição da tarefa no enunciado
 * @return o valor de shmId em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int initShm_S1 () {
    shmId = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // S1.1 Validar os ficheiros bd_passageiros.dat e bd_voos.dat
    if (access(FILE_DATABASE_PASSAGEIROS, R_OK | W_OK) != 0 || access(FILE_DATABASE_VOOS, R_OK | W_OK) != 0) {
        so_error("S1.1", "");
        return RETURN_ERROR;
    }
    so_success("S1.1", "");

    // S1.2 Abrir a SHM com a KEY IPC_KEY
    shmId = shmget(IPC_KEY, sizeof(DadosServidor), 0666);
    if (shmId < 0) {
        if (errno != ENOENT) { 
            so_error("S1.2", "");
            return RETURN_ERROR;
        }
        so_success("S1.3", "");

        // S1.4 Criar SHM
        shmId = shmget(IPC_KEY, sizeof(DadosServidor), IPC_CREAT | IPC_EXCL | 0666);
        if (shmId < 0) {
            so_error("S1.4", "");
            return RETURN_ERROR;
        }
        database = (DadosServidor *)shmat(shmId, NULL, 0);
        if (database == (DadosServidor *)-1) {
            so_error("S1.4", "");
            return RETURN_ERROR;
        }
        so_success("S1.4", "%d", shmId);

        // S1.5 Iniciar as listas de passageiros e voos
        for (int i = 0; i < MAX_PASSENGERS; i++) {
            database->listClients[i].nif = PASSENGER_NOT_FOUND;
        }
        for (int i = 0; i < MAX_FLIGHTS; i++) {
            strcpy(database->listFlights[i].nrVoo, FLIGHT_NOT_FOUND);
        }
        so_success("S1.5", "");

        // S1.6 Ler o ficheiro bd_passageiros.dat e preenche a lista de passageiros na SHM
        FILE *fp_passageiros = fopen(FILE_DATABASE_PASSAGEIROS, "rb");
        if (fp_passageiros == NULL) {
            so_error("S1.6", "");
            return RETURN_ERROR;
        }
        for (int i = 0; i < MAX_PASSENGERS; i++) {
            if (fread(&database->listClients[i], sizeof(CheckIn), 1, fp_passageiros) != 1) {
                break;
            }
            database->listClients[i].pidCliente = PID_INVALID;
            database->listClients[i].pidServidorDedicado = PID_INVALID;
        }
        fclose(fp_passageiros);
        so_success("S1.6", "");

        // S1.7 Ler o ficheiro bd_passageiros.dat e preenche a lista de passageiros na SHM
        FILE *fp_voos = fopen(FILE_DATABASE_VOOS, "rb");
        if (fp_voos == NULL) {
            so_error("S1.7", "");
            return RETURN_ERROR;
        }
        for (int i = 0; i < MAX_FLIGHTS; i++) {
            if (fread(&database->listFlights[i], sizeof(Voo), 1, fp_voos) != 1) {
                break;
            }
        }
        fclose(fp_voos);
        so_success("S1.7", "");
    } else {
        so_success("S1.2", "");

        // S1.2.1 Colocar a database a apontar para essa SHM
        database = (DadosServidor *)shmat(shmId, NULL, 0);
        if (database == (DadosServidor *)-1) {
            so_error("S1.2.1", "");
            return RETURN_ERROR;
        }
        so_success("S1.2.1", "%d", shmId);
    }
    

    so_debug("> [@return:%d]", shmId);
    return shmId;
}

/**
 * @brief S2: Ler a descrição da tarefa no enunciado
 * @return o valor de msgId em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int initMsg_S2 () {
    msgId = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // S2.1: Se a MSG já existir, apaga
    msgId = msgget(IPC_KEY, 0666);
    if (msgId != -1) {
        if (msgctl(msgId, IPC_RMID, NULL) == -1) {
            so_error("S2.1", "");
            return RETURN_ERROR;
        }
        so_success("S2.1", "");
    }

    // S2.2: Criar a Message Queue com a KEY IPC_KEY
    msgId = msgget(IPC_KEY, IPC_CREAT | 0666);
    if (msgId == -1) {
        so_error("S2.2", "");
        return RETURN_ERROR;
    }

    so_success("S2.2", "%d", msgId);

    so_debug("> [@return:%d]", msgId);
    return msgId;
}

/**
 * @brief S3: Ler a descrição da tarefa no enunciado
 * @return o valor de semId em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int initSem_S3 () {
    semId = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // S3.1: Se o SEM já existir, apaga
    semId = semget(IPC_KEY, 0, 0666);
    if (semId != -1) {
        if (semctl(semId, 0, IPC_RMID) == -1) {
            so_error("S3.1", "");
            return RETURN_ERROR;
        }
        so_success("S3.1", "");
    }

    // S3.2: Criar um grupo de 3 semáforos com a KEY IPC_KEY
    semId = semget(IPC_KEY, 3, IPC_CREAT | IPC_EXCL | 0666);
    if (semId == -1) {
        so_error("S3.2", "");
        return RETURN_ERROR;
    }
    so_success("S3.2", "%d", semId);

    // S3.3: Iniciar os semáforos
    if (semctl(semId, SEM_PASSAGEIROS, SETVAL, 1) == -1 ||
        semctl(semId, SEM_VOOS, SETVAL, 1) == -1 ||
        semctl(semId, SEM_NR_SRV_DEDICADOS, SETVAL, 0) == -1) {
        so_error("S3.3", "semctl SETVAL");
        return RETURN_ERROR;
    }
    so_success("S3.3", "");

    so_debug("> [@return:%d]", semId);
    return semId;
}

/**
 * @brief S4: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int triggerSignals_S4 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // Armar o sinal SIGINT e SIGCHLD
    if (signal(SIGINT, trataSinalSIGINT_S8) == SIG_ERR || signal(SIGCHLD, trataSinalSIGCHLD_S9) == SIG_ERR) {
        so_error("S4","");
        result = RETURN_ERROR;
    } else {
        so_success("S4","");
        result = RETURN_SUCCESS;
    }


    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief S5: O CICLO1 já está a ser feito na função main(). Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int readRequest_S5 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // Ler da MSG uma mensagem do tipo MSGTYPE_LOGIN para clientRequest.
    MsgContent msg;
    ssize_t msgSize = msgrcv(msgId, &msg, sizeof(msg.msgData), MSGTYPE_LOGIN, 0);

    if (msgSize == -1) {
        if (errno == EINTR) {
            return CICLO1_CONTINUE;
        } else {
            so_error("S5", "");
            return RETURN_ERROR;
        }
    }

    clientRequest = msg;
    int nif = clientRequest.msgData.infoCheckIn.nif;
    char *senha = clientRequest.msgData.infoCheckIn.senha;
    int pidCliente = clientRequest.msgData.infoCheckIn.pidCliente;

    so_success("S5", "%d %s %d", nif, senha, pidCliente);
    result = RETURN_SUCCESS;


    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief S6: Ler a descrição da tarefa no enunciado
 * @return PID do processo filho, se for o processo Servidor (pai),
 *         0 se for o processo Servidor Dedicado (filho),
 *         ou PID_INVALID (-1) em caso de erro
 */
int createServidorDedicado_S6 () {
    int pid_filho = PID_INVALID;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // Criar um processo filho (fork) Servidor Dedicado
    pid_filho = fork();

    if (pid_filho < 0) {
        // Erro no fork
        so_error("S6","");
        pid_filho = PID_INVALID;
    } else if (pid_filho == 0) {
        // Processo filho
        so_success("S6","Servidor Dedicado: Nasci");
        so_debug("> [@return:%d]", RETURN_SUCCESS);
        return RETURN_SUCCESS;
    } else {
        // Processo pai
        nrServidoresDedicados++;
        so_success("S6","Servidor: Iniciei SD %d", pid_filho);
        so_debug("> [@return:%d]", pid_filho);
        return pid_filho;
    }

    so_debug("> [@return:%d]", pid_filho);
    return pid_filho;
}

/**
 * @brief S7: Ler a descrição da tarefa no enunciado
 */
void terminateServidor_S7 () {
    so_debug("<");
    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug(">");
    exit(0);
}

/**
 * @brief S8: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGINT_S8 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // Recebe o sinal
    if (sinalRecebido == SIGINT) {
        so_success("S8", "");

        terminateServidor_S7();
    }

    so_debug(">");
}

/**
 * @brief S9: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGCHLD_S9 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // Identificar o PID do Servidor Dedicado
    int status;
    pid_t pidServidorDedicado = wait(&status);

    so_success("S9", "Servidor: Confirmo fim de SD %d", pidServidorDedicado);

    so_debug(">");
}

/**
 * @brief SD10: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int triggerSignals_SD10 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // Armar o sinal SIGUSR1
    if (signal(SIGUSR1, trataSinalSIGUSR1_SD19) == SIG_ERR) {
        so_error("SD10","");
        so_debug("> [@return:%d]", result);
        return RETURN_ERROR;
    }

    // Armar o sinal SIGUSR2
    if (signal(SIGUSR2, trataSinalSIGUSR2_SD20) == SIG_ERR) {
        so_error("SD10","");
        so_debug("> [@return:%d]", result);
        return RETURN_ERROR;
    }

    // Programar o sinal SIGINT para ser ignorado
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        so_error("SD10","");
        so_debug("> [@return:%d]", result);
        return RETURN_ERROR;
    }

    so_success("SD10","");
    result = RETURN_SUCCESS;

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD11: Ler a descrição da tarefa no enunciado
 * @return indexClient em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int searchClientDB_SD11 () {
    indexClient = -1;    // SD11: Inicia a variável indexClient a -1 (índice da lista de passageiros de database)
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // SD11.1 Percorrer a lista de passageiros na database
    for (int i = 0; i < MAX_PASSENGERS; i++) {
        // SD11.2 Ver se o NIF do passageiro é igual ao NIF fornecido no clientRequest
        if (database->listClients[i].nif == clientRequest.msgData.infoCheckIn.nif) {
            // SD11.3 Comparar a senha do passageiro com o do pedido clientRequest
            if (strcmp(database->listClients[i].senha, clientRequest.msgData.infoCheckIn.senha) == 0) {
                indexClient = i;
                so_success("SD11.3", "%d", indexClient);
                so_debug("> [@return:%d]", indexClient);
                return indexClient;
            } else {
                so_error("SD11.3", "Cliente %d: Senha errada", clientRequest.msgData.infoCheckIn.nif);
                so_debug("> [@return:%d]", RETURN_ERROR);
                return RETURN_ERROR;
            }
        }
    }

    so_error("SD11.1", "Cliente %d: não encontrado", clientRequest.msgData.infoCheckIn.nif);

    so_debug("> [@return:%d]", indexClient);
    return indexClient;
}

/**
 * @brief SD12: Ler a descrição da tarefa no enunciado
 * @return indexFlight em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int searchFlightDB_SD12 () {
    indexFlight = -1;    // SD12: Inicia a variável indexFlight a -1 (índice da lista de voos de database)
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // SD12.1 Percorrer a lista de voos de database
    for (int i = 0; i < MAX_FLIGHTS; i++) {
        // SD12.2 Se encontrar o nrVoo do passageiro indexClient
        if (strcmp(database->listFlights[i].nrVoo, database->listClients[indexClient].nrVoo) == 0) {
            indexFlight = i;
            so_success("SD12.2", "%d", indexFlight);
            return indexFlight;
        }
    }

    so_error("SD12.1", "Voo %s: não encontrado", database->listClients[indexClient].nrVoo);
    return RETURN_ERROR;

    so_debug("> [@return:%d]", indexFlight);
    return indexFlight;
}

/**
 * @brief SD13: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int updateClientDB_SD13 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // SD13.1

    so_success("SD13.1", "Start Check-in: %d %d", database->listClients[indexClient].nif, clientRequest.msgData.infoCheckIn.pidCliente);

    // SD13.2 Validar se pidCliente é PID_INVALID e lugarEscolhido é EMPTY_SEAT
    if (database->listClients[indexClient].pidCliente != PID_INVALID || database->listClients[indexClient].lugarEscolhido != EMPTY_SEAT) {
        so_error("SD13.2", "Cliente %d: Já fez check-in", database->listClients[indexClient].nif);
        return RETURN_ERROR;
    }

    // SD13.3
    sleep(4);

    // SD13.4 Preencher os campos pidCliente e pidServidorDedicado
    database->listClients[indexClient].pidCliente = clientRequest.msgData.infoCheckIn.pidCliente;
    database->listClients[indexClient].pidServidorDedicado = getpid();

    // SD13.5
    so_success("SD13.5", "End Check-in: %d %d", database->listClients[indexClient].nif, clientRequest.msgData.infoCheckIn.pidCliente);
    return RETURN_SUCCESS;

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD14: Ler a descrição da tarefa no enunciado
 * @param erroValidacoes booleano que diz se houve algum erro nas validações de SD11, SD12 e SD13
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int sendResponseClient_SD14 (int erroValidacoes) {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("< [@param erroValidacoes:%d]", erroValidacoes);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD15: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int readResponseClient_SD15 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD16: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int updateFlightDB_SD16 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // SD16.1
    so_success("SD16.1","Start Reserva lugar: %s %d %d", clientRequest.msgData.infoVoo.nrVoo, clientRequest.msgData.infoCheckIn.nif, clientRequest.msgData.infoCheckIn.lugarEscolhido);

    // SD16.2 Validar, na database, o lugarEscolhido no pedido clientRequest está disponível (EMPTY_SEAT) no campo lugares
    if (database->listFlights[indexFlight].lugares[clientRequest.msgData.infoCheckIn.lugarEscolhido] != EMPTY_SEAT) {
        so_error("SD16.2","Cliente %d: Lugar já estava ocupado", clientRequest.msgData.infoCheckIn.nif);
        return RETURN_ERROR;
    }

    // SD16.3
    sleep(4);

    // SD16.4 Preencher o campo lugares[lugarEscolhido] com o valor do NIF do passageiro, registado no pedido clientRequest.
    database->listFlights[indexFlight].lugares[clientRequest.msgData.infoCheckIn.lugarEscolhido] = clientRequest.msgData.infoCheckIn.nif;

    // SD16.5 Preencher o campo lugarEscolhido com o valor lugarEscolhido, registado no pedido clientRequest.
    database->listClients[indexClient].lugarEscolhido = clientRequest.msgData.infoCheckIn.lugarEscolhido;

    // SD16.6
    so_success("SD16.6","End Reserva lugar: %s %d %d", clientRequest.msgData.infoVoo.nrVoo, clientRequest.msgData.infoCheckIn.nif, clientRequest.msgData.infoCheckIn.lugarEscolhido);
    return RETURN_SUCCESS;

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD17: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int sendConfirmationClient_SD17 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD18: Ler a descrição da tarefa no enunciado
 */
void terminateServidorDedicado_SD18 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // Verificar se indexClient é válido
    if (indexClient >= 0) {
        // Atualizar os campos  pidCliente e pidServidorDedicado com o valor PID_INVALID.
        if (database != NULL && indexClient < MAX_PASSENGERS) {
            database->listClients[indexClient].pidCliente = PID_INVALID;
            database->listClients[indexClient].pidServidorDedicado = PID_INVALID;
            so_success("SD18", "");
        } else {
            so_error("SD18", "");
        }
    } else {
        so_error("SD18", "");
    }

    so_debug(">");
    exit(0);
}

/**
 * @brief SD19: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR1_SD19 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // O servidor dedicado recebeu o sinal SIGUSR1
    so_success("SD19", "SD: Recebi pedido do Cliente para terminar");

    terminateServidorDedicado_SD18();

    so_debug(">");
}

/**
 * @brief SD20: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR2_SD20 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    // O servidor dedicado recebeu o sinal SIGUSR2
    so_success("SD20","SD: Recebi pedido do Servidor para terminar");

    // Verificar se o pidCliente é válido na mensagem clientRequest
    if (clientRequest.msgData.infoCheckIn.pidCliente != PID_INVALID) {
        kill(clientRequest.msgData.infoCheckIn.pidCliente, SIGHUP);
    }

    terminateServidorDedicado_SD18();


    so_debug(">");
}