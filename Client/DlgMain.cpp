// DlgMain.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "DlgMain.h"
#include "afxdialogex.h"
#include "ADOConnection.h"
#include "ConfigDlg.h"
#include "DlgWaiting.h"

// CDlgMain 对话框

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
	// TODO: 在此添加控件通知处理程序代码

	//检测std::vector<OutputElement> outputElementVector有内容，日期时间正确
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
	//显示提示框

	//获得配置数据
	//数据库操作
	//写入检测文件
	//写入log文件
	//数据清空

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


	CADOConnection m_adoConnection((_bstr_t)m_server,(_bstr_t)m_username,(_bstr_t)m_password,(_bstr_t)m_database);//使用XML中的数据
	//CADOConnection m_adoConnection;
	m_adoConnection.OnInitAdo();
	_RecordsetPtr m_pRecordset;
	//从数据库初始化数据
	_bstr_t empSQL=_T("Select * from [caxastat].[dbo].[SYSCLASSFICATION]");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL);
	//读取数据库来填充车间，选择类型

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
	//根据outputElementVector来输出ListBox

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


// CDlgMain 消息处理程序

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

	m_listboxOutputFileType.AddString(_T("运行"));
	m_listboxOutputFileType.AddString(_T("警告"));
	m_listboxOutputFileType.AddString(_T("传输"));
	
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
	//检查outputElementVector的合法性
	//-1 未知错误，0，内容为空，1内容不完整无有效输出，2没问题
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
	// TODO: 在此添加控件通知处理程序代码
	int curSel = m_listboxDepartment.GetCurSel();

}


void CDlgMain::OnLbnSelchangeListOutfiletype()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CDlgMain::showList()
{
	listFilter();
	if (outputElementVector.size() == 0)
	{
		return ;
	}

	//清空
	for (int i = m_listboxDepartment.GetCount();i>=0;i--)
	{
		m_listboxDepartment.DeleteString(i);
	}

	for (int i = m_listboxOutputFileType.GetCount();i>=0;i--)
	{
		m_listboxOutputFileType.DeleteString(i);
	}
	
	//重置数据
	m_listboxOutputFileType.AddString(_T("运行"));
	m_listboxOutputFileType.AddString(_T("警告"));
	m_listboxOutputFileType.AddString(_T("传输"));

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