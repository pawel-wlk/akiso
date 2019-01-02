	.file	"primes.c"
	.intel_syntax noprefix
	.text
	.globl	is_prime
	.type	is_prime, @function
is_prime:
.LFB0:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	DWORD PTR -20[rbp], edi
	cmp	DWORD PTR -20[rbp], 0
	je	.L2
	cmp	DWORD PTR -20[rbp], 1
	jne	.L3
.L2:
	mov	eax, 0
	jmp	.L4
.L3:
	mov	DWORD PTR -4[rbp], 2
	jmp	.L5
.L7:
	mov	eax, DWORD PTR -20[rbp]
	cdq
	idiv	DWORD PTR -4[rbp]
	mov	eax, edx
	test	eax, eax
	jne	.L6
	mov	eax, 0
	jmp	.L4
.L6:
	add	DWORD PTR -4[rbp], 1
.L5:
	mov	eax, DWORD PTR -20[rbp]
	mov	edx, eax
	shr	edx, 31
	add	eax, edx
	sar	eax
	cmp	DWORD PTR -4[rbp], eax
	jle	.L7
	mov	eax, 1
.L4:
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	is_prime, .-is_prime
	.section	.rodata
.LC0:
	.string	"%d "
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	DWORD PTR -4[rbp], 0
	jmp	.L9
.L11:
	mov	eax, DWORD PTR -4[rbp]
	mov	edi, eax
	call	is_prime
	test	al, al
	je	.L10
	mov	eax, DWORD PTR -4[rbp]
	mov	esi, eax
	lea	rdi, .LC0[rip]
	mov	eax, 0
	call	printf@PLT
.L10:
	add	DWORD PTR -4[rbp], 1
.L9:
	cmp	DWORD PTR -4[rbp], 9999
	jle	.L11
	mov	eax, 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 8.2.1 20181127"
	.section	.note.GNU-stack,"",@progbits
