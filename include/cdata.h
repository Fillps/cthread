/*
 * cdata.h: arquivo de inclusão de uso apenas na geração da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida
 *
 * Versão de 11/09/2017
 *
 */
#ifndef __cdata__
#define __cdata__
#include "ucontext.h"
#include "support.h"

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4

#define PRIO_MAX 0
#define PRIO_ALTA 1
#define PRIO_MEDIA 2
#define PRIO_BAIXA 3

/* Os campos "tid", "state", "prio" e "context" dessa estrutura devem ser mantidos e usados convenientemente
   Pode-se acrescentar outros campos APÓS os campos obrigatórios dessa estrutura
*/
typedef struct s_TCB { 
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	unsigned int	prio;		// prioridade da thread (higest=0; lowest=3)
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos) 
	
	/* Se necessário, pode-se acresecentar campos nessa estrutura A PARTIR DAQUI! */
	
	PFILA2 _joinRequestFILA2;
} TCB_t; 

typedef struct s_sem {
    int count; // indica se recurso está ocupado ou não (livre > 0, ocupado ≤ 0)
    PFILA2 fila; // ponteiro para uma fila de threads bloqueadas no semáforo.
} csem_t;


#endif
