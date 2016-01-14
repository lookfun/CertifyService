#include <WINSOCK2.H>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")
#include "..\CertifyService\communication.h"
#include "..\CertifyService\CertifyType.h"

void TestSock(SOCKADDR_IN socketAddr,char cmd,Frame *sepa,Frame *repa);
void TestOnce(SOCKADDR_IN socketAddr,char cmd,Frame *sepa,Frame *repa);

int main(int argc,char **argv)
{
    int err;
    WORD versionRequired;
    WSADATA wsaData;
    versionRequired=MAKEWORD(1,1);
    err=WSAStartup(versionRequired,&wsaData);
    if (err)    
	{
        printf("ERROR:客户端的嵌套字打开失败!\n");
        return 1;//结束
    }

    SOCKADDR_IN clientsock_in;
    //clientsock_in.sin_addr.S_un.S_addr=inet_addr("114.215.106.37");
    clientsock_in.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
    clientsock_in.sin_family=AF_INET;
    clientsock_in.sin_port=htons(1212);

	char id[8]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	Frame sepa ;
	Frame repa;
	sepa.gen(id,NULL);

	sepa.gen(NULL,repa.code);	
	TestSock(clientsock_in,SEARCH,&sepa,&repa);

	TestOnce(clientsock_in,INSERT,&sepa,&repa);
	while(true)
	{
		sepa.gen(NULL,repa.code);
		TestOnce(clientsock_in,SEARCH,&sepa,&repa);
		//Sleep(100);
    }

	WSACleanup();
    return 0;
}

void TestOnce(SOCKADDR_IN socketAddr,char cmd,Frame *sepa,Frame *repa)
{
	SOCKET skt = socket(AF_INET,SOCK_STREAM,0);
	connect(skt,(SOCKADDR*)&socketAddr,sizeof(SOCKADDR));//开始连接
	VarifySocketClient sock(skt);

	sock.SendCommand(cmd);
	sock.SendFrame(sepa);
	sock.RecvFrame(repa);

	closesocket(sock.con);
}

void TestSock(SOCKADDR_IN socketAddr,char cmd,Frame *sepa,Frame *repa)
{
	SOCKET skt = socket(AF_INET,SOCK_STREAM,0);
	connect(skt,(SOCKADDR*)&socketAddr,sizeof(SOCKADDR));//开始连接
	VarifySocketClient sock(skt);

	sock.SendCommand(cmd);
	sock.SendFrame(sepa,10);
	Sleep(1000);
	sock.SendFrame(sepa,10);
	Sleep(1000);
	sock.SendFrame(sepa,10);
	Sleep(1000);
	sock.SendFrame(sepa,10);
	sock.RecvFrame(repa);
	closesocket(sock.con);
}