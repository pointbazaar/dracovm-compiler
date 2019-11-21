global _start

section .data

f1	dd	3.0
f2	dd	2.0

section .text

_start:

	movss xmm0, [f1]
	movss xmm1, [f2]

	ucomiss xmm0,xmm1
	
	mov eax,0
	mov ebx,1


