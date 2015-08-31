#include "lpc17xx.h"
#include "emac.h"
#include "netconf.h"

void ENET_IRQHandler (void)
{
	uint32_t status;
	while ((status = (LPC_EMAC->IntStatus & LPC_EMAC->IntEnable)) != 0) 	//获取中断状态
	{
		LPC_EMAC->IntClear = status;				//清除中断状态
		if ((status & EMAC_INT_RX_DONE))		//接收到数据
		{
			LwIP_Pkt_Handle();
		}
	}
}
void UART2_IRQHandler (void)
{
	
	
}


