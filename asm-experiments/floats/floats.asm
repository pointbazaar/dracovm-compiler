global _start

section .data

f1	dd	1.0
f2	dd	2.0

section .text

_start:
	
	;//to lead into xmm0
	mov eax, __float32__(1.0)
	push eax
	movss xmm0, [esp]
	pop eax

	movss xmm1, [f2]

	ucomiss xmm0,xmm1
	jb less	

	mov eax,0
	jmp end
less:
	mov eax,1

end:
	mov ebx,-1
