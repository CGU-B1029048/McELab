#include "C8051F040.h"
#include "LCD.h"

char LCD_status;
char array[2][16];
int cur_x, cur_y;
int a = 0;
char k;

void button_detect (){
	char key_hold;
	int key_release;
	int count;
	int N = 50;

	do {
		key_hold = P2;
	} while (!key_hold);
	
	//Stage 2: wait for key released
	key_release = 0;
	count = N;
	while (!key_release) {
		key_hold = P2;
		if (key_hold) {
			count = N;
		} else {
			count--;
			if (count==0) key_release = 1;
		}
	}//Stage 2: wait for key released
}//end of function button_detect ()

void LCD_PortConfig (){
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page
	P2MDIN = 0xff;	
	P2MDOUT = 0x00;
	P1MDOUT = 0xff;

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

void
LCD_SendCommand (char cmd);

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


void
LCD_ClearScreen ()
{
	LCD_SendCommand (0x01);
}


void Shutup_WatchDog (){
	WDTCN = 0xde;
	WDTCN = 0xad;
}//end of function Shutup_WatchDog

void jump_cursor(int x, int y) {
	if (y) {LCD_SendCommand(0x00C0);}
	else {LCD_SendCommand(0x0002);}
	for (a = 0; a < x; a++) {
		LCD_SendCommand(0x0014);
	}
}

void insert_data(char c) {
	LCD_SendData(c);
	for (a = cur_x; a < 15; a++) {
		k = array[cur_y][a+1];
		array[cur_y][a+1] = array[cur_y][cur_x];
		LCD_SendData(array[cur_y][a+1]);
		array[cur_y][cur_x] = k;
	}
	array[cur_y][cur_x] = c;
}

void main (){
	P1 = 0x00;
	Shutup_WatchDog ();
	LCD_PortConfig ();
	LCD_Init ();
	LCD_ClearScreen ();
	P1 = 0xff;
	cur_x = 0;
	cur_y = 0;
	for (a = 0; a < 16; a++){
		array[0][a] = ' ';
		array[1][a] = ' ';
	}
	while (1){
		P2 = 0x00;
		// Insert A
		if(P2 == 128) {
			insert_data('A');
			if (cur_x < 15) {
				cur_x++;
			}
			jump_cursor(cur_x, cur_y);
		// Insert B
		} else if(P2 == 64){
			insert_data('B');
			if (cur_x < 15) {
				cur_x++;
			}
			jump_cursor(cur_x, cur_y);
		// Insert C
		} else if(P2 == 32){
			insert_data('C');
			if (cur_x < 15) {
				cur_x++;
			}
			jump_cursor(cur_x, cur_y);

		// cursor left
		} else if (P2 == 8) {
			if (cur_x > 0) {
				LCD_SendCommand(0x0010);
				cur_x--;
			}
		// cursor right
		} else if (P2 == 4) {
			if (cur_x < 15) {
				LCD_SendCommand(0x0014);
				cur_x++;
			}
		// cursor up
		} else if (P2 == 2) {
			if (cur_y == 1) {
				cur_y = 0;
				jump_cursor(cur_x, cur_y);
			}
		// cursor down
		} else if (P2 == 1) {
			if (cur_y == 0) {
				cur_y = 1;
				jump_cursor(cur_x, cur_y);
			}
		}
	}
}
