// DlgMain.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "DlgMain.h"
#include "afxdialogex.h"
#include "ADOConnection.h"
#include "ConfigDlg.h"
#include "DlgWaiting.h"
#include "AddStaff.h"


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
	AfxGetApp()->m_pMainWnd = this;
	pParent->ShowWindow(SW_HIDE);
}

CDlgMain::~CDlgMain()
{
	delete pMenu;
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
	ModifyStyleEx(0,WS_EX_APPWINDOW);
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
	CAddStaff dlg;
	dlg.DoModal();
}

void CDlgMain::OnAbout()
{
	MessageBoxW(_T("About"));
}

void CDlgMain::OnBnClickedButtonOk()
{
	// TODO: 在此添加控件通知处理程序代码

	//检测std::vector<OutputElement> outputElem内容的逻辑合理性
	int temp = listFilter();
	if (4 != temp)
	{
		CString strTemp;
		strTemp.Format(_T("%d Error"),temp);
		MessageBoxW(strTemp);
		return ;
	}

	//检测日期合法性
	//end的时间大于等于start时间

	COleDateTime t1,t2,tempdate;
	m_DateStart.GetTime(t1);
	m_dateEnd.GetTime(t2);
	m_DateStart.GetTime(tempdate);
	
	if (t1>t2)
	{
		MessageBoxW(_T("日期选择不正确"));
		return;
	}
	
	t1.SetDateTime(tempdate.GetYear(),tempdate.GetMonth(),tempdate.GetDay(),0,0,0);
	m_dateEnd.GetTime(tempdate);
	t2.SetDateTime(tempdate.GetYear(),tempdate.GetMonth(),tempdate.GetDay(),23,59,59);
	

	//获取路径
	TCHAR	szFolderPath[MAX_PATH] = {0};
	CString strFolderPath;	
	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("请选择一个文件夹：");  
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;  
	sInfo.lpfn     = NULL;  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL)  
	{  
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))    
		{  
			strFolderPath = szFolderPath;  
		}  
	}  
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	}  
	
	//显示提示框
	CDlgWaiting dlgW(strFolderPath,t1,t2,m_server,m_database,m_username,m_password,outputEle);
	INT_PTR nResponse = dlgW.DoModal();
	//操作意外中断
	if (nResponse == IDOK)
	{
		MessageBoxW(_T("输出完成"));
		//Log文件
		//Log文件主要是记录这一次的操作时间，不具体记录内容
		//MD5校验码
	}
	else if (nResponse == IDCANCEL)
	{
		//Error Log
	}
	else if (nResponse == -1)
	{
		//Error Log
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}
}

void CDlgMain::OnStart()
{
	//清空复选框的数据，清空选择数据

	outputEle.clear();

	//重新获取数据库配置讯息
	
	if (S_OK != getConfigFromXML())
	{
		MessageBoxW(_T("Failed on get Config File From XML"));
		return ;
	}	

	//重新从数据库中获取数据
	//从数据库中获取可选时间范围
	CADOConnection m_adoConnection((_bstr_t)m_server,(_bstr_t)m_database,(_bstr_t)m_username,(_bstr_t)m_password);
	if (!m_adoConnection.OnInitAdo())
	{
		MessageBoxW(_T("连接数据库失败"));
		return ;
	}
	_RecordsetPtr m_pRecordset;
	//从数据库初始化数据
	_bstr_t empSQL=_T("Select * from [caxastat].[dbo].[SYSCLASSFICATION]");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL,adCmdText);
	//读取数据库来填充车间，选择类型

	int count = 0;
	while (!m_pRecordset->TheAdoEOF)
	{
		OutUnit ele;
		count++;
		_variant_t temp = (m_pRecordset->GetCollect("classname"));
		if (temp.vt != VT_NULL)
		{
			//添加车间
			ele.departName = temp.bstrVal;
			ele.isDepartment= TRUE;
			//添加文件
			FileUint fu;
			fu.fileName =_T("操作文件");fu.isCheck = TRUE; ele.fileEle.push_back(fu);
			fu.fileName =_T("报警文件");fu.isCheck = TRUE; ele.fileEle.push_back(fu);
			fu.fileName =_T("传输文件");fu.isCheck = TRUE; ele.fileEle.push_back(fu);		
			outputEle.push_back(ele);
		}
		m_pRecordset->MoveNext();
	}

	if (outputEle.empty())
	{
		return ;
	}
	
	COleDateTime* pStartDate = &(COleDateTime::GetCurrentTime());
	COleDateTime* pEndDate = &(COleDateTime::GetCurrentTime());
	VARIANT varDate;
	VariantInit(&varDate);
	V_VT(&varDate) = VT_DATE;

	empSQL=_T("SELECT Top 1 * FROM [caxastat].[dbo].[DeviceOperationAnalysis] order by starttime ");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL,adCmdText);
	while(!m_pRecordset->TheAdoEOF)
	{
		varDate = (m_pRecordset->GetCollect("starttime"));
		if (varDate.date != NULL)
		{
				pStartDate = new COleDateTime(varDate);
		}
		m_pRecordset->MoveNext();
	}

	empSQL=_T("SELECT Top 1 * FROM [caxastat].[dbo].[DeviceOperationAnalysis] order by starttime DESC");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL,adCmdText);
	while(!m_pRecordset->TheAdoEOF)
	{
		varDate = (m_pRecordset->GetCollect("starttime"));
		if (varDate.date != NULL)
		{
				pEndDate = new COleDateTime(varDate);
		}
		m_pRecordset->MoveNext();
	}

	m_adoConnection.ExitConnect();

	m_DateStart.SetRange(pStartDate,pEndDate);
	m_dateEnd.SetRange(pStartDate,pEndDate);
	delete pStartDate;
	delete pEndDate;
	showList();
	m_listboxDepartment.SetCurSel(0);
	m_listboxOutputFileType.SetCurSel(0);
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
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START, &CDlgMain::OnDtnDatetimechangeDatetimepickerStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, &CDlgMain::OnDtnDatetimechangeDatetimepickerEnd)
END_MESSAGE_MAP()


