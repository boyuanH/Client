
// ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include "MD5Encrypt.h"
#include "DlgMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace MSXML2;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDlg 对话框



CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pXMLDoc = NULL;
	pXMLErr = NULL;
	m_lt = FORBID;
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CClientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_EDIT_USER)->SetWindowTextW(_T("system"));
	GetDlgItem(IDC_EDIT_PWD)->SetWindowTextW(_T("system"));

	//init com for msxml
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
	V_BSTR(&var) = SysAllocString(L"conf/itest.xml");
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

	//pXMLDoc->get_xml(&bstr);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClientDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//生成MD5
	HRESULT hr;
	CString inputUsername,inputPassword;
	
	GetDlgItem(IDC_EDIT_USER)->GetWindowTextW(inputUsername);
	GetDlgItem(IDC_EDIT_PWD)->GetWindowTextW(inputPassword);
		
	int len = (inputPassword.GetLength()+1)*sizeof(TCHAR);
	byte* bBuf = new byte[len];
	memcpy(bBuf,inputPassword.GetBuffer(),len);
	MD5 md5;
	md5.update((void*)bBuf,len);
	string strPWD = md5.toString();

	//读取XML并且比较
	//先判断UserName存在不
	//如果存在，比较密码
	//如果密码合适，获取父节点的Attribuate
	
	MSXML2::IXMLDOMNode* pRoot = NULL;
	MSXML2::IXMLDOMNodeList* pNodeUsers = NULL;
	MSXML2::IXMLDOMNode* pNodeUser = NULL;
	MSXML2::IXMLDOMNode* pNodeLogin = NULL;
	BSTR bstr = NULL;
	MSXML2::IXMLDOMElement *pIXMLDOMElement = NULL;	

	if (bstr)
	{
		SysFreeString(bstr);
	}
	
	hr = pXMLDoc->selectSingleNode(_T("Persons"),&pRoot);
	hr = pRoot->selectNodes(_T("User"),&pNodeUsers);

	BSTR bstrLogin = NULL;
	BOOL isLoginName = FALSE;	

	while(pNodeUsers->nextNode(&pNodeUser) == S_OK)
	{
		hr = pNodeUser->selectSingleNode(_T("LoginName"),&pNodeLogin);
		hr = pNodeLogin->get_text(&bstrLogin);
		if (bstrLogin == inputUsername)
		{
			isLoginName = TRUE;
			hr = pNodeLogin->get_nextSibling(&pNodeLogin);
			hr = pNodeLogin->get_text(&bstrLogin);
			CString str2cstr(strPWD.c_str());
//			CString str2cstr(_T("222222"));
			if (bstrLogin == str2cstr)
			{				
				//判断是否是管理员
				MSXML2::IXMLDOMElement *pElement = NULL;
				pNodeUser->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void **)&pElement);
				BSTR attri = ::SysAllocString(L"LoginType");
				VARIANT var;
				var.vt = VT_BSTR;
				VariantInit(&var);
				hr = pElement->getAttribute(attri,&var);
				attri = _T("Admin");
				if ((_bstr_t)(var.bstrVal) == (_bstr_t)attri)
				{
					m_lt = ADMIN;
					//MessageBoxW(_T("OK Admin"));
				}
				else
				{
					//MessageBoxW(_T("OK Normal"));
					m_lt = NORMAL;
				}
				VariantClear(&var);
			} 
			else
			{
				MessageBoxW(_T("No OK,PSW wrong"));
			}
			break;
		}
	}



	if (!isLoginName)
	{
		MessageBoxW(_T("User not exist"));
	}
	
	if(m_lt != FORBID)
	{
		CoUninitialize();
		CDlgMain dlgMain(m_lt,this);
 		dlgMain.DoModal();
		CDialogEx::OnOK();
	}
	

}
