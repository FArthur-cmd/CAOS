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
    b compare_with_zero

compare_with_zero:
    cmp r0, #'0'
    bge compare_with_nine
    b input

compare_with_nine:
    cmp r0, #'9'
    ble output
    b input

output:
    ldr r1, =stdout
    ldr r1, [r1]
    bl fputc
    b input

end:
    pop {lr}
    bx lr


