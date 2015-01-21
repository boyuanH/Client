
// ClientDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CClientDlg �Ի���



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


// CClientDlg ��Ϣ�������

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClientDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//����MD5
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

	//��ȡXML���ұȽ�
	//���ж�UserName���ڲ�
	//������ڣ��Ƚ�����
	//���������ʣ���ȡ���ڵ��Attribuate
	
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
				//�ж��Ƿ��ǹ���Ա
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
