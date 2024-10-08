#include "C8051F040.h"
#include "glcd.h"
#include <stdlib.h>
int mode, x_cur, y_cur;
int old_x, old_y, old_wall_x, old_wall_y;
int wall_x, wall_y;
int goldfood_x, goldfood_y;
int pac_status, speed;

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

void draw_wall() {
	int i;
	//clear ghost
	mode = (old_wall_y>7) ? 0: 1;
	Set_DisplayOn(mode);
	Set_Xaddr(old_wall_x);
	Set_Yaddr(old_wall_y*8);
	for(i = 0; i < 8; i++) Send_Data(0x00);

	// Set page and address
	mode = (wall_y > 7) ? 0 : 1;
	Set_DisplayOn(mode);
	Set_Xaddr(wall_x);
	Set_Yaddr(wall_y*8);

	// draw ghost
	for (i = 0; i < 8; i++) {
		Send_Data(0xff);
	}
}

void move_pacman(int pac_status) {
	old_x = x_cur;
	old_y = y_cur;
	if (pac_status == 1 && y_cur < 15) y_cur++;//right
	else if (pac_status == 2 && y_cur > 0) y_cur--;//left
	else if (pac_status == 3 && x_cur > 0) x_cur--;//up
	else if (pac_status == 4 && x_cur < 7) x_cur++;//down

	// if (wall_status == 1 && wall_y < 15) wall_y++;//right
	// else if (wall_status == 2 && wall_y > 0) wall_y--;//left
	// else if (wall_status == 3 && wall_x > 0) wall_x--;//up
	// else if (wall_status == 4 && wall_x < 7) wall_x++;//down

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

	P1 = 0x00;
	P3 = 0x00;//score
	P6 = 0xff;
	x_cur = 1;
	y_cur = 6;
	wall_x = rand() % 8;
	wall_y = rand() % 16;
	old_wall_x = wall_x;
	old_wall_y = wall_y;
	pac_status = 1;

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
		} else if (P1 == 16) {
			old_wall_x = wall_x;
			old_wall_y = wall_y;
			wall_x = rand() % 8;
			wall_y = rand() % 16;
		}
		move_pacman(pac_status);
		
		// check if ghost touch pacman, if so, die 
		switch (pac_status) {
			case 1:
				if(wall_x == (x_cur) && wall_y == (y_cur)){
					y_cur--;
				}
			case 2:
				if(wall_x == (x_cur) && wall_y == (y_cur))
					y_cur++;
			case 3:
				if(wall_x == (x_cur) && wall_y == (y_cur))
					x_cur++;
			case 4:
				if(wall_x == (x_cur) && wall_y == (y_cur))
					x_cur--;
		}

		draw(x_cur, y_cur);
		//more if-else loop can work /change mode to local variable?
		draw_wall();

		// check if wall eat food 
		if(wall_x == goldfood_x && wall_y == goldfood_y){
			generatefood();
			drawfood();
		}
		// check if pacman eat food
		if(x_cur == goldfood_x && y_cur == goldfood_y){
			P3++;
			generatefood();
			drawfood();
		}
		GLCD_delay(speed);
	}
	while(1) P6 =~P6;
}//end of function main
