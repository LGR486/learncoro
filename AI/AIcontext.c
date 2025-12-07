// AI 生成的上下文结构源码，助于理解操作系统相关的结构 
// 与我的协程库设计关系不大
// AI封装的队列结构和调度甚至有问题
// 因此只能参考上下文结构 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// ==================== 数据类型定义 ====================
typedef uint32_t word_t;      // 32位字
typedef uint8_t byte_t;       // 8位字节
typedef uint32_t addr_t;      // 地址类型

// ==================== 寄存器定义 ====================
typedef struct {
    // 通用寄存器
    word_t eax;    // 累加器 add
    word_t ebx;    // 基址寄存器 bese
    word_t ecx;    // 计数器 counter
    word_t edx;    // 数据寄存器 data
    
    // 指针和变址寄存器 
    word_t esi;    // 源索引 souroce idenx
    word_t edi;    // 目的索引
    word_t ebp;    // 基址指针 base pointer
    word_t esp;    // 堆栈指针 stack pointer
    
    // 段寄存器
    word_t cs;     // 代码段
    word_t ds;     // 数据段
    word_t ss;     // 堆栈段
    word_t es;     // 附加段
    
    // 控制寄存器
    word_t eip;    // 指令指针
    word_t eflags; // 标志寄存器
    
    // 浮点寄存器 (简化的)
    double st[8];  // 浮点寄存器栈
} CPU_Registers;

// ==================== 内存管理单元 ====================
typedef struct {
    byte_t* memory;      // 物理内存
    size_t  size;        // 内存大小
    
    // 页表项 (简化的)
    struct {
        addr_t physical_addr;
        int    present;
        int    writable;
    } page_table[1024];
} MMU;

// ==================== 进程控制块 ====================
typedef struct ProcessControlBlock {
    int pid;                    // 进程ID
    int priority;              // 优先级
    int state;                 // 进程状态
    CPU_Registers regs;        // 寄存器上下文
    addr_t code_start;         // 代码段起始
    addr_t code_size;          // 代码段大小
    addr_t stack_start;        // 堆栈起始
    addr_t stack_size;         // 堆栈大小
    addr_t heap_start;         // 堆起始
    addr_t heap_size;          // 堆大小
    struct ProcessControlBlock* next;  // 链表指针
    
    // 时间统计
    unsigned long cpu_time;    // CPU使用时间
    unsigned long creation_time; // 创建时间
} PCB;

// ==================== 操作系统上下文 ====================
// 是的操作系统也有单独的上下文结构，与进程上下文也要切换 
typedef struct {
    // 当前运行进程
    PCB* current_process;
    
    // 进程队列
    PCB* ready_queue;
    PCB* blocked_queue;
    
    // 中断相关
    int interrupt_flag;
    word_t interrupt_vector[256];
    
    // 系统调用表
    void (*syscall_table[256])(void);
    
    // 内存管理
    MMU mmu;
    
    // 系统时钟
    unsigned long system_ticks;
    
    // 调度信息
    int quantum;              // 时间片大小
    int scheduler_type;       // 调度算法类型
} OS_Context;

// ==================== 堆栈帧结构 ====================
typedef struct {
    word_t ebp;          // 保存的基址指针
    word_t eip;          // 返回地址
    word_t parameters[6]; // 参数 (x86最多6个参数通过堆栈传递)
    word_t local_vars[8]; // 局部变量
} StackFrame;

// ==================== 中断上下文 ====================
typedef struct {
    word_t eax, ebx, ecx, edx;
    word_t esi, edi, ebp, esp;
    word_t eip, eflags;
    word_t cs, ds, es, ss;
    int interrupt_num;
    word_t error_code;
} InterruptContext;

// ==================== 函数原型 ====================
// 初始化函数
void init_os_context(OS_Context* os, size_t mem_size);
void init_cpu_registers(CPU_Registers* regs);
PCB* create_process(OS_Context* os, addr_t entry_point);

// 上下文切换
void save_context(CPU_Registers* regs, PCB* pcb);
void restore_context(PCB* pcb, CPU_Registers* regs);
void context_switch(OS_Context* os, PCB* new_process);

