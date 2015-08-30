#include <WinSock2.h>
#include "CertifyType.h"

class VarifySocket
{
public:
	SOCKET con;
	char buf[100];
	VarifySocket(SOCKET sock):con(sock){};
	int GetCommand(Frame *repa);
	int SendResponse(Frame *sepa);
};
