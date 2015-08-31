#include "lpc17xx.h"
#include "emac.h"
#include "netconf.h"

void ENET_IRQHandler (void)
{
	uint32_t status;
	while ((status = (LPC_EMAC->IntStatus & LPC_EMAC->IntEnable)) != 0) 	//��ȡ�ж�״̬
	{
		LPC_EMAC->IntClear = status;				//����ж�״̬
		if ((status & EMAC_INT_RX_DONE))		//���յ�����
		{
			LwIP_Pkt_Handle();
		}
	}
}
void UART2_IRQHandler (void)
{
	
	
}


