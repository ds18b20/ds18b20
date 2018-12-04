#include<reg52.h>
#include"18b20.h"
#include"1602.h"
#include"delay.h"

/******************************************************************/
/*                    È«¾Ö±äÁ¿                                    */
/******************************************************************/
uint temp;
uchar flag_get=1,num,second;
/*Êý×Ö¶ÔÓ¦µÄasciiÂë±í 0x30-0 0x2d-- 0x43-C 0x2b-+ */
uchar ASCII[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
uchar UART_s[]="12345678";    //´®¿ÚÊä³öÎÂ¶È¶ÔÓ¦µÄasciiÂë                                     
uchar M[]="Temperature";		
uchar INIT0[]="Init";
uchar INIT1[]="Init.";
uchar INIT2[]="Init..";
uchar INIT3[]="Init...";	
uchar TempH,TempL;
unsigned char k=1;
uchar code table[]={"0123456789ABCDEF"}; //ÓÃÓÚ1602ÏÔÊ¾

/******************************************************************/
/*                    Ö÷º¯Êý                                      */
/******************************************************************/
void main (void)
{
	uchar i;
	TMOD|=0x21;				//¶¨Ê±Æ÷ÉèÖÃ
	SCON=0x50;				//UART¹¤×÷Ä£Ê½

	TH0=0xef;
	TL0=0xf0;
	TR0=1;
	
	TH1=0xFD;				// TH1:  ÖØ×°Ä£Ê½ 9600 baud @ 11.0592MHz 
	TR1=1;					// TR1:  ÔËÐÐ¶¨Ê±Æ÷1
	
	EA=1;					//´ò¿ª×ÜÖÐ¶Ï
	ES=1;					//´ò¿ª´®¿ÚÖÐ¶Ï
	ET0=1;					//´ò¿ª¶¨Ê±Æ÷0Òç³öÖÐ¶Ï
	
	PS=0;					//´®¿ÚÖÐ¶Ï¸ßÓÅÏÈ¼¶
	PT0=0;					//¶¨Ê±Æ÷ÖÐ¶ÏµÍÓÅÏÈ¼¶
	InitLcd(); //³õÊ¼»¯1602

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
		if(flag_get==1)			//¶¨Ê±¶ÁÈ¡µ±Ç°ÎÂ¶È
		{
			if(k==8)
			{
				k=1;
			}
			temp=ReadTemperature();
			delay(600);
			if(temp&0x8000)
	    	{
		     	//¸ººÅ±êÖ¾
		     	temp=~temp;  	// È¡·´¼Ó1
			 	temp=temp+1;
				UART_s[0]=0x2d;
		 	}
		    else
	        {
				UART_s[0]=0x2b;
			}
			TempH=temp>>4;
			TempL=temp&0x0F;
			TempL=TempL*10/16;		//Ð¡Êý ÊýÖµ
			//¸üÐÂ×¼±¸ÏÔÊ¾µÄÄÚÈÝ
			//second=0;		
			UART_s[1]=ASCII[TempH/100];	
			UART_s[2]=ASCII[(TempH%100)/10];//Ê®Î»ÎÂ¶È
			
			UART_s[3]=ASCII[(TempH%100)%10];//¸öÎ»ÎÂ¶È,´øÐ¡Êýµã
			
			UART_s[4]=0x2e;
			
			UART_s[5]=ASCII[TempL];		//Ð¡Êý ÊýÖµ
			
			UART_s[6]=0x43;				//ÏÔÊ¾C·ûºÅ 
			
			UART_s[7]=0xdf;				//¡ãµÄasiië
			
			for(i=0;i<8;i++) //8¸öwendu
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
/*                  ¶¨Ê±Æ÷ÖÐ¶Ï                                    */
/******************************************************************/
void tim(void) interrupt 1 using 1//ÖÐ¶Ï£¬ÓÃÓÚÊýÂë¹ÜÉ¨ÃèºÍÎÂ¶È¼ì²â¼ä¸ô
{
	TH0=0xef;//¶¨Ê±Æ÷ÖØ×°Öµ
	TL0=0xf0;
	num++;
	if (num==100)
	{
		num=0;
		flag_get=1;				//±êÖ¾Î»ÓÐÐ§
	    //second++;  				//´Ë´¦Ò»¸ösecond=400ms
		
	}
}
/******************************************************************/
/*                  ´®¿ÚÍ¨ÐÅÖÐ¶Ï                                  */
/******************************************************************/
void UART_SER (void) interrupt 4 //´®ÐÐÖÐ¶Ï·þÎñ³ÌÐò
{
	uchar TT=0;
	if(RI)                        //ÅÐ¶ÏÊÇ½ÓÊÕÖÐ¶Ï²úÉú
	{
		RI=0;
		TT=SBUF;
		ES=0;                    	 //´®¿ÚÖÐ¶Ï
		UART_send_string(UART_s);
		ES=1;
	}
	if(TI)                        //Èç¹ûÊÇ·¢ËÍ±êÖ¾Î»£¬ÇåÁã
    	TI=0;
} 
/******************************************************************/
/*                    ´®¿ÚÊä³öÒ»×Ö½Ú                              */
/******************************************************************/
void UART_send_byte(unsigned char dat)
{
	SBUF=dat;
	while(!TI);
	TI=0;
}

/******************************************************************/
/*                    ´®¿ÚÊä³ö×Ö·û´®                              */
/******************************************************************/
void UART_send_string(unsigned char *pstr)	//const
{
	while(*pstr!='\0')
	{
		UART_send_byte(*pstr++);
	}
}

/******************************************************************/
/*                    DS18B20³õÊ¼»¯                               */
/******************************************************************/
void Init_DS18B20(void)
{
	unsigned char x=0;
	DQ = 1;    //DQ¸´Î»
	delay(8);  //ÉÔ×öÑÓÊ±
	DQ = 0;    //µ¥Æ¬»ú½«DQÀ­µÍ
	delay(80); //¾«È·ÑÓÊ± ´óÓÚ 480us
	DQ = 1;    //À­¸ß×ÜÏß
	delay(10);
	x=DQ;      //ÉÔ×öÑÓÊ±ºó Èç¹ûx=0Ôò³õÊ¼»¯³É¹¦ x=1Ôò³õÊ¼»¯Ê§°Ü
	delay(5);
}

/******************************************************************/
/*                    DS18B20¶ÁÒ»¸ö×Ö½Ú                           */
/******************************************************************/
unsigned char ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{
		DQ = 0; // ¸øÂö³åÐÅºÅ
		dat>>=1;
		DQ = 1; // ¸øÂö³åÐÅºÅ
		if(DQ)
		dat|=0x80;
		delay(5);
	}
	return(dat);
}

/******************************************************************/
/*                 DS18B20Ð´Ò»¸ö×Ö½Ú                              */
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
/*                   DS18B20¶ÁÈ¡ÎÂ¶È                              */
/******************************************************************/
unsigned int ReadTemperature(void)
{
	unsigned char i;
	unsigned char L=0;
	unsigned int H=0;
	unsigned int T=0;
	mum_select(k);
	Init_DS18B20();
	WriteOneChar(0xCC);	//Ìø¹ý¶ÁÐòºÅÁÐºÅµÄ²Ù×÷
	WriteOneChar(0x44);	//Æô¶¯ÎÂ¶È×ª»»
	delay(200);
	Init_DS18B20();
	WriteOneChar(0x55); //Æ¥ÅäÐòºÅÁÐºÅµÄ²Ù×÷
	for(i=0;i<8;i++)
	{
		WriteOneChar(ROM_NUM[i]);		
	}
	//Init_DS18B20();
	//WriteOneChar(0xCC);	//Ìø¹ý¶ÁÐòºÅÁÐºÅµÄ²Ù×÷ 
	WriteOneChar(0xBE);	//¶ÁÈ¡ÎÂ¶È¼Ä´æÆ÷µÈ£¨¹²¿É¶Á9¸ö¼Ä´æÆ÷£© Ç°Á½¸ö¾ÍÊÇÎÂ¶È
	L=ReadOneChar();	//µÍÎ»
	H=ReadOneChar();	//¸ßÎ»
	
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
