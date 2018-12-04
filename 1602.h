#ifndef	_1602_h_
#define	_1602_h_

#include<reg52.h> //����ͷ�ļ���һ���������Ҫ�Ķ���ͷ�ļ��������⹦�ܼĴ����Ķ���
#include<intrins.h>

#define Data  P0//���ݶ˿�

sbit RS = P2^4; //Pin4
sbit RW = P2^5; //Pin5
sbit E  = P2^6; //Pin6

void WriteCommand(unsigned char c);
void WriteData(unsigned char c);
void ShowChar(unsigned char pos,unsigned char c);
void ShowString (unsigned char line,char *ptr);
void InitLcd(void);

#endif