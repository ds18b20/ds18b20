#ifndef __DS18B20_H__
#define __DS18B20_H__

#define uchar	unsigned char
#define uint	unsigned int

sbit DQ=P1^3;

unsigned char code ds18b20_num1[8]={0x28,0x56,0xe6,0xf5,0x02,0x00,0x00,0x7b};
unsigned char code ds18b20_num2[8]={0x28,0x51,0xd7,0xf5,0x02,0x00,0x00,0xb7};
unsigned char code ds18b20_num3[8]={0x28,0xc1,0xb1,0xf5,0x02,0x00,0x00,0x8b};
unsigned char code ds18b20_num4[8]={0x28,0x4f,0xed,0xf5,0x02,0x00,0x00,0xc6};
unsigned char code ds18b20_num5[8]={0x28,0x01,0xb3,0xf5,0x02,0x00,0x00,0x97};
unsigned char code ds18b20_num6[8]={0x28,0x93,0xa7,0xf5,0x02,0x00,0x00,0x2b};
unsigned char code ds18b20_num7[8]={0x28,0x60,0xb2,0xf5,0x02,0x00,0x00,0xae};
unsigned char code ds18b20_num8[8]={0x28,0xd9,0xd3,0xf5,0x02,0x00,0x00,0xe3};

unsigned char ROM_NUM[8];
//uchar sn[8]; //4个器件的序列号，先读出单个序列号后填上才可以读取温度

/******************************************************************/
/*                   函数声明                                     */
/******************************************************************/
unsigned int ReadTemperature(void);
void Init_DS18B20(void);
unsigned char ReadOneChar(void);
void WriteOneChar(unsigned char dat);
void delay(unsigned int i);
void UART_send_byte(unsigned char i);
void UART_send_string(unsigned char  *pstr);
void mum_select(unsigned char m);

#endif