// CDlgMain 消息处理程序

void CDlgMain::OnLbnSelchangeListDepartment()
{
	// TODO: 在此添加控件通知处理程序代码
	int curSel = m_listboxDepartment.GetCurSel();
	if (curSel != -1)
	{
		for (int i = 0 ;i<m_listboxDepartment.GetCount();i++)
		{
			if (1 == m_listboxDepartment.GetCheck(i))
			{
				outputEle[i].isDepartment = TRUE;
			} 
			else
			{
				outputEle[i].isDepartment = FALSE;
			}
		}
	}
	showList();
}

void CDlgMain::OnLbnSelchangeListOutfiletype()
{
	// TODO: 在此添加控件通知处理程序代码
	int curSel = m_listboxDepartment.GetCurSel();
	//int temp = 0;
	if (curSel != -1)
	{
		for (size_t i = 0 ;i < outputEle[curSel].fileEle.capacity();i++)
		{
			if (1 == m_listboxOutputFileType.GetCheck(i))
			{
				outputEle[curSel].fileEle[i].isCheck = TRUE;
			} 
			else
			{
				outputEle[curSel].fileEle[i].isCheck = FALSE;
			}
		}
	}
	else
	{
		MessageBoxW(_T("未选择车间号"));
		return;
	}
}

void CDlgMain::showList()
{
	int temp1 = m_listboxDepartment.GetCurSel();
	int temp2 = m_listboxOutputFileType.GetCurSel();
	showList(temp1,temp2);
}

void CDlgMain::showList(int selDepartment,int selFile)
{
	//过滤无效数据
	//listFilter();

	if (outputEle.empty())
	{
		return;
	}
	for (int i = m_listboxDepartment.GetCount();i>=0;i--)
	{
		m_listboxDepartment.DeleteString(i);
	}

	for (int i = m_listboxOutputFileType.GetCount();i>=0;i--)
	{
		m_listboxOutputFileType.DeleteString(i);
	}

	//显示Department部分
	for (size_t i = 0;i < outputEle.capacity();i++)
	{
		m_listboxDepartment.AddString(outputEle[i].departName);

		if (outputEle[i].isDepartment)
		{
			m_listboxDepartment.SetCheck(i,BST_CHECKED);
		} 
		else
		{
			m_listboxDepartment.SetCheck(i,BST_UNCHECKED);
		}
	}
	//显示OutputFiletype部分

	int temp = 0;
	if (-1 != selDepartment)
	{
		temp = selDepartment;
	}
	size_t j = 0;

	for (;j < outputEle[temp].fileEle.capacity(); j++)
	{
		m_listboxOutputFileType.AddString(outputEle[temp].fileEle[j].fileName);
		if (outputEle[temp].fileEle[j].isCheck)
		{
			m_listboxOutputFileType.SetCheck(j,BST_CHECKED);
		} 
		else
		{
			m_listboxOutputFileType.SetCheck(j,BST_UNCHECKED);
		}
	}

	//设置选中状态
	m_listboxDepartment.SetCurSel(selDepartment);
	m_listboxOutputFileType.SetCurSel(selFile);

}

