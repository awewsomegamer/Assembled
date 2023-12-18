global main		; Change
main:	mov rax, 0x0	
	mov rcx, 0x10	
	mov rbx, 'A'	
	mov rdx, 'B'	
		
	mov rsi, 0x0A	
	mov rdi, 0x20	
	mov rcx, 2	
	; Copy	
	rep movsd	
		
	mov rcx, 10	
loop:	dec rcx	; Some loop to loop over something
	jne loop	; Jump until rcx is 0
		
	ret	
		
		
		