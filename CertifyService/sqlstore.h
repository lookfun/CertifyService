#include "CertifyType.h"

#include "ADO.h"
// class IStore
// {
// public:
// 	virtual CODE GetCode(UID *uid);
// 	virtual int Update(UID,CODE *old_code,CODE *new_code);
// };

class SqlStore// :public IStore
{
public:
	CODE GetCode(UID *uid);
	int Update(UID *uid, CODE *old_code, CODE *new_code);
	int Insert(UID *uid, CODE *code);
	int Set(UID *uid, CODE *code);
protected:
	ADOcon ado_Inst;
	_RecordsetPtr GetRecordInDB(UID *uid);
};
