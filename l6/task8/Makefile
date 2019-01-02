all: hyperbole

hyperbole: hyperbole.asm
	nasm -f elf32 hyperbole.asm -o hyperbole.o
	gcc -m32 hyperbole.o -o hyperbole
