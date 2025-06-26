#pragma once
#include "afxdialogex.h"


// CEncryptDlg 对话框

class CEncryptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEncryptDlg)

public:
	CEncryptDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEncryptDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENCRYPT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString PasswdGot, PasswdAgainGot;
	CEdit m_Passwd;
	CEdit m_PasswdAgain;
	afx_msg void OnBnClickedOk();
};
