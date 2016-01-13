#include <WinSock2.h>
#include "CertifyType.h"

class VarifySocket
{
public:
	SOCKET con;
	char buf[100];
	VarifySocket(SOCKET sock):con(sock){};
	int SendFrame(Frame *sepa,int frameLen = 40);
	int RecvFrame(Frame *repa);
	int SetTimeOut(int timeout);
protected:
	void DealWithErr();
};

class VarifySocketServer:public VarifySocket
{	
public:	
	VarifySocketServer(SOCKET sock):VarifySocket(sock){};
	int GetCommand();
};
class VarifySocketClient:public VarifySocket
{	
public:
	VarifySocketClient(SOCKET sock):VarifySocket(sock){};
	int SendCommand(char cmd);
};