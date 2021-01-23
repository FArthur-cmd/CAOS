    .text
    .global main

main:
    push {r4-r6, lr}

preprocessing:
    mov r4, #4 // one symbol size
    mov r0, #0 
    mov r1, r4
    bl realloc
    mov r5, r0 //save ptr
    mov r6, #0 // where we are
    b reading

reading:
    ldr r0, =stdin
    ldr r0, [r0]
    bl fgetc
    cmp r0, #-1
    beq writing
    b adding_in_memory

adding_in_memory:
    str r0, [r5, r6]
    add r6, #4
    cmp r6, r4
    beq adding_memory
    b reading

adding_memory:
    mov r0, r5
    mov r1, #2
    mul r4, r4, r1 
    mov r1, r4
    bl realloc
    mov r5, r0
    b reading

writing:
    cmp r6, #0
    ble end
    sub r6, r6, #4
    ldr r0, [r5, r6]
    ldr r1, =stdout
    ldr r1, [r1]
    bl fputc
    b writing

end:
    mov r0, r5
    bl free    
    pop {r4-r6, lr}
    bx lr

