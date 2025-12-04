// scheduler.c

//#include <stdlib.h>//malloc已经在window.h里
#include <stdio.h>

#include "scheduler.h"


void RunRoudRobin(scheduler_t* sch) {
	// 暂时没有删除队列，释放资源
	// 暂时没有中断
	// 暂时是死循环
	while (1) {
		resume(sch->co);
		sch->eco->next = sch->co;
		sch->eco = sch->co;
		coroutine_t* temp = sch->co;
		sch->co = sch->co->next;
		temp->next = NULL;
	}
}

void push(scheduler_t* sch, void (*func)(void*), void* arg) {
	coroutine_t* co = create(func, arg);
	if (sch->co == NULL) {
		sch->co = co;
		sch->eco = co;
		sch->lenth = 1;
	}
	else {
		sch->eco->next = co;
		sch->eco = co;
		sch->lenth++;
	}
}

scheduler_t* SchCreate(Algorithm alg) {
	scheduler_t* sch = (scheduler_t*)malloc(sizeof(scheduler_t));
	if (sch == NULL) {
		fprintf(stderr, "内存分配失败！\n");
		return NULL;
	}
	sch->co = NULL;
	sch->eco = NULL;
	sch->alg = alg;
	sch->lenth = 0;
	return sch;
}
