#pragma once
#import "msado60_Backcompat_i386.tlb" no_namespace rename("EOF","adoEOF") rename ("BOF","adoBOF")
#include <string>
using namespace std;

#define localdb
#ifndef localdb
#define serverdb
#endif

class ADOcon
{
public:
	ADOcon(void);
	~ADOcon(void);
	void InitADOcon(string strDBserver,	string strUid,	string strPwd,	string strDBName);
	void InitADOcon();
	_RecordsetPtr &GetRecordSet(_bstr_t bstrSQL);
	bool ExcuteSQL(_bstr_t bstrSQL);
	bool isClosed();
private:
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordeset;


	string m_strDBName;
	string m_strPwd;
	string m_strUid;
	string m_strDBServer;
	string m_connect_str;

};
