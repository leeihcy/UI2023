.code
;
; ����ʵ������set/get
; sub rsp, 20h, ����x64λ���������д��ע����sub rsp, 10h��release�汾�ϻ������������
; �����ķ�������һ��������Ҳ����20h������ԭ��δ֪��
;

x64asm_Set64 PROC _this:QWORD, value:QWORD,_func:QWORD
	; ���ڲ�����дrdx,rcx
	sub rsp, 20h  

	; r8 = call address, rcx/rdx���䣬͸����ȥ
	call r8

	; ��ԭ������д
	add rsp, 20h
	ret
x64asm_Set64 ENDP

x64asm_Get64 PROC _this:QWORD, _func:QWORD
	; ���ڲ�����дrdx,rcx
	sub rsp, 20h  

	call rdx

	; ��ԭ������д
	add rsp, 20h
	ret
x64asm_Get64 ENDP


END	