#include "ADO.h"
#include "Config.h"
#include <string>
#include <iostream>
#include <mutex>
//#import "msado60_Backcompat_i386.tlb" no_namespace rename("EOF","adoEOF") rename ("BOF","adoBOF")

using namespace std;

ADOcon::ADOcon(void) 
{
	adolock.lock();
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
	adolock.unlock();

}

ADOcon::~ADOcon(void)
{
}

void ADOcon::InitADOcon()
{
	::CoInitialize(NULL);
	try
	{
		adolock.lock();
		m_pConnection.CreateInstance("ADODB.Connection");

		//strConnectTmp="Provider=SQLOLEDB;Server="+m_strDBServer+";Database="+m_strDBName+";uid="+m_strUid+";pwd="+m_strPwd;
		//strConnectTmp="Provider=SQLOLEDB;Server="+m_strDBServer+";Database="+m_strDBName+";integrated security=True;";
		_bstr_t strConnect=_bstr_t(m_connect_str.c_str());
		m_pConnection->Open(strConnect,"","",adModeUnknown);
		//m_pRecordeset = new _RecordsetPtr();
		adolock.unlock();
	}
	catch (_com_error e)
	{
		adolock.unlock();
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

void ADOcon::GetRecordSet(_bstr_t bstrSQL,_RecordsetPtr& m_pRecordeset)
{
	try
	{ 
		if (m_pConnection==NULL || m_pConnection->State==adStateClosed)
			InitADOcon();
		
		adolock.lock();
		(m_pRecordeset).CreateInstance(__uuidof(Recordset));
		(m_pRecordeset)->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
		adolock.unlock();
	}
	catch (_com_error e)
	{
		adolock.unlock();
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;		
	}
	//return m_pRecordeset;
}

bool ADOcon::ExcuteSQL(_bstr_t bstrSQL)
{
	try
	{
		if (m_pConnection==NULL || m_pConnection->State==adStateClosed)
		{
			InitADOcon();
		}
		adolock.lock();
		m_pConnection->Execute(bstrSQL,NULL,adCmdText);
		adolock.unlock();
		return true;
	}
	catch (_com_error e)
	{
		adolock.unlock();
		cout<<e.ErrorMessage()<<endl<<e.Description()<<endl;
		return false;
	}
}