#ifndef _DHT11_H
#define _DHT11_H
#include "sys.h"

//IO��������
#define DHT11_IO_IN1()  {GPIOC->CRL&=0XFFFFFFF0;GPIOC->CRL|=8<<0;}
#define DHT11_IO_OUT1() {GPIOC->CRL&=0XFFFFFFF0;GPIOC->CRL|=3<<0;}
#define DHT11_IO_IN2()  {GPIOC->CRH&=0XFF0FFFFF;GPIOC->CRH|=8<<20;}
#define DHT11_IO_OUT2() {GPIOC->CRH&=0XFF0FFFFF;GPIOC->CRH|=3<<20;}
//IO��������
#define DHT11_DQ_OUT1 PCout(0)
#define DHT11_DQ_IN1  PCin(0)
#define DHT11_DQ_OUT2 PCout(13)
#define DHT11_DQ_IN2  PCin(13)

extern u8 temp1,temp2,humi1,humi2;

u8 DHT11_Init1(void);//��ʼ��DHT11
u8 DHT11_Init2(void);
u8 DHT11_Read_Data1(u8 *temp1,u8 *humi1);//��ȡ��ʪ��
u8 DHT11_Read_Data2(u8 *temp1,u8 *humi1);
u8 DHT11_Read_Byte1(void);//��һ���ֽ�
u8 DHT11_Read_Bit1(void);//��һ��λ
u8 DHT11_Read_Byte2(void);//��һ���ֽ�
u8 DHT11_Read_Bit2(void);//��һ��λ
u8 DHT11_Check1(void);//����Ƿ����DHT11
u8 DHT11_Check2(void);
void DHT11_Rst1(void);//��λDHT11
void DHT11_Rst2(void);
void DHT11_test(void);
#endif
