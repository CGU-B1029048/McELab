# McELab
Microcontroller Experiments Lab Projects

---
### Project menu
- [McELab](#mcelab)
    - [Project menu](#project-menu)
    - [Lab01](#lab01)
    - [Lab02](#lab02)
    - [Lab03](#lab03)
    - [Lab04](#lab04)
    - [Lab05](#lab05)
--- 
### Lab01
Write a Program to Compute 

$$ S= \sum^{N-1}_{i=0} A[i]*B[i] $$

### Lab02
Design your own LED box
+ initial all LED off
+ The LED run certain pattern after some button pressed
+ At least 4 different patterns
+ you can design your own pattern

### Lab03
Same as Lab02, but instead of using `debounce`, use ISR Timer Interrupt to accomplish to same goal.
Design your own LED box
+ initial all LED off
+ The LED run certain pattern after some button pressed
+ At least 4 different patterns
+ you can design your own pattern

### Lab04
Same as Lab03, but instead of using an `.asm` file to implement, we use C to write the same program, including Timer ISR.
**Only C programming is allowed** for this lab!
+ Basic Part: (25%)
    + Two buttons, one for 1-bit left shift and one for 1-bit
right shift
    + Initialize the LED with one bit set to 1, and others to 0
+ Advanced Part: (65%)
    + The LED runs 4 modes (Just like what we did before)
    + As a button is pressed, the mode changes. 
    + Mode-changing sequence: Mode 0 → Mode 1 → Mode 2 → Mode 3 → Mode 0 ... (Mode 0-3: change LED pattern every sec. Refer to Lab2-3)
+ Hint: use the timer interrupt to make two I/O devices work simultaneously

### Lab05
Write sss