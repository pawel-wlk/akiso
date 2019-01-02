all: calculator

calculator: calculator.asm
	nasm -f elf32 calculator.asm -o calculator.o
	gcc -m32 calculator.o -o calculator
