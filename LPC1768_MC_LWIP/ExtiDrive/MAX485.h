/*********************************************************************************************************/
#ifndef __MAX485_H_
#define __MAX485_H_





#define MAX485_DIR_S()  (LPC_GPIO2->FIOSET  = 0x00000100 )	 //MAX485_DIR P2.8 
#define MAX485_DIR_R()  (LPC_GPIO2->FIOCLR  = 0x00000100 )	 //MAX485_DIR P2.8 

void MAX485_Init(void);
void MAX485_SendByte (unsigned char SendData) ;

unsigned char MAX485_GetChar (void);

void MAX485_SendString(unsigned char *s) ;
 
#endif

