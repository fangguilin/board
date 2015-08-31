



#include "lpc17xx.h"                              /* LPC17xx definitions    */
/*
		LCD_RS					P2.10
		LCD_RW					P2.11
		LCD_EN					P2.12
		LCD_BLK					P2.13
		LCD_DATA				P2.0-P2.7
*/

#define LCD_Pin_RS   10
#define LCD_Pin_RW   11
#define LCD_Pin_EN   12
#define LCD_Pin_BLK  13 



void LCD_Init(void);  
void LCD_BLK(unsigned char Pdata);  

 

void LCM_Cursor(unsigned char flag,unsigned char row);
 
 
void LCM_String(  char *pc);




