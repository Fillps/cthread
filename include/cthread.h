/*
 * cthread.h: arquivo de inclusão com os protótipos das funções a serem
 *            implementadas na realização do trabalho.
 *
 * NÃO MODIFIQUE ESTE ARQUIVO.
 *
 * VERSÃO: 11/09/2017
 *
 */
#ifndef __cthread__
#define __cthread__

#include "cdata.h"

int cidentify (char *str, int size);

int ccreate (void* (*start)(void*), void *arg, int prio);
//int csetprio(int tid, int prio);	// Essa função não será utilizada em 2017/2
int cyield(void);
int cjoin(int tid);
int csem_init(csem_t *sem, int count);
int cwait(csem_t *sem);
int csignal(csem_t *sem);

//###########################   APENAS PARA TESTES, DEVERA SER REMOVIDO   ############################
#include <ucontext.h>
void startCThread();
void endThread();
void reset();
int getThreadsInfo(char *str, int size);
int remove_thread(int tid, PFILA2 queue);
#endif
