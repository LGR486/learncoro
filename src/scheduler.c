//#include <stdlib.h>//malloc已经在window.h里
#include <stdio.h>

#include "scheduler.h"


void RunRoudRobin(scheduler_t* sch) {
	// 暂时没有中断
	while (sch->co != NULL) {
		resume(sch->co);

		// 移除完成的协程，执行下一个
		if (sch->co->state == FINISED) {
			coroutine_t* temp = sch->co;
			sch->co = sch->co->next; // 这里可以为NULL
			destroy(temp);
			sch->lenth--;
		}
		// 否则移动到末尾
		else {
			sch->eco->next = sch->co;
			sch->eco = sch->co;
			coroutine_t* temp = sch->co;
			sch->co = sch->co->next;
			temp->next = NULL;
		}
		
	}
}

void RunStaticPriority(scheduler_t2* sch) {
	// 多个队列
	for (int i = 9; i >= 0; i--) {
		//debug
		printf("running to queue [%d]\n", i);
		//end
		while (sch->queue[i] != NULL) {
			resume(sch->queue[i]);
			if (sch->queue[i]->state == FINISED) {
				coroutine_t* temp = sch->queue[i];
				sch->queue[i] = sch->queue[i]->next;
				destroy(temp);
				//lenth...
			}
			else {
				sch->endq[i]->next = sch->queue[i];
				sch->endq[i] = sch->queue[i];
				coroutine_t* temp = sch->queue[i];
				sch->queue[i] = sch->queue[i]->next;
				temp->next = NULL;
			}
		}
	}
}

void push(scheduler_t* sch, void (*func)(void*), void* arg) {
	coroutine_t* co = CreateCoroutine(func, arg);

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

int push2(scheduler_t2* sch, void (*func)(void*), void* arg, int priority) {
	priority -= 1;//1~10-- => 0~9
	if (priority < 0 || priority >= 10) {
		return 1;//优先级错误
	}
	coroutine_t* co = CreateCoroutine(func, arg);
	if (sch->queue[priority] == NULL) {
		sch->queue[priority] = co;
		sch->endq[priority] = co;
		//...lenth
	}
	else {
		sch->endq[priority]->next = co;
		sch->endq[priority] = co;
		//...lenth
	}
	return 0;
}

void* SchCreate(Algorithm alg) {
	switch (alg) {
	case ROUND_ROBIN: {
		// 轮询调度
		scheduler_t* sch = (scheduler_t*)malloc(sizeof(scheduler_t));
		if (sch == NULL) {
			fprintf(stderr, "内存分配失败！\n");
			return NULL;
		}
		sch->co = NULL;
		sch->eco = NULL;
		sch->lenth = 0;
		return sch;
	}
	case STATIC_PRIORITY: {
		// 静态优先级调度
		scheduler_t2* sch = (scheduler_t2*)malloc(sizeof(scheduler_t2));
		if (sch == NULL) {
			fprintf(stderr, "内存分配失败！\n");
			return NULL;
		}
		memset(sch, 0, sizeof(scheduler_t2));//sizeof(sch)?这会报错,通过反汇编可以看出只写了4个字节
		return sch;
	}
	}
}