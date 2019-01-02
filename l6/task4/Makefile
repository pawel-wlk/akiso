all: factorial

factorial: factorial.asm
	nasm -f elf32 factorial.asm -o factorial.o
	gcc -m32 factorial.o -o factorial
