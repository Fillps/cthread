#include <stdlib.h>
#include "cthread.h"
#include "support.h"


//FILAS DE THREAD
PFILA2 p_apto; 
PFILA2 p_bloqueado;


//FUNCOES A SEREM IMPLEMENTADAS
int cidentify (char *name, int size);
int ccreate (void* (*start)(void*), void *arg, int prio);
int cyield(void);
int cjoin(int tid);
int csem_init(csem_t *sem, int count);
int cwait(csem_t *sem);
int csignal(csem_t *sem);

//IMPLEMENTACAO
int cyield(void){
	//TODO
	return 0;
}

