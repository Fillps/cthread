
#ifndef __TESTES__
#define __TESTES__

#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/util.h"
#include "../include/support.h"

//#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

csem_t semaforo;

void printInfo(char *msg){
    char *info = malloc(sizeof(char)*500);
    getThreadsInfo(info, 500);
    printf("%s\n%s\n--------------------------\n",msg,info);
    free(info);
}

void* test_ccreate_func(void* arg){

}

void test_cidentify(void){
    char good[100];
    int i = cidentify(good, 100);
    assert(0==i);
    char bad[2];
    i = cidentify(bad, 2);
    assert(0>i);
    printf("%s: PASS\n", __func__);
}

void test_ccreate(void) {
    reset();

    //teste de continuidade dos tids
    assert(1==ccreate(test_ccreate_func,NULL,0));
    assert(2==ccreate(test_ccreate_func,NULL,0));
    assert(3==ccreate(test_ccreate_func,NULL,0));
    printf("%s: PASS\n", __func__);

}

void* test_cyield_func(void* arg){
    int *i = arg;
    *i = 1;
}

void test_cyield(void){
    reset();

    int *i = malloc(sizeof(int));
    *i = 0;
    int tid = ccreate(test_cyield_func,(void*) i,0);
    cyield();
    assert(*i==1);
    printf("%s: PASS\n", __func__);
}


void* test_cjoin_func(void* arg){
    int* i = arg;
    int val = *i;
    while (*i<val*val){
        cyield();
        *i += 1;
    }
}

void test_cjoin(void){
    reset();

    int v1 = Random2()/1000;
    int v2 = Random2()/1000;
    int* i = malloc(sizeof(int));
    int* j = malloc(sizeof(int));
    *i = v1;
    *j = v2;
    int t1 = ccreate(test_cjoin_func,i,0);
    int t2 = ccreate(test_cjoin_func,j,0);
    cjoin(t2);
    assert(*j == v2*v2);
    cjoin(t1);
    assert(*i == v1*v1);
    printf("%s: PASS\n", __func__);
}

void* aux_test_wait_signal(void* arg)
{
    printf("Inicio.\n");
    cwait(&semaforo);
    printf("Zona Critica.\n");
    csignal(&semaforo);
    printf("Fim.\n");
    return 0;
}

void test_wait_signal()
{
    int pid_filha;
    csem_init(&semaforo, 1);
    pid_filha = ccreate(aux_test_wait_signal,NULL,0);

    cwait(&semaforo);
    printf("MAIN - Zona Critica \n");
    cyield();cyield();cyield();cyield();
    csignal(&semaforo);
    cjoin(pid_filha);
    printf("MAIN Fim\n");

}

int main(void) {

	test_cidentify();
    test_ccreate();
    test_cyield();
    test_cjoin();
    test_wait_signal();

	return 0;
}
#endif