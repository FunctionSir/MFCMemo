// CEncryptDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCMemo.h"
#include "afxdialogex.h"
#include "CEncryptDlg.h"


// CEncryptDlg 对话框

IMPLEMENT_DYNAMIC(CEncryptDlg, CDialogEx)

CEncryptDlg::CEncryptDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ENCRYPT, pParent)
{

}

CEncryptDlg::~CEncryptDlg()
{
}

void CEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENC_PASSWD, m_Passwd);
	DDX_Control(pDX, IDC_ENC_PASSWD_AGAIN, m_PasswdAgain);
}


BEGIN_MESSAGE_MAP(CEncryptDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEncryptDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CEncryptDlg 消息处理程序

void CEncryptDlg::OnBnClickedOk()
{
	m_Passwd.GetWindowTextW(PasswdGot);
	m_PasswdAgain.GetWindowTextW(PasswdAgainGot);
	if (PasswdGot != PasswdAgainGot) {
		MessageBox(TEXT("两次密码输入不一致"), TEXT("消息"));
		return;
	}
	CDialogEx::OnOK();
}
