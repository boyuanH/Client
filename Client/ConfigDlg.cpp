// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"
#include "ADOConnection.h"


// CConfigDlg 对话框

using namespace MSXML2;

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigDlg::IDD, pParent)
{
	pXMLDoc = NULL;
	pXMLErr = NULL;
}

CConfigDlg::~CConfigDlg()
{
	CoUninitialize();
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

 	CoInitialize(NULL);
 	HRESULT hr;
 
 
 	hr = CoCreateInstance(__uuidof(DOMDocument40),
 		NULL,
 		CLSCTX_INPROC_SERVER,
 		__uuidof(MSXML2::IXMLDOMDocument),
 		(void**)&pXMLDoc);
 
 	if (FAILED(hr)) 
 	{
 		printf("Failed to CoCreate an instance of an XML DOM\n");
 	}
 
 	BSTR bstr = NULL;
 	VARIANT_BOOL status;
 	VARIANT var;
 
 	VariantInit(&var);
 	V_BSTR(&var) = SysAllocString(L"conf/database.xml");
 	V_VT(&var) = VT_BSTR;
 	pXMLDoc->load(var, &status);
	
 	if (VARIANT_TRUE != status)
 	{
 		pXMLDoc->get_parseError(&pXMLErr);
 		pXMLErr->get_reason(&bstr);
 		if (&var) VariantClear(&var);   
 		if (pXMLDoc) pXMLDoc->Release();   
 		MessageBoxW(_T("Failed to load DOM from xml."));
 	}

	VariantClear(&var);

	MSXML2::IXMLDOMNode* pRoot = NULL;
	MSXML2::IXMLDOMNodeList* pNodeDataInfos = NULL;	
	
	if (bstr)
	{
		SysFreeString(bstr);
	}

	hr = pXMLDoc->selectSingleNode(_T("DataInfo"),&pRoot);
	hr = pRoot->selectNodes(_T("Info"),&pNodeDataInfos);

	MSXML2::IXMLDOMNode* pNodeDataInfo = NULL;
	MSXML2::IXMLDOMNode* pNodeInfo = NULL;

	VARIANT varAttri;
	varAttri.vt = VT_BSTR;
	VariantInit(&varAttri);

	while ((pNodeDataInfos->nextNode(&pNodeDataInfo)) == S_OK)
	{
		MSXML2::IXMLDOMElement *pElement = NULL;
		pNodeDataInfo->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void **)&pElement);
		BSTR attri = ::SysAllocString(L"type");
		hr = pElement->getAttribute(attri,&varAttri);
		attri = _T("Latest");
		if (((_bstr_t)varAttri.bstrVal) == ((_bstr_t)attri))
		{
			BSTR tempBSTR = NULL;
			hr = pNodeDataInfo->selectSingleNode(_T("Server"),&pNodeInfo);
				hr = pNodeInfo->get_text(&tempBSTR);
				m_server.Empty();m_server = tempBSTR;
			hr = pNodeDataInfo->selectSingleNode(_T("Catalog"),&pNodeInfo);
				hr = pNodeInfo->get_text(&tempBSTR);
				m_database.Empty();m_database = tempBSTR;
			hr = pNodeDataInfo->selectSingleNode(_T("UserID"),&pNodeInfo);
				hr = pNodeInfo->get_text(&tempBSTR);
				m_username.Empty();m_username = tempBSTR;
			hr = pNodeDataInfo->selectSingleNode(_T("Password"),&pNodeInfo);
				hr = pNodeInfo->get_text(&tempBSTR);
				m_password.Empty();m_password = tempBSTR;
			GetDlgItem(IDC_EDIT_DATASERVER)->SetWindowTextW(m_server);
			GetDlgItem(IDC_EDIT_DATAUSER)->SetWindowTextW(m_username);
			GetDlgItem(IDC_EDIT_DATAPSW)->SetWindowTextW(m_password);
			SysFreeString(tempBSTR);
		}		
	}

	VariantClear(&varAttri);
	
	return TRUE;
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CONTEST, &CConfigDlg::OnBnClickedButtonContest)
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序


