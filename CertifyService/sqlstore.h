#include "CertifyType.h"

#include "ADO.h"

class SqlStore
{
public:
	CODE GetCode(UID *uid);
	int Update(UID *uid, CODE *old_code, CODE *new_code);
	int Insert(UID *uid, CODE *code);
	int Set(UID *uid, CODE *code);
protected:
	ADOcon ado_Inst;
	void GetRecordInDB(UID *uid,_RecordsetPtr& pRst);
};
