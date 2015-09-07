#include "ADO.h"
#include "Config.h"
#include <string>
#include <iostream>
using namespace std;

ADOcon::ADOcon(void) 
{
	const char config_file[]="database.txt";
	Config config_settings(config_file);

	if (config_settings.KeyExists("ConnectString"))
	{
		m_connect_str=config_settings.Read("ConnectString",m_connect_str);
	}
	else
	{
		m_strDBServer=config_settings.Read("Server",m_strDBServer);
		m_strDBName=config_settings.Read("DBname",m_strDBName);
		m_strUid=config_settings.Read("username",m_strUid);
		m_strPwd=config_settings.Read("password",m_strPwd);
		m_connect_str="Provider=SQLOLEDB;Server="+m_strDBServer+";Database="+m_strDBName+";uid="+m_strUid+";pwd="+m_strPwd;
	}

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

		//strConnectTmp="Provider=SQLOLEDB;Server="+m_strDBServer+";Database="+m_strDBName+";uid="+m_strUid+";pwd="+m_strPwd;
		//strConnectTmp="Provider=SQLOLEDB;Server="+m_strDBServer+";Database="+m_strDBName+";integrated security=True;";
		_bstr_t strConnect=_bstr_t(m_connect_str.c_str());
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