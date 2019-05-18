#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "usmart.h"
#include "sram.h"
#include "malloc.h"
#include "enc28j60.h" 	 
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "timer.h"
#include "udp_demo.h"
#include "dht11.h"
#include "exti.h"
#include "elengine.h"
#include "motor.h"
/************************************************
 ALIENTEK ENC28J60 ����ʵ��
 ����RAW API��UDPʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

extern u8 udp_demo_flag;  //UDP ����ȫ��״̬��Ǳ���

int main(void)
{	 
	u8 key;
	delay_init();	    		//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 		//���ڳ�ʼ��Ϊ115200
	KEY_Init();	 				//��ʼ������
	LED_Init();			   	 	//LED�˿ڳ�ʼ��
	TIM3_Int_Init(1000,719);	//��ʱ��3Ƶ��Ϊ100hz  //ϵͳʱ��  ��������LWIP��׼��ʱ
 	MOTOR_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ������������� �����ʼ������ӿ� ��ΪMiniBalance_PWM_Init(9999,35) 200HZ	
 	FSMC_SRAM_Init();			//��ʼ���ⲿSRAM
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	
	while(lwip_comm_init()) 	//lwip��ʼ��  TCP/IPЭ��ջ
	{
		delay_ms(1200);
	}
	printf("LWIP Init Success!\r\n");
#if LWIP_DHCP   //ʹ��DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{
		lwip_periodic_handle();	//LWIP�ں���Ҫ��ʱ����ĺ���
	}
#endif
	delay_ms(500);			//��ʱ1s
	delay_ms(500);
	udp_demo_test();  		//UDP ģʽ  ����  ���ӳɹ����ڴ˺���
	//����ʧ�ܲŻ�ִ�� ����while(1)�����ٴ�������λ��
  	while(1)
	{	
		key = KEY_Scan(0);
		if(key == KEY1_PRES)		//��KEY1����������
		{
			if((udp_demo_flag & 1<<5)) printf("UDP�����Ѿ�����,�����ظ�����\r\n");	//������ӳɹ�,�����κδ���
			else udp_demo_test();		//���Ͽ����Ӻ�,����udp_demo_test()����
		}
		delay_ms(10);
	}
}


