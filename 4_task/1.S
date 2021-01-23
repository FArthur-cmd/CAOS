    .intel_syntax noprefix
    .text
    .global my_sin

my_sin:
    push     ebp
    mov      ebp,  esp // alpha = [ebp + 8] 
    mov      eax,   1
    sub      esp,   4
    mov      [esp], eax
    mov      eax,   -1
    sub      esp,   4
    mov      [esp], eax
    jmp      preparing_phase
    
preparing_phase:
    movsd     xmm0, [ebp + 8] // result
    movsd     xmm1, [ebp + 8] // psevdo_result
    movsd     xmm2, [ebp + 8] // current_step_calculation

    cvtsi2sd  xmm3, [ebp - 4] //conter of factorial
 
    jmp      count_step

count_step:
    cvtsi2sd  xmm4, [ebp - 4] // +1

    addsd     xmm3, xmm4       //increasing N 
    divsd     xmm2, xmm3      // and div N to get N!
    addsd     xmm3, xmm4   
    divsd     xmm2, xmm3
    
    cvtsi2sd  xmm4, [ebp - 8] // -1

    mulsd     xmm2, [ebp + 8]  //mul x^2
    mulsd     xmm2, [ebp + 8]
    mulsd     xmm2, xmm4

    addsd     xmm1, xmm2       //add to answer
    comisd    xmm1, xmm0       //compare to overgetting in double
    jz        finish_function
    jmp       update

update:
    movsd     xmm0, xmm1
    jmp       count_step

finish_function:
    sub       esp,  8
    movsd     [esp], xmm0
    fld       qword ptr [esp]
    mov       esp, ebp
    pop       ebp
    ret
