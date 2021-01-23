#include <asm/unistd_32.h>
    .intel_syntax noprefix
    .data
output_str:
    .string "Hello, World!"
output_ptr:
    .long output_str

    .text
    .global _start

_start:
    mov eax, __NR_write // write
    mov ebx, 1          // stdout
    mov ecx, output_ptr // ptr to hello
    mov edx, 13         // count of bytes
    int 0x80            // interrapting

    mov eax, __NR_exit  // return
    mov ebx, 0          // return 0
    int 0x80
