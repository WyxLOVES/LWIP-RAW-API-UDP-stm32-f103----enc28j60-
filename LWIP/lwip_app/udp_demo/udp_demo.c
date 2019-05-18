#include "udp_demo.h" 
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "dht11.h"
#include "led.h"
#include "elengine.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ENC28J60ģ��
//UDP ���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//UDP�������ݻ�����
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ����� 
//UDP������������
u8 *udp_demo_sendbuf=NULL;
u8 elengine_wyx = 1;

extern u8 temp1,temp2,humi1,humi2;
//UDP ����ȫ��״̬��Ǳ���
//bit7:û���õ�
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û��������;1,��������.
//bit4~0:����
u8 udp_demo_flag;
struct sardata sendrecv;
//��ʼ��һ��sendrecv
void initsardata(void)
{
	u8 i;
	sendrecv.head[0]=0x5A;//Z
	sendrecv.head[1]=0x4E;//N
	sendrecv.head[2]=0x4A;//J
	sendrecv.head[3]=0x4A;//J  �൱��ǰ׺
	sendrecv.length=15;
	
	for(i=0;i<8;i++)
	{
		sendrecv.data[i]=0x00;
	}
	sendrecv.humi[0]=0x00;//������0״̬
	sendrecv.humi[1]=0x00;//������1״̬
	sendrecv.elengine[0]=0x00;//���0״̬
	sendrecv.elengine[1]=0x00;//���1״̬
	sendrecv.led[0]=0x00;//led0״̬
	sendrecv.led[1]=0x00;//led1״̬
	sendrecv.led[2]=0x00;//led2״̬
}

//����Զ��IP��ַ
void udp_demo_set_remoteip(void)
{
	u8 *tbuf;
	u16 xoff;
	u8 key;
	POINT_COLOR=RED;
	  
	tbuf=mymalloc(SRAMIN,100);	//�����ڴ�
	if(tbuf==NULL)return;
	//ǰ����IP���ֺ�DHCP�õ���IPһ��  ͬһ����
	lwipdev.remoteip[0]=lwipdev.ip[0];//192
	lwipdev.remoteip[1]=lwipdev.ip[1];//168
	lwipdev.remoteip[2]=lwipdev.ip[2];//1
	lwipdev.remoteip[3]=(uint8_t)(0x01);//�Լ������������IP
	sprintf((char*)tbuf,"Remote IP:%d.%d.%d.",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2]);//Զ��IP
	POINT_COLOR=BLUE;
	xoff=strlen((char*)tbuf)*8+30;
	printf("KEY1:+  KEY0:-\r\n");  
	printf("KEY_UP:OK\r\n");
	while(1)
	{
		
		key=KEY_Scan(0);
		if(key==WKUP_PRES)break;
		else if(key)
		{
			if(key==KEY1_PRES)lwipdev.remoteip[3]++;//IP����
			if(key==KEY0_PRES)lwipdev.remoteip[3]--;//IP����
			printf("%d.%d.%d.%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//��ʾ��IP
		}
	} 
	myfree(SRAMIN,tbuf);
} 

