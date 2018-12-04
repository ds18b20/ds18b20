#include<reg52.h>
#include"18b20.h"
#include"1602.h"
#include"delay.h"

/******************************************************************/
/*                    ȫ�ֱ���                                    */
/******************************************************************/
uint temp;
uchar flag_get=1,num,second;
/*���ֶ�Ӧ��ascii��� 0x30-0 0x2d-- 0x43-C 0x2b-+ */
uchar ASCII[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
uchar UART_s[]="12345678";    //��������¶ȶ�Ӧ��ascii��                                     
uchar M[]="Temperature";		
uchar INIT0[]="Init";
uchar INIT1[]="Init.";
uchar INIT2[]="Init..";
uchar INIT3[]="Init...";	
uchar TempH,TempL;
unsigned char k=1;
uchar code table[]={"0123456789ABCDEF"}; //����1602��ʾ

/******************************************************************/
/*                    ������                                      */
/******************************************************************/
void main (void)
{
	uchar i;
	TMOD|=0x21;				//��ʱ������
	SCON=0x50;				//UART����ģʽ

	TH0=0xef;
	TL0=0xf0;
	TR0=1;
	
	TH1=0xFD;				// TH1:  ��װģʽ 9600 baud @ 11.0592MHz 
	TR1=1;					// TR1:  ���ж�ʱ��1
	
	EA=1;					//�����ж�
	ES=1;					//�򿪴����ж�
	ET0=1;					//�򿪶�ʱ��0����ж�
	
	PS=0;					//�����жϸ����ȼ�
	PT0=0;					//��ʱ���жϵ����ȼ�
	InitLcd(); //��ʼ��1602

	ShowString(0,INIT0);	
	delay(10000);
	ShowString(0,INIT1);	
	delay(10000);
	ShowString(0,INIT2);	
	delay(10000);
	ShowString(0,INIT3);	
	delay(10000);
	
	while (1)        
	{
		if(flag_get==1)			//��ʱ��ȡ��ǰ�¶�
		{
			if(k==8)
			{
				k=1;
			}
			temp=ReadTemperature();
			delay(600);
			if(temp&0x8000)
	    	{
		     	//���ű�־
		     	temp=~temp;  	// ȡ����1
			 	temp=temp+1;
				UART_s[0]=0x2d;
		 	}
		    else
	        {
				UART_s[0]=0x2b;
			}
			TempH=temp>>4;
			TempL=temp&0x0F;
			TempL=TempL*10/16;		//С�� ��ֵ
			//����׼����ʾ������
			//second=0;		
			UART_s[1]=ASCII[TempH/100];	
			UART_s[2]=ASCII[(TempH%100)/10];//ʮλ�¶�
			
			UART_s[3]=ASCII[(TempH%100)%10];//��λ�¶�,��С����
			
			UART_s[4]=0x2e;
			
			UART_s[5]=ASCII[TempL];		//С�� ��ֵ
			
			UART_s[6]=0x43;				//��ʾC���� 
			
			UART_s[7]=0xdf;				//���asii�
			
			for(i=0;i<8;i++) //8��wendu
			{
				ShowString(0,M);
				ShowChar(12,ASCII[k]);
				ShowString(1,UART_s);	
			}
			delay(65535);
			k++;
		  	flag_get=0;
	    }			  
	}
}
/******************************************************************/
/*                  ��ʱ���ж�                                    */
/******************************************************************/
void tim(void) interrupt 1 using 1//�жϣ����������ɨ����¶ȼ����
{
	TH0=0xef;//��ʱ����װֵ
	TL0=0xf0;
	num++;
	if (num==100)
	{
		num=0;
		flag_get=1;				//��־λ��Ч
	    //second++;  				//�˴�һ��second=400ms
		
	}
}
/******************************************************************/
/*                  ����ͨ���ж�                                  */
/******************************************************************/
void UART_SER (void) interrupt 4 //�����жϷ������
{
	uchar TT=0;
	if(RI)                        //�ж��ǽ����жϲ���
	{
		RI=0;
		TT=SBUF;
		ES=0;                    	 //�����ж�
		UART_send_string(UART_s);
		ES=1;
	}
	if(TI)                        //����Ƿ��ͱ�־λ������
    	TI=0;
} 
/******************************************************************/
/*                    �������һ�ֽ�                              */
/******************************************************************/
void UART_send_byte(unsigned char dat)
{
	SBUF=dat;
	while(!TI);
	TI=0;
}

/******************************************************************/
/*                    ��������ַ���                              */
/******************************************************************/
void UART_send_string(unsigned char *pstr)	//const
{
	while(*pstr!='\0')
	{
		UART_send_byte(*pstr++);
	}
}

/******************************************************************/
/*                    DS18B20��ʼ��                               */
/******************************************************************/
void Init_DS18B20(void)
{
	unsigned char x=0;
	DQ = 1;    //DQ��λ
	delay(8);  //������ʱ
	DQ = 0;    //��Ƭ����DQ����
	delay(80); //��ȷ��ʱ ���� 480us
	DQ = 1;    //��������
	delay(10);
	x=DQ;      //������ʱ�� ���x=0���ʼ���ɹ� x=1���ʼ��ʧ��
	delay(5);
}

/******************************************************************/
/*                    DS18B20��һ���ֽ�                           */
/******************************************************************/
unsigned char ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{
		DQ = 0; // �������ź�
		dat>>=1;
		DQ = 1; // �������ź�
		if(DQ)
		dat|=0x80;
		delay(5);
	}
	return(dat);
}

