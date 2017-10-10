 

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>


#include "util.h"
#include "support.h"
#include "cdata.h"

#define	NXTFILA_VAZIA		1
#define	NXTFILA_ITERINVAL	2
#define	NXTFILA_ENDQUEUE	3


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
*Inicia o TCB com os valores de criacao de thread
*/
TCB_t* create_tcb(ucontext_t context){
	static int tid_global = 0;

	TCB_t* tcb = malloc(sizeof(*tcb));
	tcb->tid = tid_global++;
	tcb->state = PROCST_CRIACAO;
	tcb->prio = PRIO_MAX;
	tcb->context = context;
	tcb->_joinRequestTCB = NULL;

	return tcb;
}

/*
*Procura uma TCB na fila por tid
*/
TCB_t* findTCBbyTid(PFILA2 queue, int tid){	
	
	if(FirstFila2(queue) == 0){
		FirstFila2(queue); //Seta o iterador da fila no primeiro elemento
		if(GetAtIteratorFila2(queue) != NULL)
			TCB_t* tcb = GetAtIteratorFila2(queue);	
	}
	while(tcb->tid != tid){
		if(NextFila2(queue) == 0)
			NextFila2(queue);
		if(GetAtIteratorFila2(queue) != NULL)
			tcb = GetAtIteratorFila2(queue); 
	}
	
	return tcb;
}

