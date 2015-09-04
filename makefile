GCC_OPTIONS = -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector -fleading-underscore -m32 -fno-asynchronous-unwind-tables

all: kernel.bin

clean:
	rm -f *.o kernel.bin

# ==== KERNEL ENTRY POINT ====

start.o: start.asm
	nasm -f aout -o start.o start.asm
#	as --32 -march=i386 -o start.o start.asm

# ==== UTILITIES ====

utils.o: utils.H utils.C
	gcc $(GCC_OPTIONS) -c -o utils.o utils.C

# ==== DEVICES ====

console.o: console.H console.C
	gcc $(GCC_OPTIONS) -c -o console.o console.C

# ==== KERNEL MAIN FILE ====

kernel.o: kernel.C
	gcc $(GCC_OPTIONS) -c -o kernel.o kernel.C
	

kernel.bin: start.o kernel.o console.o utils.o linker.ld
	ld -melf_i386 -T linker.ld -o kernel.bin start.o kernel.o console.o utils.o
	
