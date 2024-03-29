#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <stdio.h>

#include "cthread.h"
#include "util.h"


//	FILAS DE THREAD
PFILA2 ready_queue; 
PFILA2 blocked_queue;
PFILA2 finished_queue;

//	TCB executando no momento
TCB_t* _runningTCB;

void startCThread();
int swapThread();
void endThread();
void runNextThread();
void freeBlockedThreads();
void updateJoinRequests(TCB_t* tcb);


/*
*Identificação do grupo:
*Parâmetros:
*name: ponteiro para uma área de memória onde deve ser escrito um string que contém os nomes dos componentes do
*grupo e seus números de cartão. Deve ser uma linha por componente.
*size: quantidade máxima de caracteres que podem ser copiados para o string de identificação dos componentes do
*grupo.
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/

int cidentify(char *str, int size){
	static char *id = "Filipe Santos - 273175\nCamilla - 237738\nMarcelo - 230090";
	strncpy(str,id,size);
	str[size - 1] = '\0'; //strnpy nao adiciona o fim da string
	return strlen(str)-strlen(id);
}

/*
*Parâmetros:
*start: ponteiro para a função que a thread executará.
*arg: um parâmetro que pode ser passado para a thread na sua criação. (Obs.: é um único parâmetro. Se for necessário
*passar mais de um valor deve-se empregar um ponteiro para uma struct)
*Retorno:
*Quando executada corretamente: retorna um valor positivo, que representa o identificador da thread criada
*Caso contrário, retorna um valor negativo.
*/
int ccreate (void* (*start)(void*), void *arg, int prio){
    startCThread();
	ucontext_t* context = create_context(start,arg,(void*) &endThread);
	TCB_t* tcb = create_tcb(context);
    tcb->state = PROCST_APTO;
    InsertByPrio(ready_queue, tcb);
	return tcb->tid;
}


/*
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/
int cyield(void){
    startCThread();
    updatePrio(_runningTCB);
	_runningTCB->state = PROCST_APTO;
    InsertByPrio(ready_queue, _runningTCB);
    return swapThread();
}

/*
*Parâmetros:
*tid: identificador da thread cujo término está sendo aguardado.
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/
int cjoin(int tid){
    startCThread();
    updatePrio(_runningTCB);

	TCB_t* joinRequest;
	if (findTCBbyTid(finished_queue, tid)==TRUE){
        _runningTCB->state = PROCST_APTO;
        InsertByPrio(ready_queue, _runningTCB);
	}
	else if (findTCBbyTid(ready_queue, tid)==TRUE){
		_runningTCB->state = PROCST_BLOQ;
		AppendFila2(blocked_queue, _runningTCB);
		joinRequest = ready_queue->it->node;
		AppendFila2(joinRequest->_joinRequestFILA2,_runningTCB);
	}
	else if (findTCBbyTid(blocked_queue, tid)==TRUE){
		_runningTCB->state = PROCST_BLOQ;
        AppendFila2(blocked_queue, _runningTCB);
		joinRequest = blocked_queue->it->node;
        AppendFila2(joinRequest->_joinRequestFILA2,_runningTCB);
	}
	else
        return -1;//Tid não encontrado

	return swapThread();
}
/*
*Parâmetros:
*sem: ponteiro para uma variável do tipo csem_t. Aponta para uma estrutura de dados que representa a variável semáforo.
*count: valor a ser usado na inicialização do semáforo. Representa a quantidade de recursos controlados pelo semáforo.
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/

#define CSEM_INIT_SUCCESS 0
#define CSEM_INIT_ERROR -1

int csem_init(csem_t *sem, int count){

    //Inicializa o semaforo
	sem->count = count; // indica se recurso está ocupado ou não (livre > 0, ocupado ≤ 0)
    sem->fila = malloc(sizeof(PFILA2*));
    if (CreateFila2(sem->fila) != 0) 
        return CSEM_INIT_ERROR;

    return CSEM_INIT_SUCCESS;
}

/*
*Parâmetros:
*sem: ponteiro para uma variável do tipo semáforo.
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/

#define CWAIT_SUCCESS 0
#define CWAIT_ERROR -1

int cwait (csem_t *sem) {
    startCThread();

    if (sem->fila == NULL) {
        sem->fila = malloc(sizeof(PFILA2*));
        if (CreateFila2(sem->fila) != 0) {return CWAIT_ERROR;}
    }

    sem->count--;
    if (sem->count < 0) {

        _runningTCB->state = PROCST_BLOQ;
        updatePrio(_runningTCB);

        AppendFila2(blocked_queue, _runningTCB);
        // Adiciona a thread atual no semaforo em questao ja que este semaforo esta bloqueado
        AppendFila2(sem->fila, (void*) _runningTCB->tid);

        swapThread();
    }
    return CWAIT_SUCCESS;
}

/*
*Parâmetros:
*sem: ponteiro para uma variável do tipo semáforo.
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/

#define CSIGNAL_SUCCESS 0
#define CSIGNAL_ERROR -1

int csignal(csem_t *sem){
    startCThread();

	sem->count++;
    if (sem->fila == NULL)
        return CSIGNAL_ERROR;

    if (FirstFila2(sem->fila) != 0)
        return CSIGNAL_SUCCESS;

    //Retorna o tid bloqueado armazenado no nodo da fila do semáforo.
    int tid = (int) GetAtIteratorFila2(sem->fila);

    if (findTCBbyTid(blocked_queue, tid)==FALSE)
        return CSIGNAL_ERROR;
    TCB_t* thread = GetAtIteratorFila2(blocked_queue);
    thread->state = PROCST_APTO;

    //insere na lista de aptos e apaga da lista de bloqueados
    InsertByPrio(ready_queue, thread);
    DeleteAtIteratorFila2(blocked_queue);

    //apaga da lista do semáforo
    DeleteAtIteratorFila2(sem->fila);

    return CSIGNAL_SUCCESS;
}

//##############################################################
/*
 * Usado para remover todos as TCB das listas e da em execução
 */
