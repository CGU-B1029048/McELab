#include "C8051F040.h"
#include "glcd.h"
#include <stdio.h>
#include <stdlib.h>
int mode, z_cur, y_cur;
int status = 1;//right
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

const unsigned char pacman[2][32] = 
{{0x00, 0x00, 0xE0, 0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xE6, 0x66, 0x3C, 0x18, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x07, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7E, 0x7C, 0x38, 0x30, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

const unsigned char pacman_down[2][32] = 
{{0x00, 0x00, 0xC0, 0xF0, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x0F, 0x1F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x03, 0x07, 0x0F, 0x1B, 0x33, 0x1F, 0x07,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};



void draw(int mode, int z_in, int y_in, int status) //mode 0 right, 1 left
{
	int i, x, j;
	Set_DisplayStartLine (z_in);
	if(status == 2) ;
	if(status == 3) ;
	if(status == 4){
		for(i = 0; i <= 2; i++){
			for(j = 0; j <= 32; j++)
				pacman[i][j] = pacman_down[i][j];
		}
	}
	for (x = 0; x < 2; x++) {
		Set_Xaddr (x);

		if (mode && y_in < 64) { //draw left
			Set_Yaddr (y_in);
			for (i=0;i < ((y_in > 32) ? 64 - y_in : 32);i++)
				Send_Data (pacman[x][i]);
		}

		if (!mode && y_in > 32) { //draw right
			Set_Yaddr((y_in < 64)? 0: y_in-64);
			for (i = ((y_in > 63) ? 0 : 64 - y_in);i < 32;i++)
				Send_Data (pacman[x][i]);
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
		if(P1 == 1){//up
			z_cur++;
			status = 3;
		} else if(P1 == 2){//down
			z_cur--;
			status = 4;
		} else if(P1 == 4 && y_cur < 96){
			y_cur++;
			status = 1;//right
		} else if(P1 == 8 && y_cur >0){
			y_cur--;
			status = 2;//left
		}
		//draw right
		mode = 0;
		Set_DisplayOn (mode);
		draw(mode, z_cur, y_cur,status);	

		//draw left
		mode = 1;
		Set_DisplayOn (mode);
		draw(mode, z_cur, y_cur,status);
	}
}//end of function main