//UDP����
void udp_demo_test(void)
{
 	err_t err;
	struct udp_pcb *udppcb;  	//����һ��UDP���������ƿ�
	struct ip_addr rmtipaddr;  	//��������Զ��ip��ַ
 	
	u8 *tbuf;
 	u8 key;
	u8 res=0;		
	udp_demo_set_remoteip();//����Զ��IP
	tbuf=mymalloc(SRAMIN,200);	//�����ڴ�
	if(tbuf==NULL)
		return ;		//�ڴ�����ʧ����,ֱ���˳�
	sprintf((char*)tbuf,"Local IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
	sprintf((char*)tbuf,"Remote IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//Զ��IP
	sprintf((char*)tbuf,"Remote Port:%d",UDP_DEMO_PORT);//�ͻ��˶˿ں�
	POINT_COLOR=BLUE;//����Ϊ��ɫ
	//����һ��UDP���ƿ�
	udppcb=udp_new();//���Լ����ڴ�ش���
	initsardata();//��ʼ�����Ͱ�
	if(udppcb)//�����ɹ�
	{ 
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//�����u32 λ
		err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//�󶨱���IP��ַ��˿ں�
			if(err==ERR_OK)	//�����
			{
				udp_recv(udppcb,udp_demo_recv,NULL);//ע����ջص�����  ����ص�udppcb�ṹ��Ԫ�ظ�ֵ
				printf("STATUS:Connected   \r\n");//�����������(UDP�Ƿǿɿ�����,���������ʾ����UDP�Ѿ�׼����)
				udp_demo_flag |= 1<<5;			//����Ѿ�������
				POINT_COLOR=RED;
				POINT_COLOR=BLUE;//��ɫ����
			}else res=1;
		}else res=1;		
	}else res=1;
	while(res==0)//���ӳɹ�
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)break;
		if(key==KEY0_PRES)//KEY0������,��������  ���������Ƿ���ͨ
		{
			udp_demo_senddata(udppcb);
		}	
		if(udp_demo_flag&1<<6)//�Ƿ��յ�����?
		{
			udp_demo_senddata(udppcb);
			elengine_wyx = 1;
			udp_demo_flag&=~(1<<6);//��������Ѿ���������.
		} 
		lwip_periodic_handle();//��ѯ���� ά��LWIP�ں�
		delay_ms(2);
	}
	udp_demo_connection_close(udppcb); 
	myfree(SRAMIN,tbuf);
} 

//UDP�ص�����
//pΪԭʼ����
//
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ��������� 
		for(q=p;q!=NULL;q=q->next)  //����������pbuf����  ��p��������ݿ�����udp_demo_recvbuf������
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		readrecv();//��ȡ���ܻ������Ķ������Ա㴦��
		//���޸ĵĻ����޸Ĵ˴�
		Message_processing();
		/////////
		upcb->remote_ip=*addr; 				//��¼Զ��������IP��ַ
		upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		udp_demo_flag|=1<<6;	//��ǽ��յ�������
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb);
		udp_demo_flag &= ~(1<<5);	//������ӶϿ�
	} 
} 
//UDP��������������
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)udp_demo_sendbuf),PBUF_POOL); //�����ڴ�
	if(ptr)//����ɹ�
	{
		DHT11_Read_Data1(&temp1,&humi1);
		udp_demo_sendbuf=mymalloc(SRAMIN,sendrecv.length+1);
		setdata();
		setsendbuf();	//�������úõ� sendrecv����udp_demo_sendbuf
		printf(":%s  %d\r\n",udp_demo_sendbuf,strlen((char*)udp_demo_sendbuf));
		pbuf_take(ptr,(char*)udp_demo_sendbuf,strlen((char*)udp_demo_sendbuf));//����ptr�����ݲ���
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
		myfree(SRAMIN,udp_demo_sendbuf);
	} 
} 
//�ر�UDP����
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//�Ͽ�UDP���� 
	udp_demo_flag &= ~(1<<5);	//������ӶϿ�
	POINT_COLOR = RED;
	
	POINT_COLOR=BLUE;
}

void setdata(void)
{
	//temp��������
	if(((temp1&0xf0)>>4)>=0x00&&((temp1&0xf0)>>4)<0x0A)
		sendrecv.data[0]=((temp1&0xf0)>>4)+0x30;
	else
		sendrecv.data[0]=(temp1&0xf0)+0x37;
	//tempС������
	if((temp1&0x0f)>=0x00&&(temp1&0x0f)<0x0A)	
		sendrecv.data[1]=(temp1&0x0f)+0x30;
	else
		sendrecv.data[1]=(temp1&0x0f)+0x37;
	//humi����
	if(((humi1&0xf0)>>4)>=0x00&&((humi1&0xf0)>>4)<0x0A) 
		sendrecv.data[2]=((humi1&0xf0)>>4)+0x30;
	else
		sendrecv.data[2]=(humi1&0xf0)+0x37;
	//humiС��
	if((humi1&0x0f)>=0x00&&(humi1&0x0f)<0x0A)
		sendrecv.data[3]=(humi1&0x0f)+0x30;
	else
		sendrecv.data[3]=(humi1&0x0f)+0x37;
	printf("humi:%d temp: %d sendrecv.data:%s\r\n",humi1,temp1,sendrecv.data);
}

