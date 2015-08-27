#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32")
#include "pack.h"
#include <windows.h>
#include <time.h>
#include <string>
#include "ADO.h"
#include "CertifyType.h"
#include "sqlstore.h"
using namespace std;

#import "msado60_Backcompat_i386.tlb" no_namespace rename("EOF","adoEOF") rename ("BOF","adoBOF")

DWORD WINAPI ComThread(LPVOID lpParameter);//单个线程

void starsocket();
bool receivepack(SOCKET sockcon,packet &repa,int& command,char * uid,char * cuid);
void search(packet repa,packet &sepa,char * uid,char * cuid);
void insert(packet repa,packet &sepa,char * uid,char * cuid);
void set(packet repa,packet &sepa,char * uid,char * cuid);

int count=1;
_ConnectionPtr pConn;
SOCKET sockSrv;
SqlStore store;
ADOcon adocon;

void main()
{
	starsocket();

	adocon.InitADOcon();

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
	packet repa,sendpa;
	memset(&repa,0,sizeof(packet));
	memset(&sendpa,0,sizeof(packet));
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

	int command;

	if (receivepack(con,repa,command,uid,cuid))
	{
		printf("Tag:%s\n",cuid);
		printf("command:0x%02x\n",command);

		adocon.InitADOcon();
		switch (command)
		{
		case 204:
			search(repa,sendpa,uid,cuid);
			send(con,(char*)(&sendpa),sizeof(sendpa),0);
			break;
		case 51:
			insert(repa,sendpa,uid,cuid);
			send(con,(char*)(&sendpa),sizeof(sendpa),0);
			break;
		case 0xaa:
			set(repa,sendpa,uid,cuid);
			break;
		default:
			printf("Unknown Command!\n");
			break;
		}
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

bool receivepack(SOCKET sockcon,packet &repa,int& command,char * uid,char * cuid)
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
void search(packet repa,packet &sepa,char * cuid,char * suid)
{

// 	char codeontag[65]; //用字符表示16进制code
// 	memset(codeontag,0,sizeof(codeontag));
// 	charto16x(repa.code,codeontag);
	UID uid(cuid);
	CODE tag_code(repa.code);
	CODE new_code;

	int ret=store.Update(&uid,&tag_code,&new_code);

	if (ret)
	{
		printf("uid=%s is right\n",suid);
		memset(&sepa.uid,0xff,4);
		memset((void *)(sepa.uid+4),0x00,4);
	}
	else
	{
		if (ret==0)
			printf("uid=%s is not exist\n",suid);
		else
			printf("uid=%s is not right\n",suid);
		memset(&sepa.uid,0x00,4);
		memset((void *)(sepa.uid+4),0xff,4);
	}
}

void insert(packet repa,packet &sepa,char * uid,char * cuid)
{
	char ccode[65];
	memset(ccode,0,65);
	newcode(sepa.code);
	charto16x(sepa.code,ccode);

	try
	{
		char sqlcmd[200];
		memset(sqlcmd,0,sizeof(sqlcmd));
		sprintf(sqlcmd,"select code from dbo.var where uid='%s'",cuid);

		_RecordsetPtr pRst=adocon.GetRecordSet(sqlcmd);

		if (!pRst->adoEOF)
		{
			_variant_t newcode;
			newcode.SetString(ccode);
			pRst->PutCollect("code",newcode);
			pRst->Update();
		} 
		else
		{
			char excuteString[200];
			memset(excuteString,0,200);
			sprintf(excuteString,"INSERT INTO dbo.var(uid,code) VALUES('%s','%s')",cuid,ccode);

			adocon.ExcuteSQL(excuteString);
		}
		pRst->Close();
	}
	catch (_com_error e)
	{
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;
	}

	printf("Insert new Tag:%s\n",cuid);
	printf("With Code :%s\n",ccode);

	memset((void *)sepa.uid,0xff,2);
	memset((void *)(sepa.uid+2),0x00,2);
	memset((void *)(sepa.uid+4),0xff,2);
	memset((void *)(sepa.uid+6),0x00,2);
}
void set(packet repa,packet &sepa,char * uid,char * cuid)
{
	char ccode[65];
	memset(ccode,0,65);
	charto16x(repa.code,ccode);
	try
	{
		char sqlcmd[200];
		memset(sqlcmd,0,sizeof(sqlcmd));
		sprintf(sqlcmd,"select code from dbo.var where uid='%s'",cuid);
		_RecordsetPtr pRst=adocon.GetRecordSet(sqlcmd);
		if (!pRst->adoEOF)
		{
			_variant_t newcode;
			newcode.SetString(ccode);
			pRst->PutCollect("code",newcode);
			pRst->Update();
		} 
		else
		{
			char excuteString[200];
			memset(excuteString,0,200);
			sprintf(excuteString,"INSERT INTO dbo.var(uid,code) VALUES('%s','%s')",cuid,ccode);

			adocon.ExcuteSQL(excuteString);
		}
		pRst->Close();
	}
	catch (_com_error e)
	{
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;
	}
	printf("Set Tag:%s\n",cuid);
	printf("With Code :%s\n",ccode);
	memset((void *)sepa.uid,0xff,2);
	memset((void *)(sepa.uid+2),0x00,2);
	memset((void *)(sepa.uid+4),0xff,2);
	memset((void *)(sepa.uid+6),0x00,2);
}
