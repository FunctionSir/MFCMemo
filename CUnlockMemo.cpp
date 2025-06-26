// CUnlockMemo.cpp: 实现文件
//

#include "pch.h"
#include "MFCMemo.h"
#include "afxdialogex.h"
#include "CUnlockMemo.h"


// CUnlockMemo 对话框

IMPLEMENT_DYNAMIC(CUnlockMemo, CDialogEx)

CUnlockMemo::CUnlockMemo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UNLOCK, pParent)
{

}

CUnlockMemo::~CUnlockMemo()
{
}

void CUnlockMemo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UNLOCK_PASSWD, m_UlkPasswd);
}


BEGIN_MESSAGE_MAP(CUnlockMemo, CDialogEx)
	ON_BN_CLICKED(IDOK, &CUnlockMemo::OnBnClickedOk)
END_MESSAGE_MAP()


// CUnlockMemo 消息处理程序

void CUnlockMemo::OnBnClickedOk()
{
	m_UlkPasswd.GetWindowTextW(PasswdGot);
	CDialogEx::OnOK();
}
