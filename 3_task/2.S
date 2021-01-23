    .text
    .global main

main:
    //save lr
    push {lr} 
    
input:
    ldr r0, =stdin
    ldr r0, [r0]
    bl fgetc
    bl check_ending

check_ending:
    cmp r0, #-1
    beq end
    b compare
    
compare:
    push {r4}
    mov r4, r0
    bl isdigit
    cmp r0, #0
    mov r0, r4
    pop {r4}
    blne output
    b input

output:
    ldr r1, =stdout
    ldr r1, [r1]
    bl fputc 
    b input    

end:
    pop {lr}
    bx lr


