    .global summ
    .intel_syntax noprefix
    .text

summ:
    push ebx  // save register

    mov ecx, [esp+8]   //load N into counter

go_through_array:
    mov ebx, ecx   //load to ebx to watch N-1 element 
    dec ebx  //N-1
    
    mov edx, [esp + 12] //watch A array
    mov eax, [edx + 4*ebx] // A[i]
    
    mov edx, [esp + 16] // watch B array
    add eax, [edx + 4*ebx] // A[i] + B[i]
    
    mov edx, [esp + 20] // watch R array
    mov [edx + 4*ebx], eax // load to R[i]
    
    cmp ebx, 0 // if N>0 watch next
    dec ecx
    jg go_through_array

    pop ebx
    ret
