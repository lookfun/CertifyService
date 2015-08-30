#include "communication.h"
#include <WinSock2.h>
#pragma comment(lib, "WS2_32")
#include <stdio.h>
#include "pack.h"

int VarifySocket::GetCommand(Frame *repa)
{
	int cmd;
	int recivelen=0;
	char cmdbuf[2];
	char rebuf[100];
	memset(rebuf,0,2);

	while (recivelen != 1 && recivelen!= SOCKET_ERROR)
	{
		recivelen = recv(con, cmdbuf, 1, 0);
	}
	if (recivelen == 1)
		recivelen = 0;
	while (recivelen < 40 && recivelen!= SOCKET_ERROR)
	{
		recivelen=recv(con,rebuf,40,0);
	}

	if (recivelen == SOCKET_ERROR)
	{
		int id = WSAGetLastError();
		switch (id)
		{
		case WSANOTINITIALISED: printf("not initialized\n"); break;
		case WSASYSNOTREADY: printf("sub sys not ready\n"); break;
		case WSAHOST_NOT_FOUND: printf("name server not found\n");  break;
		case WSATRY_AGAIN:  printf("server fail\n");  break;
		case WSANO_RECOVERY:  printf("no recovery\n");   break;
		case WSAEINPROGRESS:  printf("socket blocked by other prog\n"); break;
		case WSANO_DATA:   printf("no data record\n");  break;
		case WSAEINTR:   printf("blocking call canceled\n");  break;
		case WSAEPROCLIM: printf("limit exceeded\n");
		case WSAEFAULT:  printf("lpWSAData in startup not valid\n");
		default: printf("unknown error id = %d\n",id); break;
		};
		printf("receive error.\n");
		return -1;
	}
	else
	{
		cmd=0xff & cmdbuf[0];
		repa->gen(rebuf,rebuf+8);
		return cmd;
	}
}

int VarifySocket::SendResponse(Frame *sepa)
{
	if (sepa == NULL)
		return -1;
	char buf[100]={0};
	memcpy_s(buf,8,sepa->uid,8);
	memcpy_s(buf+8,32,sepa->code,32);
	return send(con,buf,40,0);
}
