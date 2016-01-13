#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CertifyType.h"

char random()
{
	srand(rand()+clock());
	return (char)(rand()%256);
}

void newcode(char * code)
{
	for (int i=0;i<32;i++)
		code[i]=random();
}

void charto16x(char * in,char * out)
{
	char temp[65];
	memset(temp,0,sizeof(temp));
	for (int i=0;i<8;i++)
		sprintf_s(temp+i*8,9,"%02x%02x%02x%02x",in[4*i+0]&0xff,in[4*i+1]&0xff,in[4*i+2]&0xff,in[4*i+3]&0xff);
	sprintf_s(out,65,"%64s",temp);
}

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

Frame::Frame(state st, char* cd)
{
	if (st == VARIFY_SECCESS)
	{
		memset(uid,0xff,4);
		memset(&uid[4],0x00,4);
		memcpy_s(code,32,cd,32);

	}
	else if (st == SET_SECCESS)
	{
		memset(uid,0xff,2);
		memset(&uid[2],0x00,2);
		memset(&uid[4],0xff,2);
		memset(&uid[6],0x00,2);
		if (cd != NULL)
			memcpy_s(code,32,cd,32);
	}
	else 
	{
		memset(uid,0x00,4);
		memset(&uid[4],0xff,4);
	}
}

int Frame::gen(state st, char* cd)
{
	if (st == VARIFY_SECCESS)
	{
		memset(uid,0xff,4);
		memset(&uid[4],0x00,4);
		memcpy_s(code,32,cd,32);

	}
	else if (st == SET_SECCESS)
	{
		memset(uid,0xff,2);
		memset(&uid[2],0x00,2);
		memset(&uid[4],0xff,2);
		memset(&uid[6],0x00,2);
		if (cd != NULL)
			memcpy_s(code,32,cd,32);
	}
	else 
	{
		memset(uid,0x00,4);
		memset(&uid[4],0xff,4);
	}
	return 0;
}

int Frame::gen(char* id, char* cd)
{
	if (id != NULL)
		memcpy_s(uid, 8, id, 8);
	if (cd != NULL)
		memcpy_s(code,32,cd,32);
	return 0;
}


