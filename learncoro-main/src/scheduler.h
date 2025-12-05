#pragma once

// scheduler.h 封装调度器与各种算法

#include "context.h"

typedef enum {
	ROUND_ROBIN,
	//...
}Algorithm;


// 调度器
typedef struct scheduler {
	coroutine_t* co;// 调度器当前执行协程
	coroutine_t* eco;// 尾指针
	Algorithm alg;// 调度算法

	//int time_slice;// 时间切片,似乎不太需要
	
	// 调度器信息
	int lenth;// 队列长度
	//...
} scheduler_t;

// 创建调度器
scheduler_t* SchCreate(
	Algorithm alg
);

// 添加队列末尾
void push(
	scheduler_t* sch,// 调度器句柄
	void (*func)(void*),// 函数地址
	void* arg// 函数参数数组
);

// 轮转调度封装个队列就行
void RunRoudRobin(scheduler_t* sch);