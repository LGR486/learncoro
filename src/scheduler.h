#pragma once

// scheduler.h 封装调度器与各种算法

#include "context.h"


typedef enum {
	ROUND_ROBIN,// 轮转调度算法
	STATIC_PRIORITY,// 静态优先级调度算法
	//...
}Algorithm;

// 轮转调度器
typedef struct scheduler {
	coroutine_t* co;// 调度器当前执行协程
	coroutine_t* eco;// 尾指针
	
	// 调度器信息
	int lenth;// 队列长度
	//...
} scheduler_t;

// 优先级调度器，1~10，10最高
typedef struct scheduler2 {
	coroutine_t* queue[10];
	coroutine_t* endq[10];

	//int lenth[10];
}scheduler_t2;

// 创建调度器,应支持多个调度器框架
// 形如malloc调用,(指定指针类型)SchCreate(alg)
void* SchCreate(
	Algorithm alg
);

// 添加队列末尾
void push(
	scheduler_t* sch,// 轮转调度器句柄
	void (*func)(void*),// 函数地址
	void* arg// 函数参数数组或者结构
);

int push2(
	scheduler_t2* sch,
	void (*func)(void*),
	void* arg,
	int priority
);

// 轮转调度封装个队列就行
void RunRoudRobin(scheduler_t* sch);

// 静态优先级调度,同等优先级轮转调度
void RunStaticPriority(scheduler_t2* sch);