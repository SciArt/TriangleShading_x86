section .data
	;mask: db 0, 4, 8, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	
section .text

global x86_function

x86_function:
	push 	rbp			; push "calling procedure" frame pointer
	mov 	rbp, rsp		; set new frame pointer 
;------------------------------------------------------------------------------
	; saving registers rbp, rsp, rbx, r12, r13, r14, r15
	; rbp was saved before, so need to save only 6 registers
	push rsp	; [rbp-8]
	push rbx ; [rbp-16]
	push r12 ; [rbp-24]
	push r13 ; [rbp-32]
	push r14 ; [rbp-40]
	push r15 ; [rbp-48]
	
	; saving arguments
	push rdi ; [rbp-56] pointer to vertices
	push rsi	; [rbp-64] pointer to pixels
	push rdx	; [rbp-72] width
	push rcx	; [rbp-80] height
	push r8	; [rbp-88] mask
	
	%define vertices [rbp-56]
	%define pixels [rbp-64]
	%define width [rbp-72]
	%define height [rbp-80]

	vzeroall ; assigning zero to all ymm registers

	;mov r9, vertices
	;vmovups xmm10, [r9+16] ; color from the v1
	;vcvtps2dq xmm10, xmm10 ; converting to integers
	
	;vmovups xmm11, [r8]
	;pshufb xmm10, xmm11
	
	vmovups	xmm9, [r8] ; mask
	%define mask xmm9
;------------------------------------------------------------------------------	

	; [0.0]	loading vertices
	
	; vertice looks like [x][y][z][w][r][g][b][a], 
	; each element 4 bytes (32 bits) float value
	
	mov	r9, vertices	; pointer to vertices
	
	; v1
	mov	r10, r9
	
	; v2
	mov	r11, r9
	add	r11, 32
	
	; v3
	mov	r12, r9
	add	r12, 64	
	
	; y coordinates
	
	vmovups		ymm1, [r10+4]	; v1, but begin from y
	vcvtss2si	r13, xmm1		; v1.y
	
	vmovups		ymm2, [r11+4]	; v2, but begin from y
	vcvtss2si	r14, xmm2		; v2.y
	
	vmovups		ymm3, [r12+4]	; v3, but begin from y
	vcvtss2si	r15, xmm3		; v3.y

;------------------------------------------------------------------------------
	; [1.0]	sorting vertices
sort:

	; if( v1.y > v2.y ) swap( v1, v2 );
check_v1_v2:
	
	cmp	r13, r14	; compare v1.y with v2.y
	jle	check_v1_v3 ; skip swaping
	
	; swaping pointers to vertices
	mov	rax, r10 ; move v1 to eax
	mov	r10, r11 ; move v2 to v1
	mov	r11, rax ; move v1 (eax) to v2
	
	; swaping y values
	mov	rax, r13 ; y1 to eax
	mov	r13, r14 ; y2 to y1
	mov	r14, rax ; y1 (eax) to y2
	
	; if( v1.y > v3.y ) swap( v1, v3 );
check_v1_v3:

	cmp	r13, r15 ; compare v1.y with v3.y
	jle	check_v2_v3 ; skip swaping
	
	; swaping pointers to vertices
	mov	rax, r10 ; v1 to eax
	mov	r10, r12 ; v3 to v1
	mov	r12, rax ; v1 to v3
	
	; swaping y values
	mov	rax, r13 ; y1 to eax
	mov	r13, r15 ; y2 to y1
	mov	r15, rax ; y1 to y2
	
	; if( v2.y > v3.y ) swap( v2, v3 );
check_v2_v3:

	cmp	r14, r15 ; compare v2.y with v3.y
	jle	sort_end ; skip swaping
	
	; swaping pointers to vertices
	mov	rax, r11 ; v2 to eax 
	mov	r11, r12 ; v3 to v2
	mov	r12, rax ; v2 to v3
	
	; swaping y values
	mov	rax, r14 ; y2 to eax
	mov	r14, r15 ; y3 to y2
	mov	r15, rax ; y2 to y3

