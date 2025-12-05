#pragma once
// context.h
// 上下文结构和协程块

// 协程状态
typedef enum {
	READY,// 就绪
	RUNNING,// 运行
	AWAIT,// 休眠
	FINISED,// 完成
} State ;

#include <windows.h>// 先用WINAPI理解些原理 再自己封装上下文 到时要移动代码
// 直接使用WINAPI的上下文结构
// LPVOID 是void* context_t就是个纯void* 类型 所以context_t就是空的上下文
typedef LPVOID context_t;
// 协程控制块
typedef struct coroutine {
	context_t fiber;// 协程的上下文
	context_t main_fiber;// 主线程的上下文

	void (*func)(void*);// void func(void* arg) 这么定义
	void* arg;// 函数参数数组指针
	int state;// 协程状态
	
	struct coroutine* next;// 链入就绪队列
	//...
} coroutine_t;

static __declspec(thread) coroutine_t* current_coroutine = NULL;// 正在执行的协程,NULL表示在执行主线程
static __declspec(thread) context_t main_fiber;// 主线程纤维地址

static VOID WINAPI fiber_entry(context_t ipParameter);
coroutine_t* create(void (*func)(void*), void* arg);
void resume(coroutine_t* co);
void yield(void);
void destroy(coroutine_t* co);

