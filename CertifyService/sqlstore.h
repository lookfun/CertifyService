#include "ADO.h"
#include "CertifyType.h"

// class IStore
// {
// public:
// 	virtual CODE GetCode(UID *uid)=0;
// 	virtual int Update(UID,CODE *old_code,CODE *new_code)=0;
// };

class SqlStore// :public IStore
{
public:
	ADOcon ado_Inst;
	CODE GetCode(UID *uid);
	int Update(UID *uid, CODE *old_code, CODE *new_code);
	int Insert(UID *uid, CODE *code);
	int Set(UID *uid, CODE *code);
protected:
	_RecordsetPtr GetRecordInDB(UID *uid);
};
