
#ifndef __UTIL__
#define __UTIL__

#include "support.h"
#include "cdata.h"
#include <ucontext.h>

#define BOOL 	int
#define TRUE 	1
#define FALSE 	0

 int InsertByPrio(PFILA2 pfila, TCB_t *tcb);
 BOOL IsFilaEmpty(PFILA2 pFILA2);
 TCB_t* create_tcb(ucontext_t context);
 BOOL findTCBbyTid(PFILA2 queue, int tid);


 #endif
