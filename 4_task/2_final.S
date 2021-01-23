    .intel_syntax noprefix
    .text
    .global mergesort

mergesort:
    push ebp
    push ebx
    mov  ebp, esp
    
// [ebp + 12] = from
// [ebp + 16] = to
// [ebp + 20] = A
// [ebp + 24] = B

    mov ebx, 0        //B_counter
    mov ecx, [ebp + 12] //A_counter
    jmp copy_A_to_B

copy_A_to_B:
    cmp ecx, [ebp + 16]    //copy if A_couter < to
    jge start_merge
    mov edx, [ebp + 24]    // B*
    mov eax, ebp           // A
    add eax, 20
    mov eax, [eax]
    add eax, ecx
    add eax, ecx
    add eax, ecx
    add eax, ecx
    mov eax, [eax]
    mov [edx + 4*ebx], eax //B[i] = A[j]
    inc ebx                // ++i
    inc ecx                // ++j
    jmp copy_A_to_B

start_merge:
    mov ebx, 1           //2^k
    sub esp, 4           //alloc memory for 2^k
    mov [esp], ebx        //[ebp - 4] = 2^k
    mov ebx, 0           //start of merging
    sub esp, 4
    mov [esp], ebx        //[ebp - 8] = start of merging
    add ebx, [ebp - 4]
    sub esp, 4
    mov [esp], ebx        //[ebp - 12] = middle
    add ebx, [ebp - 4]
    sub esp, 4
    mov [esp], ebx        //[ebp - 16] = end
    jmp check_mid

prepare:
    mov ebx, 0
    mov [ebp - 8], ebx     // set start of first array   
    add ebx, [ebp - 4]    
    mov [ebp - 12], ebx   // set mid of two parts
    add ebx, [ebp - 4]
    mov [ebp - 16], ebx   // set end of two parts
    jmp check_mid

check_mid:
    mov ebx, [ebp - 12]   // if mid >= size -> next iter
    mov ecx, [ebp + 16]
    sub ecx, [ebp + 12]
    cmp ebx, ecx
    jge increase_counter 
    jmp check_end

check_end:  
    mov ebx, [ebp - 16]   // if end >= size -> end = size
    mov ecx, [ebp + 16]
    sub ecx, [ebp + 12]
    cmp ebx, ecx
    jle copy_first_part
    mov ebx, ecx
    mov [ebp - 16], ebx
    jmp copy_first_part
    
increase_counter:
    mov ebx, [ebp - 4]  // 2^k * 2
    imul ebx, 2
    mov [ebp - 4], ebx
    mov ebx, [ebp + 16]
    sub ebx, [ebp + 12]
    cmp ebx, [ebp - 4]
    jle final
    jmp prepare
    
copy_first_part:
    mov ebx, [ebp + 24]  //copy everything from A to B not to spoil A
    mov ecx, [ebp - 12]
    jmp copy_element

copy_element:
    cmp ecx, [ebp - 8]
    jle sort
    mov edx, ecx
    dec edx
    sub esp, 4
    mov eax, [ebx + 4*edx]
    mov [esp], eax
    dec ecx
    jmp copy_element
    
sort:
    mov ecx, [ebp - 8]  //where in first part
    mov edx, [ebp - 12] //where in second part
    mov eax, [ebp + 24] //Array
    mov ebx, ecx        //where to place
    jmp place

place:
    cmp ecx, [ebp - 12] // while left < mid
    jge next_iter //some elements from second are on right place already
    cmp edx, [ebp - 16] // and second_left < right
    jge copy_first_to_answer //else put everything from stack
    add eax, edx
    add eax, edx
    add eax, edx
    add eax, edx
    mov eax, [eax]
    cmp [esp], eax
    mov eax, [ebp + 24]
    jle first_smaller  // if first < second put first 
    jmp second_smaller // else put second

next_iter:
    mov ecx, [ebp - 8] //set next borders 
    add ecx, [ebp - 4] // as previous borders + 2*2^k
    add ecx, [ebp - 4] 
    mov [ebp - 8], ecx
    mov edx, [ebp - 12]
    add edx, [ebp - 4]
    add edx, [ebp - 4]
    mov [ebp - 12], edx
    mov ebx, [ebp - 16]
    add ebx, [ebp - 4]
    add ebx, [ebp - 4]
    mov [ebp - 16], ebx
    jmp check_mid

first_smaller:
    push edx              //put first number in array from stack
    mov edx, [esp + 4]
    mov [eax + 4*ebx], edx
    pop edx
    inc ebx
    inc ecx
    add esp, 4
    jmp place
    
second_smaller:          //put second number from second part of array
    push ecx
    mov ecx, [eax + 4*edx]
    mov [eax + 4*ebx], ecx
    pop ecx
    inc ebx
    inc edx
    jmp place    

copy_first_to_answer:  //if all second already placed
    cmp ecx, [ebp - 12] // place all from stack
    jge next_iter
    push edx
    mov edx, [esp + 4]
    mov [eax + 4*ebx], edx
    pop edx
    inc ebx
    inc ecx
    add esp, 4
    jmp copy_first_to_answer    

final:
    mov esp, ebp
    pop ebx
    pop ebp
    ret
