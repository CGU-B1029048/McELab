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

	P2_cword = P2_CWORD_TEMPLATE;
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

	P2_cword = P2_CWORD_TEMPLATE;
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
	P2_cword = P2_CWORD_TEMPLATE;
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
	P2_cword = P2_CWORD_TEMPLATE;
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
	P2_cword = P2_CWORD_TEMPLATE;
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
	P2_cword = P2_CWORD_TEMPLATE;
	P2_cword = P2_cword | (P2_RS);		//set RS bit
	P2_cword = P2_cword & (~P2_RW);		//clear RW bit
	P4_cword = pattern;

	///flush out control signals
	while (GLCD_IsBusy());
	GLCD_Write (P2_cword, P4_cword);
}//end of function Send_Data

void
Set_DisplayOn (int DVD_side)
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
	P2_cword = P2_CWORD_TEMPLATE;
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

	P2_cword = P2_CWORD_TEMPLATE;
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
	if (mode) { //draw left
		for (i=16;i<32;i++)
			Send_Data (DVD[x][i]);
	}
	for (i = 16; i < 64; i++)
		Send_Data (0x00);
	if (!mode) { //draw right
		for (i=0;i<16;i++)
			Send_Data (DVD[x][i]);
	}

	Set_Xaddr (++x);
	if (mode) { //draw left
		for (i=16;i<32;i++)
			Send_Data (DVD[x][i]);
	}
	for (i = 16; i < 64; i++)
		Send_Data (0x00);
	if (!mode) { //draw right
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
