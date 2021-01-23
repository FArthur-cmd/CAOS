#include <asm/unistd_32.h>
     .intel_syntax noprefix
     .text
     .global _start

_start:
    sub esp, 4 //create space for input char

read:
    mov eax, __NR_read //call read
    mov ebx, 0 //stdin
    mov ecx, esp //put in esp
    mov edx, 1   //count of reading
    int 0x80
    cmp eax, 0
    jz end

write:
    mov eax, __NR_write //call write
    mov ebx, 1    //stdout
    mov ecx, esp  //put out esp
    mov edx, 1   //count of writing
    int 0x80
    jmp read

end:
   add esp, 4
   mov eax, __NR_exit
   mov ebx, 0
   int 0x80
