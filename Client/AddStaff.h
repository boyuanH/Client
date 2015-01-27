#pragma once
#include "afxwin.h"


// CAddStaff 对话框

class CAddStaff : public CDialogEx
{
	DECLARE_DYNAMIC(CAddStaff)

public:
	CAddStaff(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddStaff();

// 对话框数据
	enum { IDD = IDD_DIALOG_ADDSTAFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CButton m_isAdmin;

private:
	HRESULT AppendChildToParent(MSXML2::IXMLDOMNode *pChild, MSXML2::IXMLDOMNode *pParent);
};
