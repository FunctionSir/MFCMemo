// CEditEntry.cpp: 实现文件
//

#include "pch.h"
#include "MFCMemo.h"
#include "afxdialogex.h"
#include "CEditEntry.h"


// CEditEntry 对话框

IMPLEMENT_DYNAMIC(CEditEntry, CDialogEx)

CEditEntry::CEditEntry(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_ENTRY, pParent)
{

}

CEditEntry::CEditEntry(int idx)
	: CDialogEx(IDD_EDIT_ENTRY, nullptr)
{
	SelIdx = (int)AllPlain.size()-1-idx;
	printf("C_EDIT_ENTRY_DLG_IDX: %d\n",SelIdx);
	DelSymbol = false;
	NewDDL = AllPlain[SelIdx].Deadline;
	NewContent = CString(AllPlain[SelIdx].Content.c_str());
}

CEditEntry::~CEditEntry()
{
}

void CEditEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DP, m_DatePick);
	DDX_Control(pDX, IDC_EDIT_TP, m_TimePick);
	DDX_Control(pDX, IDC_EDIT_C, m_ContentEdt);
}


BEGIN_MESSAGE_MAP(CEditEntry, CDialogEx)
	ON_BN_CLICKED(IDC_DEL_ENTRY, &CEditEntry::OnBnClickedDelEntry)
	ON_BN_CLICKED(IDOK, &CEditEntry::OnBnClickedOk)
END_MESSAGE_MAP()


// CEditEntry 消息处理程序

BOOL CEditEntry::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ContentEdt.SetWindowTextW(NewContent);
	m_DatePick.SetTime(&NewDDL);
	m_TimePick.SetTime(&NewDDL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CEditEntry::OnBnClickedDelEntry()
{
	auto res = MessageBox(TEXT("您确定？这会永久删除！"),TEXT("确认"),MB_YESNO);
	if (res == IDYES) {
		DelSymbol = true;
		CDialogEx::OnOK();
	}
}

void CEditEntry::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CTime tmpDate, tmpTime;
	m_DatePick.GetTime(tmpDate);
	m_TimePick.GetTime(tmpTime);
	CTime joined(tmpDate.GetYear(), tmpDate.GetMonth(), tmpDate.GetDay(), tmpTime.GetHour(), tmpTime.GetMinute(), tmpTime.GetSecond());
	NewDDL = joined;
	m_ContentEdt.GetWindowTextW(NewContent);
	CDialogEx::OnOK();
}
