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


int cidentify (char *name, int size);
int ccreate (void* (*start)(void*), void *arg, int prio);
int cyield(void);
int cjoin(int tid);
int csem_init(csem_t *sem, int count);
int cwait(csem_t *sem);
int csignal(csem_t *sem);

void mainThread();
void endThread();
void runThread(TCB_t* _tcb);
void runNextThread();
void freeBlockedThreads();
void insertReadyQueue(TCB_t* tcb);
void updateJoinRequests(TCB_t* tcb);

ucontext_t* setup_empty_context();
ucontext_t* setup_context(ucontext_t* next_context);
ucontext_t* create_context(void* (*start)(void*), void *arg);
ucontext_t* create_end_context();



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

int cidentify(char *name, int size){
	char *names = "Filipe Santos - 273175\nCamilla - 237738\nMarcelo - 230090";
	strncpy(name,names,size);
	name[size - 1] = '\0'; //strnpy nao adiciona o fim da string
	return strlen(name)-strlen(names);
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
	mainThread();
	ucontext_t* context = create_context(start,arg);
	TCB_t* tcb = create_tcb(*context);
	insertReadyQueue(tcb);
	return tcb->tid;
}


/*
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/
int cyield(void){
	_runningTCB->prio += stopTimer();
	_runningTCB->state = PROCST_APTO;
	insertReadyQueue(_runningTCB);

	BOOL isRet = FALSE;
	getcontext(&(_runningTCB->context));
	if (isRet == FALSE){
		isRet = TRUE;
		freeBlockedThreads();
		runNextThread();
		return -1;//runNextThread nao volta
	}
	return 0;
}

/*
*Parâmetros:
*tid: identificador da thread cujo término está sendo aguardado.
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/
int cjoin(int tid){
	_runningTCB->prio += stopTimer();

	TCB_t* joinRequest;
	if ((joinRequest = findTBCbyTid(finished_queue, tid))!=NULL){	
		insertReadyQueue(_runningTCB);
	}
	else if ((joinRequest = findTBCbyTid(ready_queue, tid))!=NULL){
		_runningTCB->state = PROCST_BLOQ;
		InsertByPrio(blocked_queue, _runningTCB);
		joinRequest->_joinRequestTCB = _runningTCB;
	}
	else if ((joinRequest = findTBCbyTid(blocked_queue, tid))!=NULL){
		_runningTCB->state = PROCST_BLOQ;
		InsertByPrio(blocked_queue, _runningTCB);
		joinRequest->_joinRequestTCB = _runningTCB;
	}
	else{
		perror("Tid não encontrado");
		return (-1);
	}

	BOOL isRet = FALSE;
	getcontext(&(_runningTCB->context));
	if (isRet == FALSE){
		isRet = TRUE;
		freeBlockedThreads();
		runNextThread();
		return -1;//runNextThread nao volta
	}
	return 0;
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
	sem->count = count; // indica se recurso está ocupado ou não (livre > 0, ocupado ≤ 0)
    sem->fila = (PFILA2) malloc(sizeof(FILA2));
    if (CreateFila2(sem->fila) != 0) {return CSEM_INIT_ERROR;}
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
        sem->fila = (PFILA2) malloc(sizeof(FILA2));
        if (CreateFila2(sem->fila) != 0) {return CWAIT_ERROR;}
    }

    sem->count--;
    if (sem->count < 0) {
        //TCB_t* thread;
        //thread = _runningTCB;
        //thread->state = PROCST_BLOQ;
    	// coloca thread na fila de bloqueado
    	// passa a fila
        //_runningTCB = NULL;
        // swap do contexto
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
int csignal(csem_t *sem){
	//TODO
	return 0;
}

//##############################################################

void mainThread(){
	static BOOL inic = FALSE;
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
		ucontext_t* context = create_end_context();
		_runningTCB = create_tcb(*context);

		startTimer();
		inic = TRUE;
	}
}

void endThread(){
	_runningTCB->prio += stopTimer();
	_runningTCB->state = PROCST_TERMINO;
	InsertByPrio(finished_queue, _runningTCB);
	updateJoinRequests(_runningTCB);
	freeBlockedThreads();
	runNextThread();
}

void runThread(TCB_t* _tcb){
	//TODO
}

void runNextThread(){
	//TODO
	FirstFila2(ready_queue);
	_runningTCB = ready_queue->it->node;
	DeleteAtIteratorFila2(ready_queue);
	startTimer();
	setcontext(&(_runningTCB->context));
}

void freeBlockedThreads(){
	//TODO
}

void insertReadyQueue(TCB_t* tcb){
	tcb->state = PROCST_APTO;
	InsertByPrio(ready_queue, tcb);
}

void updateJoinRequests(TCB_t* tcb){
	if (tcb->_joinRequestTCB != NULL){
		TCB_t* _joinTCB = tcb->_joinRequestTCB;
		_joinTCB->state = PROCST_APTO;
		tcb->_joinRequestTCB = NULL;
	}
}

/*##############################################################
*	Funções para gerenciamento de contextos
*/

/*
*Inicia a criação de um contexto que possui uc_link = NULL.
*/
ucontext_t* setup_empty_context(){
	ucontext_t* context = malloc(sizeof(*context));
	if (context==NULL){
		perror("Erro ao alocar o contexto!");
		exit(-1);
	}
	if(getcontext(context)!=0){
		perror("Erro ao pegar o contexto!");
		exit(-1);
	}

    context->uc_stack.ss_sp = (void*) malloc(SIGSTKSZ);
    context->uc_stack.ss_size = SIGSTKSZ;

    if (context->uc_stack.ss_sp==NULL){
    	printf("ERRO AO ALOCAR A STACK!!");
    	exit(-1);
    }
    return context;
}

/*
*Inicia a criação de um contexto que possui o uc_link igual ao parametro next_context.
*/
ucontext_t* setup_context(ucontext_t* next_context){
	ucontext_t* context = setup_empty_context();
	context->uc_link = next_context;   
    return context;
}

/*
*Cria um contexto com a função indicada no parametro. Tambem associa o uc_link a funcão endThread.
*/
ucontext_t* create_context(void* (*start)(void*), void *arg){
	ucontext_t* context = setup_context(create_end_context());
    makecontext(context,(void*) start, 1, arg);
	return context;
}

/*
*Cria um contexto com a funcão endThread, tendo o uc_link = NULL
*/
ucontext_t* create_end_context(){
	ucontext_t* context = setup_empty_context();
    makecontext(context,&endThread,0);
	return context;
}





