#include "context.h"

static VOID WINAPI fiber_entry(context_t ipParameter) {
	coroutine_t* co = (coroutine_t*)ipParameter;

	co->func(co->arg);

	co->state = FINISED;
	current_coroutine = NULL;

	SwitchToFiber(main_fiber);
}

// 创建协程
coroutine_t* create(void (*func)(void*), void* arg) {
	if (!main_fiber) {
		main_fiber = ConvertThreadToFiber(NULL);//WINPIA
		if (!main_fiber) {
			main_fiber = ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH);
		}
	}

	coroutine_t* co = (coroutine_t*)malloc(sizeof(coroutine_t));
	if (co == NULL) {
		perror("内存分配失败");
		return NULL;
	}

	co->func = func;
	co->arg = arg;
	co->state = READY;
	co->main_fiber = main_fiber;
	co->next = NULL;

	co->fiber = CreateFiber(0, fiber_entry, co);
	if (!co->fiber) {
		co->fiber = CreateFiberEx(0, 0, FIBER_FLAG_FLOAT_SWITCH, fiber_entry, co);
	}
	return co;
}

// resume(co)分配CPU,给协程
void resume(coroutine_t* co) {
	if (!co || co->state == FINISED) return;

	coroutine_t* prev = current_coroutine;
	current_coroutine = co;
	co->state = RUNNING;

	SwitchToFiber(co->fiber);

	current_coroutine = prev;
	if (co->state == RUNNING) {
		co->state = READY;
	}
}

// yield()让出CPU,给主线程,
// 不是阻塞,主线程可能会结束
void yield(void) {
	if (!current_coroutine) return;

	coroutine_t* co = current_coroutine;
	current_coroutine = NULL;

	SwitchToFiber(co->main_fiber);
}

// destroy(co)销毁协程
void destroy(coroutine_t* co) {
	if (co->fiber) {
		DeleteFiber(co->fiber);
	}
	free(co);
}
