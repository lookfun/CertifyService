#include "ADO.h"
#include <string>
#include <iostream>
using namespace std;

ADOcon::ADOcon(void) 
{
#ifdef localdb
	m_strDBServer="(local)\\SQLEXPRESS";
	m_strDBName="var";
	m_strUid="sa";
	m_strPwd="kb134";
#endif
#ifdef serverdb
	m_strDBServer="(local)";
	m_strDBName="var";
	m_strUid="sa";
	m_strPwd="5ef053cb";
#endif
}

ADOcon::~ADOcon(void)
{
}

void ADOcon::InitADOcon()
{
	::CoInitialize(NULL);
	try
	{
		m_pConnection.CreateInstance("ADODB.Connection");
		string strConnectTmp;
		strConnectTmp="Provider=SQLOLEDB;Server="+m_strDBServer+";Database="+m_strDBName+";uid="+m_strUid+";pwd="+m_strPwd;
		//strConnectTmp="Provider=SQLOLEDB;Server="+m_strDBServer+";Database="+m_strDBName+";integrated security=True;";
		_bstr_t strConnect=_bstr_t(strConnectTmp.c_str());
		m_pConnection->Open(strConnect,"","",adModeUnknown);
	}
	catch (_com_error e)
	{
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;
	}
}

void ADOcon::InitADOcon(string strDBServer, string strUid, string strPwd, string strDBName)
{
	m_strDBServer=strDBServer;
	m_strDBName=strDBName;
	m_strUid=strUid;
	m_strPwd=strPwd;

	InitADOcon();

}

bool ADOcon::isClosed()
{
	return (m_pConnection->State==adStateClosed);
}

_RecordsetPtr &ADOcon::GetRecordSet(_bstr_t bstrSQL)
{
	try
	{ 
		if (m_pConnection==NULL || m_pConnection->State==adStateClosed)
			InitADOcon();
		if (m_pRecordeset!=NULL && m_pRecordeset->State!=adStateClosed)
		{
			m_pRecordeset->Close();
			m_pRecordeset=NULL;
		}

		m_pRecordeset.CreateInstance(__uuidof(Recordset));
		m_pRecordeset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch (_com_error e)
	{
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;
	}
	return m_pRecordeset;
}

bool ADOcon::ExcuteSQL(_bstr_t bstrSQL)
{
	try
	{
		if (m_pConnection==NULL || m_pConnection->State==adStateClosed)
		{
			InitADOcon();
		}
		m_pConnection->Execute(bstrSQL,NULL,adCmdText);
		return true;
	}
	catch (_com_error e)
	{
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;
		return false;
	}
}