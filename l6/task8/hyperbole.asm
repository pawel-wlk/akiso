section .data
  message: db "insert (s)inh or (a)rcsinh and number", 10, 0
  scanfformat: db "%c %lf", 0
  resultformat: db "= %lf", 10, 0

section .bss
  num1 resb 8
  choice resb 1
  
section .text
  global main
  extern printf
  extern scanf

main:
  push message
  call printf
  add esp, 2

  push num1
  push choice
  push scanfformat
  call scanf
  add esp, 6

  cmp byte [choice], 'a'
  je arcsinh
  cmp byte [choice], 's'
  je sinh

  arcsinh:
    finit
    fld1
    fld qword [num1]
    fld st0
    fld st0
    fmul; multiply first two xs
    fld1; load 1
    fadd; add x**2 + 1
    fsqrt; sqrt(x**2+1)
    fadd; x+sqrt(x**2+1_
    fyl2x; log_2(whole upper thing)
    fldl2e
    fdiv 
    jmp printing

  sinh:
    ;(2^(x*log2e) - 2^(-x*log2e))/2
    finit
    fldl2e
    fmul qword [num1]
    fld1
    fld st1
    fprem
    f2xm1
    fadd
    fscale
    fld st0
    fld1
    fdivr
    fsub
    fld1
    fld1
    fadd
    fdiv

    jmp printing

  printing:
    fst qword [num1]
    push dword [num1+4]
    push dword [num1]
    push resultformat
    call printf
    add esp, 4 

  mov ebx, 0
  mov eax, 1  ; sys_exit
  int 0x80
