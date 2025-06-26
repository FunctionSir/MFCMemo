#pragma once
#include "afxdialogex.h"


// CNewEntryDlg 对话框

class CNewEntryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewEntryDlg)

public:
	CNewEntryDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNewEntryDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEW_MEMO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTime TimeGot;
	CString ContentGot;
	CDateTimeCtrl m_DatePick;
	CDateTimeCtrl m_TimePick;
	CEdit m_Content;
	afx_msg void OnBnClickedOk();
};
