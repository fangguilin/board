



#include "LCD1602.h"


extern void TDelay( unsigned long Delay_Time );

#define   ROW     16      // ������ʾ����ַ�����    

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
     LCDWrite_Command(0x38);  //�趨LCDΪ16*2��ʾ��5*7����8λ���ݽӿ�
	   TDelay( 10000)	 ; 
     LCDWrite_Command(0x01);  //��ʾ������
	   TDelay( 10000)	 ; 
     LCDWrite_Command(0x06);  //��ʾ����Զ����ƣ��������ƶ���
	   TDelay( 10000)	 ; 
     LCDWrite_Command(0x0C);  //����ʾ������ʾ���  
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
 //дһ�������ֽ�,����æ���
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
 //дһ�������ֽ�,����æ���
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
����ԭ�ͣ�void   LCM_String(unsigned char code * pc)  
�������ܣ���ʾ�ַ���  
��ڲ�����unsigned char code * pc  
���ڲ�������  
==============================================*/   
void LCM_String(  char *pc)   
{  
    while (*pc)   
    { 
        LCDWrite_Data(*pc);   
        pc++;   
	} 
}  
 

