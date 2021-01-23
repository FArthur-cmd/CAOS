     .intel_syntax noprefix
     .text
     .global dot_product

dot_product:
    push     ebp
    mov      ebp, esp 
    //int    N = [ebp + 8]
    //float *A = [ebp + 12]
    //float *B = [ebp + 16]
    push     ebx

    mov      eax, [ebp + 8]  //N
    mov      ebx, [ebp + 12] //A
    mov      edx, [ebp + 16] //B
    mov      ecx, 0          //index in current array
    cvtsi2sd xmm2, ecx      //xmm2 = 0, -for result
    jmp      count_dot_product_while_n_bigger_than_four

count_dot_product_while_n_bigger_than_four:
    cmp      eax,  4
    jl       count_dot_product_n_smalle_than_four
   
    movups   xmm0, [ebx + ecx*4]   //load A[i .. i+3]
    movups   xmm1, [edx + ecx*4]   //load B[i .. i+3]
    dpps     xmm0, xmm1, 0xF1      // make mask 0001111
    // bpps calculates dot product for A[j] and B[j] if 
    // imm8[4 + j] == 1, we calculate all elements
    // and place result in last element of first array
    // making other elements equal to zero
    
    addss    xmm2, xmm0           //save result
    sub      eax, 4               // N     -= 4
    add      ecx, 4               // index += 4
    jmp      count_dot_product_while_n_bigger_than_four


count_dot_product_n_smalle_than_four:  
    cmp      eax, 0 
    jle      ending

    movss    xmm0, [ebx + ecx*4]   //load A[i]
    movss    xmm1, [edx + ecx*4]   //load B[i]
   
    mulss    xmm0, xmm1            //A[i]*B[i]
    addss    xmm2, xmm0            //save result

    dec      eax                   //--N
    inc      ecx                   //++index

    jmp      count_dot_product_n_smalle_than_four

ending:
    sub     esp,   4
    movss   [esp], xmm2
    fld     dword ptr [esp]
    mov     esp,   ebp
    sub     esp,   4
    pop     ebx
    pop     ebp
    ret