void CConfigDlg::OnBnClickedOk()
{
	CoInitialize(NULL);

	GetDlgItem(IDC_EDIT_DATASERVER)->GetWindowTextW(m_server);
	GetDlgItem(IDC_EDIT_DATAUSER)->GetWindowTextW(m_username);
	GetDlgItem(IDC_EDIT_DATAPSW)->GetWindowTextW(m_password);
	
	HRESULT hr;
	MSXML2::IXMLDOMNode* pRoot = NULL;
	MSXML2::IXMLDOMNodeList* pNodeDataInfos = NULL;	
	BSTR bstr = NULL;
	if (bstr)
	{
		SysFreeString(bstr);
	}

	hr = pXMLDoc->selectSingleNode(_T("DataInfo"),&pRoot);
	hr = pRoot->selectNodes(_T("Info"),&pNodeDataInfos);

	MSXML2::IXMLDOMNode* pNodeDataInfo = NULL;
	MSXML2::IXMLDOMNode* pNodeInfo = NULL;

	VARIANT varAttri;
	varAttri.vt = VT_BSTR;
	VariantInit(&varAttri);

	while ((pNodeDataInfos->nextNode(&pNodeDataInfo)) == S_OK)
	{
		MSXML2::IXMLDOMElement *pElement = NULL;
		pNodeDataInfo->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void **)&pElement);
		BSTR attri = ::SysAllocString(L"type");
		hr = pElement->getAttribute(attri,&varAttri);
		attri = _T("Latest");
		if (((_bstr_t)varAttri.bstrVal) == ((_bstr_t)attri))
		{
			BSTR tempBstr = NULL;
			tempBstr = m_server.AllocSysString();
				hr = pNodeDataInfo->selectSingleNode(_T("Server"),&pNodeInfo);
					hr = pNodeInfo->put_text(tempBstr);
			if (m_database.IsEmpty())
			{
			tempBstr = ::SysAllocString(_T("caxastat"));
			}
			else
			{
			tempBstr = m_database.AllocSysString();
			}
				hr = pNodeDataInfo->selectSingleNode(_T("Catalog"),&pNodeInfo);
					hr = pNodeInfo->put_text(tempBstr);
			tempBstr = m_username.AllocSysString();
				hr = pNodeDataInfo->selectSingleNode(_T("UserID"),&pNodeInfo);
					hr = pNodeInfo->put_text(tempBstr);
			tempBstr = m_password.AllocSysString();
				hr = pNodeDataInfo->selectSingleNode(_T("Password"),&pNodeInfo);
					hr = pNodeInfo->put_text(tempBstr);
			SysFreeString(tempBstr);
		}		
	}
	
	VARIANT varPath;
	varAttri.vt = VT_BSTR;
	VariantInit(&varPath);
	V_BSTR(&varPath) = SysAllocString(L"conf/database.xml");
	V_VT(&varPath) = VT_BSTR;

	hr = pXMLDoc->save(varPath);
	VariantClear(&varAttri);
	VariantClear(&varPath);
	
	CDialogEx::OnOK();
}


void CConfigDlg::OnBnClickedButtonContest()
{
	// TODO: 在此添加控件通知处理程序代码
	CString c_server;
	CString c_database(_T("caxastat"));
	CString c_user;
	CString c_password;

	GetDlgItem(IDC_EDIT_DATASERVER)->GetWindowTextW(c_server);
	GetDlgItem(IDC_EDIT_DATAUSER)->GetWindowTextW(c_user);
	GetDlgItem(IDC_EDIT_DATAPSW)->GetWindowTextW(c_password);

	CADOConnection m_adoConnection((_bstr_t)c_server,(_bstr_t)c_user,(_bstr_t)c_password,(_bstr_t)c_database);
	if (m_adoConnection.OnInitAdo())
	{
		MessageBoxW(_T("连接成功"));
		m_adoConnection.ExitConnect();	
	}
	else
	{
		MessageBoxW(_T("连接不成功"));
	}	
	
}
