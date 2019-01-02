section .data
  format: db "%d", 10, 0
  
section .text
  global main
  extern printf

main:
  mov ecx, 10000
  forloop:
    cmp ecx, 0
    je endofloop ; skip if 0
    cmp ecx, 1
    je endofloop ; skip if 1
    cmp ecx, 2
    je print

    mov ebx, 2
    innerforloop:
      mov edx, 0
      mov eax, ecx
      div ebx

      cmp edx, 0
      je endofloop
      
      innerforloopinc:
        inc ebx
        cmp ebx, ecx
        jl innerforloop

    print:
      push ecx; printf
      push dword format
      call printf
      pop edx
      pop ecx; cleanup printf

  endofloop: 
    loop forloop
  mov eax, 1  ; sys_exit
  int 0x80

