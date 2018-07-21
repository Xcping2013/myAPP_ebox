#include "ebox.h"

#include <string.h>

#define USART_REC_LEN  					200 
uint8_t USART_RX_BUF[USART_REC_LEN];
uint16_t USART_RX_STA=0; 

void uart1_read_ti(void)
{
    uint8_t Res;
    Res = uart1.read();
	
	if((USART_RX_STA&0x8000)==0)				//����δ���
	{
		if(USART_RX_STA&0x4000)						//���յ���0x0d
		{
			if(Res!=0x0a)USART_RX_STA=0;		//���մ���,���¿�ʼ
			else USART_RX_STA|=0x8000;			//��������� 
		}
		else 															//��û�յ�0X0D
		{	
			if(Res==0x0d)USART_RX_STA|=0x4000;
			else
			{
				USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
				USART_RX_STA++;
				if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
			}		 
		}
	} 
}

void setup(void)
{
	ebox_init();
	uart1.begin(115200);    
	uart1.attach(uart1_read_ti,RxIrq);
	uart1.interrupt(RxIrq,ENABLE);
	uart1.printf(" fixture init ok>>!");
}

int main(void)
{
	uint8_t len;
	setup();          
	while(1)
	{
		if(USART_RX_STA&0x8000)									//���յ�\r\n						
		{
			len=USART_RX_STA&0x3fff;							//�õ��˴ν��յ������ݳ���
			USART_RX_BUF[len]='\0';								//��ĩβ���������. 
			uart1.printf((char*)USART_RX_BUF);
			uart1.printf("\n");
			if(USART_RX_BUF[0]=='\0')	uart1.printf("\n");
			else if(strcmp("[SN]",	(char*)USART_RX_BUF)==0)	{uart1.printf("[SN]<sn-GFH8136001AJJNV08>\n");USART_RX_STA=0;}
			else if(strcmp("[BATMAN]",	(char*)USART_RX_BUF)==0)	
			{
				uart1.printf("[BATMAN]<batman-084%%-V:3081-CS:03-L-%-04.4d%%-CS:00-V:%-04.4d-I:%-04.4d-1-R-255%%-CS:00-V:0000-I:0000-1>\n",
				random(100),random(2000),random(2000));
				USART_RX_STA=0;
			}
			else 
			{
				uart1.printf("<NA>\n");								
				uart1.printf(">>");	
				USART_RX_STA=0;
			}
		}
	}	
}

