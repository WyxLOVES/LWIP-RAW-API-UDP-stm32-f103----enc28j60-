#include "elengine.h"
#include "sys.h"
#include "delay.h"

void dianjiqudong()
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;				 //PB12  13  14  15�˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	 GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);						 //PB 12  13  14  15 �����
}
//�򿪵��1����
void Open_Elengine_1(int dir)
{
	u16 i=0;
	if(dir)//��ת
	{
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			TIM1->CCR4 = 3000;//���1ת���ٶȵ���
			delay_ms(1000);
	}
	else//��ת
	{
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			TIM1->CCR4 = 3000;//���1ת���ٶȵ���
		delay_ms(1000);
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
}
//�򿪵��2����
void Open_Elengine_2(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
	TIM1->CCR1 = 2000;//���1ת���ٶȵ���
}
//�رյ��1����
void Close_Elengine_1(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	TIM1->CCR4 = 0;//���1ת���ٶȵ���
}
//�رյ��2����
void Close_Elengine_2(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
	TIM1->CCR1 = 0;//���1ת���ٶȵ���
}
