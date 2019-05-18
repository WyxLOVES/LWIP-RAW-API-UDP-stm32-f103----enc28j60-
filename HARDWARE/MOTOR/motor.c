#include "motor.h"
#include "delay.h"




//���PWM��ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
//��ʱ��1 ͨ��1��ͨ��4
void MOTOR_PWM_Init(u16 arr,u16 psc)
{   
	RCC->APB2ENR|=1<<11;    		//ʹ��TIM1ʱ��	
	RCC->APB2ENR|=1<<2;    			//ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<3;    			//ʹ��PORTBʱ��
	//���PWM����ʹ��
	GPIOA->CRH&=0XFFFF0FF0;			//PA8��11���	
	GPIOA->CRH|=0X0000B00B;			//���ù������ 	

	TIM1->ARR=arr;             	//�趨�������Զ���װֵ 
	TIM1->PSC=psc;             		//Ԥ��Ƶ������Ƶ
	TIM1->CCMR2|=6<<12;        		//CH4 PWM1ģʽ	
	TIM1->CCMR1|=6<<4;         		//CH1 PWM1ģʽ	
	TIM1->CCMR2|=1<<11;        		//CH4Ԥװ��ʹ��	 
	TIM1->CCMR1|=1<<3;         		//CH1Ԥװ��ʹ��	  
	TIM1->CCER|=1<<12;         		//CH4���ʹ��	   
	TIM1->CCER|=1<<0;          		//CH1���ʹ��	
	TIM1->BDTR |= 1<<15;       		//TIM1����Ҫ��仰�������PWM
	TIM1->CR1=0x8000;          		//ARPEʹ�� 
	TIM1->CR1|=0x01;          		//ʹ�ܶ�ʱ��1 	
}


