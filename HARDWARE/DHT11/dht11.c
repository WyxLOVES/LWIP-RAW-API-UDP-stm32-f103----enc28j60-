#include "dht11.h"
#include "delay.h"
#include "lcd.h"
#include "usart.h"
//DHT11����ʪ�¶ȴ��������õ��������ݸ�ʽ�������������Ŷ˿�����������˫���䡣�����ݰ���5Byte��40Bit����ɡ����ݷ�С�����ֺ��������֣�
//һ�����������ݴ���Ϊ40bit����λ�ȳ���

//DHT11�����ݸ�ʽΪ��8bitʪ����������+8bitʪ��С������+8bit�¶���������+8bit�¶�С������+8bitУ��͡�

u8 temp1,temp2,humi1,humi2;
//��λ
void DHT11_Rst1(void)
{
		DHT11_IO_OUT1();//SET OUTPUT
		DHT11_DQ_OUT1=0;//����������
		delay_ms(20);//��������18ms
		DHT11_DQ_OUT1=1;//����������
		delay_us(30);//����20-40us
}

void DHT11_Rst2(void)
{
		DHT11_IO_OUT2();
		DHT11_DQ_OUT2=0;
		delay_ms(20);
		DHT11_DQ_OUT2=1;
		delay_us(30);
}
//�ȴ���Ӧ
//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_Check1(void)
{
		u8 retry=0;
		DHT11_IO_IN1();//�������ݿ�Ϊ����
		while(DHT11_DQ_IN1&&retry<100)//�ȴ�DHT11��Ӧ	�͵�ƽΪ��Ӧ��  ������40~80us
		{
			retry++;
			delay_us(1);
		}
		if(retry>=100) return 1;//û��⵽DHT11
		else retry=0;
		while(!DHT11_DQ_IN1&&retry<100)//�ٴεȴ�DHT11��Ӧ  �ߵ�ƽΪ��Ӧ��  ���ͺ���ٴ�����40~80us
		{
			retry++;
			delay_us(1);
		}
		if(retry>=100) return 1;
		else return 0;
}

//u8 DHT11_Check2(void)
//{
//		u8 retry=0;
//		DHT11_IO_IN2();
//		while(DHT11_DQ_IN2&&retry<100)
//		{
//			retry++;
//			delay_us(1);
//		}
//		if(retry>=100) return 1;
//		else retry=0;
//		while(!DHT11_DQ_IN2&&retry<100)
//		{
//			retry++;
//			delay_us(1);
//		}
//		if(retry>=100) return 1;
//		else return 0;
//}
//��DHT11��ȡһ��λ   ����ֵ��1/0
u8 DHT11_Read_Bit1(void)
{
		u8 retry=0;
		while(DHT11_DQ_IN1&&retry<100)//�ȴ���Ϊ�͵�ƽ
		{
			retry++;
			delay_us(1);
		}
		retry=0;
		while(!DHT11_DQ_IN1&&retry<100)//�ȴ���ߵ�ƽ
		{
			retry++;
			delay_us(1);
		}
		delay_us(40);//�ȴ�40us
		if(DHT11_DQ_IN1) return 1;
		else return 0;
}

u8 DHT11_Read_Bit2(void)
{
		u8 retry=0;
		while(DHT11_DQ_IN2&&retry<100)
		{
			retry++;
			delay_us(1);
		}
		retry=0;
		while(!DHT11_DQ_IN2&&retry<100)
		{
			retry++;
			delay_us(1);
		}
		delay_us(40);
		if(DHT11_DQ_IN2) return 1;
		else return 0;
}
//��DHT11��ȡһ���ֽ�
//����ֵ������������
u8 DHT11_Read_Byte1()
{
		u8 i,dat;
		dat=0;
		for(i=0;i<8;i++)
		{
				dat<<=1;
				dat|=DHT11_Read_Bit1();
		}
		return dat;
}

u8 DHT11_Read_Byte2()
{
		u8 i,dat;
		dat=0;
		for(i=0;i<8;i++)
		{
				dat<<=1;
				dat|=DHT11_Read_Bit2();
		}
		return dat;
}
//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)     humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 DHT11_Read_Data1(u8 *temp1,u8 *humi1)//�¶ȡ�ʪ��
{
		u8 buf1[5];
		u8 i;
		DHT11_Rst1();
		if(DHT11_Check1()==0)
		{
			for(i=0;i<5;i++)//��ȡ40λ����  5���ֽ�
			{
					buf1[i]=DHT11_Read_Byte1();
			}
			if(buf1[0]+buf1[1]+buf1[2]+buf1[3]==buf1[4])
			{
					*humi1=buf1[0];
					*temp1=buf1[2];
			}
		}
			else return 1;
			return 0;
}



//u8 DHT11_Read_Data2(u8 *temp2,u8 *humi2)
//{
//		u8 buf1[5];
//		u8 i;
//		DHT11_Rst2();
//		if(DHT11_Check2()==0)
//		{
//			for(i=0;i<5;i++)
//			{
//					buf1[i]=DHT11_Read_Byte2();
//			}
//			if(buf1[0]+buf1[1]+buf1[2]+buf1[3]==buf1[4])
//			{
//					*humi2=buf1[0];
//					*temp2=buf1[2];
//			}
//		}
//			else return 1;
//			return 0;
//}

u8 DHT11_Init1(void)
{
		GPIO_InitTypeDef GPIO_InitStructure1;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
		
		GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC,&GPIO_InitStructure1);
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
		
		
		DHT11_Rst1();
		return DHT11_Check1();
}

//u8 DHT11_Init2(void)
//{
//		GPIO_InitTypeDef GPIO_InitStructure;
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
//		
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC,&GPIO_InitStructure);
//		GPIO_SetBits(GPIOC,GPIO_Pin_13);
//		
//		
//		DHT11_Rst2();
//		return DHT11_Check2();
//}

void DHT11_test(void)
{
		u8 t=0;
		while(DHT11_Init1()) //DHT11 1��ʼ��
		{ 
				LCD_ShowString(30,120,200,16,16,"DHT11111 Error");
				delay_ms(200);
		}
//		while(DHT11_Init2()) //DHT11 2��ʼ��
//		{
//				LCD_ShowString(30,120,200,16,16,"DHT11222 Error");
//				delay_ms(200);
//		}
		LCD_ShowString(30,200,200,16,16,"DHT11 OK");
		LCD_ShowString(30,220,200,16,16,"TEMP1 :  C");
		LCD_ShowString(30,240,200,16,16,"HUMI1 :  %");
//		LCD_ShowString(30,260,200,16,16,"TEMP2 :  C");
//		LCD_ShowString(30,280,200,16,16,"HUMI2 :  %");
		
		while(t<100)//�����¶ȴ�����
		{
				if(t%10==0)
				{
						DHT11_Read_Data1(&temp1,&humi1);
						//DHT11_Read_Data2(&temp2,&humi2);
						printf("temp1 %d C",temp1);
						printf("HUMI  %d %\r\n",humi1);
						//LCD_ShowNum(30+50,240,humi1,2,16);
//						LCD_ShowNum(30+50,260,temp2,2,16);
//						LCD_ShowNum(30+50,280,humi2,2,16);
						
				}
				delay_ms(10);
				t++;
		}
						LCD_ShowNum(30+50,240,humi1,2,16);
//						LCD_ShowNum(30+50,260,temp2,2,16);
//						LCD_ShowNum(30+50,280,humi2,2,16);
						
}