int CDlgMain::listFilter()
{
	//0.禁止outputEle内容为空时输出			提示0
	//1.没有任何选择						提示1
	//2.禁止车间选择为是的项，文件选择为空	提示2
	//3.禁止车间选择为否的项，文件有选择	提示3
	//4.合法的输出							提示4
	
	if (outputEle.empty())
	{
		return 0;
	}	

	
	BOOL flagA = FALSE;
	for (size_t i = 0;i < outputEle.capacity();i++)
	{
		if (outputEle[i].isDepartment)
		{
			BOOL flag = FALSE; flagA = TRUE;
			for (size_t j = 0; j < outputEle[i].fileEle.capacity();j++)
			{
				if (outputEle[i].fileEle[j].isCheck)
				{
					flag = TRUE;
				}
			}
			if (!flag)
			{
				return 2;
			}
		} 
		else
		{
// 			BOOL flag = FALSE;
// 			for (int j = 0; j < outputEle[i].fileEle.capacity();j++)
// 			{
// 				if (outputEle[i].fileEle[j].isCheck)
// 				{
// 					flag = TRUE;
// 				}
// 			}
// 			if (flag)
// 			{
// 				return 3;
// 			}
 			for (size_t j = 0; j < outputEle[i].fileEle.capacity();j++)
 			{
 				if (outputEle[i].fileEle[j].isCheck)
 				{
 					return 3;
 				}
 			}			
		}
	}

	if (!flagA)
	{
		return 1;
	}

	return 4;	
}
 
HRESULT CDlgMain::getConfigFromXML()
{
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
		return hr;
	}
	
	BSTR bstr = NULL;
	VARIANT_BOOL status;
	VARIANT var;

	VariantInit(&var);
	V_BSTR(&var) = SysAllocString(L"conf/database.xml");
	V_VT(&var) = VT_BSTR;
	hr = pXMLDoc->load(var, &status);

	if (VARIANT_TRUE != status)
	{
		pXMLDoc->get_parseError(&pXMLErr);
		pXMLErr->get_reason(&bstr);
		if (&var) VariantClear(&var);   
		if (pXMLDoc) pXMLDoc->Release();   
		MessageBoxW(_T("Failed to load DOM from Database xml."));
		return hr;
	}

	VariantClear(&var);

	MSXML2::IXMLDOMNode* pRoot = NULL;
	MSXML2::IXMLDOMNodeList* pNodeDataInfos = NULL;	

	if (bstr)
	{
		SysFreeString(bstr);
	}

	hr = pXMLDoc->selectSingleNode(_T("DataInfo"),&pRoot);
	CHECKHR
	hr = pRoot->selectNodes(_T("Info"),&pNodeDataInfos);
	CHECKHR
	MSXML2::IXMLDOMNode* pNodeDataInfo = NULL;
	MSXML2::IXMLDOMNode* pNodeInfo = NULL;

	VARIANT varAttri;
	varAttri.vt = VT_BSTR;
	VariantInit(&varAttri);
	
	while ((pNodeDataInfos->nextNode(&pNodeDataInfo)) == S_OK)
	{
		MSXML2::IXMLDOMElement *pElement = NULL;
		hr = pNodeDataInfo->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void **)&pElement);
		CHECKHR
		BSTR attri = ::SysAllocString(L"type");
		hr = pElement->getAttribute(attri,&varAttri);
		CHECKHR
		attri = _T("Latest");
		if (((_bstr_t)varAttri.bstrVal) == ((_bstr_t)attri))
		{
			BSTR tempBSTR = NULL;
			hr = pNodeDataInfo->selectSingleNode(_T("Server"),&pNodeInfo);	CHECKHR
			hr = pNodeInfo->get_text(&tempBSTR);							CHECKHR
			m_server.Empty();m_server = tempBSTR;
			hr = pNodeDataInfo->selectSingleNode(_T("Catalog"),&pNodeInfo);	CHECKHR
			hr = pNodeInfo->get_text(&tempBSTR);							CHECKHR
			m_database.Empty();m_database = tempBSTR;
			hr = pNodeDataInfo->selectSingleNode(_T("UserID"),&pNodeInfo);	CHECKHR
			hr = pNodeInfo->get_text(&tempBSTR);							CHECKHR
			m_username.Empty();m_username = tempBSTR;
			hr = pNodeDataInfo->selectSingleNode(_T("Password"),&pNodeInfo);CHECKHR
			hr = pNodeInfo->get_text(&tempBSTR);							CHECKHR
			m_password.Empty();m_password = tempBSTR;
			SysFreeString(tempBSTR);
		}		
	}
	VariantClear(&varAttri);
	CoUninitialize();

	if ((m_server.IsEmpty())||(m_database.IsEmpty())||(m_database.IsEmpty()))
	{
		return ERROR_INVALID_DATA;
	}

	return hr;
}

void CDlgMain::OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	*pResult = 0;
}


void CDlgMain::OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
