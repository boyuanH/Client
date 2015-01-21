#pragma once
#include "const.h"
#include <vector>
#include "afxwin.h"
#include "afxdtctl.h"

// CDlgMain 对话框

class CDlgMain : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMain)

public:
	CDlgMain(CWnd* pParent = NULL);   // 标准构造函数
	CDlgMain(LoginType lt,CWnd* pParent = NULL);

	virtual ~CDlgMain();

	

// 对话框数据
	enum { IDD = IDD_DIALOG_Main };
private:
	LoginType m_loginType;
	CMenu* pMenu;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnStart();
	afx_msg void OnExit();
	afx_msg void OnDataConfig();
	afx_msg void OnAddStaff();
	afx_msg void OnAbout();

	DECLARE_MESSAGE_MAP()
private:
	std::vector<OutputElement> outputElementVector;
	int outputList();
	int checkList();
	void listFilter();
private:
	CString m_server;
	CString m_database;
	CString m_username;
	CString m_password;
public:
	CCheckListBox m_listboxDepartment;
	CCheckListBox m_listboxOutputFileType;


	CDateTimeCtrl m_DateStart;
	CDateTimeCtrl m_dateEnd;
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnLbnSelchangeListDepartment();
	afx_msg void OnLbnSelchangeListOutfiletype();
};
