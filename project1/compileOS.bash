#!/bin/bash

../PathSetter.bash

#compile the kernel 
bcc -ansi -c -o kernel.o kernel.c
        
#assemble the kernel.asm file
as86 kernel.asm -o kernel_asm.o
      
#link kernel.o and kernel_asm.o into executable
ld86 -o kernel -d kernel.o kernel_asm.o
    
#copy program into the disk image at sector 3 
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

#run the program
bochs -f opsys.bxrc
