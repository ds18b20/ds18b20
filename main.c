#include<reg52.h>
#include"18b20.h"
#include"1602.h"
#include"delay.h"

/******************************************************************/
/*                    全局变量                                    */
/******************************************************************/
uint temp;
uchar flag_get=1,num,second;
/*数字对应的ascii码表 0x30-0 0x2d-- 0x43-C 0x2b-+ */
uchar ASCII[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
uchar UART_s[]="12345678";    //串口输出温度对应的ascii码                                     
uchar M[]="Temperature";		
uchar INIT0[]="Init";
uchar INIT1[]="Init.";
uchar INIT2[]="Init..";
uchar INIT3[]="Init...";	
uchar TempH,TempL;
unsigned char k=1;
uchar code table[]={"0123456789ABCDEF"}; //用于1602显示

/******************************************************************/
/*                    主函数                                      */
/******************************************************************/
void main (void)
{
	uchar i;
	TMOD|=0x21;				//定时器设置
	SCON=0x50;				//UART工作模式

	TH0=0xef;
	TL0=0xf0;
	TR0=1;
	
	TH1=0xFD;				// TH1:  重装模式 9600 baud @ 11.0592MHz 
	TR1=1;					// TR1:  运行定时器1
	
	EA=1;					//打开总中断
	ES=1;					//打开串口中断
	ET0=1;					//打开定时器0溢出中断
	
	PS=0;					//串口中断高优先级
	PT0=0;					//定时器中断低优先级
	InitLcd(); //初始化1602

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
		if(flag_get==1)			//定时读取当前温度
		{
			if(k==8)
			{
				k=1;
			}
			temp=ReadTemperature();
			delay(600);
			if(temp&0x8000)
	    	{
		     	//负号标志
		     	temp=~temp;  	// 取反加1
			 	temp=temp+1;
				UART_s[0]=0x2d;
		 	}
		    else
	        {
				UART_s[0]=0x2b;
			}
			TempH=temp>>4;
			TempL=temp&0x0F;
			TempL=TempL*10/16;		//小数 数值
			//更新准备显示的内容
			//second=0;		
			UART_s[1]=ASCII[TempH/100];	
			UART_s[2]=ASCII[(TempH%100)/10];//十位温度
			
			UART_s[3]=ASCII[(TempH%100)%10];//个位温度,带小数点
			
			UART_s[4]=0x2e;
			
			UART_s[5]=ASCII[TempL];		//小数 数值
			
			UART_s[6]=0x43;				//显示C符号 
			
			UART_s[7]=0xdf;				//°的asii�
			
			for(i=0;i<8;i++) //8个wendu
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
/*                  定时器中断                                    */
/******************************************************************/
void tim(void) interrupt 1 using 1//中断，用于数码管扫描和温度检测间隔
{
	TH0=0xef;//定时器重装值
	TL0=0xf0;
	num++;
	if (num==100)
	{
		num=0;
		flag_get=1;				//标志位有效
	    //second++;  				//此处一个second=400ms
		
	}
}
/******************************************************************/
/*                  串口通信中断                                  */
/******************************************************************/
void UART_SER (void) interrupt 4 //串行中断服务程序
{
	uchar TT=0;
	if(RI)                        //判断是接收中断产生
	{
		RI=0;
		TT=SBUF;
		ES=0;                    	 //串口中断
		UART_send_string(UART_s);
		ES=1;
	}
	if(TI)                        //如果是发送标志位，清零
    	TI=0;
} 
/******************************************************************/
/*                    串口输出一字节                              */
/******************************************************************/
void UART_send_byte(unsigned char dat)
{
	SBUF=dat;
	while(!TI);
	TI=0;
}

/******************************************************************/
/*                    串口输出字符串                              */
/******************************************************************/
void UART_send_string(unsigned char *pstr)	//const
{
	while(*pstr!='\0')
	{
		UART_send_byte(*pstr++);
	}
}

/******************************************************************/
/*                    DS18B20初始化                               */
/******************************************************************/
void Init_DS18B20(void)
{
	unsigned char x=0;
	DQ = 1;    //DQ复位
	delay(8);  //稍做延时
	DQ = 0;    //单片机将DQ拉低
	delay(80); //精确延时 大于 480us
	DQ = 1;    //拉高总线
	delay(10);
	x=DQ;      //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	delay(5);
}

/******************************************************************/
/*                    DS18B20读一个字节                           */
/******************************************************************/
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
		delay(5);
	}
	return(dat);
}

/******************************************************************/
/*                 DS18B20写一个字节                              */
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
/*                   DS18B20读取温度                              */
/******************************************************************/
unsigned int ReadTemperature(void)
{
	unsigned char i;
	unsigned char L=0;
	unsigned int H=0;
	unsigned int T=0;
	mum_select(k);
	Init_DS18B20();
	WriteOneChar(0xCC);	//跳过读序号列号的操作
	WriteOneChar(0x44);	//启动温度转换
	delay(200);
	Init_DS18B20();
	WriteOneChar(0x55); //匹配序号列号的操作
	for(i=0;i<8;i++)
	{
		WriteOneChar(ROM_NUM[i]);		
	}
	//Init_DS18B20();
	//WriteOneChar(0xCC);	//跳过读序号列号的操作 
	WriteOneChar(0xBE);	//读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	L=ReadOneChar();	//低位
	H=ReadOneChar();	//高位
	
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
