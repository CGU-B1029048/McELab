# Microcontroller Experiment Lab07 Report

- [Microcontroller Experiment Lab07 Report](#microcontroller-experiment-lab07-report)
	- [Problem Description](#problem-description)
	- [Theory Explanation](#theory-explanation)
	- [Code \& Explanation](#code--explanation)
			- [Timer Setting and Pin Define](#timer-setting-and-pin-define)
			- [Main Function - Setup Configuration](#main-function---setup-configuration)
			- [Play Note and Count Second Function in ISR](#play-note-and-count-second-function-in-isr)
			- [Play a Song](#play-a-song)
	- [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
			- [`Lab07.c`](#lab07c)
    
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

## Theory Explanation
To count the frequency and Time for ISR, formula is shown as following.
$$ T = \frac{1}{f} $$
When we set the ISR frequency as T and set it to ring the buzzer, The buzzer will make sound with frequency $f$.

The 8051 board has a Oscillator cycle of 24.5MHz, With Timer mode 0 or 1, the timer will 除頻 by 12 and divide 1 (8 by default) to roughly 2MHz, cycle time of 0.5 $\mu$s

if we set `0xffec = (TH_0, TL_0)` $\rightarrow$ 20 cycle (`0xffff - 0xffec` = 20) per 1 ISR, 
$$24.5 \times 10^6 \text{(Hz)} \div 12 \div 1 = f\approx 2\times 10^6 $$
$$T = 1/f \approx 5 \times 10^{-7} \text{(s)} = 0.5 (\mu s)$$
$$20 \times 0.5 = 10 (\mu s) = 0.01 (ms)$$
so we get roughly 0.01ms for 1 ISR cycle, which 100000 ISR cycle is a second.

Then we get frequency of notes show as below.

| Note | 簡譜 | Frequency (Hz) |
|---|---|---|
| Do | 1 | 261.63 |
| Re | 2 | 293.66 |
| Mi | 3 | 329.63 |
| Fa | 4 | 349.23 |
| So | 5 | 392 |
| La | 6 | 440 |
| Si | 7 | 493.88 |
| 高音Do | 1^ | 523.25 |

So we get the frequency of the note $f_{notes}$, we which means the buzzer must give $f_{notes}$ pulses in 1 second. So we can get the cycle time $T$ from following formula for pulses generation.
$$T = \frac{1}{2} \times \frac{1}{f_{notes}}$$
note: T get divided by 2 since a pulse cycle is on $\rightarrow$ off $\rightarrow$ on, 2 signal sent in one cycle.

Then we convert the $T$ from second to amount of ISR cycles, which will get the following formula.
$$T_{ISR} = 100000 \times T\text{(sec)} = 100000 \times \frac{1}{2} \times \frac{1}{f_{notes}}  $$

So we'll get the following table represent the ISR cycle counts for each note in 1 second.

| Note | $T_{\text{ISR cycle}}$ |
|---|---|
| Do | 190 |
| Re | 170 |
| Mi | 150 |
| Fa | 143 |
| So | 127 | 
| La | 113 |
| Si | 100 |
| 高音Do | 92 |

But the board has bug so actually 50000 ISR cycle per 1 second. So we just use 50000 ISR cycle for 1 second in this lab, we have found out the reason of it, which will explain in [[#Difficulties Encountered and Solutions]].

## Code & Explanation
We divide the Problem into 2 files to implement, `Lab07_basic.c`, `Lab07.c`.
#### Timer Setting and Pin Define
```c
#include "C8051F040.h"

void Port_Configuration (){
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE; // Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;
	P1MDOUT = 0xff;

	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Port_Configuration  

void Timer_Configuration (){
	TMOD = 0x01;
	TCON = 0x10;
	CKCON = 0x10;
	
	IE = 0x82;
	TL0 = 0xec;
	TH0 = 0xff;
	
	TL1 = 0xf0;
	TH1 = 0xD8;
}//end of function Timer_Configuration#include "C8051F040.h"

void Config (){

	//turn-off watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;
	
	SFRPAGE = CONFIG_PAGE;
	OSCICN = 0x83;
	CLKSEL = 0x00;
	
	Port_Configuration ();
	Timer_Configuration ();

}//end of function Default_Config
```

Here we set the `TL0 = 0xec` and `TH0 = 0xff`, so we'll get the timer ISR interrupt when `setup` cycles, which is `ffff - ffec(TH_0,TL_0) = 20`, and we setup `OSCICN` and `CLKSEL` so that the Oscillator will divide it's frequency to 1, getting 100000 ISR cycle for 1 second which is what we want.

<div style="break-after: page; page-break-after: always;"></div>

#### Main Function - Setup Configuration
```c
int song_table[8] = {190/*D01*/, 170/*RE2*/, 150/*MI3*/, 143/*FA4*/, 127/*SO5*/, 113/*LA6*/, 100/*SI7*/, 92/*DO11*/};

int main (){
	Config ();
	status = 0x00;
	count = 0;
	count_sec = 0;
	n = 0;
	P1 = 0;
	while (1) {
		P2 = status;
	}//end while (1)
}//end of function main
```

We create the table to store how many ISR cycles buzzer should change cycle in 1 second in `song_table`, then we use `status` to record whether the buzzer should be on or not, then use while loop to send status into `P2`, which will later send to buzzer.

#### Play Note and Count Second Function in ISR

```c
void Timer0_ISR () interrupt 1 {
	count++;
	count_sec++;
	
	//duration
	if (count_sec >= 12500) { // 1sec = 50000
		count_sec = 0;
		
		n++;
		if(n >= 8) n = 0;
		if ((n%4) == 0) P1++;

	}
	
	//frequency
	if (count >= (song_table[n%8])/2) {
		count = 0;
		status = ~status;
	}

	TH0 = 0xff;
	TL0 = 0xec;
}//end of function Timer0_ISR
```
+ Play Note (part under comment `//frequency`)
	We use `count` to change `status` so buzzer will turn on and off when reach $T_{\text{ISR cycle}}$, creating sound with frequency of the note. Since we later found out 1 second is 50000 ISR cycles, so we divide 2 at `song_table` where `count` check buzzer status to match the modified cycle time.
+ Count Second (part under comment `//duration`)
	Since 50000 cycle is 1 second, we set each note to play 0.25 seconds, so each note will play 0.25 second, the change to next note, and when a second reached, which is recorded by `n`, we make `P1` to plus 1, where `P1` LED will record how many second passed.

Thus we finished playing note from Do ~ Do(高音) and counting seconds.

#### Play a Song

```c
int song_table[8] = {190/*D01*/, 170/*RE2*/, 150/*MI3*/, 143/*FA4*/, 127/*SO5*/, 113/*LA6*/, 100/*SI7*/, 92/*DO11*/};
int sheet[36] = {3,3,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,3,2,1/**/,1,1,3,3,3,3,3,3,3,3,3,3,3,3,1,6};//6,6,1,1,1,1,1,1,2,2,2,2,1,1,3,3,3,3};
```
First we store the music sheet of 稻香 (song ordered by Lucy)

```c
	if (count >= (song_table[sheet[n]-1])/2) {
		count = 0;
		status = ~status;
	}
```
Then we changed the part to play notes from simple frequency of Do to Si to the music sheet of 稻香, which will change note every 0.25 second.

Finally, we can hear our song.
<div style="break-after: page; page-break-after: always;"></div>

## Difficulties Encountered and Solutions
We've encounter quite a lot of problem this time, but most of it is at how to implement and implementing the board to count for 1 second.
1. First is the theory part. We took quite a period of time calculating the cycle time, Oscillator frequency, timer and interrupt settings, how to convert the 24.5 MHz Oscillator clock to our note like Do for 261 Hz, but after quite a time, we figure it out with the help from TA, professor and Internet. How to do is written in [[#Theory Explanation]].
2. After Implementation, the board will stuck at after playing Mi or Fa, which confused us. With the help of the professor to debug, we found out the problem is at ISR, where we count the cycles with `count` and `count_sec`, we use `=` instead of `>=`, so when it overflow the program will just stuck.
   ```c
   // failure code example
	//        |
	//        v
	if (count = (song_table[sheet[n]-1])/2) {
		count = 0;
		status = ~status;
	}
	```
3. Miscount of 1 second - Part 1
   After the code running, we found out the frequency and timing is weird, which isn't the same as what we calculated. After quite a time of debuging, we still cannot find anything, so we ask the professor. We thus found out the `OSCICN` and `CKCON` settings for the Oscillator is written in `CONFIG_PAGE`, but we setup up it at `LEGACY_PAGE`, so the Oscillator isn't setup and will divide the frequency by 8 by default. 
4. Miscount of 1 second - Part 2
   Finally, When we think the program should work, the sound of note is still weird, which we later found out the actual time for 1 second is 50000 cycles. It cost us more then a week dealing with this problem, we found everything will be fine as long as using 50000 cycle for 1 second. But it differs from out calculation. After several time asking the professor, we found out that is actually because we use 20 Oscillator cycle for 1 ISR cycles, but the code execute in ISR also takes time. Usually the execution time for code inside can be ignored, but usually ISR will interrupt for 10000 cycles or more, which only few time in 1 second, so the time spent on executing interrupt can be ignored. But since we set it only 20 times, our interrupt code will be executed every $\mu$s, creating delay on the interrupt time. And it happened just exactly to be double the amount of time. If we set the interrupt cycle from 20 Oscillator cycle to 10000, 1 second is the same as we calculated. Thus found out the reason.

<div style="break-after: page; page-break-after: always;"></div>

## Discussion
After this Lab, I've understand more about how to program the buzzer in C with 8051 board. We also know more about the Timer setup and Interrupts, also with playing beautiful sounds and notes using the buzzer. I've also know how to program more about timing and what problem may encounter while playing with interrupts.
This time our board miraculously is functional, but we stuck more on other problem.  Hope our final can be smooth to complete. 
<div style="break-after: page; page-break-after: always;"></div>

## Appendix

### Full code

#### `Lab07_basic.c`
```c
#include "C8051F040.h"

void Port_Configuration (){
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;
	P1MDOUT = 0xff;
	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Port_Configuration

void Timer_Configuration (){
	TMOD = 0x01;
	TCON = 0x10;
	CKCON = 0x10;

	IE = 0x82;
	TL0 = 0xec;
	TH0 = 0xff;

	TL1 = 0xf0;
	TH1 = 0xD8; 
}//end of function Timer_Configuration#include "C8051F040.h"

void Config (){
	//turn-off watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	SFRPAGE = CONFIG_PAGE;
	
	OSCICN = 0x83;
	CLKSEL = 0x00;

	Port_Configuration ();
	Timer_Configuration ();
}//end of function Default_Config

unsigned char status;
long count, count_sec;
int n;

void Timer0_ISR ();

//noted haven't done
int song_table[8] = {190/*D01*/, 170/*RE2*/, 150/*MI3*/, 143/*FA4*/, 127/*SO5*/, 113/*LA6*/, 100/*SI7*/, 92/*DO11*/};
int sheet[36] = {3,3,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,3,2,1/**/,1,1,3,3,3,3,3,3,3,3,3,3,3,3,1,6};//6,6,1,1,1,1,1,1,2,2,2,2,1,1,3,3,3,3};

int main (){
	Config ();
	status = 0x00;
	count = 0;
	count_sec = 0;
	n = 0;
	P1 = 0;
	while (1) {
		P2 = status;
	}//end while (1)
}//end of function main

//to control signal frequency
void Timer0_ISR () interrupt 1 {
	count++;
	count_sec++;
	
	//duration
	if (count_sec >= 12500) { // 1sec = 50000
		count_sec = 0;
		
		n++;
		if(n >= 8) n = 0;
		if ((n%4) == 0) P1++;

	}
	
	//frequency
	if (count >= (song_table[n%8])/2) {
		count = 0;
		status = ~status;
	}

	TH0 = 0xff;
	TL0 = 0xec;
}//end of function Timer0_ISR



//to change the tone
void Timer1_ISR () interrupt 3{
	//counting for 1 second…
	//n = Song_Table [i];
	//i = (i+1)%7;
}
``` 

#### `Lab07.c` 

```c
#include "C8051F040.h"

void Port_Configuration (){
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;
	P1MDOUT = 0xff;
	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Port_Configuration

void Timer_Configuration (){
	TMOD = 0x01;
	TCON = 0x10;
	CKCON = 0x10;

	IE = 0x82;
	TL0 = 0xec;
	TH0 = 0xff;

	TL1 = 0xf0;
	TH1 = 0xD8; 
}//end of function Timer_Configuration#include "C8051F040.h"

void Config (){
	//turn-off watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	SFRPAGE = CONFIG_PAGE;
	
	OSCICN = 0x83;
	CLKSEL = 0x00;

	Port_Configuration ();
	Timer_Configuration ();
}//end of function Default_Config

unsigned char status;
long count, count_sec;
int n;

void Timer0_ISR ();

//noted haven't done
int song_table[8] = {190/*D01*/, 170/*RE2*/, 150/*MI3*/, 143/*FA4*/, 127/*SO5*/, 113/*LA6*/, 100/*SI7*/, 92/*DO11*/};
int sheet[36] = {3,3,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,3,2,1/**/,1,1,3,3,3,3,3,3,3,3,3,3,3,3,1,6};//6,6,1,1,1,1,1,1,2,2,2,2,1,1,3,3,3,3};

int main (){
	Config ();
	status = 0x00;
	count = 0;
	count_sec = 0;
	n = 0;
	P1 = 0;
	while (1) {
		P2 = status;
	}//end while (1)
}//end of function main

//to control signal frequency
void Timer0_ISR () interrupt 1 {
	count++;
	count_sec++;
	
	//duration
	if (count_sec >= 12500) { // 1sec = 50000
		count_sec = 0;
		
		n++;
		if(n >= 36) n = 0;
		if ((n%4) == 0) P1++;

	}
	
	//frequency
	if (count >= (song_table[sheet[n]-1])/2) {
		count = 0;
		status = ~status;
	}

	TH0 = 0xff;
	TL0 = 0xec;
}//end of function Timer0_ISR



//to change the tone
void Timer1_ISR () interrupt 3{
	//counting for 1 second…
	//n = Song_Table [i];
	//i = (i+1)%7;
}
```