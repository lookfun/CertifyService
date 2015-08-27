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
	int Update(UID *uid,CODE *old_code,CODE *new_code);
protected:
	_RecordsetPtr GetRecordInDB(UID *uid);
};

_RecordsetPtr SqlStore::GetRecordInDB(UID *uid)
{
	string strsearch;
	strsearch="select code from dbo.var where uid='";
	strsearch.append(uid->m_str);
	strsearch.append("'");

	_RecordsetPtr pRst=ado_Inst.GetRecordSet(strsearch.data());

	return pRst;
}

CODE SqlStore::GetCode(UID *uid)
{
	CODE code;

	_RecordsetPtr pRst=GetRecordInDB(uid);

	if (pRst->adoEOF)
	{
		printf("uid=%s is not exist!\n",uid->m_data);
	}
	else
	{
		_variant_t db_code;
		db_code=pRst->GetCollect("code");
		code=(LPSTR)(LPCSTR)_bstr_t(db_code);
	}
	try
	{
		pRst->Close();
	}
	catch(_com_error e)
	{
		cout<<"pRst Close Error!"<<endl;
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;
	}
	return code;
}

int SqlStore::Update(UID* uid,CODE *old_code,CODE *new_code)
{
	string code;
	string Old_Code(old_code->m_str);
	int ret=0;
	_RecordsetPtr pRst=GetRecordInDB(uid);

	if (pRst->adoEOF)
	{
		printf("uid=%s is not exist!\n",uid->m_data);
		ret=0;
	}
	else
	{
		_variant_t db_code;
		db_code=pRst->GetCollect("code");
		code=(LPSTR)(LPCSTR)_bstr_t(db_code);
		if (code.compare(Old_Code)==0)
		{
			new ((CODE*)new_code) CODE();
			_variant_t newcode;
			newcode.SetString(new_code->m_str);
			try
			{
				pRst->PutCollect("code",newcode);
				pRst->Update();
				ret=1;
			}
			catch(_com_error e)
			{
				cout<<e.Description()<<endl;
			}
		}
		else ret=-1;
	}
	try
	{
		pRst->Close();
	}
	catch(_com_error e)
	{
		cout<<"pRst Close Error!"<<endl;
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;
	}
	return ret;
}