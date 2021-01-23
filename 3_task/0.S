    .data
    .global R
R:
    .word

    .text
    .global calculate
calculate:
    push {r4-r7}
    ldr r0, .Lvar
    ldr r1, .Lvar + 4
    ldr r2, .Lvar + 8
    ldr r3, .Lvar + 12
    ldr r4, .Lvar + 16
    
    ldr r0, [r0]
    ldr r1, [r1]
    ldr r2, [r2]
    ldr r3, [r3]

    mul r5, r0, r1       //A*B
    mul r6, r2, r3       //C*D
    add r7, r5, r6       //(A*B) + (C*D)
    
    str r7, [r4]

    pop {r4-r7}
    bx lr
.Lvar:
    .word A
    .word B
    .word C
    .word D
    .word R
