.data
  format: .asciz "%d\n"
  scan: .asciz "%d %d"
  num1: .word 0
  num2: .word 0
.text
  .global main
  .extern printf
  .extern scanf
main:
  ldr r0, =scan
  ldr r1, =num1 // load num1 address to r1
  ldr r2, =num2 // load num2 address to r2
  bl scanf
  ldr r1, =num1 // load num1 address to r1
  ldr r2, =num2 // load num2 address to r2
  ldr r1, [r1] // load values
  ldr r2, [r2]
gcd:
  CMP r1, r2
  SUBGT r1, r1, r2
  SUBLT r2, r2, r1
  BNE gcd
print:
  LDR r0, =format //print r1
  BL printf
  MOV r7, #1 // syscall exit
  SWI 0
