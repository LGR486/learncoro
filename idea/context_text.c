// 头脑风暴！！！！！！！！！！！！！！！！ 
// 以下全是我不了解上下文时对实现上下文切换的思考，算是自己想清楚了原理 

/*  结论就是需要定义结构保存全局变量信息
	需要时从其中恢复，恢复前必须保存此时
	的信息到一个局部变量 
	为此需要定义一个结构，和一系列全局变量
	还需要个控制流
	至此应该可以实现数据结构+控制流的上下文切换 
*/

//#include <stdio.h> //NULL竟然在stdio.h里面定义 
#include <stdlib.h> //NULL竟然在stdlib.h里面也有 

// 定义合理的上下文结构...
struct Context {
	int a;
	int b; 
	//...
};
typedef struct Context* context_t;

// 全局变量区...
static context_t current_context = NULL;// 当前协程 
static context_t main_context = NULL;// 主线程地址 

static int a = 0;
static int b = 0;
//...

void SwitchContext(context_t new_text){
	context_t old_text = current_context;
	Save(old_text);
	Restore(new_text);
	// 跳转，根据恢复的全局变量开始跳转 
	// ...
}

void Save(context_t text){
	//	保存在哪? 
	// 应该是保存了快照 
	// 事实上是对当前的全局变量信息保存到context 
	// 所以这里应该是对context赋值
	text->a = a;//保存全局变量 
	text->b = b;//
	//...
}

void Restore(context_t text) {
	current_context = text;
	// 如何跳转??? 
	// 对全局变量赋值恢复 
	a = text->a;
	b = text->b;
}

void yeild(){
	//current_context = NULL;//或者 
	SwitchContext(main_context);
}
void resume(context_t text){
	SwitchContext(text);
}
void destroy(context_t text) {
	free(text);
}
