section .data
  message: db "Welcome, please insert arithmetic operation", 10, 0
  scanfformat: db "%lf %c %lf", 0
  resultformat: db "= %lf", 10, 0

section .bss
  num1 resb 8
  num2 resb 8
  operand resb 1
  
section .text
  global main
  extern printf
  extern scanf

main:
  push message
  call printf
  add esp, 2

  push num2
  push operand
  push num1
  push scanfformat
  call scanf
  add esp, 8

  finit
  fld qword [num1]

  cmp byte [operand], '+'
  je addition
  cmp byte [operand], '-'
  je substraction
  cmp byte [operand], '*'
  je multiplication
  cmp byte [operand], '/'
  je division

  addition:
    fadd qword [num2]
    jmp printing
  substraction:
    fsub qword [num2]
    jmp printing
  multiplication:
    fmul qword [num2]
    jmp printing
  division:
    fdiv qword [num2]
    jmp printing

  printing:
    fst qword [num1]
    push dword [num1+4]
    push dword [num1]
    push resultformat
    call printf
    add esp, 10 

  mov ebx, 0
  mov eax, 1  ; sys_exit
  int 0x80
