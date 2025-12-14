
; 有关VS2022 C语言和汇编
; 首先添加项目依赖 masm
; 设置项目属性X64, X86
; 新建.asm文件
; 注意.asm文件不要和.c文件重名,因为生成的都是.obj文件,这会覆盖掉其中一个,导致link失败

.386
.model flat,c

.code

; WIN32
; 暂时有bug
SwitchContext proc
	
	mov eax, [esp+4]
	mov [eax+4], ebx
	mov [eax+8], ecx
	mov [eax+12], edx
	mov [eax+16], edi
	mov [eax+20], esi
	mov [eax+24], ebp
	mov [eax+28], esp

	mov eax, [esp+8]
	mov ebx, [eax+4]
	mov ecx, [eax+8]
	mov edx, [eax+12]
	mov edi, [eax+16]
	mov esi, [eax+20]
	mov ebp, [eax+24]
	mov esp, [eax+28]

	ret

SwitchContext endp

end

