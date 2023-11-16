# Microcontroller Experiment Lab07 Report

- [Microcontroller Experiment Lab07 Report](#microcontroller-experiment-lab07-report)
	- [Problem Description](#problem-description)
	- [Code \& Explanation](#code--explanation)
		- [Basic](#basic)
			- [Pin define](#pin-define)
			- [Button Detect](#button-detect)
			- [Main function](#main-function)
		- [Bonus 1](#bonus-1)
			- [Pin Define and Button Detect](#pin-define-and-button-detect)
			- [Main Function](#main-function-1)
		- [Bonus 2](#bonus-2)
			- [Main Function Initialize](#main-function-initialize)
			- [Main Function - Cursor Movement](#main-function---cursor-movement)
	- [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
	- [Disscussion](#disscussion)
	- [Appendix](#appendix)
		- [Full code](#full-code)
			- [`lab05_basic.c`](#lab05_basicc)
    
## Problem Description
+ Task for this Lab : Play a song you like
	+ Multiple tones
	+ Frequency of sound changes regularly
+ Basic : play each tone by request
	+ Do, Re, Mi, Fa, So, La, Si, Do
+ Bonus 1 : Play a song
	+ Music sheet (歌譜) stored in an array and can be changed without modifying the program
+ Bonus 2 : Show the time-counting on LCD while the song is playing 
	+ Three jobs controlled by timers
	+ Using 3 timers? (I don’t think so...)
+ Your Report: 
    + Bonus: include the discussion of the previous two Q’s
<div style="break-after: page; page-break-after: always;"></div>

To count the frequency and Time for ISR, formula is shown as following.
$$ T = \frac{1}{f} $$
When we set the ISR frequency as T and set it to ring the buzzer, The buzzer will make sound with frequency $f$.

The 8051 board has a Oscillator cycle of 24.5MHz, With Timer mode 0 or 1, the timer will 除頻 by 12 and divide 8 by default to roughly 0.255MHz, cycle time of 4 $\mu$s

if 0xffec (TH_0, TL_0) $\rightarrow$ 24.5MHZ/12/1, roughly 0.01ms for 20 cycle.
## Code & Explanation
We divide the Problem into 3 files to implement, `lab05_basic.c`, `Lab05_b1.c`, `Lab05.c`.
### Basic
We implement the basic part here. (`lab05_basic.c`)
#### Pin define
```c
```

<div style="break-after: page; page-break-after: always;"></div>

#### Button Detect
```c

```
<div style="break-after: page; page-break-after: always;"></div>

#### Main function

```c
```

<div style="break-after: page; page-break-after: always;"></div>

### Bonus 1
We implement the bonus 1 part here. (`lab05_b1.c`)

#### Pin Define and Button Detect
```c
```
<div style="break-after: page; page-break-after: always;"></div>

#### Main Function
As the same, I'll skip the display of LCD control function given in the example file. Full code will be provided in the Appendix.
```c
```
<div style="break-after: page; page-break-after: always;"></div>

### Bonus 2
We implement the bonus 1 part here. (`lab05.c`)

#### Main Function Initialize
```c
```

<div style="break-after: page; page-break-after: always;"></div>

#### Main Function - Cursor Movement
```c
```


## Difficulties Encountered and Solutions
We've encounter quite a lot of problem this time.
First is the Board Problem. Maybe our luck is too good, we have already meet a lot of time from lab3 until now. This lab we spend 2 weeks to complete, but we encountered it at both week. Always when we are debugging on our code, but it turns out that is board problem. Hope next time we can find a borad without problem at the first time.
Later, wehn we are making our bonus 1, we encounter an overflow at line changing at demo. When pressed over 16 time from 0, the character will disappear at LCD and after 24 times it will display in the second row. We thus go and redesign our code. We come out with a solution by resrict our data length, which limit them in only 16 characters, thus solve the problem.
Finally, When we working on bonus 2, while inserting data at the cursor position, the LCD always cannot display correct characters behind insert data. Which caused us a lot of time debugging. After we ask the professor and TA, we found out is our code's logic has flaw. We then rewrite our `insert_data()` function, which solved our problem.

## Disscussion
After this Lab, I've understand more about how to program the LCD in C with 8051 board. I've also learn how to position the cursor with C, and how inserting a character might take to display it. We were used to typing and inserting word in program such as word or VScode, even terminal. They've written these function already so we don't need to worry for it. By this Lab, I've understand more about the control of it. But the most of it is, again, our 8051 board. We've almost encountered failure board every time in lab recently, and struggled for so long to find our is the board have problem. Hope next Lab we can be free from the cycle of getting failure boards.

<div style="break-after: page; page-break-after: always;"></div>

## Appendix
### Full code
#### `lab05_basic.c`
```c
```