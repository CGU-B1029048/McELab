#include "C8051F040.h"
#include "glcd.h"
#include "LCD.h"
#include <stdlib.h>
int mode, x_cur, y_cur;
int old_x, old_y;
int ghost_x, ghost_y;
int food_x, food_y;
int goldfood_x, goldfood_y;
int pac_status, speed;
/*******************************************************************************
 *
 * LCD
 *
 ******************************************************************************/
char LCD_status;
void LCD_PortConfig (){
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page
	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P3MDOUT = 0x3f;
	
	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function LCD_PortConfig ()

/****************
The delay_lcd seem to be not long enough. 
Try to change the default value as 10000...
****************/
unsigned int delay_lcd=20000;

void LCD_Delay (){
	int i;
	for (i=0;i<delay_lcd;i++); // wait for a long enough time...
}

void LCD_SendCommand (char cmd);

void LCD_Init (){
	LCD_SendCommand(0x02);        // Initialize as 4-bit mode
	LCD_SendCommand (0x28);		//Display function: 2 rows for 4-bit data, small 
	LCD_SendCommand (0x0e);		//display and curson ON, curson blink off
	LCD_SendCommand (0x01);		//clear display, cursor to home
	//LCD_SendCommand (0x10);		//cursor shift left
	//LCD_SendCommand (0x06);		//cursor increment, shift off
}

void LCD_Status_SetRS (){
	LCD_status = LCD_status | 1;
}

void
LCD_Status_ClearRS ()
{
	LCD_status = LCD_status & 0xfe;
}

void LCD_Status_SetWord (char word){
	word = word & 0x0f;
	LCD_status = LCD_status & 0x03;
	LCD_status = LCD_status | (word<<2);
}

void LCD_Status_SetEnable (){
	LCD_status = LCD_status | 0x02;
}


void LCD_Status_ClearEnable (){
	LCD_status = LCD_status & 0xfd;
}


void LCD_SendCommand (char cmd){
	LCD_Status_ClearRS ();

	///send the higher half
	LCD_Status_SetWord ((cmd>>4) & 0x0f);
	LCD_Status_SetEnable ();
	P3 = LCD_status;
	LCD_Delay ();
	LCD_Status_ClearEnable ();
	P3 = LCD_status;
	LCD_Delay ();

	///send the lower half
	LCD_Status_SetWord (cmd&0x0f);
	LCD_Status_SetEnable ();
	P3 = LCD_status;
	LCD_Delay ();
	LCD_Status_ClearEnable ();
	P3 = LCD_status;
	LCD_Delay ();
}

void LCD_SendData (char dat){
	LCD_Status_SetRS ();

	///send the higher half
	LCD_Status_SetWord ((dat>>4) & 0x0f);
	LCD_Status_SetEnable ();
	P3 = LCD_status;
	LCD_Delay ();
	LCD_Status_ClearEnable ();
	P3 = LCD_status;
	LCD_Delay ();

	///send the lower half
	LCD_Status_SetWord (dat&0x0f);
	LCD_Status_SetEnable ();
	P3 = LCD_status;
	LCD_Delay ();
	LCD_Status_ClearEnable ();
	P3 = LCD_status;
	LCD_Delay ();
}

void LCD_PrintString (char* str){
	int i;

	for (i=0; str[i]!=0; i++) {
		LCD_SendData (str[i]);
	}//for i
}

/*******************************************************************************
 *
 * GLCD
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
	P6MDOUT = 0xff;
	//P1 input
	P1MDIN = 0xff;
	//score
	P3MDOUT = 0xff;

}//end of function system_init_config


/*******************************************************************************
 *
 * functions to drive hardware signals
 *
 ******************************************************************************/

void GLCD_delay (int speed){
	int i;
	for (i=0;i<speed;i++);
}//end of function GLCD_delay

void GLCD_Write (char P2_cword, char P4_cword){
	char P2_cword_rep;

	P2_cword_rep = P2_cword;
	set_GLCD_WriteMode ();
	GLCD_delay (speed);

	P2_cword_rep = P2_cword_rep & (~P2_E);	//clear E bit
	P2 = P2_cword_rep;
	P4 = P4_cword;
	GLCD_delay (speed);

	P2_cword_rep = P2_cword_rep | P2_E;		//set E bit
	P2 = P2_cword_rep;
	GLCD_delay (speed);

	P2_cword_rep = P2_cword_rep & (~P2_E);	//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay (speed);
	P0 = P2_cword_rep; // nien debug
	
}//end of function GLCD_Write

char GLCD_Read (char P2_cword){
	char status;
	char P2_cword_rep;

	P2_cword_rep = P2_cword;
	set_GLCD_ReadMode ();
	GLCD_delay (speed);

	P2_cword_rep = P2_cword_rep & (~P2_E);		//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay (speed);

	P2_cword_rep = P2_cword_rep | P2_E;			//set E bit  
	P2 = P2_cword_rep;
	GLCD_delay (speed);

	status = P4;

	P2_cword_rep = P2_cword_rep & (~P2_E);		//clear E bit
	P2 = P2_cword_rep;
	GLCD_delay (speed);

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

void GLCD_Reset (){
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
//const unsigned char gold_food[4] = {0x18, 0x3C, 0x3C, 0x18};


void generatefood(){
	goldfood_x = rand() % 8;//0~7
	goldfood_y = rand() % 16;//0~15
}

void drawfood(){
	int food = 0x18;
	int gold_food = 0x3C;
	int x ,y; 
	x = goldfood_x;
	y = goldfood_y;
	// set side and y address
	mode = (y > 7) ? 0 : 1;

	// set drawing position
	Set_DisplayOn (mode); //right:left
	Set_Xaddr(x);
	Set_Yaddr (y*8+2);	
	
	// drawing food
	Send_Data(food);
	Send_Data(gold_food);
	Send_Data(gold_food);
	Send_Data(food);
}


void draw(int x_in, int y_in) { //mode 0 right, 1 left
	const unsigned char* pacman;
	int i;
	
	if(pac_status == 1 || pac_status == 2) pacman = pacman_right;
	if(pac_status == 3 || pac_status == 4) pacman = pacman_up;
	// begin erase
	mode = (old_y>7) ? 0: 1;
	Set_DisplayOn(mode);
	Set_Xaddr(old_x);
	Set_Yaddr(old_y*8);
	for(i = 0; i < 8; i++) Send_Data(0x00);

	if(y_in > 7) mode = 0;
	else mode = 1;
	Set_DisplayOn(mode);

	Set_Xaddr(x_in);
	Set_Yaddr(y_in * 8);

	for (i = 0; i < 8; i++) {
		if (pac_status == 2){
			Send_Data (pacman[7-i]);
		}else if (pac_status == 3){
			if(pacman[i] != 0)
				Send_Data (pacman[((i+3)%6)? (i+3)%6: 6]);
			else 
				Send_Data (pacman[i]);
		} else 
			Send_Data (pacman[i]);
	}

}

void draw_ghost() {
	// Set page and address
	mode = (ghost_y > 7) ? 0 : 1;
	Set_DisplayOn(mode);
	Set_Xaddr(ghost_x);
	Set_Yaddr(ghost_y*8+1);

	// draw ghost
	Send_Data(0x3c);
	Send_Data(0x76);
	Send_Data(0x3E);
	Send_Data(0x3E);
	Send_Data(0x76);
	Send_Data(0x3c);
}

void move_pacman(int pac_status, int ghost_status) {
	old_x = x_cur;
	old_y = y_cur;
	if (pac_status == 1 && y_cur < 15) y_cur++;//right
	else if (pac_status == 2 && y_cur > 0) y_cur--;//left
	else if (pac_status == 3 && x_cur > 0) x_cur--;//up
	else if (pac_status == 4 && x_cur < 7) x_cur++;//down

	if (ghost_status == 1 && ghost_y < 15) ghost_y++;//right
	else if (ghost_status == 2 && ghost_y > 0) ghost_y--;//left
	else if (ghost_status == 3 && ghost_x > 0) ghost_x--;//up
	else if (ghost_status == 4 && ghost_x < 7) ghost_x++;//down

	GLCD_delay(speed);
}

/*******************************************************************************
 *
 * the main drawing function
 *
 ******************************************************************************/

int main (){
	system_init_config ();
	
	GLCD_Reset ();
	mode = 0;
	GLCD_Clean();
	mode = 1;
	GLCD_Clean();

	int ghost_status;
	P1 = 0x00;
	P3 = 0x00;//score
	P6 = 0xff;
	x_cur = 1;
	y_cur = 6;
	ghost_x = 0;
	ghost_y = 0;
	pac_status = 1;
	ghost_status = 1;

	generatefood();
	drawfood();
	speed = 500;
	while(1){
		if (P1 == 1){//up
			pac_status = 3;
		} else if(P1 == 2){//down
			pac_status = 4;
		} else if(P1 == 4){
			pac_status = 1;//right
		} else if(P1 == 8){
			pac_status = 2;//left
		}
		ghost_status = rand() % 4 + 1;
		move_pacman(pac_status, ghost_status);
		draw(x_cur, y_cur);
		//more if-else loop can work /change mode to local variable?
		draw_ghost();

		// check if ghost eat food 
		if(ghost_x == goldfood_x && ghost_y == goldfood_y){
			generatefood();
			drawfood();
		}
		// check if pacman eat food
		if(x_cur == goldfood_x && y_cur == goldfood_y){
			P3++;
			generatefood();
			drawfood();
		}
		// check if ghost touch pacman, if so, die 
		if (ghost_x == x_cur && ghost_y == y_cur) {
			break;
		}
		GLCD_delay(speed);
	}
	// Game over
	P3 = 0xff;
	LCD_PortConfig ();
	LCD_Init();
	LCD_SendCommand(0x01);
	LCD_SendData('G');
	LCD_SendData('A');
	LCD_SendData('M');
	LCD_SendData('E');
	LCD_SendData(' ');
	LCD_SendData('O');
	LCD_SendData('V');
	LCD_SendData('E');
	LCD_SendData('R');
}//end of function main
