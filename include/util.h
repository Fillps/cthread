
#ifndef __UTIL__
#define __UTIL__

#include "support.h"
#include "cdata.h"
#include <ucontext.h>

#define BOOL 	int
#define TRUE 	1
#define FALSE 	0

BOOL isInitialized();
void setInitialized(BOOL ini);
void reset();

TCB_t* create_tcb(ucontext_t* context);
char* printTCB(TCB_t* tcb);

int InsertByPrio(PFILA2 pfila, TCB_t *tcb);
BOOL IsFilaEmpty(PFILA2 pFILA2);
BOOL findTCBbyTid(PFILA2 queue, int tid);
BOOL removeTCBbyTid(PFILA2 queue, int tid);
BOOL freeAllTCB(PFILA2 queue);
int printFila2(PFILA2 fila, char *str, int size);

void startClock();
double stopClock();
void updatePrio(TCB_t* tcb);

ucontext_t* setup_empty_context();
ucontext_t* setup_context(ucontext_t* next_context);
ucontext_t* create_context(void* (*start)(void*), void *arg,void* (*end)(void*));
ucontext_t* create_end_context(void* (*end)(void*));

#endif
