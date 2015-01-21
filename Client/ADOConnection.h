
#if !defined(AFX_ADOCONN_H__INCLUDED_)
#define AFX_ADOCONN_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once
class CADOConnection
{
public:
	_RecordsetPtr m_pRecordset;//记录集指针
	_ConnectionPtr m_pConnection;//数据库连接指针
	_CommandPtr	m_pCommand;//命令指针

public:
	CADOConnection(void);
	CADOConnection(_bstr_t dataSource,_bstr_t userID,_bstr_t serverPassword,_bstr_t catalog);
	virtual ~CADOConnection(void);
	BOOL OnInitAdo();
	_RecordsetPtr& GetRecordset(_bstr_t bstrSQL);
	BOOL ExecuteSQL(_bstr_t bstrSQL);
	void ExitConnect();
	void ExecuteProc(_bstr_t proc);

private:
	_bstr_t m_dataSource;
	_bstr_t m_userID;
	_bstr_t m_serverPassword;
	_bstr_t m_catalog;
};

#endif