// 堆栈操作
void push_stack(OS_Context* os, word_t value);
word_t pop_stack(OS_Context* os);
StackFrame* create_stack_frame(OS_Context* os, addr_t return_addr, 
                              word_t* params, int param_count);

// 内存操作
word_t read_memory(OS_Context* os, addr_t address);
void write_memory(OS_Context* os, addr_t address, word_t value);

// 进程调度
void schedule(OS_Context* os);
void add_to_ready_queue(OS_Context* os, PCB* process);
PCB* remove_from_ready_queue(OS_Context* os);

// 系统调用
void syscall_handler(OS_Context* os, int syscall_num, word_t* args);
void register_syscall(OS_Context* os, int num, void (*handler)(void));

// ==================== 实现部分 ====================
// 初始化操作系统上下文
void init_os_context(OS_Context* os, size_t mem_size) {
    printf("初始化操作系统上下文...\n");
    
    // 初始化内存
    os->mmu.memory = (byte_t*)malloc(mem_size);
    os->mmu.size = mem_size;
    memset(os->mmu.memory, 0, mem_size);
    
    // 初始化页表
    for (int i = 0; i < 1024; i++) {
        os->mmu.page_table[i].present = 0;
        os->mmu.page_table[i].writable = 0;
    }
    
    // 初始化进程队列
    os->current_process = NULL;
    os->ready_queue = NULL;
    os->blocked_queue = NULL;
    
    // 初始化中断标志
    os->interrupt_flag = 0;
    
    // 初始化系统时钟
    os->system_ticks = 0;
    os->quantum = 100;  // 100个时钟滴答
    
    printf("操作系统上下文初始化完成，内存大小: %zu bytes\n", mem_size);
}

// 初始化CPU寄存器
void init_cpu_registers(CPU_Registers* regs) {
    memset(regs, 0, sizeof(CPU_Registers));
    
    // 设置段寄存器初始值
    regs->cs = 0x08;    // 代码段选择子
    regs->ds = 0x10;    // 数据段选择子
    regs->ss = 0x10;    // 堆栈段选择子
    regs->es = 0x10;    // 附加段选择子
    
    // 设置标志寄存器初始值
    regs->eflags = 0x00000202;  // 中断使能，保留位
    
    printf("CPU寄存器初始化完成\n");
}

// 创建新进程
PCB* create_process(OS_Context* os, addr_t entry_point) {
    static int next_pid = 1;
    
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    if (!pcb) {
        fprintf(stderr, "内存分配失败\n");
        return NULL;
    }
    
    // 初始化PCB
    pcb->pid = next_pid++;
    pcb->priority = 10;
    pcb->state = 1;  // 就绪状态
    
    // 初始化寄存器上下文
    init_cpu_registers(&pcb->regs);
    pcb->regs.eip = entry_point;
    
    // 分配内存空间
    pcb->code_start = 0x1000;
    pcb->code_size = 0x10000;      // 64KB代码段
    
    pcb->stack_start = 0x200000;   // 2MB处开始
    pcb->stack_size = 0x10000;     // 64KB堆栈
    
    pcb->heap_start = 0x300000;    // 3MB处开始
    pcb->heap_size = 0x100000;     // 1MB堆
    
    // 设置堆栈指针
    pcb->regs.esp = pcb->stack_start + pcb->stack_size;
    pcb->regs.ebp = pcb->regs.esp;
    
    pcb->next = NULL;
    pcb->cpu_time = 0;
    pcb->creation_time = os->system_ticks;
    
    printf("创建进程 PID=%d, 入口点=0x%08x\n", pcb->pid, entry_point);
    printf("堆栈指针 ESP=0x%08x, 堆指针=0x%08x\n", 
           pcb->regs.esp, pcb->heap_start);
    
    return pcb;
}

// 保存上下文到PCB
void save_context(CPU_Registers* regs, PCB* pcb) {
    if (!pcb || !regs) return;
    
    memcpy(&pcb->regs, regs, sizeof(CPU_Registers));
    printf("保存进程 %d 的上下文\n", pcb->pid);
}

// 从PCB恢复上下文
void restore_context(PCB* pcb, CPU_Registers* regs) {
    if (!pcb || !regs) return;
    
    memcpy(regs, &pcb->regs, sizeof(CPU_Registers));
    printf("恢复进程 %d 的上下文\n", pcb->pid);
}

