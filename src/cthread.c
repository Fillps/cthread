#include <stdlib.h>
#include "cthread.h"
#include "support.h"
#include <string.h>

#include <stdio.h>


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
	char *names = "Filipe Santos - 273175\nCamilla - \nMarcelo - ";
	strncpy(name,names,size);
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
	//TODO
	return 0;
}


/*
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/
int cyield(void){
	//TODO
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
	//TODO
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
int csem_init(csem_t *sem, int count){
	//TODO
	return 0;
}

/*
*Parâmetros:
*sem: ponteiro para uma variável do tipo semáforo.
*Retorno:
*Quando executada corretamente: retorna 0 (zero)
*Caso contrário, retorna um valor negativo.
*/
int cwait(csem_t *sem){
	//TODO
	return 0;
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







