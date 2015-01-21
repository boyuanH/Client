// DlgMain.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "DlgMain.h"
#include "afxdialogex.h"
#include "ADOConnection.h"
#include "ConfigDlg.h"
#include "DlgWaiting.h"

// CDlgMain �Ի���

IMPLEMENT_DYNAMIC(CDlgMain, CDialogEx)

CDlgMain::CDlgMain(CWnd* pParent /*= NULL*/)	
	:CDialogEx(CDlgMain::IDD, pParent)
{
	
}

CDlgMain::CDlgMain(LoginType lt,CWnd* pParent)
	:CDialogEx(CDlgMain::IDD, pParent)
{		
	m_loginType = lt;
	//pParent->ShowWindow(SW_HIDE);
	AfxGetApp()->m_pMainWnd = this;
}

CDlgMain::~CDlgMain()
{
}

void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEPARTMENT, m_listboxDepartment);
	DDX_Control(pDX, IDC_LIST_OUTFILETYPE, m_listboxOutputFileType);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_DateStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dateEnd);
}

BOOL CDlgMain::OnInitDialog()
{

	CDialogEx::OnInitDialog();
	pMenu  = new CMenu();
	if (m_loginType == ADMIN)
	{
		pMenu->LoadMenuW(IDR_MENU_ADMIN);
	}
	else
	{
		pMenu->LoadMenuW(IDR_MENU_NORMAL);
	}

	SetMenu(pMenu);
 	return TRUE;
}

void CDlgMain::OnExit()
{
	CWnd::DestroyWindow();
}

void CDlgMain::OnDataConfig()
{
	CConfigDlg dlg(this);
	dlg.DoModal();
}

void CDlgMain::OnAddStaff()
{
	MessageBoxW(_T("Staff"));
}

void CDlgMain::OnAbout()
{
	MessageBoxW(_T("About"));
}

void CDlgMain::OnBnClickedButtonOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//���std::vector<OutputElement> outputElementVector�����ݣ�����ʱ����ȷ
	if (FALSE)
	{
		MessageBoxW(_T("Error input information"));		
	}
	else
	{
		CDlgWaiting dlg;
		dlg.DoModal();
		MessageBoxW(_T("OK"));	
	}
	//��ʾ��ʾ��

	//�����������
	//���ݿ����
	//д�����ļ�
	//д��log�ļ�
	//�������

}

void CDlgMain::OnStart()
{
	//read XML

	outputElementVector.clear();

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
			SysFreeString(tempBSTR);
		}		
	}
	VariantClear(&varAttri);


	CADOConnection m_adoConnection((_bstr_t)m_server,(_bstr_t)m_username,(_bstr_t)m_password,(_bstr_t)m_database);//ʹ��XML�е�����
	//CADOConnection m_adoConnection;
	m_adoConnection.OnInitAdo();
	_RecordsetPtr m_pRecordset;
	//�����ݿ��ʼ������
	_bstr_t empSQL=_T("Select * from [caxastat].[dbo].[SYSCLASSFICATION]");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL);
	//��ȡ���ݿ�����䳵�䣬ѡ������

	int count = 0;
	while (!m_pRecordset->TheAdoEOF)
	{
		OutputElement ele;
		count++;
		_variant_t temp = (m_pRecordset->GetCollect("classname"));
		if (temp.vt != VT_NULL)
		{
			ele.department = temp.bstrVal;
			ele.isDepartment= TRUE;
			ele.isAlarm = TRUE;
			ele.isOperation = TRUE;
			ele.isTrans = TRUE;
		}
		
		outputElementVector.push_back(ele);
		m_pRecordset->MoveNext();
	}

	m_adoConnection.ExitConnect();
	//����outputElementVector�����ListBox

	outputList();
}

BEGIN_MESSAGE_MAP(CDlgMain, CDialogEx)
	//ON_COMMAND(ID_BUTTON_LINE,&CMyMFCView::OnLine)
	ON_COMMAND(IDC_EXIT,&CDlgMain::OnExit)
	ON_COMMAND(IDC_DATACONF,&CDlgMain::OnDataConfig)
	ON_COMMAND(IDC_ADD,&CDlgMain::OnAddStaff)
	ON_COMMAND(IDC_ABOUT,&CDlgMain::OnAbout)
	ON_COMMAND(IDC_OPEN,&CDlgMain::OnStart)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgMain::OnBnClickedButtonOk)
	ON_LBN_SELCHANGE(IDC_LIST_DEPARTMENT, &CDlgMain::OnLbnSelchangeListDepartment)
	ON_LBN_SELCHANGE(IDC_LIST_OUTFILETYPE, &CDlgMain::OnLbnSelchangeListOutfiletype)
