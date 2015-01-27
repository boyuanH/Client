// DlgWaiting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ADOConnection.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"
#include "const.h"


// CDlgWaiting �Ի���

IMPLEMENT_DYNAMIC(CDlgWaiting, CDialogEx)

CDlgWaiting::CDlgWaiting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWaiting::IDD, pParent)
{

}

CDlgWaiting::CDlgWaiting(CString strFolderPath,COleDateTime startDateTime,COleDateTime endDateTime,CString strServer,CString strDatabase,CString strUserName,CString strPSW,std::vector<OutUnit> outputEle,CWnd* pParent /* = NULL */)
	:CDialogEx(CDlgWaiting::IDD,pParent)
{
	CString strFileFolder;
	COleDateTime dateTimeCur = COleDateTime::GetCurrentTime();
	strFileFolder.Format(_T("\\%d%d%d%d%d"),dateTimeCur.GetYear(),dateTimeCur.GetMonth(),dateTimeCur.GetDay(),dateTimeCur.GetHour(),dateTimeCur.GetMinute());
	m_FolderPath	 = strFolderPath + strFileFolder ;
	m_server		 = strServer;
	m_database		 = strDatabase;
	m_username		 = strUserName;
	m_password		 = strPSW;
	m_startDateTime	 = startDateTime;
	m_endDateTime	 = endDateTime;
	m_outputEle		 = outputEle;

	m_outInfo.m_FolderPath		 = strFolderPath + strFileFolder ;
	m_outInfo.m_server			 = strServer;
	m_outInfo.m_database		 = strDatabase;
	m_outInfo.m_username		 = strUserName;
	m_outInfo.m_password		 = strPSW;
	m_outInfo.m_startDateTime	 = startDateTime;
	m_outInfo.m_endDateTime		 = endDateTime;
	m_outInfo.m_outputEle		 = outputEle;
}

CDlgWaiting::~CDlgWaiting()
{
}

void CDlgWaiting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWaiting, CDialogEx)

	ON_BN_CLICKED(ID_EXITDLG, &CDlgWaiting::OnBnClickedExitdlg)
	ON_MESSAGE(WM_CLOSE,CDlgWaiting::OnClose)
END_MESSAGE_MAP()


// CDlgWaiting ��Ϣ�������





void CDlgWaiting::OnBnClickedExitdlg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int response = MessageBoxW(_T("���Ҫ�ж�"),_T("����"),MB_OKCANCEL);
	if (IDOK == response)
	{
		MessageBoxW(_T("������������ж��쳣"));
		//���Ŀ¼�����ļ���ɾ���Ѿ��������ļ�
		deleteDirectory(m_outInfo.m_FolderPath);
		//�˳�
		CDialogEx::OnCancel();
	}
	if (IDCANCEL == response)
	{
		return ;
	}
}

void CDlgWaiting::deleteDirectory(CString strDir)
{
	if(strDir.IsEmpty())   
	{ 
		RemoveDirectory(strDir); 
		
		return; 
	} 

	//����ɾ���ļ������ļ��� 

	CFileFind   ff; 
	BOOL bFound = ff.FindFile(strDir+ _T("\\*"),0); 


	while(bFound) 
	{ 
		bFound = ff.FindNextFile(); 
		if(ff.GetFileName()== _T(".")||ff.GetFileName()== _T("..")) 
			continue; 
		//ȥ���ļ�(��)ֻ�������� 
		SetFileAttributes(ff.GetFilePath(),FILE_ATTRIBUTE_NORMAL); 
		if(ff.IsDirectory())  
		{   
			//�ݹ�ɾ�����ļ��� 
			deleteDirectory(ff.GetFilePath()); 
			RemoveDirectory(ff.GetFilePath()); 
		} 
		else   
		{ 
			DeleteFile(ff.GetFilePath());   //ɾ���ļ� 
		} 
	} 
	ff.Close(); 
	//Ȼ��ɾ�����ļ��� 
	RemoveDirectory(strDir); 
}

