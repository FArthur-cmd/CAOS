// int solve(int A, int B, int C, int D);
// r0        r0     r1     r2     r3

    .text
    .global solve

solve:
    push {r4-r7}   // reserve for using
    sub r4, r4, r4 // create zero for iteration i=0
    b calculate

iterations:
    add r4, r4, #1 //++i
    b calculate    

calculate:
    //A * x^3
    mul r5, r0, r4 
    mul r5, r5, r4
    mul r5, r5, r4
    
    //B*x^2
    mul r6, r1, r4
    mul r6, r6, r4
    
    //C * x
    mul r7, r2, r4
    

    //A*x^3 + B*x^2 + C*x + D
    add r7, r7, r3
    add r7, r7, r6
    add r7, r7, r5

    //check if it is solution
    b check_zero

check_zero:
    cmp r7, #0    // if not zero, continue iterations
    bne iterations 
    
    mov r0, r4
    pop {r4-r7}
    bx lr
