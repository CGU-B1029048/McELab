# Microcontroller Experiment Lab06 Report

- [Microcontroller Experiment Lab06 Report](#microcontroller-experiment-lab06-report)
	- [Problem Description](#problem-description)
	- [Code \& Explanation](#code--explanation)
		- [Basic](#basic)
			- [Pin define](#pin-define)
			- [Bitmap Generation](#bitmap-generation)
			- [Switch Page(half)](#switch-pagehalf)
			- [Initialize at middle (basic and bonus 1)](#initialize-at-middle-basic-and-bonus-1)
		- [Bonus 2](#bonus-2)
			- [Main Function](#main-function)
			- [Draw Function](#draw-function)
	- [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
	- [Disscussion](#disscussion)
	- [Appendix](#appendix)
		- [Full code](#full-code)
			- [`Lab06_bouns01.c`](#lab06_bouns01c)
			- [`glcd.c`](#glcdc)
    
## Problem Description
+ learn to display characters on the LCD
+ Basic: 
  + “Draw” a still image, such as an apple, a tree, or a smiling face (Do not show meaningless lines, patches, or squares)
  + Hint: find a tool to generate the 128x64 bitmap of your image (Google may help...)
+ Advanced:
  + Initialized in the middle of 128x64 panel
  + Animation
    + Move the pattern horizontally [Note: The animation should pass through both panels!]
    + Move the pattern vertically (+15%) [Note: the animation should move one pixel up / down at a time!]

<div style="break-after: page; page-break-after: always;"></div>

## Code & Explanation
We divide the Problem into 2 files to implement, `glcd.c`, `Lab06_bonus01.c`.
### Basic
We implement the basic part and the Initizlize part of Advanced here. (`Lab06_bonus01.c`)
#### Pin define
```c
#include "C8051F040.h"
#include "glcd.h"
int mode;

void
system_init_config ()
{
	//turn-off the watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;		// Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;
	P0MDOUT = 0xff;
}//end of function system_init_config
```
Semae as the example file. `P2` & `P4` for controlling the GLCD. `P2` is used to select mode, left/right half and enable while `P4` for sending the data or command.

<div style="break-after: page; page-break-after: always;"></div>

#### Bitmap Generation
```c
const unsigned char DVD[2][32] = {
{0x00, 0xF0, 0xFB, 0x3B, 0x83, 0x83, 0x83, 0x83, 0xE7, 0x7F, 0x3F, 0x03, 0x07, 0x7F, 0xF8, 0xC0,
0xC0, 0x70, 0x1C, 0x0E, 0x07, 0xC3, 0xFB, 0x7B, 0x03, 0x03, 0x83, 0x83, 0xC6, 0x7E, 0x3C, 0x00 },
{0x00, 0x21, 0x31, 0x71, 0x71, 0x51, 0x50, 0x50, 0x58, 0x58, 0x48, 0x78, 0xC8, 0xC8, 0xCB, 0x89,
0x88, 0xC8, 0xC8, 0xC8, 0x78, 0x49, 0x59, 0x59, 0x59, 0x51, 0x51, 0x50, 0x70, 0x30, 0x20, 0x20 }
};
```
We decide to generate this image
![DVD](https://upload.wikimedia.org/wikipedia/zh/thumb/1/18/Dvd-video-logo.svg/1200px-Dvd-video-logo.svg.png)
So we must first process the image into bitmap like above for GLCD to display. We use 小畫家 to adjust this picture into 32x32 pixels btimap `.bmp` file. At last use LCDAssistant transfer to array above.

<div style="break-after: page; page-break-after: always;"></div>

#### Switch Page(half)
By setup the P2_CWORD_TEMPLATE, we can change page in GLCD.
We modify the code in `glcd.h` to define the setting of Right page and left page.
```c
#define P2_CWORD_TEMPLATE_RIGHT	0x21
#define P2_CWORD_TEMPLATE_LEFT	0x22
```
Then we do the switch page function using a global variable `mode` to define which page we are at and which page we want to print image.

| mode | Page  |
|:---:|:---:|
| 0    | right |
| 1    | left  |

Then we change all code which require to set page like `Set_DisplayOn()`, `Set_Xddr()`, `Set_Yaddr()` etc, and add the following code to make them change page according to `mode`.
```c
//right
if (mode == 0)
	P2_cword = P2_CWORD_TEMPLATE_RIGHT;
//left
else
	P2_cword = P2_CWORD_TEMPLATE_LEFT;
```

<div style="break-after: page; page-break-after: always;"></div>

#### Initialize at middle (basic and bonus 1)
```c
main ()
{
	system_init_config ();
	
	GLCD_Reset ();
	
	//draw right
	mode = 0;
	Set_DisplayOn (mode);
	draw(mode);	

	//draw left
	mode = 1;
	Set_DisplayOn (mode);
	draw(mode);	
	
	while (1);
}//end of function main
```
We claer the GLCD first and then draw the right page then left page, by controlling the `mode`. `draw()` will adjust the data by itself shown as below. 
```c
void draw(int mode) //mode 0 right, 1 left
{
	int i, x;
	Set_DisplayStartLine (0);
	Set_Yaddr (0);
	x = 0;
	Set_Xaddr (x);
	if (!mode) { //draw right
		for (i=16;i<32;i++)
			Send_Data (DVD[x][i]);
	}
	for (i = 16; i < 64; i++)
		Send_Data (0x00);
	if (mode) { //draw left
		for (i=0;i<16;i++)
			Send_Data (DVD[x][i]);
	}

	Set_Xaddr (++x);
	if (!mode) { //draw right
		for (i=16;i<32;i++)
			Send_Data (DVD[x][i]);
	}
	for (i = 16; i < 64; i++)
		Send_Data (0x00);
	if (mode) { //draw left
		for (i=0;i<16;i++)
			Send_Data (DVD[x][i]);
	}

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);
}
```
As we store the data in `DVD`, we use if-statement to make it draw the left part when `mode = 1`, i.e `DVD[x][0~15]`, and right part when `mode = 0`, i.e `DVD[x][16~31]`.

<div style="break-after: page; page-break-after: always;"></div>

### Bonus 2
We implement the bonus 2 part here. (`glcd.c`)
The bonus 2 modify 2 part, `main` for button control and `draw()` for drawing.

#### Main Function
```c
main ()
{
	system_init_config ();
	
	GLCD_Reset ();
	mode = 0;
	GLCD_Clean();
	mode = 1;
	GLCD_Clean();

	P1 = 0x00;
	z_cur = 0;
	y_cur = 48;
	while(1){
		if(P1 == 1) z_cur++;
		if(P1 == 2) z_cur--;
		if(P1 == 4 && y_cur < 96) y_cur++;
		if(P1 == 8 && y_cur >0) y_cur--;
		//draw right
		mode = 0;
		Set_DisplayOn (mode);
		draw(mode, z_cur, y_cur);	

		//draw left
		mode = 1;
		Set_DisplayOn (mode);
		draw(mode, z_cur, y_cur);
	}
}//end of function main		
```
We first Clean the GLCD, for our image to move, we use 4 button for up, down, right, left. To implement, we first set `z_cur` and `y_cur` to be the x and y coordinate on GLCD. So when button were pressed, our coordinate will change accordingly. Since y axis may change page, so we add the limit for y so it won't go out of border.

Buttons to their correspond command is list at table next page.

<div style="break-after: page; page-break-after: always;"></div>


| button | Command Sent to GLCD     |
|--------|--------------------------|
| P1.0   | up                       |
| P1.1   | down                     |
| P1.2   | right                    |
| P1.3   | left                     |


#### Draw Function
```c
void draw(int mode, int z_in, int y_in) //mode 0 right, 1 left
{
	int i, x;
	Set_DisplayStartLine (z_in);

	for (x = 0; x < 2; x++) {
		Set_Xaddr (x);

		if (mode && y_in < 64) { //draw left
			Set_Yaddr (y_in);
			for (i=0;i < ((y_in > 32) ? 64 - y_in : 32);i++)
				Send_Data (DVD[x][i]);
		}

		if (!mode && y_in > 32) { //draw right
			Set_Yaddr((y_in < 64)? 0: y_in-64);
			for (i = ((y_in > 63) ? 0 : 64 - y_in);i < 32;i++)
				Send_Data (DVD[x][i]);
		}
	}
}
```

The function will draw the DVD picture at the given x and y coordinate.
x-axis is implement with `Set_DisplayStartLine()` so the line will begin drawing at given line, creating up and down movement.
The we make it to draw according to y-axis. 
If `y < 33` the picture will only on the left page, so we only make it draw all of it.
Same as `y > 64` as it only show on page right.
When `y` is between, it require to change page. So we make it to draw from start to edge of left page, then draw from start of right page to the end.
Thus complete to drawing function.

Here is the result : [Bonus 2](https://drive.google.com/file/d/1Wp_rUQY8yNIvYPNadDtL0BmNVZsCsp-q/view?usp=sharing)

<div style="break-after: page; page-break-after: always;"></div>

## Difficulties Encountered and Solutions
First, DVD image cannot transfer to correct image, we've tried many method, online trannsfer won't work at all, always filled with 0xff. So we look for another tool to transfer and find the suitable size let DVD can display on GLCD normally.

Then, we stuck on finding how to change the left/right page. Since we need to display the image on the middle of the screen, and we find that when image display on left side is define at `P2_CWORD_TEMPLATE`

| `P2_CWORD_TEMPLATE` | value | `CS1`/`CS1`|
|---|---|---|
|`P2_CWORD_TEMPLATE_RIGHT`| `0x21` | 0/1 |
|`P2_CWORD_TEMPLATE_LEFT` | `0x22` | 1/0 |

Then we stuck on moving in the x dimension of GLCD, we thought we must calculate all data to do left shift and right shift for up and down, which will be a really hard task. Thankfully, When we study at the ppt we find that can use `Set_DisplayStartLine()` to implement it.

Finally, when doing move left/right page, we find that when we change the page and it will have one pixel didn’t clean after go right and return back, because it don’t clean well, so we let DVD last pixel be `0x00` to solve this problem.

## Disscussion
After this Lab, I've understand more about how to program the GLCD in C with 8051 board. I've also learn how to position the picture with GLCD board, and how pictures are displayed. Image must transfer into bitmap for the GLCD to display. Also we've complete the classic DVD loading screen on the board, which is a nice achieve to done.

[DVD loading screen](https://drive.google.com/file/d/1vYLmvUxdT4ocX9uWEIGUiN8jOg_c--Ky/view?usp=sharing)

<div style="break-after: page; page-break-after: always;"></div>

## Appendix
### Full code
#### `Lab06_bouns01.c`
```c
/*******************************************************************************
 *
 * file: [glcd.c]
 *
 ******************************************************************************/

#include "C8051F040.h"
#include "glcd.h"
int mode;
/*******************************************************************************
 *
 * functions for configuring the hardware
 *
 ******************************************************************************/

void
set_GLCD_WriteMode ()
{
	P4MDOUT = 0xff;
}//end of function set_GLCD_WriteMode


void
set_GLCD_ReadMode ()
{
	P4MDOUT = 0x00;
	P4 = 0xff;
}//end of function set_GLCD_ReadMode

void
system_init_config ()
{
	//turn-off the watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;		// Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;
	P0MDOUT = 0xff;
}//end of function system_init_config


/*******************************************************************************
 *
 * functions to drive hardware signals
 *
 ******************************************************************************/

void
GLCD_delay ()
{
	int i;
	for (i=0;i<10;i++);
}//end of function GLCD_delay

void
GLCD_Write (char P2_cword, char P4_cword)
{
	char P2_cword_rep;

	P2_cword_rep = P2_cword;
	set_GLCD_WriteMode ();
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep & (~P2_E);	//clear E bit
	P2 = P2_cword_rep;
	P4 = P4_cword;
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep | P2_E;		//set E bit
	P2 = P2_cword_rep;
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep & (~P2_E);	//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay ();
	P0 = P2_cword_rep; // nien debug
	
}//end of function GLCD_Write

char
GLCD_Read (char P2_cword)
{
	char status;
	char P2_cword_rep;

	P2_cword_rep = P2_cword;
	set_GLCD_ReadMode ();
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep & (~P2_E);		//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep | P2_E;			//set E bit  
	P2 = P2_cword_rep;
	GLCD_delay ();

	status = P4;

	P2_cword_rep = P2_cword_rep & (~P2_E);		//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay ();

	return status;
}//end of function GLCD_Read



/*******************************************************************************
 *
 * GLCD read operations
 *
 ******************************************************************************/

char
GLCD_ReadStatus ()
{
	char P2_cword;
	char status;

	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);
	P2_cword = P2_cword | (P2_RW);
	status = GLCD_Read (P2_cword);

	return status;
}//end of function GLCD_ReadStatus


char
GLCD_ReadData ()
{
	char P2_cword;
	char dat;

	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword | (P2_RS);
	P2_cword = P2_cword | (P2_RW);
	dat = GLCD_Read (P2_cword);

	return dat;
}//end of function GLCD_ReadData

int
GLCD_IsBusy ()
{
	char status;

	status = GLCD_ReadStatus ();
	if (status&P4_Busy)
		return 1;
	else
		return 0;
}//end of function GLCD_IsBusy


int
GLCD_IsReset ()
{
	char status;

	status = GLCD_ReadStatus ();
	if (status & P4_Reset)
		return 1;
	else
		return 0;
}//end of function GLCD_IsReset


int
GLCD_IsON ()
{
	return !GLCD_IsOFF ();
}//end of function GLCD_IsON


int
GLCD_IsOFF ()
{
	char status;

	status = GLCD_ReadStatus ();
	if (status & P4_Status_On)
		return 1;
	else
		return 0;
}//end of function GLCD_IsOFF


/*******************************************************************************
 *
 * functions to send commands and data to GLCD
 *
 ******************************************************************************/

void
Set_Xaddr (char x)
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//clear RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Xaddr_TMPL;
	P4_cword = P4_cword | (x & 0x07);

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_Xaddr


void
Set_Yaddr (char y)
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//clear RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Yaddr_TMPL;
	P4_cword = P4_cword | (y & 0x3f);

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_Yaddr


void
Set_DisplayStartLine (char z)
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//clear RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Zaddr_TMPL;
	P4_cword = P4_cword | (z & 0x3f);
	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_DisplayStartLine


void
Send_Data (char pattern)
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword | (P2_RS);		//set RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = pattern;

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Send_Data

void
Set_DisplayOn (int mode)
{
	char P2_cword, P4_cword;

	///prepare control words
	//right
	if (mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	//left
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//set RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Display_TMPL;
	P4_cword = P4_cword | P4_Display_On;	//set display ON bit

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_DisplayOn


void
Set_DisplayOff ()
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//set RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Display_TMPL;
	P4_cword = P4_cword & (~P4_Display_On);	//clear display ON bit

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_DisplayOff


void
GLCD_Reset ()
{
	char P2_cword, P4_cword;

	set_GLCD_WriteMode ();

	P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	P4_cword = 0;

	P2_cword = P2_cword | P2_RST;		//set reset bit
	GLCD_Write (P2_cword, P4_cword);

	P2_cword = P2_cword & (~P2_RST);	//clear reset bit
	GLCD_Write (P2_cword, P4_cword);

	P2_cword = P2_cword | P2_RST;		//set reset bit
	GLCD_Write (P2_cword, P4_cword);

	while (GLCD_IsReset());
}//end of function GLCD_Reset


/*******************************************************************************
 *
 * Drawing functions that you implement
 *
 ******************************************************************************/

const unsigned char DVD[2][32] = {
{0x00, 0xF0, 0xFB, 0x3B, 0x83, 0x83, 0x83, 0x83, 0xE7, 0x7F, 0x3F, 0x03, 0x07, 0x7F, 0xF8, 0xC0,
0xC0, 0x70, 0x1C, 0x0E, 0x07, 0xC3, 0xFB, 0x7B, 0x03, 0x03, 0x83, 0x83, 0xC6, 0x7E, 0x3C, 0x00 },
{0x00, 0x21, 0x31, 0x71, 0x71, 0x51, 0x50, 0x50, 0x58, 0x58, 0x48, 0x78, 0xC8, 0xC8, 0xCB, 0x89,
0x88, 0xC8, 0xC8, 0xC8, 0x78, 0x49, 0x59, 0x59, 0x59, 0x51, 0x51, 0x50, 0x70, 0x30, 0x20, 0x20 }
};


void draw(int mode) //mode 0 right, 1 left
{
	int i, x;
	Set_DisplayStartLine (0);
	Set_Yaddr (0);
	x = 0;
	Set_Xaddr (x);
	if (!mode) { //draw right
		for (i=16;i<32;i++)
			Send_Data (DVD[x][i]);
	}
	for (i = 16; i < 64; i++)
		Send_Data (0x00);
	if (mode) { //draw left
		for (i=0;i<16;i++)
			Send_Data (DVD[x][i]);
	}

	Set_Xaddr (++x);
	if (!mode) { //draw right
		for (i=16;i<32;i++)
			Send_Data (DVD[x][i]);
	}
	for (i = 16; i < 64; i++)
		Send_Data (0x00);
	if (mode) { //draw left
		for (i=0;i<16;i++)
			Send_Data (DVD[x][i]);
	}

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);

	Set_Xaddr (++x);
	for (i=0;i<64;i++)
		Send_Data (0x00);
}


/*******************************************************************************
 *
 * the main drawing function
 *
 ******************************************************************************/

main ()
{
	system_init_config ();
	
	GLCD_Reset ();
	
	//draw right
	mode = 0;
	Set_DisplayOn (mode);
	draw(mode);	

	//draw left
	mode = 1;
	Set_DisplayOn (mode);
	draw(mode);	
	
	while (1);
}//end of function main
```
<div style="break-after: page; page-break-after: always;"></div>

#### `glcd.c`
```c
#include "C8051F040.h"
#include "glcd.h"
int mode, z_cur, y_cur;
/*******************************************************************************
 *
 * functions for configuring the hardware
 *
 ******************************************************************************/

void
set_GLCD_WriteMode ()
{
	P4MDOUT = 0xff;
}//end of function set_GLCD_WriteMode


void
set_GLCD_ReadMode ()
{
	P4MDOUT = 0x00;
	P4 = 0xff;
}//end of function set_GLCD_ReadMode

void
system_init_config ()
{
	//turn-off the watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;		// Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;
	P0MDOUT = 0xff;
	//P1 input
	P1MDOUT = 0xff;
}//end of function system_init_config


/*******************************************************************************
 *
 * functions to drive hardware signals
 *
 ******************************************************************************/

void
GLCD_delay ()
{
	int i;
	for (i=0;i<10;i++);
}//end of function GLCD_delay

void
GLCD_Write (char P2_cword, char P4_cword)
{
	char P2_cword_rep;

	P2_cword_rep = P2_cword;
	set_GLCD_WriteMode ();
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep & (~P2_E);	//clear E bit
	P2 = P2_cword_rep;
	P4 = P4_cword;
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep | P2_E;		//set E bit
	P2 = P2_cword_rep;
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep & (~P2_E);	//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay ();
	P0 = P2_cword_rep; // nien debug
	
}//end of function GLCD_Write

char
GLCD_Read (char P2_cword)
{
	char status;
	char P2_cword_rep;

	P2_cword_rep = P2_cword;
	set_GLCD_ReadMode ();
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep & (~P2_E);		//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay ();

	P2_cword_rep = P2_cword_rep | P2_E;			//set E bit  
	P2 = P2_cword_rep;
	GLCD_delay ();

	status = P4;

	P2_cword_rep = P2_cword_rep & (~P2_E);		//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay ();

	return status;
}//end of function GLCD_Read



/*******************************************************************************
 *
 * GLCD read operations
 *
 ******************************************************************************/

char
GLCD_ReadStatus ()
{
	char P2_cword;
	char status;

	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);
	P2_cword = P2_cword | (P2_RW);
	status = GLCD_Read (P2_cword);

	return status;
}//end of function GLCD_ReadStatus


char
GLCD_ReadData ()
{
	char P2_cword;
	char dat;

	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword | (P2_RS);
	P2_cword = P2_cword | (P2_RW);
	dat = GLCD_Read (P2_cword);

	return dat;
}//end of function GLCD_ReadData

int
GLCD_IsBusy ()
{
	char status;

	status = GLCD_ReadStatus ();
	if (status&P4_Busy)
		return 1;
	else
		return 0;
}//end of function GLCD_IsBusy


int
GLCD_IsReset ()
{
	char status;

	status = GLCD_ReadStatus ();
	if (status & P4_Reset)
		return 1;
	else
		return 0;
}//end of function GLCD_IsReset


int
GLCD_IsON ()
{
	return !GLCD_IsOFF ();
}//end of function GLCD_IsON


int
GLCD_IsOFF ()
{
	char status;

	status = GLCD_ReadStatus ();
	if (status & P4_Status_On)
		return 1;
	else
		return 0;
}//end of function GLCD_IsOFF


/*******************************************************************************
 *
 * functions to send commands and data to GLCD
 *
 ******************************************************************************/

void
Set_Xaddr (char x)
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//clear RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Xaddr_TMPL;
	P4_cword = P4_cword | (x & 0x07);

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_Xaddr


void
Set_Yaddr (char y)
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//clear RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Yaddr_TMPL;
	P4_cword = P4_cword | (y & 0x3f);

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_Yaddr


void
Set_DisplayStartLine (char z)
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//clear RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Zaddr_TMPL;
	P4_cword = P4_cword | (z & 0x3f);
	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_DisplayStartLine


void
Send_Data (char pattern)
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword | (P2_RS);		//set RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = pattern;

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Send_Data

void
Set_DisplayOn (int mode)
{
	char P2_cword, P4_cword;

	///prepare control words
	//right
	if (mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	//left
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//set RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Display_TMPL;
	P4_cword = P4_cword | P4_Display_On;	//set display ON bit

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_DisplayOn


void
Set_DisplayOff ()
{
	char P2_cword, P4_cword;

	///prepare control words
	if(mode == 0)
		P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	else
		P2_cword = P2_CWORD_TEMPLATE_LEFT;
	P2_cword = P2_cword & (~P2_RS);		//set RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = P4_Set_Display_TMPL;
	P4_cword = P4_cword & (~P4_Display_On);	//clear display ON bit

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Set_DisplayOff


void
GLCD_Reset ()
{
	char P2_cword, P4_cword;

	set_GLCD_WriteMode ();

	P2_cword = P2_CWORD_TEMPLATE_RIGHT;
	P4_cword = 0;

	P2_cword = P2_cword | P2_RST;		//set reset bit
	GLCD_Write (P2_cword, P4_cword);

	P2_cword = P2_cword & (~P2_RST);	//clear reset bit
	GLCD_Write (P2_cword, P4_cword);

	P2_cword = P2_cword | P2_RST;		//set reset bit
	GLCD_Write (P2_cword, P4_cword);

	while (GLCD_IsReset());
}//end of function GLCD_Reset

void GLCD_Clean() {
	int i, x;
	Set_DisplayStartLine (0);
	Set_Yaddr (0);

	for (x=0; x<8; x++) {
		Set_Xaddr (x);
		for (i=0;i<64;i++)
			Send_Data (0x00);
	}
}
/*******************************************************************************
 *
 * Drawing functions that you implement
 *
 ******************************************************************************/

const unsigned char DVD[2][32] = {
{0x00, 0xF0, 0xFB, 0x3B, 0x83, 0x83, 0x83, 0x83, 0xE7, 0x7F, 0x3F, 0x03, 0x07, 0x7F, 0xF8, 0xC0,
0xC0, 0x70, 0x1C, 0x0E, 0x07, 0xC3, 0xFB, 0x7B, 0x03, 0x03, 0x83, 0x83, 0xC6, 0x7E, 0x3C, 0x00 },
{0x00, 0x21, 0x31, 0x71, 0x71, 0x51, 0x50, 0x50, 0x58, 0x58, 0x48, 0x78, 0xC8, 0xC8, 0xCB, 0x89,
0x88, 0xC8, 0xC8, 0xC8, 0x78, 0x49, 0x59, 0x59, 0x59, 0x51, 0x51, 0x50, 0x70, 0x30, 0x20, 0x00 }
};


void draw(int mode, int z_in, int y_in) //mode 0 right, 1 left
{
	int i, x;
	Set_DisplayStartLine (z_in);

	for (x = 0; x < 2; x++) {
		Set_Xaddr (x);

		if (mode && y_in < 64) { //draw left
			Set_Yaddr (y_in);
			for (i=0;i < ((y_in > 32) ? 64 - y_in : 32);i++)
				Send_Data (DVD[x][i]);
		}

		if (!mode && y_in > 32) { //draw right
			Set_Yaddr((y_in < 64)? 0: y_in-64);
			for (i = ((y_in > 63) ? 0 : 64 - y_in);i < 32;i++)
				Send_Data (DVD[x][i]);
		}
	}
}


/*******************************************************************************
 *
 * the main drawing function
 *
 ******************************************************************************/

main ()
{
	system_init_config ();
	
	GLCD_Reset ();
	mode = 0;
	GLCD_Clean();
	mode = 1;
	GLCD_Clean();

	P1 = 0x00;
	z_cur = 0;
	y_cur = 48;
	while(1){
		if(P1 == 1) z_cur++;
		if(P1 == 2) z_cur--;
		if(P1 == 4 && y_cur < 96) y_cur++;
		if(P1 == 8 && y_cur >0) y_cur--;
		//draw right
		mode = 0;
		Set_DisplayOn (mode);
		draw(mode, z_cur, y_cur);	

		//draw left
		mode = 1;
		Set_DisplayOn (mode);
		draw(mode, z_cur, y_cur);
	}
}//end of function main
```