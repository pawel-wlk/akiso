section .data
  message: db "Welcome, please insert a decimal number", 10, 0
  scanfformat: db "%d", 0
  resultformat: db "%d! = %08x%08x%08x%08x (hex)", 10, 0

section .bss
  num resb 4
  result resb 16
  

section .text
  global main
  extern printf
  extern scanf

main:
  push message
  call printf
  add esp, 2

  push num
  push scanfformat
  call scanf
  add esp, 4

  mov ecx, [num]
  mov dword [result],1
  movd xmm1, dword [result]

  xorps xmm3, xmm3
  xorps xmm4, xmm4
  xorps xmm5, xmm5

  calculatingloop:
    cmp ecx, 1
    je printing

    movd xmm2,ecx

    pmuludq xmm1, xmm2
    pmuludq xmm3, xmm2
    pmuludq xmm4, xmm2
    pmuludq xmm5, xmm2

    movdqu xmm0, xmm1
    psrlq xmm0, 32
    paddq xmm3, xmm0

    movdqu xmm0, xmm3
    psrlq xmm0, 32
    paddq xmm4, xmm0

    movdqu xmm0, xmm4
    psrlq xmm0, 32
    paddq xmm5, xmm0

    loop calculatingloop

  printing:
    movdqu oword[result], xmm1
    movdqu oword[result+4], xmm3
    movdqu oword[result+8], xmm4
    movdqu oword[result+16], xmm5
    push dword [result],
    push dword [result+4],
    push dword [result+8],
    push dword [result+12],
    push dword [num]
    push resultformat
    call printf
    add esp, 24 

  mov ebx, 0
  mov eax, 1  ; sys_exit
  int 0x80
