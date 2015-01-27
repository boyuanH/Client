// AddStaff.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "AddStaff.h"
#include "afxdialogex.h"
#include "const.h"
#include "MD5Encrypt.h"

// CAddStaff 对话框

IMPLEMENT_DYNAMIC(CAddStaff, CDialogEx)

CAddStaff::CAddStaff(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddStaff::IDD, pParent)
{

}

CAddStaff::~CAddStaff()
{
}

void CAddStaff::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_ISADMIN, m_isAdmin);
}


BEGIN_MESSAGE_MAP(CAddStaff, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddStaff::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddStaff 消息处理程序


void CAddStaff::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CString m_strName,m_strPSW;

	GetDlgItem(IDC_EDIT_LOGINNAME)->GetWindowTextW(m_strName);
	GetDlgItem(IDC_EDIT_LOGINPSW)->GetWindowTextW(m_strPSW);

	
	CoInitialize(NULL);
	HRESULT hr;

	MSXML2::IXMLDOMDocument *pXMLDoc;
	MSXML2::IXMLDOMParseError *pXMLErr;

	hr = CoCreateInstance(__uuidof(MSXML2::DOMDocument40),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(MSXML2::IXMLDOMDocument),
		(void**)&pXMLDoc);
	if (FAILED(hr)) 
	{
		printf("Failed to CoCreate an instance of an XML DOM\n");
		return ;
	}

	BSTR bstr = NULL;
	VARIANT_BOOL status;
	VARIANT var;

	VariantInit(&var);
	V_BSTR(&var) = SysAllocString(L"conf/itest.xml");
	V_VT(&var) = VT_BSTR;
	hr = pXMLDoc->load(var, &status);

	if (VARIANT_TRUE != status)
	{
		pXMLDoc->get_parseError(&pXMLErr);
		pXMLErr->get_reason(&bstr);
		if (&var) VariantClear(&var);
		if (pXMLDoc) pXMLDoc->Release();
		MessageBoxW(_T("Failed to load DOM from xml."));
		return ;
	}

	VariantClear(&var);

	MSXML2::IXMLDOMNode* pRoot = NULL;
	MSXML2::IXMLDOMNodeList* pNodeUsers = NULL;
	MSXML2::IXMLDOMNode* pNodeUser = NULL;
	MSXML2::IXMLDOMNode* pNodeLogin = NULL;	
	MSXML2::IXMLDOMNode* pNodePSW = NULL;	

	hr = pXMLDoc->selectSingleNode(_T("Persons"),&pRoot);
	hr = pRoot->selectNodes(_T("User"),&pNodeUsers);

	BSTR bstrLogin = NULL;	
	BOOL flag = TRUE;
	while(pNodeUsers->nextNode(&pNodeUser) == S_OK)
	{
		hr = pNodeUser->selectSingleNode(_T("LoginName"),&pNodeLogin);
		hr = pNodeLogin->get_text(&bstrLogin);
		if (bstrLogin == m_strName)
		{
			MessageBoxW(_T("重复名称"));	
			pXMLDoc->get_parseError(&pXMLErr);
			pXMLErr->get_reason(&bstr);
			if (&var) VariantClear(&var);   
			if (pXMLDoc) pXMLDoc->Release(); 
			CoUninitialize();
			flag = FALSE;
			break;
		}
	}

	if (FALSE == flag)
	{		
		return;
	}

//	SAFE_RELEASE(pNodeUsers);SAFE_RELEASE(pNodeUser);SAFE_RELEASE(pNodeLogin);SAFE_RELEASE(pNodePSW);

	VARIANT var_Str;
	VariantInit(&var_Str);
	V_BSTR(&var_Str) = SysAllocString(L"User");
	
	_variant_t varNodeType((short)MSXML2::NODE_ELEMENT);
	MSXML2::IXMLDOMNode* pNodeLoginUser = NULL;
	MSXML2::IXMLDOMNode* pNodeLoginName = NULL;
	MSXML2::IXMLDOMNode* pNodeLoginPSW = NULL;
	pXMLDoc->createNode(varNodeType,_T("User"),NULL,&pNodeLoginUser);	
	MSXML2::IXMLDOMNode *pChildOut = NULL;
	pRoot->appendChild(pNodeLoginUser,&pChildOut);
	BSTR tempBstr = NULL;
	pXMLDoc->createNode(varNodeType,_T("LoginName"),NULL,&pNodeLoginName);	
	pChildOut = NULL;tempBstr = m_strName.AllocSysString();
	pNodeLoginUser->appendChild(pNodeLoginName,&pChildOut);
	pNodeLoginName->put_text(tempBstr);

	pXMLDoc->createNode(varNodeType,_T("Password"),NULL,&pNodeLoginPSW);	
	pChildOut = NULL;

	int len = (m_strPSW.GetLength()+1)*sizeof(TCHAR);
	byte* bBuf = new byte[len];
	memcpy(bBuf,m_strPSW.GetBuffer(),len);
	MD5 md5;
	md5.update((void*)bBuf,len);
	string strPWD = md5.toString();
	CString str2cstr(strPWD.c_str());
	tempBstr = str2cstr.AllocSysString();
	pNodeLoginUser->appendChild(pNodeLoginPSW,&pChildOut);
	pNodeLoginPSW->put_text(tempBstr);

	MSXML2::IXMLDOMElement *pElement = NULL;
	pNodeLoginUser->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void **)&pElement);
	BSTR attri = ::SysAllocString(L"LoginType");
	VARIANT varAttri;
	VariantInit(&var);
	varAttri.vt = VT_BSTR;
	if (1 == m_isAdmin.GetCheck())
	{
		V_BSTR(&varAttri) = SysAllocString(L"Admin");
	} 
	else
	{
		V_BSTR(&varAttri) = SysAllocString(L"Normal");
	}		
	hr = pElement->setAttribute(attri,varAttri);
	



	VARIANT varPath;
	varPath.vt = VT_BSTR;
	VariantInit(&varPath);
	V_BSTR(&varPath) = SysAllocString(L"conf/itest.xml");
	V_VT(&varPath) = VT_BSTR;
	pXMLDoc->save(varPath);
	VariantClear(&varPath);
	VariantClear(&var_Str);
	CDialogEx::OnOK();
}

HRESULT CAddStaff::AppendChildToParent(MSXML2::IXMLDOMNode *pChild, MSXML2::IXMLDOMNode *pParent)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNode *pChildOut = NULL;
	CHK_HR(pParent->appendChild(pChild, &pChildOut));

CleanUp:
	SAFE_RELEASE(pChildOut);
	return hr;
}