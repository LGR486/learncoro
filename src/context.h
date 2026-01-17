#pragma once
// context.h
// 上下文结构和协程块

typedef enum {
	READY,// 就绪
	RUNNING,// 运行
	AWAIT,// 休眠
	FINISED,// 完成
} State;

//启用自定义结构体
#define MYSELF

#if defined(MYSELF)
typedef struct context {
	int eip; //指令地址
	int ebx; //基址寄存器
	int ecx; //计数器
	int edx; //数据寄存器
	int edi; //目的索引寄存器
	int esi; //源索引寄存器
	int ebp; //栈帧指针
	char* esp; //栈顶指针
} context_t; 

typedef struct coroutine {
	context_t* ctx;// 上下文
	context_t* main;// 主线程

	void (*func)(void*);// 函数
	void* arr; // 参数

	int state; // 状态
	int priority; // 优先级,1~10,10为最高优先级

	struct coroutine* next; // 链入队列

	int stack_size;
	char stack[8 * 1024]; // 8kb的独立栈
} coroutine_t;

#include<stdlib.h>
#include<stdio.h>

// 静态指针体积不大

static coroutine_t* current_coroutine = NULL;
// 主线程的上下文是随机地址,但是知道地址就行了,必须分配一个随机地址,
// 不然默认就是NULL,在NULL附近写入会发生异常,为此必须引入get_main_context()
static context_t* main_ctx = NULL;

// 获得main_ctx的地址,实现初始化和不断地空指针检查
context_t* getptr_main_context();

coroutine_t* CreateCoroutine(void (*func)(void*), void* arr);

// 以下是自定义上下文结构要实现的函数

// 汇编上下文切换函数
// 见context_switch.asm
extern void SwitchContext(context_t* old_ctx, context_t* new_ctx);// 只需重写这个函数在加上数据结构就可以实现上下文切换//用汇编实现

//void SaveContext();// 保存
//void RestoreContext();// 恢复
void yield();
void resume(coroutine_t* co);
void destroy(coroutine_t* co);

#else 
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

static __declspec(thread) coroutine_t* win_current_coroutine = NULL;// 正在执行的协程,NULL表示在执行主线程,
static __declspec(thread) context_t main_fiber;// 主线程纤维地址

// 以下都是与WINAPI直接交互的函数, 用自定义上下文结构要重现这些函数接口
// SwitchToFiber()整个WINPAPI用到的最重要的函数，自定义上下文切换就是要重写这个函数

static VOID WINAPI fiber_entry(context_t ipParameter);

coroutine_t* CreateCoroutine(void (*func)(void*), void* arg);
void resume(coroutine_t* co);
void yield(void);
void destroy(coroutine_t* co);
#endif

