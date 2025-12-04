// main.c暂时的测试文件

#include <stdio.h>

#include "context.h"
#include "scheduler.h"

void todo(void* arg) {
	printf("hello\n");
	yield();
	printf("world\n");
}

int main() {
	fprintf(stderr, "内存分配失败！\n");
	printf("hello wrold\n");
	scheduler_t* sch2 = SchCreate(ROUND_ROBIN);
	scheduler_t sch = {
		NULL,
		NULL,
		ROUND_ROBIN,
		0,
	};
	for (int i = 0; i < 10; i++) {
		push(sch2, todo, NULL);
		push(&sch, todo, NULL);
	}
	printf("%d\n", sch2->lenth);
	RunRoudRobin(sch2);//目前的Run是死循环
	RunRoudRobin(&sch);
	return 0;
}
