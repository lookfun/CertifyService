#include "pack.h"
#include <string.h>

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
