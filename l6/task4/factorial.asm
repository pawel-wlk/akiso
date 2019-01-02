section .data
  message: db "Welcome, please insert a number", 10, 0
  scanfformat: db "%d", 0
  resultformat: db "%d! = %ld", 10, 0

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

  movd xmm1, [num]

  calculatingloop:
    cmp ecx, 1
    jle printing

    ; dunno what to do
    movd xmm1, ecx
    pmullw xmm0, xmm1
    
    loop calculatingloop
    

  printing:
    push dword [result]
    push dword [num]
    push resultformat
    call printf
    add esp, 10 

  mov ebx, 0
  mov eax, 1  ; sys_exit
  int 0x80
