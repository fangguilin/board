


#include "lpc17xx.h"                            
#include "uart.h"   
#include "MAX485.h"       




void MAX485_Init(void)
{    
	LPC_GPIO2->FIODIR  |= 0x00000100;          //MAX485_DIR P2.8   …Ë÷√Œ™ ‰≥ˆ
	MAX485_DIR_R();
	UART2_Init ( );
} 
 

void MAX485_SendByte (unsigned char SendData) 
{ 
	  UART2_SendByte (SendData); 
} 

 unsigned char MAX485_GetChar ()  
 {
    return UART2_GetChar ();
 }
 
void MAX485_SendString(unsigned char *s)  
{	
		MAX485_DIR_S();  
		TDelay(10000);
		UART2_SendString( s ) ; 
		TDelay(10000);
		MAX485_DIR_R();
}





