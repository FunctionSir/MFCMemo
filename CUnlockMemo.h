﻿#pragma once
#include "afxdialogex.h"


// CUnlockMemo 对话框

class CUnlockMemo : public CDialogEx
{
	DECLARE_DYNAMIC(CUnlockMemo)

public:
	CUnlockMemo(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUnlockMemo();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNLOCK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString PasswdGot;
	CEdit m_UlkPasswd;
};