// 上下文切换
void context_switch(OS_Context* os, PCB* new_process) {
    if (!os->current_process && !new_process) return;
    
    printf("=================== 上下文切换 ===================\n");
    
    // 保存当前进程上下文
    if (os->current_process) {
        save_context(&os->current_process->regs, os->current_process);
        
        // 如果不是终止，则放回就绪队列
        if (os->current_process->state == 1) {  // 就绪状态
            add_to_ready_queue(os, os->current_process);// 队列头 
        }
    }
    
    // 切换到新进程
    PCB* old_process = os->current_process;
    os->current_process = new_process;
    
    if (new_process) {
        // 从就绪队列移除
        PCB* prev = NULL;
        PCB* curr = os->ready_queue;
        while (curr) {//???
            if (curr->pid == new_process->pid) {
                if (prev) prev->next = curr->next;
                else os->ready_queue = curr->next;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
        
        // 恢复新进程上下文
        restore_context(new_process, &new_process->regs);
        
        printf("从进程 %d 切换到进程 %d\n", 
               old_process ? old_process->pid : -1, 
               new_process->pid);
        printf("新进程 EIP=0x%08x, ESP=0x%08x\n", 
               new_process->regs.eip, new_process->regs.esp);
    }
    
    printf("================================================\n");
}

// 压栈操作
void push_stack(OS_Context* os, word_t value) {
    if (!os->current_process) return;
    
    PCB* pcb = os->current_process;
    
    // 检查堆栈溢出
    if (pcb->regs.esp <= pcb->stack_start) {
        fprintf(stderr, "堆栈溢出！进程 %d\n", pcb->pid);
        return;
    }
    
    // 调整堆栈指针并写入值
    pcb->regs.esp -= sizeof(word_t);
    write_memory(os, pcb->regs.esp, value);
    
    printf("进程 %d 压栈: 值=0x%08x, ESP=0x%08x\n", 
           pcb->pid, value, pcb->regs.esp);
}

// 弹栈操作
word_t pop_stack(OS_Context* os) {
    if (!os->current_process) return 0;
    
    PCB* pcb = os->current_process;
    
    // 检查堆栈下溢
    if (pcb->regs.esp >= pcb->stack_start + pcb->stack_size) {
        fprintf(stderr, "堆栈下溢！进程 %d\n", pcb->pid);
        return 0;
    }
    
    // 读取值并调整堆栈指针
    word_t value = read_memory(os, pcb->regs.esp);
    pcb->regs.esp += sizeof(word_t);
    
    printf("进程 %d 弹栈: 值=0x%08x, ESP=0x%08x\n", 
           pcb->pid, value, pcb->regs.esp);
    
    return value;
}

// 创建堆栈帧
StackFrame* create_stack_frame(OS_Context* os, addr_t return_addr, 
                              word_t* params, int param_count) {
    if (!os->current_process) return NULL;
    
    PCB* pcb = os->current_process;
    
    // 压入参数（从右到左）
    for (int i = param_count - 1; i >= 0; i--) {
        push_stack(os, params[i]);
    }
    
    // 压入返回地址
    push_stack(os, return_addr);
    
    // 保存当前ebp并设置新的ebp
    word_t old_ebp = pcb->regs.ebp;
    push_stack(os, old_ebp);
    pcb->regs.ebp = pcb->regs.esp;
    
    printf("创建堆栈帧: 返回地址=0x%08x, 参数个数=%d\n", 
           return_addr, param_count);
    printf("当前 EBP=0x%08x, ESP=0x%08x\n", 
           pcb->regs.ebp, pcb->regs.esp);
    
    // 返回堆栈帧指针
    return (StackFrame*)pcb->regs.ebp;
}

// 读取内存
word_t read_memory(OS_Context* os, addr_t address) {
    if (address >= os->mmu.size) {
        fprintf(stderr, "内存读取越界: 0x%08x\n", address);
        return 0;
    }
    
    word_t value;
    memcpy(&value, os->mmu.memory + address, sizeof(word_t));
    return value;
}

// 写入内存
void write_memory(OS_Context* os, addr_t address, word_t value) {
    if (address >= os->mmu.size) {
        fprintf(stderr, "内存写入越界: 0x%08x\n", address);
        return;
    }
    
    memcpy(os->mmu.memory + address, &value, sizeof(word_t));
}

// 添加到就绪队列
void add_to_ready_queue(OS_Context* os, PCB* process) {
    if (!process) return;
    
    process->next = os->ready_queue;// 看起来这里是添加到队列头 
    os->ready_queue = process;
    
    printf("进程 %d 添加到就绪队列\n", process->pid);
}

// 调度函数
void schedule(OS_Context* os) {
    if (!os->ready_queue) {
        printf("就绪队列为空\n");
        return;
    }
    
    // 简单的轮转调度
    PCB* next_process = os->ready_queue;
    
    // 执行上下文切换
    context_switch(os, next_process);
}

// 系统调用处理
void syscall_handler(OS_Context* os, int syscall_num, word_t* args) {
    printf("系统调用 #%d\n", syscall_num);
    
    switch (syscall_num) {
        case 1:  // 退出进程
            printf("进程 %d 退出\n", os->current_process->pid);
            os->current_process->state = 0;  // 终止状态
            schedule(os);
            break;
            
        case 2:  // 创建进程
            printf("创建新进程的系统调用\n");
            // 实际实现会创建新进程...
            break;
            
        case 3:  // 内存分配
            printf("内存分配请求: %u 字节\n", args[0]);
            // 实际实现会分配内存...
            break;
            
        default:
            printf("未知系统调用: %d\n", syscall_num);
            break;
    }
}

// ==================== 演示程序 ====================
void demo_context_operations() {
    printf("\n========== 上下文操作演示 ==========\n");
    
    // 1. 初始化操作系统上下文
    OS_Context os;
    init_os_context(&os, 16 * 1024 * 1024);  // 16MB内存
    
    // 2. 创建几个示例进程
    PCB* proc1 = create_process(&os, 0x10000);
    PCB* proc2 = create_process(&os, 0x20000);
    PCB* proc3 = create_process(&os, 0x30000);
    
    // 3. 将进程添加到就绪队列
    add_to_ready_queue(&os, proc1);
    add_to_ready_queue(&os, proc2);
    add_to_ready_queue(&os, proc3);
    
    // 4. 模拟调度和上下文切换
    printf("\n>>> 开始调度演示 <<<\n");
    
    // 第一次调度
    schedule(&os);
    
    // 模拟进程1执行一些操作
    if (os.current_process) {
        printf("\n进程 %d 正在执行...\n", os.current_process->pid);
        
        // 创建堆栈帧
        word_t params[2] = {0x1111, 0x2222};
        create_stack_frame(&os, 0x1000, params, 2);
        
        // 压栈操作
        push_stack(&os, 0xAAAAAAAA);
        push_stack(&os, 0xBBBBBBBB);
        push_stack(&os, 0xCCCCCCCC);
        
        // 弹栈操作
        word_t val = pop_stack(&os);
        printf("弹出值: 0x%08x\n", val);
        
        // 模拟系统调用
        word_t syscall_args[1] = {1024};
        syscall_handler(&os, 3, syscall_args);
        
        // 模拟时间片用完
        printf("\n进程 %d 时间片用完\n", os.current_process->pid);
    }
    
    // 第二次调度
    schedule(&os);
    
    // 模拟进程2执行
    if (os.current_process) {
        printf("\n进程 %d 正在执行...\n", os.current_process->pid);
        
        // 创建堆栈帧
        word_t params[3] = {0x3333, 0x4444, 0x5555};
        create_stack_frame(&os, 0x2000, params, 3);
        
        // 模拟系统调用退出
        syscall_handler(&os, 1, NULL);
    }
    
    // 第三次调度
    schedule(&os);
    
    printf("\n========== 演示结束 ==========\n");
    
    // 清理内存
    free(os.mmu.memory);
    free(proc1);
    free(proc2);
    free(proc3);
}

// ==================== 主函数 ====================
int main() {
    printf("C语言上下文结构模拟器\n");
    printf("================================\n");
    
    demo_context_operations();
    
    return 0;
}
