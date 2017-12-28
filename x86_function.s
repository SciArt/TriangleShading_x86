	section .text
	
	global x86_function

x86_function:
	push 	rbp			; push "calling procedure" frame pointer
	mov 	rbp, rsp		; set new frame pointer 
			;	- "this procedure" frame pointer
;------------------------------------------------------------------------------




end:
;------------------------------------------------------------------------------
	mov 	rsp, rbp		; restore original stack pointer
	pop 	rbp			; restore "calling procedure" frame pointer
	ret
