#include "stdafx.h"
#include "ADOConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CADOConnection::CADOConnection(void)
{
	m_dataSource = _T("127.0.0.1");
	m_userID = _T("sa");
	m_serverPassword = _T("000000");
	m_catalog = _T("caxastat");
}

CADOConnection::CADOConnection(_bstr_t dataSource,_bstr_t userID,_bstr_t serverPassword,_bstr_t catalog)
{
	 m_dataSource = dataSource;
	 m_userID = userID;
	 m_serverPassword = serverPassword;
	 m_catalog = catalog;
}

CADOConnection::~CADOConnection(void)
{

}

BOOL CADOConnection::OnInitAdo()
{
	BOOL isConnected = FALSE;
	::CoInitialize(NULL);
	try
	{
		m_pConnection.CreateInstance("ADODB.Connection");
		_bstr_t strConnect;
		strConnect = _T("Provider=SQLOLEDB.1;Persist Security Info=False");
		strConnect = strConnect +_T(";Data Source=")+ m_dataSource +_T(";Initial Catalog=")+m_catalog +_T(";User ID=") + m_userID +_T(";Password=") + m_serverPassword;
		//strConnect="Provider=SQLOLEDB.1;Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=AdventureWorks2008;Data Source=HUANGBOYUAN-PC";
		//strConnect="File Name=ADO.udl";
		m_pConnection->Open(strConnect,"","",adModeUnknown);
		isConnected = TRUE;
	}
	catch(_com_error e)
	{
		//AfxMessageBox(_T("打开数据库失败"));
		//AfxMessageBox(e.ErrorMessage());
		::CoUninitialize();
		return isConnected;
	}
	return isConnected;
}
_RecordsetPtr& CADOConnection::GetRecordset(_bstr_t bstrSQL)
{
	try
	{
		if(m_pConnection==NULL)
			OnInitAdo();
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);

	}
	catch(_com_error e)
	{
		AfxMessageBox(_T("打开记录集失败"));
		AfxMessageBox(e.ErrorMessage());
	}
	return m_pRecordset;
}
BOOL CADOConnection::ExecuteSQL(_bstr_t bstrSQL)
{
	try
	{
		if(m_pConnection==NULL)
			OnInitAdo();
		m_pConnection->Execute(bstrSQL,NULL,adCmdText);
		return true;
	}
	catch(_com_error e)
	{
		CString str;
		str.Format(_T("不能打开记录集!%s"),e.ErrorMessage());
		AfxMessageBox(str);
		return false;
	}
}
void CADOConnection::ExitConnect()
{
	if(m_pRecordset!=NULL)
		m_pRecordset->Close();
	m_pConnection->Close();
	::CoUninitialize();
}