END_MESSAGE_MAP()


// CDlgMain ��Ϣ�������

int CDlgMain::outputList()
{

	if (outputElementVector.size() == 0)
	{
		return -1;
	}

	checkList();

	for (int i = m_listboxDepartment.GetCount();i>=0;i--)
	{
		m_listboxDepartment.DeleteString(i);
	}

	for (int i = m_listboxOutputFileType.GetCount();i>=0;i--)
	{
		m_listboxOutputFileType.DeleteString(i);
	}

	m_listboxOutputFileType.AddString(_T("����"));
	m_listboxOutputFileType.AddString(_T("����"));
	m_listboxOutputFileType.AddString(_T("����"));
	
	for (int i = 0;i<outputElementVector.size();i++)
	{
		m_listboxDepartment.AddString(outputElementVector[i].department);
		if (outputElementVector[i].isDepartment)
		{
			m_listboxDepartment.SetCheck(i,BST_CHECKED);
		}
		else
		{
			m_listboxDepartment.SetCheck(i,BST_UNCHECKED);
		}
	}

	int temp = m_listboxDepartment.GetCurSel();
	if (-1 == temp)
	{
		temp = 0;
		m_listboxDepartment.SetCurSel(temp);
	}
	if (outputElementVector[temp].isOperation)
	{
		m_listboxOutputFileType.SetCheck(0,BST_CHECKED);
	} 
	else
	{
		m_listboxOutputFileType.SetCheck(0,BST_UNCHECKED);
	}
	if (outputElementVector[temp].isAlarm)
	{
		m_listboxOutputFileType.SetCheck(1,BST_CHECKED);
	} 
	else
	{
		m_listboxOutputFileType.SetCheck(1,BST_UNCHECKED);
	}
	if (outputElementVector[temp].isTrans)
	{
		m_listboxOutputFileType.SetCheck(2,BST_CHECKED);
	} 
	else
	{
		m_listboxOutputFileType.SetCheck(2,BST_UNCHECKED);
	}
	return 1;
}

int CDlgMain::checkList()
{
	//���outputElementVector�ĺϷ���
	//-1 δ֪����0������Ϊ�գ�1���ݲ���������Ч�����2û����
	int res = -1;
	if (outputElementVector.empty())
	{
		return 0;
	}
	int temp = 1;
	for (int i=0;i<outputElementVector.capacity();i++)
	{
		
	}
	return res;
}

void CDlgMain::OnLbnSelchangeListDepartment()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int curSel = m_listboxDepartment.GetCurSel();

}


void CDlgMain::OnLbnSelchangeListOutfiletype()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CDlgMain::showList()
{
	listFilter();
	if (outputElementVector.size() == 0)
	{
		return ;
	}

	//���
	for (int i = m_listboxDepartment.GetCount();i>=0;i--)
	{
		m_listboxDepartment.DeleteString(i);
	}

	for (int i = m_listboxOutputFileType.GetCount();i>=0;i--)
	{
		m_listboxOutputFileType.DeleteString(i);
	}
	
	//��������
	m_listboxOutputFileType.AddString(_T("����"));
	m_listboxOutputFileType.AddString(_T("����"));
	m_listboxOutputFileType.AddString(_T("����"));

	for (int i = 0;i<outputElementVector.size();i++)
	{
		m_listboxDepartment.AddString(outputElementVector[i].department);
		if (outputElementVector[i].isDepartment)
		{
			m_listboxDepartment.SetCheck(i,BST_CHECKED);
		}
		else
		{
			m_listboxDepartment.SetCheck(i,BST_UNCHECKED);
		}
	}

	int temp = m_listboxDepartment.GetCurSel();
	if (-1 == temp)
	{
		temp = 0;
		m_listboxDepartment.SetCurSel(temp);
	}
	if (outputElementVector[temp].isOperation)
	{
		m_listboxOutputFileType.SetCheck(0,BST_CHECKED);
	} 
	else
	{
		m_listboxOutputFileType.SetCheck(0,BST_UNCHECKED);
	}
	if (outputElementVector[temp].isAlarm)
	{
		m_listboxOutputFileType.SetCheck(1,BST_CHECKED);
	} 
	else
	{
		m_listboxOutputFileType.SetCheck(1,BST_UNCHECKED);
	}
	if (outputElementVector[temp].isTrans)
	{
		m_listboxOutputFileType.SetCheck(2,BST_CHECKED);
	} 
	else
	{
		m_listboxOutputFileType.SetCheck(2,BST_UNCHECKED);
	}
}