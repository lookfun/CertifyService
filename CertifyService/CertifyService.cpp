#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <time.h>
#include <string>
#include "CertifyType.h"
#include "sqlstore.h"
#include "communication.h"
#include <thread>
#include <vector>
#include <mutex>
using namespace std;

DWORD WINAPI ComThread(LPVOID lpParameter);//单个线程

typedef void (*proc)(Frame repa,Frame &sepa);

int startsocket();
bool receivepack(SOCKET sockcon,Frame &repa,int& command,char * uid,char * cuid);
void search(Frame repa,Frame &sepa);
void insert(Frame repa,Frame &sepa);
void set(Frame repa,Frame &sepa);
bool CheckCmd(int cmd,proc* handler);


typedef struct CMD_HANDLER
{
	int cmd;
	proc handler;
}CMD_HANDLER;

CMD_HANDLER CMD_LIST[]=
{
	{SEARCH,search},
	{INSERT,insert},
	{SET,set}
};

int count=1;
SOCKET sockSrv;
SqlStore store;
std::mutex oplock;

void main()
{
	int ret;
	while(ret=startsocket())
	{
		static int count=0;count++;
		ret = GetLastError();
		ret = closesocket(sockSrv);
		ret = WSACleanup();
		if (count>3)exit(1);
	};

	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);
	//std::vector<std::thread*> threadList ;
	while(1)
	{
		SOCKET con=accept(sockSrv,(SOCKADDR*)&addrClient,&len);
		if (con!=SOCKET_ERROR)
		{
			//threadList.push_back(new std::thread(ComThread,con));
		HANDLE hthread1;
		hthread1=CreateThread(NULL,0,ComThread,(LPVOID)con,0,NULL);
		CloseHandle(hthread1);
		}
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
	VarifySocketServer conSock((SOCKET)lpParameter);

	int command = conSock.GetCommand();
	proc handler;
	if (!CheckCmd(command,&handler))
		return -1;

	int palen = conSock.RecvFrame(&repa);
	
	oplock.lock();

	printf("Tag:%s\n",UID(repa.uid).m_str);
	printf("command:0x%02x\n",command);
	handler(repa, sendpa);

	oplock.unlock();

	conSock.SendFrame(&sendpa);
	
	closesocket(conSock.con);
	return 0;
}

int startsocket()
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
	//sockSrv=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	sockSrv=WSASocket(AF_INET, SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(1212);
	::bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	return listen(sockSrv,3);
}

void search(Frame repa,Frame &sepa)
{
	UID uid(repa.uid);
	CODE tag_code(repa.code);
	CODE new_code;
	int ret=store.Update(&uid,&tag_code,&new_code);

	if (ret == 1)
	{
		printf("uid = %s is right\n",uid.m_str);
		printf("Tag code: %s\nNew code: %s\n",tag_code.m_str,new_code.m_str);
		sepa.gen(VARIFY_SECCESS, new_code.m_data);
	}
	else
	{
		if (ret == 0)
			printf("uid=%s is not exist\n",uid.m_str);
		else
			printf("uid=%s is not right\n",uid.m_str);		
		sepa.gen(VARIFY_FAILED, NULL);
		printf("Tag code: %s\nRet code: %s\n",tag_code.m_str,CODE(sepa.code).m_str);
	}
}

void insert(Frame repa,Frame &sepa)
{
	UID uid(repa.uid);
	CODE code;
	store.Insert(&uid,&code);

	printf("Insert new Tag: %s\n",uid.m_str);
	printf("New Code: %s\n",code.m_str);

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

bool CheckCmd(int cmd,proc* handler)
{
	int CmdCount = sizeof(CMD_LIST)/sizeof(CMD_HANDLER);
	for (int i =0;i<CmdCount;i++)
	{
		if (cmd == CMD_LIST[i].cmd)
		{
			*handler = CMD_LIST[i].handler;
			return true;
		}
	}
	
	printf("Unknown Command!\n");
	return false;
}