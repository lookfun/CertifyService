#include <WinSock2.h>
#pragma comment(lib, "WS2_32")
#include <stdio.h>
#include "communication.h"


int VarifySocket::RecvFrame(Frame *repa)
{
	int recivelen=0;
	char rebuf[100];
	memset(rebuf,0,2);
	int ret = 0;
	while (recivelen < 40 && ret!= SOCKET_ERROR)
	{
		ret = recv(con,rebuf,40,0);
		recivelen += ret;
	}

	if (recivelen == SOCKET_ERROR)
	{
		DealWithErr();
		return -1;
	}

	repa->gen(rebuf,rebuf+8);
	return recivelen;
}
int VarifySocket::SendFrame(Frame *sepa,int Framelen)
{
	if (sepa == NULL)
		return -1;
	char buf[100]={0};
	memcpy_s(buf,8,sepa->uid,8);
	memcpy_s(buf+8,32,sepa->code,32);
	return send(con,buf,Framelen,0);
}

int VarifySocket::SetTimeOut(int timeout = 3000)
{
	::setsockopt(con,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(timeout));
	return ::setsockopt(con,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout));
}

void VarifySocket::DealWithErr()
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
}

int VarifySocketServer::GetCommand()
{
	int cmd;
	int recivelen=0;
	char cmdbuf[2];

	while (recivelen != 1 && recivelen!= SOCKET_ERROR)
	{
		recivelen = recv(con, cmdbuf, 1, 0);
	}

	if (recivelen == SOCKET_ERROR)
	{
		DealWithErr();
		return -1;
	}
	else
	{
		cmd=0xff & cmdbuf[0];
		return cmd;
	}
}

int VarifySocketClient::SendCommand(char cmd)
{	
	return send(con,&cmd,1,0);
}