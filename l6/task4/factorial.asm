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

  calculatingloop:
    cmp ecx, 1
    je printing

    movd xmm2,ecx

    movdqu xmm3, xmm1
    movdqu xmm4, xmm1
    movdqu xmm5, xmm1

    psrlq xmm3, 32
    psrlq xmm4, 64
    psrlq xmm5, 96

    pmuludq xmm1, xmm2
    pmuludq xmm3, xmm2
    pmuludq xmm4, xmm2
    pmuludq xmm5, xmm2

    psllq xmm3, 32
    psllq xmm4, 64
    psllq xmm5, 96

    paddq xmm1, xmm3
    paddq xmm1, xmm4
    paddq xmm1, xmm5


    loop calculatingloop

  printing:
    movdqu oword[result], xmm1
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
