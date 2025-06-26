// CNewEntryDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCMemo.h"
#include "afxdialogex.h"
#include "CNewEntryDlg.h"


// CNewEntryDlg 对话框

IMPLEMENT_DYNAMIC(CNewEntryDlg, CDialogEx)

CNewEntryDlg::CNewEntryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NEW_MEMO, pParent)
{

}

CNewEntryDlg::~CNewEntryDlg()
{
}

void CNewEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DDL_DATE, m_DatePick);
	DDX_Control(pDX, IDC_DDL_TIME, m_TimePick);
	DDX_Control(pDX, IDC_CONTENT, m_Content);
}


BEGIN_MESSAGE_MAP(CNewEntryDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CNewEntryDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewEntryDlg 消息处理程序

void CNewEntryDlg::OnBnClickedOk()
{
	CTime tmpDate, tmpTime;
	m_DatePick.GetTime(tmpDate);
	m_TimePick.GetTime(tmpTime);
	CTime joined(tmpDate.GetYear(),tmpDate.GetMonth(),tmpDate.GetDay(),tmpTime.GetHour(),tmpTime.GetMinute(),tmpTime.GetSecond());
	TimeGot = joined;
	m_Content.GetWindowTextW(ContentGot);
	CDialogEx::OnOK();
}