void resetCThread(){
    freeAllTCB(ready_queue);
    free(ready_queue);
    freeAllTCB(finished_queue);
    free(finished_queue);
    freeAllTCB(blocked_queue);
    free(blocked_queue);
    free(_runningTCB);
}
/*
 * Inicializa todas as variáveis necessárias
 */
void startCThread(){

	if (isInitialized()==FALSE){

        if (_runningTCB!=NULL)
            resetCThread();

		//alocacao e criacao das as filas
		ready_queue = malloc(sizeof(*ready_queue));
		finished_queue = malloc(sizeof(*finished_queue));
		blocked_queue = malloc(sizeof(*blocked_queue));

		if (ready_queue==NULL || 
			finished_queue==NULL || 
			blocked_queue==NULL){
			perror("Falha ao alocar memoria para as filas");
			exit(-1);
		}
			

		if (CreateFila2(ready_queue)!=0 || 
			CreateFila2(finished_queue)!=0 || 
			CreateFila2(blocked_queue)!=0){
			perror("Falha ao criar as filas");
			exit(-1);
		}
		
		//adiciona para a thread em execucao
		ucontext_t* context = create_end_context((void*) &endThread);
		_runningTCB = create_tcb(context);
        _runningTCB->state = PROCST_EXEC;

        startClock();
        setInitialized(TRUE);
	}
}
/*
 * Salva o contexto da thread atual e chama a funcão para iniciar a próxima thread
 */
int swapThread(){
    BOOL isRet = FALSE;
    getcontext(&(_runningTCB->context));
    if (isRet == FALSE){
        isRet = TRUE;
        runNextThread();
    }
    return 0;
}
/*
 * Função em que todas as threads retornam após acabarem.
 * Faz a atualização do tempo de execução, coloca a thread na fila de término e
 * coloca em execução a proxima thread.
 */
void endThread(){
	updatePrio(_runningTCB);
	_runningTCB->state = PROCST_TERMINO;
	AppendFila2(finished_queue, _runningTCB);
	updateJoinRequests(_runningTCB);
	runNextThread();
}
/*
 * Coloca a primeiraa thread da fila de aptos em execução,pois a fila foi inserida por prioridades.
 */
void runNextThread(){
	if(FirstFila2(ready_queue)==0){
		_runningTCB = ready_queue->it->node;
		DeleteAtIteratorFila2(ready_queue);
        _runningTCB->state = PROCST_EXEC;
		startClock();
		setcontext(&(_runningTCB->context));
	}
    perror("Sem threads na fila de aptos");
    exit(-1);
}

/*
*Libera todas as threads que não possuem mais impedimentos de serem aptas.
*Sendo assim, percorre a lista de bloqueados verificando se cada thread ja foi liberada, 
*passando ela para a lista de aptas.
*/
void freeBlockedThreads(){
    if (FirstFila2(blocked_queue)==0){
        do {
            TCB_t* tcb = blocked_queue->it->node;
            if(tcb->state == PROCST_APTO){
                InsertByPrio(ready_queue, GetAtIteratorFila2(blocked_queue));
                DeleteAtIteratorFila2(blocked_queue);
            } else
                NextFila2(blocked_queue);
        } while (blocked_queue->it!=NULL);
    }
}
/*
 * Percorre a lista de Join da thread em execução, as colocando em estado de APTO
 */
void updateJoinRequests(TCB_t* tcb){
    while(FirstFila2(tcb->_joinRequestFILA2)==0){
        TCB_t* _joinTCB = tcb->_joinRequestFILA2->it->node;
        _joinTCB->state = PROCST_APTO;
        DeleteAtIteratorFila2(tcb->_joinRequestFILA2);
    }
    freeBlockedThreads();
}


//APENAS PARA TESTES - Pega as informções das listas e thread em execução.
int getThreadsInfo(char *str, int size){
    char *info = malloc(sizeof(char)*1000);
    char aptos[100],bloq[100],ter[100];
    printFila2(ready_queue,aptos,100);
    printFila2(blocked_queue,bloq,100);
    printFila2(finished_queue,ter,100);
    sprintf(info,"thread atual: %s\naptos: %s\nbloqueados: %s\nterminos: %s",printTCB(_runningTCB),aptos,bloq,ter);
    strncpy(str,info,size);
    str[size - 1] = '\0';
    free(info);
    return strlen(str)-strlen(info);
}

//APENAS PARA TESTES - imprime todos os valores das listas e da thread em execução.
void printInfo(char *msg){
    char *info = malloc(sizeof(char)*500);
    getThreadsInfo(info, 500);
    printf("%s\n%s\n--------------------------\n",msg,info);
    free(info);
}


