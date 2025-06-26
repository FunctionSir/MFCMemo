// MFCMemoDlg.h: 头文件
//

#pragma once


// CMFCMemoDlg 对话框
class CMFCMemoDlg : public CDialogEx
{
// 构造
public:
	CMFCMemoDlg(CWnd* pParent = nullptr);	// 标准构造函数
	UINT_PTR m_nTimer;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCMEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	afx_msg void OnAppLicense();
	CListCtrl m_MemoList;
	CEdit m_MemoFilter;
	afx_msg void OnEnChangeFilter();
	afx_msg void OnUnlock();
	afx_msg void OnOpen();
	afx_msg void OnNew();
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnExportCsv();
	afx_msg void OnNewEntry();
	afx_msg void OnClose();
	afx_msg void OnEncrypt();
	afx_msg void OnNMClickMemoList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnLock();
	virtual void OnCancel();
	afx_msg void OnExportTsv();
	afx_msg void OnExportTxt();
	afx_msg void OnSortLastEdit();
	afx_msg void OnSortEarliest();
	afx_msg void OnSortContent();
	afx_msg void OnRev();
	afx_msg void OnAutoF();
	afx_msg void OnManF();
	afx_msg void OnAppHelp();
	afx_msg void OnAppReport();
	afx_msg void OnAppGithub();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
