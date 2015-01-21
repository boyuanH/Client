// DlgWaiting.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"


// CDlgWaiting 对话框

IMPLEMENT_DYNAMIC(CDlgWaiting, CDialogEx)

CDlgWaiting::CDlgWaiting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWaiting::IDD, pParent)
{

}

CDlgWaiting::~CDlgWaiting()
{
}

void CDlgWaiting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWaiting, CDialogEx)
END_MESSAGE_MAP()


// CDlgWaiting 消息处理程序
