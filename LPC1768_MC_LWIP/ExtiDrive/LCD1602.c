



#include "LCD1602.h"


extern void TDelay( unsigned long Delay_Time );

#define   ROW     16      // 单行显示最大字符个数    

void LCDWrite_Command( unsigned char CommandData );
 
void LCD_EN(unsigned char Pdata); 
void LCD_RS(unsigned char Pdata);  
void LCD_RW(unsigned char Pdata);

	
	
	
	
	
	
	
	
	
	


void LCD_Init(void)
{
	   volatile  unsigned char i  ; 
	   LPC_GPIO2->FIODIRL |= 0x3CFF;   
	   LCD_EN(0);  
     TDelay( 10000 )	 ; 
     LCDWrite_Command(0x38);  //设定LCD为16*2显示，5*7点阵，8位数据接口
	   TDelay( 10000)	 ; 
     LCDWrite_Command(0x01);  //显示清屏。
	   TDelay( 10000)	 ; 
     LCDWrite_Command(0x06);  //显示光标自动右移，整屏不移动。
	   TDelay( 10000)	 ; 
     LCDWrite_Command(0x0C);  //开显示，不显示光标  
}


void LCD_BLK(unsigned char Pdata)
{
	if(Pdata)
	{
		 LPC_GPIO2->FIOSET=1<<LCD_Pin_BLK;
	}
	else 
  {
		 LPC_GPIO2->FIOCLR=1<<LCD_Pin_BLK;
	} 
	
}


void LCD_EN(unsigned char Pdata)
{
	if(Pdata)
	{
		 LPC_GPIO2->FIOSET=1<<LCD_Pin_EN;
	}
	else 
  {
		 LPC_GPIO2->FIOCLR=1<<LCD_Pin_EN;
	} 
	TDelay(500 )	 ;
}


void LCD_RS(unsigned char Pdata)
{
	if(Pdata)
	{
		 LPC_GPIO2->FIOSET=1<<LCD_Pin_RS;
	}
	else 
  {
		 LPC_GPIO2->FIOCLR=1<<LCD_Pin_RS;
	} 
	TDelay( 500)	 ;
}



void LCD_RW(unsigned char Pdata)
{
	if(Pdata)
	{
		 LPC_GPIO2->FIOSET=1<<LCD_Pin_RW;
	}
	else 
  {
		 LPC_GPIO2->FIOCLR=1<<LCD_Pin_RW;
	} 
	TDelay( 500)	 ;
}

void LCD_DATA(unsigned char Pdata)
{
 
	LPC_GPIO2->FIOPIN0=Pdata;
}


 unsigned char LCD_DATA_PORT (void)
 { 
	volatile  unsigned char Pdata;
	 Pdata=LPC_GPIO2->FIOPIN0;
	 return Pdata;
 }

//===========================
 //写一个命令字节,进行忙检测
 //===========================
 void LCDWrite_Command( unsigned char CommandData )
 {    
       LCD_EN(0);  
	     LCD_RW(0);  
       LCD_RS(0);  
       LCD_EN(1);  
       LCD_DATA(CommandData);
       LCD_EN(0);  
 }
 

 //===========================
 //写一个命令字节,进行忙检测
 //===========================
 void LCDWrite_Data(unsigned  char Buf_Code)
 {   
       LCD_EN(0);  
       LCD_RW(0);  
       LCD_RS(1);  
       LCD_EN(1);  
       LCD_DATA(  Buf_Code);
       LCD_EN(0); 
 }


void  LCM_Cursor(unsigned char flag,unsigned char row)   
{   
 
    if (row>=ROW)   
    {   
        row=0;   
    }  
	switch(flag)
	{
	   case 0:{row|=0x80;LCDWrite_Command(row);} break;
	   case 1:{row|=0xC0;LCDWrite_Command(row);} break; 
	} 
}   
 
   
/*==============================================  
函数原型：void   LCM_String(unsigned char code * pc)  
函数功能：显示字符串  
入口参数：unsigned char code * pc  
出口参数：无  
==============================================*/   
void LCM_String(  char *pc)   
{  
    while (*pc)   
    { 
        LCDWrite_Data(*pc);   
        pc++;   
	} 
}  
 

