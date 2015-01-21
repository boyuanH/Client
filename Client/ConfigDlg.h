#pragma once


// CConfigDlg �Ի���

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_DATABASECONF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	CString m_server;
	CString m_database;
	CString m_username;
	CString m_password;

	MSXML2::IXMLDOMDocument *pXMLDoc;
	MSXML2::IXMLDOMParseError *pXMLErr;

public:
	CString getServer(){return m_server;}
	CString getDatebase(){return m_database;}
	CString getUsername(){return m_username;}
	CString getPassword(){return m_password;}

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonContest();
};
