ORG 0h; Make sure program starts from this program

mov R0, #20h; R0 = A[start] at $20h
mov R1, #28h; B = B[start] at $28h
mov R2, #8;   R2 = N
mov R3, #0;   R3 = sum
loop:
    mov A,@R0; A <= A[i]
    mov B,@R1; B <= B[i]
    mul AB;    A*B
    add A, R3; sum += A*B
    mov R3, A; store sum
    inc R0;    A inc
    inc R1;    B inc
    djnz R2, loop; loop until n = 0
here:
    sjmp here; Ensure program ends here
END