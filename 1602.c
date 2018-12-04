#include "1602.h"
#include "string.h"
/******************************************************************/
/*                    ��������                                    */
/******************************************************************/
/******************************************************************/
/*                    ΢����ʱ����                                */
/******************************************************************/
void DelayUs(unsigned char us)//delay us
{
	unsigned char uscnt;
	uscnt=us>>1;        /*12MHzƵ��*/
	while(--uscnt);
}
/******************************************************************/
/*                    ���뺯������                                */
/******************************************************************/
void DelayMs(unsigned char ms)
{
	while(--ms)
	{
		DelayUs(250);
		DelayUs(250);
		DelayUs(250);
		DelayUs(250);
	}
}
/******************************************************************/
/*                   д�������                                 */
/******************************************************************/
void WriteCommand(unsigned char c)
{
	DelayMs(5);//����ǰ������ʱ����֤�ź��ȶ�
	E=0;
	RS=0;
	RW=0;
	_nop_();
	E=1;
	Data=c;
	E=0;
}
/******************************************************************/
/*                   д�����ݺ���                                 */
/******************************************************************/
void WriteData(unsigned char c)
{
	DelayMs(5);  //����ǰ������ʱ����֤�ź��ȶ�
	E=0;
	RS=1;
	RW=0;
	_nop_();
	E=1;
	Data=c;
	E=0;
	RS=0;
}
/******************************************************************/
/*                   д���ֽں���                                 */
/******************************************************************/
void ShowChar(unsigned char pos,unsigned char c)
{
	unsigned char p;
	if(pos>=0x10)
		p=pos+0xb0; //�ǵڶ�������������4λΪ0xc
	else 
		p=pos+0x80; //�ǵ�һ������������4λΪ0x8
	WriteCommand(p);//д����
	WriteData(c);   //д����
}
/******************************************************************/
/*                   д���ַ�������                               */
/******************************************************************/
void ShowString(unsigned char line,char *ptr)
{
	unsigned char l,i;
	l=line<<4;
	for(i=0;i<strlen(ptr);i++)
	{
		ShowChar(l++,*(ptr+i));//ѭ����ʾ16���ַ�
	}
}
/******************************************************************/
/*                   ��ʼ������                                   */
/******************************************************************/
void InitLcd(void)
{
	DelayMs(15);
	WriteCommand(0x38); //display mode
	WriteCommand(0x38); //display mode
	WriteCommand(0x38); //display mode
	WriteCommand(0x06); //��ʾ����ƶ�λ��
	WriteCommand(0x0c); //��ʾ�����������
	WriteCommand(0x01); //��ʾ����
} 