#include "C8051F040.h"
#include "glcd.h"
//#include "LCD.h"
#include <stdlib.h>
int mode, z_cur, y_cur;
int food_x, food_y;
int goldfood_x = 1, goldfood_y = 1;
int pac_status;//right
/*******************************************************************************
 *
 * functions for configuring the hardware
 *
 ******************************************************************************/

void set_GLCD_WriteMode (){
	P4MDOUT = 0xff;
}//end of function set_GLCD_WriteMode


void set_GLCD_ReadMode (){
	P4MDOUT = 0x00;
	P4 = 0xff;
}//end of function set_GLCD_ReadMode

void system_init_config (){
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

void GLCD_delay (){
	int i;
	for (i=0;i<200;i++);
}//end of function GLCD_delay

void GLCD_Write (char P2_cword, char P4_cword){
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

char GLCD_Read (char P2_cword){
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

char GLCD_ReadStatus (){
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


char GLCD_ReadData (){
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

int GLCD_IsBusy (){
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


int GLCD_IsON (){
	return !GLCD_IsOFF ();
}//end of function GLCD_IsON


int GLCD_IsOFF (){
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

void Set_Xaddr (char x){
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


void Set_Yaddr (char y){
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


void Set_DisplayStartLine (char z){
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

void Set_DisplayOn (int mode){
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
 * Drawing functions that you implement
 ******************************************************************************/

const unsigned char pacman_right[8] = {0x00,0x38, 0x7c, 0xfe, 0xee, 0xc6, 0x82, 0x00};
const unsigned char pacman_up[8] = {0x00, 0x78, 0x3c, 0x1e, 0x1e, 0x3c, 0x78, 0x00};
//const unsigned char food[2] = {0xC0, 0xC0};
const unsigned char gold_food[4] = {0x60, 0xFF, 0xFF, 0x60};


void generatefood(int foodmode){
  food_x = rand() % 64;
  if (food_x % 8 == 7 || food_x % 8 == 0) food_x - 1;
  food_y = rand() % (128 - foodmode*2);
}

void drawfood(){
	int i, x, y;
	int food = 0x03;
	x = food_x, y = food_y;
	
	// check page
	Set_Xaddr(x/8);

	// set side and y address
	Set_DisplayOn ((mode = (y > 64))? 1 : 0); 
	Set_Yaddr (y);	
	
	// determine which level on current x page
	for (i = 0; i <= x%8; i++) {
		food *= 2;
	}
	Send_Data(food);

	// change side
	if (food_y == 63) {
		Set_DisplayOn(1);
		y = 0;
		Set_Yaddr(y);
	}

	Send_Data(food);
}


void draw(int mode, int z_in, int y_in) {
	//mode 0 right, 1 left
	unsigned char *pacman;
	int i;
	Set_DisplayStartLine (z_in);
	if(pac_status == 1 || pac_status == 2) pacman = pacman_right;
	if(pac_status == 3 || pac_status == 4) pacman = pacman_up;
		Set_Xaddr (0);

		if (mode && y_in < 64) { //draw left
			Set_Yaddr (y_in);
			for (i=0;i < ((y_in > 56) ? 64 - y_in : 8);i++)
				if (pac_status == 2)
					Send_Data (pacman[7-i]);
				else if (pac_status == 4){
					if(pacman[i] != 0)
					 	Send_Data (pacman[((i+3)%6)? (i+3)%6: 6]);
					else 
						Send_Data (pacman[i]);
				} else 
					Send_Data (pacman[i]);
		}

		if (!mode && y_in > 48) { //draw right
			Set_Yaddr((y_in < 64)? 0: y_in-64);
			for (i = ((y_in > 63) ? 0 : 64 - y_in);i < 8;i++)
				if (pac_status == 2)
					Send_Data (pacman[7-i]);
				else if (pac_status == 4){
					if(pacman[i] != 0)
						Send_Data (pacman[((i+3)%6)? (i+3)%6: 6]);
					else 
						Send_Data (pacman[i]);
				} else 
					Send_Data (pacman[i]);
		}
}

void move_pacman(int pac_status) {
	if(pac_status == 1 && y_cur < 120) y_cur++;
	else if (pac_status == 2 && y_cur > 0) y_cur--;
	else if (pac_status == 3 && z_cur > 8) z_cur--;
	else if (pac_status == 4 && z_cur < 63) z_cur++;
	GLCD_delay();
}

/*******************************************************************************
 *
 * the main drawing function
 *
 ******************************************************************************/

int main ()
{
	system_init_config ();
	
	GLCD_Reset ();
	mode = 0;
	GLCD_Clean();
	mode = 1;
	GLCD_Clean();

	P1 = 0x00;
	z_cur = 32;
	y_cur = 48;
	pac_status = 4;
	
	while(1){
		if(P1 == 1){//up
			//z_cur++;
			pac_status = 4;
		} else if(P1 == 2){//down
			//z_cur--;
			pac_status = 3;
		} else if(P1 == 4){
			//y_cur++;
			pac_status = 1;//right
		} else if(P1 == 8){
			//y_cur--;
			pac_status = 2;//left
		} else if(P1 == 16){
			generatefood(1);
			drawfood();
		}
		move_pacman(pac_status);
		//draw right
		mode = 0;
		Set_DisplayOn (mode);
		draw(mode, z_cur, y_cur);	

		//draw left
		mode = 1;
		Set_DisplayOn (mode);
		draw(mode, z_cur, y_cur);

		//GLCD_delay();

	}
}//end of function main


