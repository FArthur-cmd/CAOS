#include <asm/unistd_32.h>
    .intel_syntax noprefix
    
    .data
buffer_size:
    .long 8096  //4 kB
buffer_ptr:
    .long 0
endl:
    .byte '\n'   // '\n'
previous_endl:
    .long 0
current_endl:
    .long 0
need_endl:
    .byte 0

    .text
    .global _start

_start:
    mov eax, __NR_brk //call brk to fing pointer
    mov ebx, [buffer_ptr] 
    int 0x80

    mov [buffer_ptr], eax //save ptr to begining

    mov eax, __NR_brk     //resize to first size
    mov ebx, [buffer_ptr]
    add ebx, [buffer_size]
    int 0x80

    sub esp, 4
    mov eax, 0
    mov [esp], eax //counter of current bytes
    jmp read

read:
    mov eax, __NR_read     //read some letters
    mov ebx, 0
    mov ecx, [buffer_ptr]
    add ecx, [esp]
    mov edx, 1024
    int 0x80
    
    cmp eax, 0  //if input ended go to write
    jz write
    
    add [esp], eax       //need more memory?
    mov ebx, [buffer_size]
    sub ebx, 1024
    cmp ebx, [esp] 
    jle add_memory
    jmp read

add_memory:
    mov ebx, [buffer_size] //resize *2
    add ebx, [buffer_size]
    mov [buffer_size], ebx
    mov eax, __NR_brk
    mov ebx, [buffer_ptr]
    add ebx, [buffer_size]
    int 0x80
    jmp read

write:
    mov eax, [buffer_ptr]
    add eax, [esp]
    mov [previous_endl], eax //first \n in ouput is last \n in input
    cmp eax, [buffer_ptr] //check for emptiness
    jz print_endl
    sub eax, 1      //look at N-1 byte
    mov edx, [endl] //compare with \n
    mov ecx, [eax]  
    cmp cl, dl       
    jz fill_need   //check out do we need last \n
    add eax, 1     // if last not \n than collect it in string to out
    jmp start_to_print

fill_need:
    mov edx, 1
    mov [need_endl], edx 

start_to_print:
    mov [previous_endl], eax

find_all_symbols:
    dec eax
    cmp eax, [buffer_ptr]  //go next symbol, until \n
    jz set_start           // or start of array
    mov edx, [endl]
    mov ebx, [eax]
    cmp bl, dl
    jnz find_all_symbols
    mov [current_endl], eax
    jmp write_line

set_start:
    mov ecx, [buffer_ptr]    //if start is first element, put start - 1
    dec ecx
    mov [current_endl], ecx

write_line:
    mov eax, __NR_write     //print from start + 1 to end - 1
    mov ebx, 1              // not to include two \n 
    mov ecx, [current_endl] // only printing text between them
    add ecx, 1
    mov edx, [previous_endl]
    sub edx, [current_endl] //count of symbols
    sub [esp], edx        //N -=  (count of printed)
    sub edx, 1
    int 0x80

check_last_symbol:
    mov eax, [need_endl]   //if we don't need \n
    cmp eax, 0             
    jz check_current_endl
    jmp print_endl

check_current_endl:
    mov eax, [current_endl] //and it is the last string we have
    add eax, 1
    cmp eax, [buffer_ptr]
    jz end                  // don't put \n
    
print_endl:
    mov ecx, [previous_endl] //print only one \n
    mov eax, [endl]          //after strings which need \n
    mov [ecx], eax
    mov eax, __NR_write
    mov ebx, 1
    mov edx, 1
    int 0x80
    mov edx, [esp]
    cmp edx, 0     //if it was last symbol
    jnz write

end: 
    add esp, 4
    mov eax, __NR_exit
    mov ebx, 0
    int 0x80           //return 0