/******************************************************************/
/*                 DS18B20дһ���ֽ�                              */
/******************************************************************/
void WriteOneChar(unsigned char dat)
{
	unsigned char i=0;
	for (i=8; i>0; i--)
	{
		DQ = 0;
		DQ = dat&0x01;
		delay(5);
		DQ = 1;
		dat>>=1;
	}
	delay(5);
}

/******************************************************************/
/*                   DS18B20��ȡ�¶�                              */
/******************************************************************/
unsigned int ReadTemperature(void)
{
	unsigned char i;
	unsigned char L=0;
	unsigned int H=0;
	unsigned int T=0;
	mum_select(k);
	Init_DS18B20();
	WriteOneChar(0xCC);	//����������кŵĲ���
	WriteOneChar(0x44);	//�����¶�ת��
	delay(200);
	Init_DS18B20();
	WriteOneChar(0x55); //ƥ������кŵĲ���
	for(i=0;i<8;i++)
	{
		WriteOneChar(ROM_NUM[i]);		
	}
	//Init_DS18B20();
	//WriteOneChar(0xCC);	//����������кŵĲ��� 
	WriteOneChar(0xBE);	//��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
	L=ReadOneChar();	//��λ
	H=ReadOneChar();	//��λ
	
	H<<=8;
	T=L+H;
	
	return(T);
}

void mum_select(unsigned char m)
{
	unsigned char n;
	if(m==1)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num1[n];
		}
	}
	else if(m==2)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num2[n];
		}
	}
	else if(m==3)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num3[n];
		}
	}
	else if(m==4)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num4[n];
		}
	}
	else if(m==5)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num5[n];
		}
	}
	else if(m==6)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num6[n];
		}
	}
	else if(m==7)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num7[n];
		}
	}
	else if(m==8)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num8[n];
		}
	}
//	else if(m==9)
//	{
//		for(n=0;n<8;n++)
//		{
//			ROM_NUM[n]=ds18b20_num9[n];
//		}
//	}
//	else if(m==10)
//	{
//		for(n=0;n<8;n++)
//		{
//			ROM_NUM[n]=ds18b20_num10[n];
//		}
//	}
//	else if(m==11)
//	{
//		for(n=0;n<8;n++)
//		{
//			ROM_NUM[n]=ds18b20_num11[n];
//		}
//	}
//	else
//	{
//		for(n=0;n<8;n++)
//		{
//			ROM_NUM[n]=ds18b20_num12[n];
//		}
//	}
}
