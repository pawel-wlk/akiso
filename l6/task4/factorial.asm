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
  movd xmm2, dword [result]

  xorps xmm3, xmm3 ; clear xmms
  xorps xmm4, xmm4
  xorps xmm5, xmm5
  ; our number looks like that: xmm5 xmm4 xmm3 xmm2
  ; xmm0 is shifting buffer
  ; xmm1 is multiplier

  calculatingloop:
    cmp ecx, 1
    je printing

    movd xmm1,ecx

    pmuludq xmm2, xmm1 ; multiply every part of number by xmm1
    pmuludq xmm3, xmm1
    pmuludq xmm4, xmm1
    pmuludq xmm5, xmm1

    movdqu xmm0, xmm2 ; use xmm0 as buffer to properly add carry and add to next segment of number
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
    sub esp, 16
    movdqu oword [esp], xmm5
    movdqu oword [esp+4], xmm4
    movdqu oword [esp+8], xmm3
    movdqu oword [esp+12], xmm2
    push dword [num]
    push resultformat
    call printf
    add esp, 24 

  mov ebx, 0
  mov eax, 1  ; sys_exit
  int 0x80
