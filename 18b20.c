
#include"delay.h"
#include"18b20.h"
#define uchar unsigned char

unsigned char code ds18b20_num1[8]={0x28,0x30,0xc5,0xb8,0x00,0x00,0x00,0x8e};
unsigned char code ds18b20_num2[8]={0x28,0x31,0xc5,0xb8,0x00,0x00,0x00,0xB9};
unsigned char code ds18b20_num3[8]={0x28,0x32,0xc5,0xb8,0x00,0x00,0x00,0xE0};
unsigned char code ds18b20_num4[8]={0x28,0x33,0xc5,0xb8,0x00,0x00,0x00,0xD7};
unsigned char code ds18b20_num5[8]={0x28,0x34,0xc5,0xb8,0x00,0x00,0x00,0x52};
unsigned char code ds18b20_num6[8]={0x28,0x35,0xc5,0xb8,0x00,0x00,0x00,0x65};
unsigned char code ds18b20_num7[8]={0x28,0x36,0xc5,0xb8,0x00,0x00,0x00,0x3c};
unsigned char code ds18b20_num8[8]={0x28,0x37,0xc5,0xb8,0x00,0x00,0x00,0x0B};
unsigned char code ds18b20_num9[8]={0x28,0x38,0xc5,0xb8,0x00,0x00,0x00,0x2F};
unsigned char code ds18b20_num10[8]={0x28,0x39,0xc5,0xb8,0x00,0x00,0x00,0x18};
unsigned char code ds18b20_num11[8]={0x28,0x3a,0xc5,0xb8,0x00,0x00,0x00,0x41};
unsigned char code ds18b20_num12[8]={0x28,0x3b,0xc5,0xb8,0x00,0x00,0x00,0x76};
unsigned char ROM_NUM[8];
unsigned char k;
uchar sn[8]; //4个器件的序列号，先读出单个序列号后填上才可以读取温度
/*------------------------------------------------
                    18b20初始化
------------------------------------------------*/
bit Init_DS18B20(void)
{
	 bit dat=0;
	 DQ = 1;    //DQ复位
	 DelayUs2x(5);   //稍做延时
	 DQ = 0;         //单片机将DQ拉低
	 DelayUs2x(200); //精确延时 大于 480us 小于960us
	 DelayUs2x(200);
	 DQ = 1;        //拉高总线
	 DelayUs2x(50); //15~60us 后 接收60-240us的存在脉冲
	 dat=DQ;        //如果x=0则初始化成功, x=1则初始化失败
	 DelayUs2x(25); //稍作延时返回
	 return dat;
}

/*------------------------------------------------
                    读取一个字节
------------------------------------------------*/
unsigned char ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	 {
		  DQ = 0; // 给脉冲信号
		  dat>>=1;
		  DQ = 1; // 给脉冲信号
		  if(DQ)
		   dat|=0x80;
		  DelayUs2x(25);
	 }
	 return(dat);
}
/*------------------------------------------------
                    写入一个字节
------------------------------------------------*/
void WriteOneChar(unsigned char dat)
{
	 unsigned char i=0;
	 for (i=8; i>0; i--)
	 {
		  DQ = 0;
		  DQ = dat&0x01;
		  DelayUs2x(25);
		  DQ = 1;
		  dat>>=1;
	 }
	DelayUs2x(25);
}

/*------------------------------------------------
                    读取温度
------------------------------------------------*/
unsigned int ReadTemperature()
{
	unsigned char i;
	unsigned char a=0;
	unsigned int b=0;
	unsigned int t=0;
	float tt=0;
	mum_select(k);
	Init_DS18B20();
	WriteOneChar(0xCC); // 跳过读序号列号的操作
	WriteOneChar(0x44); // 启动温度转换
	DelayMs(100);
	Init_DS18B20();
	WriteOneChar(0x55); //匹配序号列号的操作
	for(i=0;i<8;i++)
	{
		WriteOneChar(ROM_NUM[i]);		
	}
	WriteOneChar(0xBE); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	a=ReadOneChar();   //低位
	b=ReadOneChar();   //高位	
	b<<=8;
	t=a+b;	
	return(t);
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
	else if(m==9)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num9[n];
		}
	}
	else if(m==10)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num10[n];
		}
	}
	else if(m==11)
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num11[n];
		}
	}
	else
	{
		for(n=0;n<8;n++)
		{
			ROM_NUM[n]=ds18b20_num12[n];
		}
	}
}
