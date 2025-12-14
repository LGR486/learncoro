#include <stdio.h>

// ¡Ÿ ±≤‚ ‘Œƒº˛


#include "scheduler.h"

//extern void SwitchContext(context_t* old_ctx, context_t* new_ctx);

void todo(void* arg) {
	printf("hello\n");
	yield();
	printf("world\n");
}

int main() {
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
	printf("lenth: %d == 10\n", sch2->lenth);
	RunRoudRobin(sch2);
	RunRoudRobin(&sch);
	printf("lenth: %d == 0\n", sch.lenth);
	
	return 0;
}