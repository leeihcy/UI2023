.code
;
; 用于实现属性set/get
; sub rsp, 20h, 用于x64位上面参数回写。注：仅sub rsp, 10h在release版本上会出错，按照其它
; 函数的反汇编代码一样，这里也采用20h，具体原因未知。
;

x64asm_Set64 PROC _this:QWORD, value:QWORD,_func:QWORD
	; 用于参数回写rdx,rcx
	sub rsp, 20h  

	; r8 = call address, rcx/rdx不变，透传过去
	call r8

	; 还原参数回写
	add rsp, 20h
	ret
x64asm_Set64 ENDP

x64asm_Get64 PROC _this:QWORD, _func:QWORD
	; 用于参数回写rdx,rcx
	sub rsp, 20h  

	call rdx

	; 还原参数回写
	add rsp, 20h
	ret
x64asm_Get64 ENDP


END	