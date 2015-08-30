#include <time.h>
#include "CertifyType.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

UID::UID(char *data)
{
	m_data[8]=0;
	memcpy_s(m_data,9,data,9);
	sprintf_s(m_str,"%02x%02x%02x%02x%02x%02x%02x%02x",data[0]&0xff,data[1]&0xff,data[2]&0xff,data[3]&0xff,data[4]&0xff,data[5]&0xff,data[6]&0xff,data[7]&0xff);
}

CODE::CODE(char *data)
{
	m_data[32]=0;
	memcpy_s(m_data,33,data,33);
	charto16x(m_data,m_str);
}

CODE::CODE()
{
	newcode(m_data);
	charto16x(m_data,m_str);
}

void newcode(char * code)
{
	for (int i=0;i<32;i++)
		code[i]=random();
}

char random()
{
	srand(rand()+clock());
	return (char)(rand()%256);
}

void charto16x(char * in,char * out)
{
	char temp[65];
	memset(temp,0,sizeof(temp));
	for (int i=0;i<8;i++)
		sprintf(temp+i*8,"%02x%02x%02x%02x",in[4*i+0]&0xff,in[4*i+1]&0xff,in[4*i+2]&0xff,in[4*i+3]&0xff);
	sprintf(out,"%64s",temp);
}