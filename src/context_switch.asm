
; 有关VS2022 C语言和汇编
; 首先添加项目依赖 masm
; 设置项目属性X64, X86
; 新建.asm文件
; 注意.asm文件不要和.c文件重名,因为生成的都是.obj文件,这会覆盖掉其中一个,导致link失败



.386
.model flat,c

.data
fmt db 'stack value: %08x', 0Ah, 0
sp_value dd 0

;.text


.code

extern printf:proc

; WIN32
; 暂时有bug
; 根据libco改编,支持windows的汇编
SwitchContext proc
	
	mov eax, [esp+4] ; old_ctx

	mov [eax+4], ebx
	mov [eax+8], ecx
	mov [eax+12], edx
	mov [eax+16], edi
	mov [eax+20], esi
	mov [eax+24], ebp
	mov [eax+28], esp

	mov eax, [esp+8] ; new_ctx

	mov ebx, [eax+4]
	mov ecx, [eax+8]
	mov edx, [eax+12]
	mov edi, [eax+16]
	mov esi, [eax+20]
	mov ebp, [eax+24]
	mov esp, [eax+28]
	
	ret
	; ret return 的缩写，将放回地址addr弹出栈顶并加载到CPU的指令指针寄存器eip

SwitchContext endp

; 调试打印栈信息函数--------------->bug
; call printf会改变eax,ecx寄存器,大概需要定义.data段的一个变量保存new_ptr指针
debug_printf proc
	push ebp ; 插入ebp
	mov ebp, esp ; 保存栈顶指针
	mov eax, esp ; 
	mov [sp_value], eax

	push dword ptr [sp_value] ;???
	push offset [fmt] ;???
	call printf 
	add esp, 8
	pop ebp
	ret
debug_printf endp

end


