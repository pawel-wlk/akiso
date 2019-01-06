all: primes

primes: primes.asm
	nasm -f elf32 primes.asm -o primes.o
	gcc -m32 primes.o -o primes
	#ld -m elf_i386 -e main -dynamic-linker primes.o -o primes -lc
