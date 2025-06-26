#pragma once
#include "afxdialogex.h"


// CEditEntry 对话框

class CEditEntry : public CDialogEx
{
	DECLARE_DYNAMIC(CEditEntry)

public:
	CEditEntry(CWnd* pParent = nullptr);   // 标准构造函数
	CEditEntry(int idx);
	virtual ~CEditEntry();

public:
	bool DelSymbol;
	int SelIdx;
	CString NewContent;
	CTime NewDDL;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_ENTRY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl m_DatePick;
	CDateTimeCtrl m_TimePick;
	CEdit m_ContentEdt;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDelEntry();
	afx_msg void OnBnClickedOk();
};
