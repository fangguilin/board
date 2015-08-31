/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of and a contribution to the lwIP TCP/IP stack.
 *
 * Credits go to Adam Dunkels (and the current maintainers) of this software.
 *
 * Christiaan Simons rewrote this file to get a more stable echo example.
 *
 **/
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "webpage.h"
#include "LPC17xx.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"


#if LWIP_TCP

volatile int flag = 0 ;

static struct tcp_pcb *tcp_echoserver_pcb;
extern unsigned char WebSide[] ;



/* ECHO protocol states */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* �ص������������� */
struct tcp_echoserver_struct
{
  u8_t state;             /* current connection state */
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};

struct udp_pcb * pcb;

static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);

/*********************************************************************************************************
** Function name:       tcp_echoserver_init
** Descriptions:        TCP��������ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void tcp_echoserver_init(void)
{
  tcp_echoserver_pcb = tcp_new();	//����TCP PCB

  if (tcp_echoserver_pcb != NULL)
  {
    err_t err;
    
    err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, 8080);	//���ӵ����ض˿�
    
    if (err == ERR_OK)
    {
      tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);		//�����˿�
      tcp_accept(tcp_echoserver_pcb, tcp_echoserver_accept);	//tcp_accept�ص�����
    }
  }
}

/*********************************************************************************************************
** Function name:       tcp_echoserver_accept
** Descriptions:        tcp_accept�ص�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* allocate structure es to maintain tcp connection informations */
  es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->p = NULL;
    
    tcp_arg(newpcb, es);		//����es��Ϊ�ص������β�
    
    tcp_recv(newpcb, tcp_echoserver_recv);//tcp_recv�ص�����
    tcp_err(newpcb, tcp_echoserver_error);//tcp_err�ص�����   
    tcp_poll(newpcb, tcp_echoserver_poll, 1);	//tcp_poll�ص�����
    
    ret_err = ERR_OK;
  }
  else
  {
    ret_err = ERR_MEM;
  }
  return ret_err;  
}

