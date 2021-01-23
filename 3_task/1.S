    .data
inp:     
    .asciz "%d%d"
outp:   
    .asciz "%d"


    .text
    .global main
main:
    //save lr
    push {lr}

    //move sp for reserving data
    sub sp, sp, #8
    //input string
    ldr r0, =inp
    //for first number
    mov r1, sp
    // for second number
    mov r2, sp
    add r2, r2, #4

    bl scanf
    //load first
    ldr r1, [sp]
    add sp, sp, #4
    //load second
    ldr r2, [sp]
    add sp, sp, #4
    
    //print answer
    add r1, r1, r2
    ldr r0, =outp
    bl printf

    pop {lr}
    bx lr
