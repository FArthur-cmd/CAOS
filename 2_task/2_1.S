//int summ(int x0, int N, int *X)
//x0 - r0
//N  - r1
//*X - r2
//return r
    .text
    .global summ

summ:
    ldr r3, [r2]   //loading X[0] in r3
    add r0, r0, r3 //x0 + X[0]
    sub r1, r1, #1 //decreasing N
    add r2, r2, #4 //X[1]

    cmp r1, #0     //compere N with 0
    bgt summ       //if N>0 continue summ

    bx lr
