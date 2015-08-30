#include <iostream>
#include <WinSock2.h>
//#pragma comment(lib, "WS2_32")
#include <windows.h>
#include <time.h>
#include <string>
#include "CertifyType.h"
#include "sqlstore.h"
#include "communication.h"
using namespace std;

DWORD WINAPI ComThread(LPVOID lpParameter);//单个线程

void starsocket();
bool receivepack(SOCKET sockcon,Frame &repa,int& command,char * uid,char * cuid);
void search(Frame repa,Frame &sepa);
void insert(Frame repa,Frame &sepa);
void set(Frame repa,Frame &sepa);

int count=1;
_ConnectionPtr pConn;
SOCKET sockSrv;
SqlStore store;

void main()
{
	starsocket();
	
	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);

	while(1)
	{
		SOCKET con=accept(sockSrv,(SOCKADDR*)&addrClient,&len);
		HANDLE hthread1;
		hthread1=CreateThread(NULL,0,ComThread,(LPVOID)con,0,NULL);
		CloseHandle(hthread1);
	}

}

DWORD WINAPI ComThread(LPVOID lpParameter)
{
	SOCKET con=(SOCKET)lpParameter;
	//extern int count;
	Frame repa,sendpa;
	memset(&repa,0,sizeof(Frame));
	memset(&sendpa,0,sizeof(Frame));
	char uid[9];
	char cuid[17];
	memset(uid,0,sizeof(uid));
	memset(cuid,0,sizeof(cuid));

	printf("\n%d:\n",::count++);
	time_t rawtime; 
	struct tm * timeinfo; 
	time ( &rawtime ); 
	timeinfo = localtime( &rawtime); 
	//	errno_t errnotime=localtime_s(timeinfo,&rawtime);
	printf ( "%s", asctime(timeinfo)); 
	VarifySocket conSock((SOCKET)lpParameter);
	int command = conSock.GetCommand(&repa);

	if (command != -1)
	{
		printf("Tag:%s\n",cuid);
		printf("command:0x%02x\n",command);

		switch (command)
		{
		case 204:
			search(repa,sendpa);
			//send(con,(char*)(&sendpa),sizeof(sendpa),0);
			break;
		case 51:
			insert(repa,sendpa);
			//send(con,(char*)(&sendpa),sizeof(sendpa),0);
			break;
		case 0xaa:
			set(repa,sendpa);
			break;
		default:
			printf("Unknown Command!\n");
			break;
		}
		conSock.SendResponse(&sendpa);
	}
	return 0;
}
void starsocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if (err!=0)
	{
		cout<<"Socket建立失败！"<<endl;
	}
	if (LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
	}
	sockSrv=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(1212);
	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	listen(sockSrv,5);
}

bool receivepack(SOCKET sockcon,Frame &repa,int& command,char * uid,char * cuid)
{
	unsigned char com[2];
	memset(com,0,2);

	int comlen=recv(sockcon,(char *)&com,1,0);
	int idlen=recv(sockcon,(char*)&repa,8,0);
	int codelen=recv(sockcon,((char*)&repa)+8,32,0);

	command=int(com[0]);

	for (int i=0;i<8;i++)
	{
		uid[i]=repa.uid[i];
	}
	sprintf(cuid,"%02x%02x%02x%02x%02x%02x%02x%02x",uid[0]&0xff,uid[1]&0xff,uid[2]&0xff,uid[3]&0xff,uid[4]&0xff,uid[5]&0xff,uid[6]&0xff,uid[7]&0xff);

	if (comlen==SOCKET_ERROR || idlen==SOCKET_ERROR || codelen==SOCKET_ERROR)
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
		return false;
	}
	else
	{
		return true;
	}
};
void search(Frame repa,Frame &sepa)
{

// 	char codeontag[65]; //用字符表示16进制code
// 	memset(codeontag,0,sizeof(codeontag));
// 	charto16x(repa.code,codeontag);
	UID uid(repa.uid);
	CODE tag_code(repa.code);
	CODE new_code;

	int ret=store.Update(&uid,&tag_code,&new_code);

	if (ret == 1)
	{
		printf("uid=%s is right\n",uid.m_str);
		sepa.gen(VARIFY_SECCESS, new_code.m_data);
	}
	else
	{
		if (ret == 0)
			printf("uid=%s is not exist\n",uid.m_str);
		else
			printf("uid=%s is not right\n",uid.m_str);
		sepa.gen(VARIFY_FAILED, NULL);
	}
}

void insert(Frame repa,Frame &sepa)
{
	UID uid(repa.uid);
	CODE code;
	store.Insert(&uid,&code);

	printf("Insert new Tag:%s\n",uid.m_str);
	printf("With Code :%s\n",code.m_str);

	sepa.gen(SET_SECCESS,code.m_data);
}
void set(Frame repa,Frame &sepa)
{
	UID uid(repa.uid);
	CODE code(repa.code);
	store.Set(&uid, &code);
	printf("Set Tag:%s\n",uid.m_str);
	printf("With Code :%s\n",code.m_str);
	sepa.gen(SET_SECCESS, NULL);
}
