#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/util.h"

//#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef __TESTES__
#define __TESTES__


BOOL teste_cidentify(){
	char good[100];
	int i = cidentify(good, 100);
	
	if (i<0){
		printf("%s \n",good);
		return FALSE;
	}
	char bad[2];
	i = cidentify(bad, 2);
	
	if (i>=0){
		printf("\n%i | %s \n",i,bad);
		return FALSE;
	}
	return TRUE;
}
void* teste_ccreate_func(void* arg){
	while(1){
		printf("teste_ccreate_func\n");
	}
	return NULL;
}

BOOL teste_ccreate(){

	ccreate(teste_ccreate_func,(void*) NULL,0);
	return TRUE;
}

BOOL teste_create_tcb(){
	ucontext_t context;
	TCB_t* tcb1 = create_tcb(context);
	TCB_t* tcb2 = create_tcb(context);

	if (tcb1->tid != tcb2->tid - 1)
		return FALSE;
	return TRUE;
}

void* teste(void* arg){
	printf("Hello World!\n");
}

BOOL teste_create_context(){
	ccreate(teste,NULL,0);
	setcontext(create_context(teste,NULL));
}

int main(void){
	printf("teste_cidentify: %i \n",teste_cidentify());
	printf("teste_ccreate: %i \n",teste_ccreate());
	printf("teste_create_tcb: %i \n",teste_create_tcb());
	teste_create_context();
	return 0;
}

#endif