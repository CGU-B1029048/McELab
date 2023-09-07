ORG OH

mov R0, #20h; //R0 = A[start]
mov R1, #28h; //B = B[start]
mov R2, #8; //R2 = N
mov R3, #0;
loop:
    mov A,@R0;
    mov B,@R1;
    mul AB;
    add A, R3;
    mov R3, A;
    inc R0;
    inc R1;
    djnz R2, loop;
here:
    sjmp here;
END