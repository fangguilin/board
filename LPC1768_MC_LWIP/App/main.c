/*******************************************************************************
  * Company: Wang Electronic Technology Co., Ltd.
  ******************************************************************************
  * 文件名称：main.c
  * 功能说明：LWIP裸机实验WEB服务器
  * 版    本：V1.1
	* 作    者：jeansonm
  * 日    期：2014-6-9
********************************************************************************
  * 文件名称：
  * 功能说明：
  * 版    本：
	* 更新作者:	
  * 日    期：
	* 更新原因：
********************************************************************************/
#include "lpc17xx.h"
#include "emac.h"
#include "netconf.h"
#include "uart.h"
#include "LCD1602.h"
#include "board.h"
#include "MAX485.h"
#include "stdio.h"
#include "stdlib.h"

extern void tcp_echoserver_init(void);

extern void udp_echo_init(void);

extern int flag;

/*
void send();
unsigned char const UDPArr[20] = {"01234567890!"};
struct udp_pcb *Udppcb1;
struct ip_addr ipaddr1;
struct pbuf *p;
*/

/*****************************************************************************
**   Main Function  main()
******************************************************************************/


void TDelay( unsigned long Delay_Time )
{
	volatile unsigned long Delay_Times=Delay_Time;
    while(Delay_Times--);
}

void LED_config(void)
{ 
	LPC_GPIO0->FIODIR  = 0x00800000;  //P0.23  LED
	LPC_GPIO0->FIOCLR |= 0x00800000; 
}

int main(void)
{
		LED_config ();

		UART0_Init();
	
		MAX485_Init();
	
		LwIP_Init(); 
	
		udp_echo_init();


//LCD 部分
		LCD_Init( );   
		LCD_BLK(1);     
		LCM_Cursor(0,0);           //0行 0列
		LCM_String("1111");       
		LCM_Cursor(1,0);
		LCM_String("192.168.17.148");       //1行 0列
		
//MAX485 部分	  
		MAX485_DIR_S();  
		MAX485_SendString("LPC1768\r\n"); 
		MAX485_SendString("192.168.17.148\r\n");  
		MAX485_DIR_R();  
		TDelay(10000);
/*		
void udp_send()
{
p = pbuf_alloc(PBUF_TRANSPORT , sizeof(UDPArr) , PBUF_RAM );
p->payload =(void *)(UDPArr);
IP4_ADDR(&ipaddr1 , 192,1,1,136);   
Udppcb1 = udp_new( );
udp_bind( Udppcb1 , IP_ADDR_ANY , 9999 );
udp_connect( Udppcb1 , &ipaddr1 , 9999 );
}
*/


		while (1) 
		{ 
			MAX485_DIR_S();  
			MAX485_SendString("LPC1768\r\n"); 
			MAX485_SendString("192.168.17.148\r\n");  
			MAX485_DIR_R();  
			UART0_SendString ("192.168.17.148\r\n");
			TDelay(50000000);
			send(NULL,NULL,NULL);
		} 
}
/******************************************************************************
**                            End Of File
******************************************************************************/





