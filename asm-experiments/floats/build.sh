nasm -f elf -F dwarf -g floats.asm

ld -melf_i386 -o floats floats.o
