%define FIBONACCI_COUNT 35		
		
global main		
extern printf		
section .text		
main:	push rbp	; Save stack base
	mov rbp, rsp	
	mov rdx, 0	
	mov rax, [n1]	
loop:	mov rbx, [n2]	
	add rax, rbx	
	not rcx	
	cmp rcx, 0	
	jle loop	
		
	mov rdi, string	
	mov rsi, rax	
	mov rdx, FIBONACCI_COUNT	
	call printf	
		
	xor rax, rax	
	mov rcx, 0	
	jne latter	
	mov [n1], rax	
	jmp end	
latter:	mov [n2], rax	
end:	inc rdx	
	cmp rdx, FIBONACCI_COUNT	
	mov rsp, rbp	
	pop rbp	
	ret	
section .data		
n1:	dq 0	; First number
n2:	dq 1	; Second number