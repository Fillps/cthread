#include <stdlib.h>
#include <string.h>
#include <ucontext.h>

#include "cthread.h"
#include "support.h"
#include "util.h"
#include "cdata.h"

//Perguntar se pode usar
#include <signal.h>
//temp - para testes
#include <stdio.h>

//VARIAVEIS GLOBAIS

//	FILAS DE THREAD
PFILA2 ready_queue; 
PFILA2 blocked_queue;
PFILA2 finished_queue;

//	TCB executando no moment
TCB_t* _runningTCB;


int cidentify (char *str, int size);
int ccreate (void* (*start)(void*), void *arg, int prio);
int cyield(void);
int cjoin(int tid);
int csem_init(csem_t *sem, int count);
int cwait(csem_t *sem);
int csignal(csem_t *sem);

void startCThread();
int swapThread();
void endThread();
void runNextThread();
void freeBlockedThreads();
void updateJoinRequests(TCB_t* tcb);


int getThreadsInfo(char *str, int size);

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
	_runningTCB->prio += getRunningTime();
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
	_runningTCB->prio += getRunningTime();

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
    sem->fila = (PFILA2) malloc(sizeof(PFILA2));
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
    if (sem->fila == NULL) {
        sem->fila = (PFILA2) malloc(sizeof(PFILA2));
        if (CreateFila2(sem->fila) != 0) {return CWAIT_ERROR;}
    }

    sem->count--;
    if (sem->count <= 0) {
        TCB_t* thread;
        thread = _runningTCB;
        thread->state = PROCST_BLOQ;
    	
        AppendFila2(&blocked_queue, (void*) thread);
        // Adiciona a thread atual no semaforo em questao ja que este semaforo esta bloqueado
        AppendFila2(sem->fila, (void*) thread);
        
        _runningTCB = NULL;
        
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
	sem->count++;
    if (sem->fila == NULL) {
        return CSIGNAL_ERROR;
    }

    if (FirstFila2(sem->fila) != 0) {
        freeBlockedThreads();
        return CSIGNAL_SUCCESS;
    }

    
    TCB_t* thread;
    //Retorna o ponteiro armazenado no conteúdo do item endereçado pelo iterador da fila.
    thread = (TCB_t*) GetAtIteratorFila2(sem->fila);
    thread->state = PROCST_APTO;

    remove_thread(thread->tid, &blocked_queue);
    AppendFila2(&ready_queue, thread);
    DeleteAtIteratorFila2(sem->fila);
    
    return CSIGNAL_SUCCESS;

}

//##############################################################
static BOOL inic = FALSE;

void reset(){
    inic = FALSE;
    free(ready_queue);
    free(finished_queue);
    free(blocked_queue);
    free(_runningTCB);
    resetTID();
    startCThread();
}

void startCThread(){

	if (inic==FALSE){
		
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

        startClock();
		inic = TRUE;
	}
}

int swapThread(){
    BOOL isRet = FALSE;
    getcontext(&(_runningTCB->context));
    if (isRet == FALSE){
        isRet = TRUE;
        runNextThread();
    }
    return 0;
}

void endThread(){
	_runningTCB->prio += getRunningTime();
	_runningTCB->state = PROCST_TERMINO;
	AppendFila2(finished_queue, _runningTCB);
	updateJoinRequests(_runningTCB);
	runNextThread();
}

void runNextThread(){
	freeBlockedThreads();
	if(FirstFila2(ready_queue)==0){
		_runningTCB = ready_queue->it->node;
		DeleteAtIteratorFila2(ready_queue);
		startClock();
		setcontext(&(_runningTCB->context));
	}
    perror("Sem threads na fila de aptos");
    exit(-1);
}

/*
*Libera todas as threads que não possuem mais impedimentos de serem aptas.
*(por semáforo ou por join, se for join, o estado dela já foi mudado para apta, 
*mas ainda continua na lista de bloquedas).
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

void updateJoinRequests(TCB_t* tcb){
    while(FirstFila2(tcb->_joinRequestFILA2)==0){
        TCB_t* _joinTCB = tcb->_joinRequestFILA2->it->node;
        _joinTCB->state = PROCST_APTO;
        DeleteAtIteratorFila2(tcb->_joinRequestFILA2);
    }
}


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

/*
*
* Remover determinada thread da fila de threads.
*
*/

int remove_thread(int tid, PFILA2 queue){
    if(FirstFila2(queue) != 0) {
        return -1;
    }
    TCB_t* thread;
    do {
        if (queue->it == 0) {
            return -1;
        }
        thread = GetAtIteratorFila2(queue);
        if (thread == NULL) {
            return -1;
        }
        if (thread->tid == tid) {
            if (thread != NULL) {
                DeleteAtIteratorFila2(queue);
                return 0;
            }
            break;
        }
    } while (NextFila2(queue) == 0);
    return -1;
}