sort_end:
;------------------------------------------------------------------------------
	; x coordinates
	vmovups		ymm1, [r10]		; v1, begin from [x]	
	vmovups		ymm2, [r11]		; v2, begin from [x]	
	vmovups		ymm3, [r12]		; v3, begin from [x]
	
	; coordinates
	; r13, r14, r15 = y1, y2, y3
	
	; colors
	; xmm10, xmm11, xmm12 = v1.rgba, v2.rgba, v3.rgba
	
	%define v1 ymm1
	%define v2 ymm2
	%define v3 ymm3
	
	%define y1 r13
	%define y2 r14
	%define y3 r15
;------------------------------------------------------------------------------
	
	; [2.0]	if v1.y is equal v3.y jump to the END
	cmp	y1, y3
	je		end
	
	%define vB ymm4 ; v begin
	%define vE ymm5 ; v end
	%define d13 ymm6
	%define d12 ymm7
	%define d23 ymm8
	
	; [2.1]	calculating differences d13 (v1, v3)
	; v3 - v1
	vsubps d13, v3, v1
	
	; y3 - y1
	mov rax, y3
	sub rax, y1
	push rax
	VBROADCASTSS ymm0, DWORD[rdi+8] ; filling vector with lower 32 bits of rax
	pop rax
	
	vdivps d13, d13, ymm0 ; v3-v1 / y3-y1
	
	; [2.2]	begin and end vertex = v1
	VMOVUPS vB, v1
	VMOVUPS vE, v1
	
;------------------------------------------------------------------------------
first_stage:

	; [3.0]	if v1.y is equal v2.y jump to [4.0]
	cmp	y1, y2
	je		second_stage
	
	; [3.1]	calculating differences d12 (v1, v2)
	; v2 - v1
	vsubps d12, v2, v1
	
	; y2 - y1
	mov rax, y2
	sub rax, y1
	push rax
	VBROADCASTSS ymm0, DWORD[rdi+8] ; filling vector with lower 32 bits of rax
	pop rax
	
	vdivps d12, d12, ymm0 ; v2-v1 / y2-y1
	
	; [3.2]	FIRST STAGE of drawing
	
	; [3.3]	while( y <= v2.y ) 
	; 		[3.3.0]	draw a line from begin to end
	;		[3.3.1]	calculate next line begin and end
;------------------------------------------------------------------------------
second_stage:

	; [4.0]	if v2.y is equal to v3.y jump to the END
	cmp	y2, y3
	je		end
	
	; [4.1]	calculating differences d23 (v2, v3)
	
	; [4.2]	SECOND STAGE of drawing
	
	; [4.3]	while( y <= v3.y ) 
	; 		[4.3.0]	draw a line from begin to end
	;		[4.3.1]	calculate next line begin and end
	
;------------------------------------------------------------------------------
	
	;mov	rax, x1
	;imul	rax, 4
	
	;vcvtss2si	rcx, xmm2
	;mov	rbx, rcx
	
	;mov	rbx, pixels
	;sub	rax, 1
;loop:
	;mov	BYTE[rbx], 0
	;vcvtps2dq xmm4, c1
	;pshufb xmm4, mask
	;vmovd [rbx], xmm4
	;add	rbx, 4
	;sub	rax, 4
	;cmp	rax, 0
	;jnle	loop
	
;------------------------------------------------------------------------------	
end:
	; free space - saved arguments
	; 5 * 8 = 40 bytes (5 registers, rdi, rsi, rdx, rcx, r9 each 8 bytes)
	add rsp, 40
	
	; loading saved registers rbp, rsp, rbx, r12, r13, r14, r15
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rsp

;------------------------------------------------------------------------------
	mov 	rsp, rbp		; restore original stack pointer
	pop 	rbp			; restore "calling procedure" frame pointer
	ret
