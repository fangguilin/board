/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
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
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"
#include "err.h"
#include "ethernetif.h"

#include "lpc17xx.h"
#include "emac.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/
#define  EMAC_MAX_PACKET_SIZE  1536  /* 1536 bytes */
/* Global Rx Buffer data */
uint8_t __attribute__ ((aligned (4))) gRxBuf[EMAC_MAX_PACKET_SIZE];
uint8_t __attribute__ ((aligned (4))) gTxBuf[EMAC_MAX_PACKET_SIZE];

/* Define those to better describe your network interface. */
#define IFNAME0 'W'
#define IFNAME1 'B'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int unused;
};

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
	EMAC_CFG_Type Emac_Config;	
	uint8_t EMACAddr[] = {ETHADDR0, ETHADDR1, ETHADDR2, ETHADDR3, ETHADDR4, ETHADDR5};		//MAC地址
	uint32_t i;

  netif->hwaddr_len = ETHARP_HWADDR_LEN;	//设置MAC地址长度

  /* 设置MAC地址 */
  netif->hwaddr[0] =  EMACAddr[0];
  netif->hwaddr[1] =  EMACAddr[1];
  netif->hwaddr[2] =  EMACAddr[2];
  netif->hwaddr[3] =  EMACAddr[3];
  netif->hwaddr[4] =  EMACAddr[4];
  netif->hwaddr[5] =  EMACAddr[5];

  netif->mtu = EMAC_MAX_PACKET_SIZE;		//设置包最大长度
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;  // 设置网络标志 */

  /* 硬件配置 */	
	LPC_PINCON->PINSEL2 = 0x50150105;			//端口配置
	LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~0x0000000F) | 0x00000005;
	
	Emac_Config.Mode = EMAC_MODE_100M_FULL;
	Emac_Config.pbEMAC_Addr = EMACAddr;
	
	while (EMAC_Init(&Emac_Config) == 0)	//初始化EMAC模块
	{
		for (i = 0x100000; i; i--);
	}
	/* 中断使能 */
	EMAC_IntCmd((EMAC_INT_RX_OVERRUN | EMAC_INT_TX_DONE | EMAC_INT_RX_DONE ) , ENABLE);
	/* 设置中断优先级 */
	NVIC_SetPriority(ENET_IRQn, ((0x01<<3)|0x01));
	NVIC_EnableIRQ(ENET_IRQn);
}


int ethernet_send(void *pPacket, int size)
{
	EMAC_PACKETBUF_Type TxPack;

	/* Check size */
	if(size == 0)
	{
		return 0;
	}

	/* check Tx Slot is available */
	if (EMAC_CheckTransmitIndex() == FALSE)
	{
		return -1;
	}

	size = MIN(size,EMAC_MAX_PACKET_SIZE);

	/* Setup Tx Packet buffer */
	TxPack.ulDataLen = size;
	TxPack.pbDataBuf = (uint32_t *)pPacket;
	EMAC_WritePacketBuffer(&TxPack);
	EMAC_UpdateTxProduceIndex();

	return 0;
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int len = 0;

  SYS_ARCH_DECL_PROTECT(sr);
  /* Interrupts are disabled through this whole thing to support multi-threading
	   transmit calls. Also this function might be called from an ISR. */
  SYS_ARCH_PROTECT(sr);

  for(q = p; q != NULL; q = q->next) 
  {
    memcpy((u8_t*)&gTxBuf[len], q->payload, q->len);
		len = len + q->len;
  }
  ethernet_send(gTxBuf, len);

  SYS_ARCH_UNPROTECT(sr);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
	EMAC_PACKETBUF_Type RxDatbuf;
	struct pbuf *p, *q;
	uint16_t len;

	/* Packet received, check if packet is valid. */
	if (EMAC_CheckReceiveIndex())
	{
		if (!EMAC_CheckReceiveDataStatus(EMAC_RINFO_LAST_FLAG))
		{
			/* Release frame from EMAC buffer */
		    EMAC_UpdateRxConsumeIndex();
			return NULL;
		}
		/* Get data size, trip out 4-bytes CRC field, note that length in (-1) style format */
		len = EMAC_GetReceiveDataSize() - 3;
		/* Note that packet added 4-bytes CRC created by yourself */
		if ((len > EMAC_MAX_PACKET_SIZE) || (EMAC_CheckReceiveDataStatus(EMAC_RINFO_ERR_MASK))) 
		{
			/* Release frame from EMAC buffer */
		    EMAC_UpdateRxConsumeIndex();
			return NULL;
		}
		/* Valid Frame, just copy it */
		RxDatbuf.pbDataBuf = (uint32_t *)gRxBuf;
		RxDatbuf.ulDataLen = len;
		EMAC_ReadPacketBuffer(&RxDatbuf);
		EMAC_UpdateRxConsumeIndex();

		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
		if (p != NULL) 
		{	  
		    len = 0;
		
			/* We iterate over the pbuf chain until we have read the entire
			 * packet into the pbuf. */
			for(q = p; q != NULL; q = q->next) 
			{
			  /* Read enough bytes to fill this pbuf in the chain. The
			   * available data in the pbuf is given by the q->len
			   * variable. */
				memcpy((u8_t*)q->payload, (u8_t*)&gRxBuf[len], q->len);
				len = len + q->len;
			}
		}
    }
	else
	{
	    return NULL;
	}
	
	return p;  
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
err_t
ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);

  /* no packet could be read, silently ignore this */
  if (p == NULL) return ERR_MEM;

  err = netif->input(p, netif);
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
  }

  return err;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

