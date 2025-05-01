#!/bin/bash
mkdir bin
#compiling bootloader
nasm -f bin src/boot.asm -o bin/boot.bin
#compiling kernel
nasm -f bin src/kernel.asm -o bin/kernel.bin
#linking
cat bin/boot.bin bin/kernel.bin > kosoro_build1.bin
#running
qemu-system-x86_64 -fda kosoro_build1.bin
#cleaning
rm -r bin; rm kosoro_build1.bin