HRESULT CDlgWaiting::dataOperation()
{

	HRESULT hr;
	//�ļ�·����Ψһ�Լ��
	CFileFind   ff; 
	BOOL bFound = ff.FindFile(m_FolderPath,0); 
	if (bFound)
	{
		MessageBoxW(_T("��·�������ظ��ļ��У�����"));
		return E_FAIL;
	}
	else
	{
		//����path�����µ��ļ���
		CreateDirectory(m_FolderPath,NULL);
	}
	//�������ݿ�
	
	_bstr_t procName=_T("[dbo].[sql2csv]");
	//�������� [caxastat].

	size_t sz = 0;
	std::vector<_ParameterPtr> paras;
	for (;sz < m_outputEle.capacity();sz++)
	{
	
		if (m_outputEle[sz].isDepartment)
		{
			m_FolderPath = m_FolderPath +_T("\\") + m_outputEle[sz].departName;
			CreateDirectory(m_FolderPath,NULL);
			for (size_t szt = 0;szt < m_outputEle[sz].fileEle.capacity();szt++)
			{
				if (m_outputEle[sz].fileEle[szt].isCheck)
				{
					//����������	
					//@filepath		varchar(256)	--FILEPATHȡֵΪ../../../���س���/
					//,@outputType	int			--	1,2,3�ֱ�ΪDeviceOperationAnalysis,DeviceAlarm,DeviceFILETRANLOG
					//	,@department	varchar(50)	
					//	,@startDate		datetime		--2014-12-04 00:00:00
					//	,@endDate		datetime
					//	,@UID			varchar(128)
					//	,@UPSW			varchar(256)

					CADOConnection m_adoConnection((_bstr_t)m_server,(_bstr_t)m_database,(_bstr_t)m_username,(_bstr_t)m_password);
					if (!m_adoConnection.OnInitAdo())
					{
						MessageBoxW(_T("�������ݿ�ʧ��"));
						return E_FAIL;
					}

					_ParameterPtr paraTemp;
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("filepath",adVarWChar,adParamInput,m_FolderPath.GetLength()+1,(_variant_t)(m_FolderPath));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("outputType",adInteger,adParamInput,sizeof(int),(_variant_t)(szt+1));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("department",adVarWChar,adParamInput,m_outputEle[sz].departName.GetLength()+1,(_variant_t)(m_outputEle[sz].departName));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					_variant_t  vtDate;
					vtDate.vt = VT_DATE;
					vtDate = m_startDateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("startDate",adDBTimeStamp,adParamInput,135,vtDate);
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					vtDate = m_endDateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("endDate",adDBTimeStamp,adParamInput,135,vtDate);
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("UID",adVarWChar,adParamInput,m_username.GetLength()+1,(_variant_t)(m_username));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("UPSW",adVarWChar,adParamInput,m_password.GetLength()+1,(_variant_t)(m_password));
					paras.push_back(paraTemp);
					if (m_adoConnection.ExecuteProc(procName,paras))
					{
						//LOG�ļ�����Ӽ�¼
						hr = S_OK;
					}
					else
					{
						//ERRORLOG��Ӽ�¼
					}
					paras.clear();
					m_adoConnection.ExitConnect();					
				}
			}
		}
	}
	
	return 0;
}

