#pragma once
#include "afxwin.h"


// CAddStaff �Ի���

class CAddStaff : public CDialogEx
{
	DECLARE_DYNAMIC(CAddStaff)

public:
	CAddStaff(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddStaff();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADDSTAFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CButton m_isAdmin;

private:
	HRESULT AppendChildToParent(MSXML2::IXMLDOMNode *pChild, MSXML2::IXMLDOMNode *pParent);
};
