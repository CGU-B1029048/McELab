# Microcontroller Experiment Lab01 Report
#### menu
- [Microcontroller Experiment Lab01 Report](#microcontroller-experiment-lab01-report)
      - [menu](#menu)
  - [Problem Description](#problem-description)
    - [Goal of Lab01](#goal-of-lab01)
    - [Detail Description](#detail-description)
  - [Code \& Explanation](#code--explanation)
  - [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
  - [Disscussion](#disscussion)
## Problem Description
### Goal of Lab01
+ Introduction to our first 8051 assembly program
+ write a program to compute the following equation
$$ S= \sum^{N-1}_{i=0} A[i]*B[i] $$
### Detail Description
+ Introduction of 8051
To understand the basic setup and structure of 8051 and brief introduction to the 8051's assemable language. Learn to use Sylicon Lab IDE to design your microcontroller program and assembly programming.
+ write a program to compute the following equation
$$ S= \sum^{N-1}_{i=0} A[i]*B[i] $$
where A[i], B[i] are integer array (8-bit) in 8051's internal memory (RAM).
We set the start address of array A at 20h, array B at 28h, and N = 8. The data in the array will be modify through the IDE, and the sum will store in a register (R3).

<div style="break-after: page; page-break-after: always;"></div>

## Code & Explanation
```asm
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
```
We first setup our variables before loop, which setup the begining of array A and B, N and sum. Array's begining location is store in register `R0` and `R1`, so we can use it as a pointer. Later, we move in the loop to do the sum of multiplication. Since `mul` can only use register `A` & `B`, so we move `A[i]` and `B[i]` into `A` and `B` using the address store in `R0` and `R1`. Then, we add the multiplication result with sum to get new sum, then store back to `R3`. Finally, we will do `i++`, which is `inc` the address store in `R0` and `R1`, and loop again if `R2` is 0.
![Pic](Lab01.png)
We can see the result that A and B is [1,1,1,1,1,1,1,1] and [1,2,3,4,5,6,7,8], after program finished, `R0` is at end of A, 28h, `R1` at end of B, 30h. result is at `R3` equals
$ \sum^{n=8}_{i=1} 1*i = 36 = 30h$

<div style="break-after: page; page-break-after: always;"></div>

## Difficulties Encountered and Solutions
We initially face difficulties when we were unable to connect the board to the computer. After reading the manual, we located all the necessary cables and the USB debug adapter. However, we soon discovered that the board still couldn't be connected to the computer. 
At first, we thought the problem was with the USB debug adapter or it's cable's problem, but after switching many cables and adapters, the problem still remain unsolved, only to find that our first adapter is defective. After our relentless attemp, we finally discover the core problem, a defective power cable. Once we replaced the power cable, everything started working smoothly.
We then encounter our second problem: debuging. We had successful written and built the assembly program, but we've no idea how to check if the result is correct. We learned from our teacher and classmate that we could verify our code by open the register and RAM viewing panel and modifying our testcases there.
After we solved all of the problems, we had a basic understanding of assembly programming and completed our demo.
## Disscussion
In this lab, I learned that hardware can have a big impact on our programs.For example, we had to change 3 USB adapters and more than 5 cables before we found the disabled power cable was what causing the problem. But also, I've learned the basic of our 8051 board and some basic assembly language, including its limits. Specific operation is assigned to specific registers. I also learned to program from a hardware-first perspective. I think I've learned much in this lesson, and hope our next lab can go smoothly for the board to work without changing lots of cable again.