BOOL CDlgWaiting::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//��ͨ�汾
	/*
	HRESULT hr;
	hr = dataOperation();
	if (SUCCEEDED(hr))
	{
		CDialogEx::OnOK();
	}
	else
	{
		CDialogEx::OnCancel();
	}
	*/
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	//���̰߳汾
	m_outInfo.hWnd = m_hWnd;
	m_pThread = AfxBeginThread(ThreadFunc,&m_outInfo);
	if (m_pThread ==NULL)
	{
		MessageBoxW(_T("����ʧ��!"));
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

LRESULT CDlgWaiting::OnClose(WPARAM wParam,LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (wParam == 0)
	{
		//ʧ��
		deleteDirectory(m_outInfo.m_FolderPath);
		//�˳�
		CDialogEx::OnCancel();		
	} 
	else
	{
		CDialogEx::OnOK();
	}
	return E_FAIL;	
}

UINT ThreadFunc(LPVOID pParm)
{
	OutputInfo *pInfo = (OutputInfo *)pParm;
	HRESULT hr;
	//�ļ�·����Ψһ�Լ��
	CFileFind   ff; 
	BOOL bFound = ff.FindFile(pInfo->m_FolderPath,0); 
	if (bFound)
	{
		//MessageBoxW(_T("��·�������ظ��ļ��У�����"));
		AfxMessageBox(_T("����ʧ��!"));
		//�����߳�
		::SendMessage(pInfo->hWnd,WM_CLOSE,0,0);
		return E_FAIL;
	}
	else
	{
		//����path�����µ��ļ���
		CreateDirectory(pInfo->m_FolderPath,NULL);
	}
	//�������ݿ�

	_bstr_t procName=_T("[dbo].[sql2csv]");
	//�������� [caxastat].

	size_t sz = 0;
	std::vector<_ParameterPtr> paras;
	for (;sz < pInfo->m_outputEle.capacity();sz++)
	{

		if (pInfo->m_outputEle[sz].isDepartment)
		{
			pInfo->m_FolderPath = pInfo->m_FolderPath +_T("\\") + pInfo->m_outputEle[sz].departName;
			CreateDirectory(pInfo->m_FolderPath,NULL);
			for (size_t szt = 0;szt < pInfo->m_outputEle[sz].fileEle.capacity();szt++)
			{
				if (pInfo->m_outputEle[sz].fileEle[szt].isCheck)
				{
					//����������	
					//@filepath		varchar(256)	--FILEPATHȡֵΪ../../../���س���/
					//,@outputType	int			--	1,2,3�ֱ�ΪDeviceOperationAnalysis,DeviceAlarm,DeviceFILETRANLOG
					//	,@department	varchar(50)	
					//	,@startDate		datetime		--2014-12-04 00:00:00
					//	,@endDate		datetime
					//	,@UID			varchar(128)
					//	,@UPSW			varchar(256)

					CADOConnection m_adoConnection((_bstr_t)pInfo->m_server,(_bstr_t)pInfo->m_database,(_bstr_t)pInfo->m_username,(_bstr_t)pInfo->m_password);
					if (!m_adoConnection.OnInitAdo())
					{
						AfxMessageBox(_T("�������ݿ�ʧ��"));
						return E_FAIL;
					}

					_ParameterPtr paraTemp;
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("filepath",adVarWChar,adParamInput,pInfo->m_FolderPath.GetLength()+1,(_variant_t)(pInfo->m_FolderPath));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("outputType",adInteger,adParamInput,sizeof(int),(_variant_t)(szt+1));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("department",adVarWChar,adParamInput,pInfo->m_outputEle[sz].departName.GetLength()+1,(_variant_t)(pInfo->m_outputEle[sz].departName));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					_variant_t  vtDate;
					vtDate.vt = VT_DATE;
					vtDate = pInfo->m_startDateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("startDate",adDBTimeStamp,adParamInput,135,vtDate);
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					vtDate = pInfo->m_endDateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("endDate",adDBTimeStamp,adParamInput,135,vtDate);
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("UID",adVarWChar,adParamInput,pInfo->m_username.GetLength()+1,(_variant_t)(pInfo->m_username));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("UPSW",adVarWChar,adParamInput,pInfo->m_password.GetLength()+1,(_variant_t)(pInfo->m_password));
					paras.push_back(paraTemp);
					if (m_adoConnection.ExecuteProc(procName,paras))
					{
						//LOG�ļ�����Ӽ�¼
						hr = S_OK;
					}
					else
					{
						//ERRORLOG��Ӽ�¼
					}
					paras.clear();
					m_adoConnection.ExitConnect();					
				}
			}
		}
	}

	::SendMessage(pInfo->hWnd,WM_CLOSE,1,0);
	return 0;
}