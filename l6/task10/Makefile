all: gcd

gcd: gcd.asm
	arm-none-eabi-as gcd.asm -o gcd.o
	arm-none-eabi-ld gcd.o -o gcd
	#arm-none-eabi-gcc gcd.o -o gcd
