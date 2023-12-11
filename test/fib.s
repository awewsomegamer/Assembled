%define FIBONACCI_COUNT 40		
		
global main		
extern printf		
section .text		
main:	push rbp	; Save stack base
	mov rbp, rsp	; New stack frame
		
	mov rcx, 0	; Zero state
	mov rdx, 0	; Zero counter
loop:	mov rax, [n1]	; Get the second number
	mov rbx, [n2]	; Add them
	add rax, rbx	; Get the first number
		
	not rcx	; Test to see if RCX is zero
	cmp rcx, 0	; Compare RCX to zero
	jne latter	; If it is not zero jump over
	mov [n1], rax	; Sum goes into n1
	jmp end	; Jump over
latter:	mov [n2], rax	; Sum goes into n2 
end:	inc rdx	; Increment RDX counter
	cmp rdx, FIBONACCI_COUNT	; Reached 16 iterations??
	jle loop	; No - loop
		
	mov rdi, string	; Set format
	mov rsi, rax	; Set fibonacci number
	mov rdx, FIBONACCI_COUNT	
	call printf	; Print
		
	xor rax, rax	; Zero return value
		
	mov rsp, rbp	; Restore stack pointer 
	pop rbp	; Restore stack base
		
	ret	
		
section .data		
		
n1:	dq 0	; First number
n2:	dq 1	; Second number
string:	db "%d is at %d in the fibonacci sequence", 0xA, 0x0	; String format
		