u8 judge(void)
{
		u8 position;
		for(position=0;position<sendrecv.length;position++)
		{
				if(udp_demo_recvbuf[position]==0x5A)//Z
				if(udp_demo_recvbuf[position+1]==0x4E)//N
				if(udp_demo_recvbuf[position+2]==0x4A)//J
				if(udp_demo_recvbuf[position+3]==0x4A)//J
					break;
		}
		if(position>=sendrecv.length) return 0;
		else return position+4;//��positionָ���һ����Ч����λ
}

void readrecv(void)
{
	u8 position;
	if(judge()==0);//�ж������Ƿ����ZNJJ 0 ������  1  ����
	else//��������ش���
	{
		position=judge();//�Ӹ�λ��ȡ��������
		sendrecv.led[0]=udp_demo_recvbuf[position];
		position++;
		sendrecv.led[1]=udp_demo_recvbuf[position];
		position++;
		sendrecv.led[2]=udp_demo_recvbuf[position];
		position++;
		sendrecv.elengine[0]=udp_demo_recvbuf[position];
		position++;
		sendrecv.elengine[1]=udp_demo_recvbuf[position];
		position++;
		sendrecv.humi[0]=((udp_demo_recvbuf[position]-0x30));
		position++;
		sendrecv.humi[1]=((udp_demo_recvbuf[position]-0x30));
	}
}

void setsendbuf(void)
{
	u8 i;
	udp_demo_sendbuf[0]=sendrecv.head[0];
	udp_demo_sendbuf[1]=sendrecv.head[1];
	udp_demo_sendbuf[2]=sendrecv.head[2];
	udp_demo_sendbuf[3]=sendrecv.head[3];
	udp_demo_sendbuf[9]=sendrecv.humi[0]+0x30;
	udp_demo_sendbuf[10]=sendrecv.humi[1]+0x30;
	for(i=11;i<15;i++)
	{
		udp_demo_sendbuf[i]=sendrecv.data[i-11];
	}
	
	udp_demo_sendbuf[7]=sendrecv.elengine[0];
	udp_demo_sendbuf[8]=sendrecv.elengine[1];
	udp_demo_sendbuf[4]=sendrecv.led[0];
	udp_demo_sendbuf[5]=sendrecv.led[1];
	udp_demo_sendbuf[6]=sendrecv.led[2];
}
//��ش������Լ�д��  �ԵƵ���Ĳ������޸Ĵ˴�
void Message_processing()
{		
	printf("l:%s e:%s %s  \r\n",sendrecv.led,sendrecv.elengine,udp_demo_recvbuf);
	if(sendrecv.led[0]=='1')           	LED0=1;
	else 							 	LED0=0;
	if(sendrecv.led[1]=='1')          	LED1=1;
	else 							 	LED1=0;
	if(sendrecv.led[2]=='1')           	LED2=1;
	else 							 	LED2=0;
	//printf("wwwwwwwww%d  \r\n",sendrecv.elengine[0]);
	if(sendrecv.humi[0]==1){
		//printf("wwwwwwwww%d  \r\n",sendrecv.humi[0]);
		if(sendrecv.elengine[0]=='1'&&sendrecv.elengine[1]=='1'&&elengine_wyx)//���Ʒ�����
		{
			Open_Elengine_1(1);
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			//printf("qqqqqqqqq%d  %d\r\n",sendrecv.elengine[0],sendrecv.elengine[1]);
			elengine_wyx = 0;
		}
		if(sendrecv.elengine[0]=='1'&&sendrecv.elengine[1]=='0'&&elengine_wyx)//���Ʒ����� 
		{
			Open_Elengine_1(0);
			//printf("wwwwwwwww%d  \r\n",sendrecv.elengine[0]);
			elengine_wyx = 0;
		}
	}
	else
	{
		//printf("22222222%d \r\n",sendrecv.humi[0]);
		Close_Elengine_1();
	}
	if(sendrecv.elengine[0]=='0'&&sendrecv.elengine[1]=='1')//���Ʒ�����   
	{
		//printf("eeeeeeeee%d  %d\r\n",sendrecv.elengine[0],sendrecv.elengine[1]);
		Open_Elengine_2();			
	}
	if(sendrecv.elengine[0]=='0'&&sendrecv.elengine[1]=='0')//���Ʒ�����  
	{
		Close_Elengine_2();
		//printf("rrrrrrrrr%d  %d\r\n",sendrecv.elengine[0],sendrecv.elengine[1]);
	}
}
