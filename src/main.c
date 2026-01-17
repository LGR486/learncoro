#include <stdio.h>

// ¡Ÿ ±≤‚ ‘Œƒº˛


#include "scheduler.h"

void todo(void* arg) {
	printf("hello\n");
	yield();
	printf("world\n");
}

void todo2(void* arg) {
	printf("H\n");
	yield();
	printf("F\n");
}

void test_priority() {
	scheduler_t2* sch = SchCreate(STATIC_PRIORITY);
	for (int i = 0; i < 5; i++) {
		push2(sch, todo2, NULL, 10);
	}
	for (int i = 0; i < 5; i++) {
		push2(sch, todo, NULL, 9);
	}
	RunStaticPriority(sch);
}

int main() {
	printf("hello wrold\n");

	scheduler_t* sch2 = SchCreate(ROUND_ROBIN);
	for (int i = 0; i < 10; i++) {
		push(sch2, todo, NULL);
	}
	printf("lenth: %d == 10\n", sch2->lenth);
	RunRoudRobin(sch2);
	printf("lenth: %d == 0\n", sch2->lenth);
	
	test_priority();

	return 0;
}