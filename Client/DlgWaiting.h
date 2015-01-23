#pragma once


// CDlgWaiting �Ի���
#include <vector>
#include "const.h"
class CDlgWaiting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWaiting)

public:
	CDlgWaiting(CWnd* pParent = NULL);   // ��׼���캯��
	CDlgWaiting(CString strFolderPath,COleDateTime startDateTime,COleDateTime endDateTime,CString strServer,CString strDatabase,CString strUserName,CString strPSW,std::vector<OutUnit> outputEle,CWnd* pParent = NULL);
	virtual ~CDlgWaiting();

// �Ի�������
	enum { IDD = IDD_DIALOG_WAITING };

private:
	CString m_FolderPath;
	CString m_server;
	CString m_database;
	CString m_username;
	CString m_password;

	COleDateTime m_startDateTime;
	COleDateTime m_endDateTime;
	std::vector<OutUnit> m_outputEle;

private:
	void deleteDirectory(CString strDir);

	HRESULT dataOperation();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	

	afx_msg void OnBnClickedExitdlg();
	virtual BOOL OnInitDialog();
};