/*********************************************************************************************************
** Function name:       GetAD7Val
** Descriptions:        ��ȡADת��ֵ
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
unsigned int GetAD7Val(void)		
{
  unsigned int val;

  LPC_ADC->ADCR |=  (1<<24);                     /* start conversion */
  while (!(LPC_ADC->ADGDR & (1UL<<31)));         /* Wait for Conversion end */
  val = ((LPC_ADC->ADGDR >> 4) & 0xFFF);         /* read converted value */
  LPC_ADC->ADCR &= ~(7<<24);                     /* stop conversion */

  return(val);                                   /* result of A/D process */ 
}
/*********************************************************************************************************
** Function name:       tcp_echoserver_recv
** Descriptions:        tcp_recv�ص�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct tcp_echoserver_struct *es;
	char *data;  

  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct tcp_echoserver_struct *)arg;
  
  if (p == NULL)			//���յ�һ����tcp֡
  {
    es->state = ES_CLOSING;
    if(es->p == NULL)		//��ɷ��͹ر�����
    {
       /* we're done sending, close connection */
       tcp_echoserver_connection_close(tpcb, es);
    }
    else							//û����ɷ���  ����Ӧ���
    {
      tcp_sent(tpcb, tcp_echoserver_sent);
      tcp_echoserver_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  else if(err != ERR_OK)		//���մ���
  {
    if (p != NULL)			//�ͷ�pbuf
    {
      es->p = NULL;
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)	//��һ�ν��յ�����
  {
    es->state = ES_RECEIVED;
		data = p->payload;
    es->p = p;
		if (strncmp(data, "GET /", 5) == 0)
		{  
			p->payload = WebSide;
			p->len = sizeof(WebSide);
		}
		
    tcp_sent(tpcb, tcp_echoserver_sent);	//��ʼ��tcp_sent�ص�����
    tcp_echoserver_send(tpcb, es);		//�ش�����
  
    ret_err = ERR_OK;
      tcp_echoserver_connection_close(tpcb, es);
  }
  else if (es->state == ES_RECEIVED)	//���յ�����
  {
    if(es->p == NULL)
    {
      es->p = p;
      tcp_echoserver_send(tpcb, es);	//�ش�����
    }
    else		//��ӵ�pbuf������
    {
      struct pbuf *ptr;

      ptr = es->p;
      pbuf_chain(ptr,p);
    }
    ret_err = ERR_OK;
  }
  
  else		//���ӶϿ�
  {
    tcp_recved(tpcb, p->tot_len);
    es->p = NULL;		//�ͷ�pbuf
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/*********************************************************************************************************
** Function name:       tcp_echoserver_error
** Descriptions:        tcp_err�ص�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static void tcp_echoserver_error(void *arg, err_t err)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}

/*********************************************************************************************************
** Function name:       tcp_echoserver_poll
** Descriptions:        tcp_poll�ص�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection */
        tcp_echoserver_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/*********************************************************************************************************
** Function name:       tcp_echoserver_sent
** Descriptions:        tcp_sent�ص�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_echoserver_struct *)arg;
  
  if(es->p != NULL)
  {
    tcp_echoserver_send(tpcb, es);
  }
  else
  {
    if(es->state == ES_CLOSING)
      tcp_echoserver_connection_close(tpcb, es);
  }
  return ERR_OK;
}

/*********************************************************************************************************
** Function name:       tcp_echoserver_send
** Descriptions:        TCP����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) && (es->p != NULL) && (es->p->len <= tcp_sndbuf(tpcb)))
  {
    ptr = es->p;			//��ȡpbuf�ṹ��ָ��
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);		//TCP��������    
    if (wr_err == ERR_OK)		//�������
    {
      u16_t plen;
      plen = ptr->len;
      es->p = ptr->next;	//ָ����һ��pbufָ��
      
      if(es->p != NULL)		//�����Ͳ������һ��pbuf�ṹ��
      {
        pbuf_ref(es->p);
      }      
      pbuf_free(ptr);			//�ͷ�pbufָ��
      tcp_recved(tpcb, plen);	//����TCP���ڴ�С
   }
   else if(wr_err == ERR_MEM)	//�ڴ治��
   {
     es->p = ptr;
   }
   else
   {
   }
  }
}

/*********************************************************************************************************
** Function name:       tcp_echoserver_connection_close
** Descriptions:        �ر�TCP����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  tcp_arg(tpcb, NULL);  	// ��ջص����� 
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  if (es != NULL)					//ɾ���ṹ��
    mem_free(es);
  
	tcp_close(tpcb); 				//�ر�TCP���� 

}

unsigned char const UDPArr[6]={"hello!"};
void zzsend()
{
	struct udp_pcb *Udppcb;
	struct ip_addr ipaddr;
	struct pbuf *p;

	p=pbuf_alloc( PBUF_RAW, sizeof(UDPArr), PBUF_RAM);
	p->payload=(void*)(UDPArr);
	IP4_ADDR( &ipaddr, 192,1,1,115);
	Udppcb = udp_new();
	udp_bind( Udppcb, IP_ADDR_ANY, 8888);
	udp_connect( Udppcb, &ipaddr, 8888);
	udp_send( Udppcb ,p);
	udp_disconnect(Udppcb);
	udp_remove(Udppcb);
//	TDelay(50000000);
	pbuf_free(p);
}

void recvsendto(struct udp_pcb *pcb, struct pbuf *pbuf, struct ip_addr *addr)
{
	struct ip_addr destAddr = *addr;
 	udp_sendto(pcb,pbuf,&destAddr,8888); // ���õ�ǰ��pcb����udp_send����udp������ɺ󣬻ظ�ԭ��������
	pbuf_free(pbuf); 	//�ͷ�udp�� 
}





void send(struct udp_pcb *pcb, struct pbuf *pbuf, struct ip_addr *addr)
{
	flag++;
	if (pcb == NULL && pbuf == NULL && addr == NULL && flag>1 )
		{
				zzsend();
		}
	else if(pcb != NULL && pbuf != NULL && addr != NULL )
		{	
			recvsendto(pcb,pbuf,addr);
		}
		else 
		{
			LWIP_DEBUGF(UDP_DEBUG, ("send failed!\n"));
		}
}



void udp_echo_recv(void *arg, struct udp_pcb *pcb, 
				struct pbuf *pbuf, struct ip_addr *addr, u16_t port)
{
//    struct ip_addr destAddr = *addr; // ��ȡԶ������IP��ַ
	if( pbuf != NULL )
	{
//		struct ip_addr destAddr = *addr;
		send(pcb, pbuf, addr);
		flag=0;
/*		udp_sendto(pcb,pbuf,&destAddr,port); // ���õ�ǰ��pcb����udp_send����udp������ɺ󣬻ظ�ԭ��������
		pbuf_free(pbuf); 	//�ͷ�udp�� 
	*/	
	}
}


/*
struct ip_addr destAddr;
void send()
{
	struct udp_pcb *udppcb;
    struct ip_addr ipaddr;
    struct pbuf buf;
	char data[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
	buf.payload=data;
	buf.len=buf.tot_len=10;
	destAddr.addr = 0xc0010188; // ��ȡԶ������IP��ַ 
	udp_sendto(pcb,&buf,&destAddr,8888);
}
*/


void udp_echo_init(void)
{
	struct ip_addr local_addr;
	local_addr.addr = 0xC00101A8;//ip��ַ
	
	// get new pcb
    pcb = udp_new();//�����µ�udp��
    if (pcb == NULL)
	{
        LWIP_DEBUGF(UDP_DEBUG, ("udp_new failed!\n"));//����lwip���������
        return;
    }
    //bind to any IP address on port 7
   if (udp_bind(pcb, IP_ADDR_ANY, 8888)!= ERR_OK)
	{
        LWIP_DEBUGF(UDP_DEBUG, ("udp_bind failed!\n"));
        return;
    }
	//udp_connect(pcb, &local_addr, 9999);  //��¼��ָ����IP��ַ�Ͷ˿ںţ���Զ����������
	//set udp_echo_recv() as callback function
	//for received packets
	udp_recv(pcb, udp_echo_recv, NULL);//���ûص�����
}

/*

unsigned char const UDPArr[6]={"hello!"};

void send()
{
	struct udp_pcb *Udppcb1;
	struct ip_addr ipaddr;
	struct pbuf *p;
	
	struct udp_pcb *pcb;
	struct pbuf *pbuf;
	struct ip_addr *addr;
	struct ip_addr destAddr = *addr;
	u16_t port;
	
	if(flag==1)
	{
	p=pbuf_alloc( PBUF_RAW, sizeof(UDPArr), PBUF_RAM);
	p->payload=(void*)(UDPArr);
	IP4_ADDR( &ipaddr, 192,1,1,115);
	Udppcb1 = udp_new();
	udp_bind( Udppcb1, IP_ADDR_ANY, 8888);
	udp_connect( Udppcb1, &ipaddr, 8888);
	udp_send( Udppcb1 ,p );
	TDelay(50000000);
	pbuf_free(p);
	flag=0;
	}
	if(flag==0)
	{
		udp_sendto(pcb,pbuf,&destAddr,8888); // ���õ�ǰ��pcb����udp_send����udp������ɺ󣬻ظ�ԭ��������
		pbuf_free(pbuf); 	//�ͷ�udp�� 
	}

}

*/


/* LWIP_TCP */
#endif 
