

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "util.h"

#define	NXTFILA_VAZIA		1
#define	NXTFILA_ITERINVAL	2
#define	NXTFILA_ENDQUEUE	3



static int tid_global = 0;


//*funcoes que definem se deve inicializar a cthread

static BOOL inic = FALSE;

BOOL isInic(){
    return inic;
}

void setInic(BOOL ini){
    if (ini==TRUE || ini==FALSE)
        inic = ini;
}

void reset(){
    tid_global = 0;
    setInic(FALSE);
}

/*
*Inicia o TCB com os valores de criacao de thread
*/
TCB_t* create_tcb(ucontext_t* context){

	TCB_t* tcb = malloc(sizeof(*tcb));
	tcb->tid = tid_global++;
	tcb->state = PROCST_CRIACAO;
	tcb->prio = PRIO_MAX;
	tcb->context = *context;
	tcb->_joinRequestFILA2 = malloc(sizeof(PFILA2*));
    CreateFila2(tcb->_joinRequestFILA2);
	return tcb;
}

char* printTCB(TCB_t* tcb){
    char *str = malloc(sizeof(char)*40);
    char *join = malloc(sizeof(char)*4);

    sprintf(str,"tid:%i tempo:%i",tcb->tid,tcb->prio);
    if (FirstFila2(tcb->_joinRequestFILA2)==0) {
        strcat(str, " join:");
        do {
            TCB_t *join_tcb = tcb->_joinRequestFILA2->it->node;
            sprintf(join, "%i ", join_tcb->tid);
            strcat(str, join);
        } while (NextFila2(tcb->_joinRequestFILA2) == 0);
    }
    return str;
}


/********************************************************************
	Vers. 17.2 - 11/09/2017
********************************************************************/
/*--------------------------------------------------------------------
Função: Insere um nodo na lista indicada, segundo o campo "prio" do TCB_t
	A fila deve estar ordenada (ou ter sido construída usado apenas essa funcao)
	O primeiro elemento da lista (first) é aquele com menor vamor de "prio"
Entra:	pfila -> objeto FILA2
	pnodo -> objeto a ser colocado na FILA2
Ret:	==0, se conseguiu
	!=0, caso contrário (erro)
--------------------------------------------------------------------*/
int	InsertByPrio(PFILA2 pfila, TCB_t *tcb) {
    TCB_t *tcb_it;

    // pfile vazia?
    if (FirstFila2(pfila)==0) {
        do {
            tcb_it = (TCB_t *) GetAtIteratorFila2(pfila);
            if (tcb->prio < tcb_it->prio) {
                return InsertBeforeIteratorFila2(pfila, tcb);
            }
        } while (NextFila2(pfila)==0);
    }
    return AppendFila2(pfila, (void *)tcb);
}


BOOL IsFilaEmpty(PFILA2 pFILA2){
    if (FirstFila2(pFILA2)==0)
        return FALSE;
    return TRUE;
}


/*
*Procura uma TCB na fila por tid
*return TRUE se achou
*	    FALSE se não foi encontrado
*/
BOOL findTCBbyTid(PFILA2 queue, int tid){
    if (FirstFila2(queue)==0){
        do {
            TCB_t* tcb = queue->it->node;
            if (tcb->tid==tid)
                return TRUE;
        } while (NextFila2(queue)==0);
    }
    return FALSE;
}
/*
*Procura uma TCB na fila por tid remove
*return TRUE se removeu
*	    FALSE se não foi encontrado
*/
BOOL removeTCBbyTid(PFILA2 queue, int tid) {
    if (findTCBbyTid(queue, tid)==TRUE) {
        DeleteAtIteratorFila2(queue);
        return TRUE;
    }
    return FALSE;
}

int printFila2(PFILA2 fila, char *str, int size){
    char *ret;
    if (IsFilaEmpty(fila)==TRUE){
        ret = "FILA VAZIA";
    } else{
        FirstFila2(fila);
        TCB_t* tcb =  fila->it->node;
        ret = printTCB(tcb);

        while (NextFila2(fila)==0){
            strcat(ret," | ");
            tcb =  fila->it->node;
            strcat(ret,printTCB(tcb));
        }
    }
    strncpy(str,ret,size);
    str[size - 1] = '\0';
    return strlen(str)-strlen(ret);
}


clock_t begin;

void startClock(){
    begin = clock();
}

unsigned int getRunningTime(){
    return clock() - begin;
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

    context->uc_stack.ss_sp = malloc(SIGSTKSZ);
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
ucontext_t* create_context(void* (*start)(void*), void *arg,void* (*end)(void*)){
    ucontext_t* context = setup_context(create_end_context(end));
    makecontext(context,(void*) start, 1, arg);
    return context;
}

/*
*Cria um contexto com a funcão endThread, tendo o uc_link = NULL
*/
ucontext_t* create_end_context(void* (*end)(void*)){
    ucontext_t* context = setup_empty_context();
    makecontext(context,(void*) end,0);
    return context;
}