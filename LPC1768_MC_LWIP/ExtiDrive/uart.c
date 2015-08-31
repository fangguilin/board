/****************************************Copyright (c)****************************************************
**                                 http://www.OpenMCU.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           uart.c
** Last modified Date:  2010-05-12
`** Last Version:        V1.07
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Created by:          OpenMCU
** Created date:        2010-05-10
** Version:             V1.07
** Descriptions:        编写示例代码
**
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "lpc17xx.h"                              /* LPC17xx definitions    */
#include "uart.h"

#if 1
#define FOSC                        12000000                            /*  振荡器频率                  */
#define FCCLK                      (100000000)                          /*  主时钟频率<=100Mhz          */
                                                                        /*  FOSC的整数倍                */
#define FCCO                       (FCCLK * 4)                          /*  PLL频率(275Mhz~550Mhz)      */
                                                                        /*  与FCCLK相同，或是其的偶数倍 */
#define FPCLK                      (FCCLK / 4)                          /*  外设时钟频率,FCCLK的1/2、1/4*/
#else                                                                   /*  或与FCCLK相同               */
#define FOSC                        12000000                            /*  振荡器频率                  */

#define FCCLK                      (FOSC  * 8)                          /*  主时钟频率<=100Mhz          */
																														            /*  FOSC的整数倍                */
#define FCCO                       (FCCLK * 3)                          /*  PLL频率(275Mhz~550Mhz)      */
																														            /*  与FCCLK相同，或是其的偶数倍 */
#define FPCLK                      (FCCLK / 4)                          /*  外设时钟频率,FCCLK的1/2、1/4*/
																														            /*  或与FCCLK相同               */
#endif


#define UART0_BPS     115200                                          /* 串口0通信波特率             */
#define UART2_BPS     115200                                             /* 串口2通信波特率             */
/*********************************************************************************************************
** Function name:       UART0_Init
** Descriptions:        按默认值初始化串口0的引脚和通讯参数。设置为8位数据位，1位停止位，无奇偶校验
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UART0_Init (void)
{
	uint16_t usFdiv;
	/* UART0 */
	LPC_PINCON->PINSEL0 |= (1 << 4);             /* Pin P0.2 used as TXD0 (Com0) */
	LPC_PINCON->PINSEL0 |= (1 << 6);             /* Pin P0.3 used as RXD0 (Com0) */

	LPC_UART0->LCR  = 0x83;                      /* 允许设置波特率               */
	usFdiv = (FPCLK / 16) / UART0_BPS;           /* 设置波特率                   */
	LPC_UART0->DLM  = usFdiv / 256;
	LPC_UART0->DLL  = usFdiv % 256; 
	LPC_UART0->LCR  = 0x03;                      /* 锁定波特率                   */
	LPC_UART0->FCR  = 0x06; 				   
}

/*********************************************************************************************************
** Function name:       UART0_SendByte
** Descriptions:        从串口0发送数据
** input parameters:    data: 发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int UART0_SendByte (int ucData)
{
	while (!(LPC_UART0->LSR & 0x20));
    return (LPC_UART0->THR = ucData);
}

/*********************************************************************************************************
** Function name:       UART0_GetChar
** Descriptions:        从串口0接收数据
** input parameters:    无
** output parameters:   无
** Returned value:      接收到的数据
*********************************************************************************************************/
int UART0_GetChar (void) 
{
  	while (!(LPC_UART0->LSR & 0x01));
  	return (LPC_UART0->RBR);
}

/*********************************************************************************************************
** Function name:       UART0_SendString
** Descriptions:	    	向串口发送字符串
** input parameters:    s:   要发送的字符串指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UART0_SendString (unsigned char *s) 
{
  	while (*s != 0) 
	{
   		UART0_SendByte(*s++);
	}
}

/*********************************************************************************************************
** Function name:       UART2_Init
** Descriptions:        按默认值初始化串口2的引脚和通讯参数。设置为8位数据位，1位停止位，无奇偶校验
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UART2_Init (void)
{
	uint16_t usFdiv;
	/* UART2 */
	LPC_PINCON->PINSEL0 |= (1 << 20);             /* Pin P0.10 used as TXD2 (Com2) */
	LPC_PINCON->PINSEL0 |= (1 << 22);             /* Pin P0.11 used as RXD2 (Com2) */
  
	LPC_SC->PCONP = LPC_SC->PCONP|(1<<24);	      /*打开UART2电源控制位	           */
	
 
	
	
	LPC_UART2->LCR  = 0x83;                       /* 允许设置波特率                */
	usFdiv = (FPCLK / 16) / UART2_BPS;            /* 设置波特率                    */
	LPC_UART2->DLM  = usFdiv / 256;
	LPC_UART2->DLL  = usFdiv % 256; 
	LPC_UART2->LCR  = 0x03;                       /* 锁定波特率                    */
	LPC_UART2->FCR  = 0x06;
}

/*********************************************************************************************************
** Function name:       UART2_SendByte
** Descriptions:        从串口2发送数据
** input parameters:    data: 发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int UART2_SendByte (int ucData)
{
	while (!(LPC_UART2->LSR & 0x20));
    return (LPC_UART2->THR = ucData);
}

/*********************************************************************************************************
** Function name:       UART2_GetChar
** Descriptions:        从串口2接收数据
** input parameters:    无
** output parameters:   无
** Returned value:      接收到的数据
*********************************************************************************************************/
int UART2_GetChar (void) 
{
  	while (!(LPC_UART2->LSR & 0x01));
  	return (LPC_UART2->RBR);
}

/*********************************************************************************************************
** Function name:       UART2_SendString
** Descriptions:        UART2_SendString
** input parameters:    s:   要发送的字符串指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UART2_SendString (unsigned char *s) 
{
  while (*s != 0) 
	{
   		UART2_SendByte(*s++);
	}
}
