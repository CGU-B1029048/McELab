mov R0, #20h; //R0 = A[start]
mov B, #28h; //B = B[start]
mov R2, #8; //R2 = N
mov R1, #0;
loop:
mov A,@R0;
mul AB;
add A, R1;
mov R1, A;
inc R0;
inc B;
djnz R2, loop;
here:
sjmp here;
END