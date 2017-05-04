#!/bin/bash

source ../PathSetter.bash


#compile the kernel 
bcc -ansi -c -o kernel.o kernel.c

#assemble the kernel.asm file
as86 kernel.asm -o kernel_asm.o
      
#link kernel.o and kernel_asm.o into executable
#ld86 -o kernel -d kernel.o kernel_asm.o



#compile proc.c
bcc -ansi -c -o proc.o proc.c
#link kernel and proc
ld86 -o kernel -d kernel.o kernel_asm.o proc.o


    
#copy program into the disk image at sector 3 
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

#Store the Disk Map in sector 1 of the disk
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc

#Store the disk directory in sector 2 of the disk
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc



#compile loadFile
gcc -o loadFile loadFile.c

#copy the contents of the message.txt to your disk image
./loadFile message.txt




#assemble the lib.asm file
as86 lib.asm -o lib.o
#compile the uprog1.c program
bcc -ansi -c -o uprog1.o uprog1.c
#link the lib.o and uprog1.o into executable
ld86 -o uprog1 -d uprog1.o lib.o
#copy the contents of uprog1 into the disk image
./loadFile uprog1

#assemble the lib.asm file
as86 lib.asm -o lib.o
#compile the uprog2.c program
bcc -ansi -c -o uprog2.o uprog2.c
#link the lib.o and uprog2.o into executable
ld86 -o uprog2 -d uprog2.o lib.o
#copy the contents of uprog2 into the disk image
./loadFile uprog2


#assemble the lib.asm file
as86 lib.asm -o lib.o
#compile the uprog3.c program
bcc -ansi -c -o uprog3.o uprog3.c
#link the lib.o and uprog3.o into executable
ld86 -o uprog3 -d uprog3.o lib.o userlib.o
#copy the contents of uprog3 into the disk image
./loadFile uprog3


#assemble the lib.asm file
as86 lib.asm -o lib.o
#compile the uprog4.c program
bcc -ansi -c -o uprog4.o uprog4.c
#link the lib.o and uprog4.o into executable
ld86 -o uprog4 -d uprog4.o lib.o userlib.o
#copy the contents of uprog4 into the disk image
./loadFile uprog4


#assemble the lib.asm file
as86 lib.asm -o lib.o
#compile the shell.c program
bcc -ansi -c -o shell.o shell.c
#link the lib.asm and shell.o into executable
#ld86 -o shell -d shell.o lib.o
#copy the contents of shell into the disk image
#./loadFile shell


#compile the userlib.c file
bcc -ansi -c -o userlib.o userlib.c
#link userlib.o, lib.o, and shell.o
ld86 -o shell -d shell.o userlib.o lib.o
#copy the contents of shell into the disk image
./loadFile shell

#compile the texted.c file
bcc -ansi -c -o texted.o texted.c
#link the texted.o, lib.o and userlib.o
ld86 -o texted -d texted.o lib.o userlib.o
#copy the contents of texted into the disk image
./loadFile texted



#run the program
bochs -f opsys.bxrc
