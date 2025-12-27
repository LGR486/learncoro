#include "context.h"

#if defined(MYSELF)
// 每个协程的入口
static void co_entry(coroutine_t* co) {
	co->func(co->arr);
	co->state = FINISED;

	current_coroutine = NULL;
	SwitchContext(co->ctx, main_ctx);
	// 交给主线程释放内存
}

// 必须初始化
context_t* CreateContext(void (*func)()) {
	context_t* ctx = (context_t*)malloc(sizeof(context_t));
	if (ctx == NULL) {
		fprintf(stderr, "CreateContext:内存分配失败");
		return NULL;
	}
	memset(ctx, 0, sizeof(context_t));
	//ctx->eip = 0;
	//ctx->ebx = 0;
	//ctx->ecx = 0;
	//ctx->edx = 0;
	//ctx->edi = 0;
	//ctx->esi = 0;
	//ctx->ebp = 0;
	//ctx->esp = 0;

	// make...???放在CreateCoroutine

	//ctx->esp = func; //

	return ctx;
}

// 创建协程并初始化协程栈
coroutine_t* CreateCoroutine(void (*func)(void*),void* arr) {
	coroutine_t* co = (coroutine_t*)malloc(sizeof(coroutine_t));
	if (co == NULL) {
		fprintf(stderr, "CreateCoroutine:内存分配失败");
		return NULL;
	}

	//memset(co, 0, sizeof(co)); //必须初始化，否则被调试器判断未初始化，0xCDCDCDCD
	
	co->main = getptr_main_context();
	co->state = READY;
	co->next = NULL;

	co->func = func;
	co->arr = arr;

	co->ctx = CreateContext(co_entry);// bug

	// coroutine_stack_init 
	// 要始终保持16字节对齐
	// 可以通过反汇编来确定栈布局，这是最准确的
	co->stack_size = 8 * 1024;
	char* sp = co->stack + co->stack_size - sizeof(co);
	sp = (void*)((unsigned long)sp & ~15);
	*(coroutine_t**)(sp) = co;

	sp = sp - sizeof(void*)*2;//返回地址以上8字节是参数1
	void** addr = (void**)(sp);// *(void**)(sp) = (void*)(函数指针)
	*addr = (void*)(co_entry);

	co->ctx->esp = sp;
	//co->ctx->ebp = sp;//不要ebp赋值，co_entry()函数开头会自动push ebp
	//init_end
	
	//...

	return co;// 有一段时间debug发现竟然是忘记写return co了!!!
}

context_t* getptr_main_context() {
	if (main_ctx == NULL) {
		main_ctx = (context_t*)malloc(sizeof(context_t));

		if (main_ctx == NULL) {
			fprintf(stderr, "getptr_main_context:内存分配失败");
			return NULL;
		}

		//debug
		//memset(main_ctx, 1, sizeof(context_t));
	}
	return main_ctx;
}


void resume(coroutine_t *co) {
	
	coroutine_t* old_co = current_coroutine;//
	current_coroutine = co;

	SwitchContext(getptr_main_context(), co->ctx);
}

// 给主线程
void yield() {
	if (!current_coroutine) {
		return;
	}
	coroutine_t* co = current_coroutine;
	current_coroutine = NULL;// NULL表没有运行的协程，即运行主线程
	SwitchContext(co->ctx, co->main);
	// addr
}

void destroy(coroutine_t* co) {
	if (co->ctx) {
		free(co->ctx);
	}
	free(co);
}

#else 
// 协程的入口
static VOID WINAPI fiber_entry(context_t ipParameter) {
	coroutine_t* co = (coroutine_t*)ipParameter;

	co->func(co->arg);

	co->state = FINISED;// 协程函数在这里结束

	// 执行主线程
	win_current_coroutine = NULL;
	SwitchToFiber(main_fiber);
	// 在调度器中直接跳到resume()后,因此一般都在resume()后处理完成的协程
}

// 创建协程
coroutine_t* CreateCoroutine(void (*func)(void*), void* arg) {
	// 主线程纤维初始化,可以单独放出来init
	if (!main_fiber) {
		main_fiber = ConvertThreadToFiber(NULL);
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
	//if (!co || co->state == FINISED) return;

	coroutine_t* prev = win_current_coroutine;
	win_current_coroutine = co;
	co->state = RUNNING;

	SwitchToFiber(co->fiber);

	win_current_coroutine = prev;
	if (co->state == RUNNING) { // 是否有必要判断 RUNNING ???
		co->state = READY;
	}
}

// yield()让出CPU,给主线程,
// 不是阻塞,主线程可能会结束
void yield(void) {
	if (!win_current_coroutine) return;

	coroutine_t* co = win_current_coroutine;
	win_current_coroutine = NULL;

	SwitchToFiber(co->main_fiber);
}

// destroy(co)销毁协程
void destroy(coroutine_t* co) {
	if (co->fiber) {
		DeleteFiber(co->fiber);
	}
	free(co);
}
#endif