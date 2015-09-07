#include <iostream>
#include <WinSock2.h>
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

typedef void (*proc)(Frame repa,Frame &sepa);

typedef struct CMD_HANDLER
{
	int cmd;
	proc handler;
}CMD_HANDLER;

CMD_HANDLER CMD_LIST[]=
{
	{0xcc,search},
	{0x33,insert},
	{0xaa,set}
};

int count=1;
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
	Frame repa,sendpa;
	memset(&repa,0,sizeof(Frame));
	memset(&sendpa,0,sizeof(Frame));

	printf("\n%d:\n",::count++);
	time_t rawtime; 
	struct tm * timeinfo; 
	time ( &rawtime ); 
	timeinfo = localtime( &rawtime); 
	printf ( "%s", asctime(timeinfo)); 
	VarifySocket conSock((SOCKET)lpParameter);
	int command = conSock.GetCommand(&repa);

	if (command != -1)
	{
		printf("Tag:%s\n",UID(repa.uid).m_str);
		printf("command:0x%02x\n",command);
		int i;
		bool isCMD = false;
		for (i=0;i<sizeof(CMD_LIST)/sizeof(CMD_HANDLER);i++)
		{
			if (command==CMD_LIST[i].cmd)
			{
				isCMD = true;
				CMD_LIST[i].handler(repa, sendpa);
				break;
			}
		}
		if (isCMD)
			conSock.SendResponse(&sendpa);
		else
			printf("Unknown Command!\n");
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

void search(Frame repa,Frame &sepa)
{
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
