#ifndef	_1602_h_
#define	_1602_h_

#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include<intrins.h>

#define Data  P0//数据端口

sbit RS = P2^4; //Pin4
sbit RW = P2^5; //Pin5
sbit E  = P2^6; //Pin6

void WriteCommand(unsigned char c);
void WriteData(unsigned char c);
void ShowChar(unsigned char pos,unsigned char c);
void ShowString (unsigned char line,char *ptr);
void InitLcd(void);

#endif