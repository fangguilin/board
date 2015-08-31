#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);
struct netif *ethernetif_register(void);
int ethernetif_poll(void);
void Set_MAC_Address(unsigned char* macadd);

/* 定义的MAC地址 */   
#define ETHADDR0   0x00
#define ETHADDR1   0x00
#define ETHADDR2   0x00
#define ETHADDR3   0x00
#define ETHADDR4   0x00
#define ETHADDR5   0x01
 
/* 定义的IP地址 */
#define	 IPADDR0  192
#define	 IPADDR1  168
#define	 IPADDR2  1
#define	 IPADDR3  199

/* 定义的网关地址 */
#define GATEWAY_ADDR0  192
#define GATEWAY_ADDR1  168
#define GATEWAY_ADDR2  1
#define GATEWAY_ADDR3  1

/* 定义的掩码地址 */
#define NET_MASK0  255
#define NET_MASK1  255
#define NET_MASK2  255
#define NET_MASK3  0

#endif 
