// MFCMemoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCMemo.h"
#include "MFCMemoDlg.h"
#include "afxdialogex.h"
#include "CUnlockMemo.h"
#include "CEditEntry.h"
#include "crypt.h"
#include "dbops.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CEncryptDlg.h"
#include "CNewEntryDlg.h"

string DbFile;
vector<RawMemo> AllRaws;
vector<bool> Show;
vector<PlainMemo> AllPlain;
bool IsMemoSelected;
bool IsMemoEncrypted;
bool IsMemoUnlocked;
bool AutoFilter;
bool EnterPressed;
EncInfo MemoEnc;
ByteSlice IV;
bool FilterChanged;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCMemoDlg 对话框



CMFCMemoDlg::CMFCMemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCMEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCMemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMO_LIST, m_MemoList);
	DDX_Control(pDX, IDC_FILTER, m_MemoFilter);
}

BEGIN_MESSAGE_MAP(CMFCMemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_APP_ABOUT, &CMFCMemoDlg::OnAppAbout)
	ON_COMMAND(ID_APP_EXIT, &CMFCMemoDlg::OnAppExit)
	ON_COMMAND(ID_APP_LICENSE, &CMFCMemoDlg::OnAppLicense)
	ON_EN_CHANGE(IDC_FILTER, &CMFCMemoDlg::OnEnChangeFilter)
	ON_COMMAND(ID_UNLOCK, &CMFCMemoDlg::OnUnlock)
	ON_COMMAND(ID_OPEN, &CMFCMemoDlg::OnOpen)
	ON_COMMAND(ID_NEW, &CMFCMemoDlg::OnNew)
	ON_COMMAND(ID_SAVE, &CMFCMemoDlg::OnSave)
	ON_COMMAND(ID_SAVE_AS, &CMFCMemoDlg::OnSaveAs)
	ON_COMMAND(ID_EXPORT_CSV, &CMFCMemoDlg::OnExportCsv)
	ON_COMMAND(ID_NEW_ENTRY, &CMFCMemoDlg::OnNewEntry)
	ON_COMMAND(ID_CLOSE, &CMFCMemoDlg::OnClose)
	ON_COMMAND(ID_ENCRYPT, &CMFCMemoDlg::OnEncrypt)
	ON_NOTIFY(NM_CLICK, IDC_MEMO_LIST, &CMFCMemoDlg::OnNMClickMemoList)
	ON_COMMAND(ID_LOCK, &CMFCMemoDlg::OnLock)
	ON_COMMAND(ID_EXPORT_TSV, &CMFCMemoDlg::OnExportTsv)
	ON_COMMAND(ID_EXPORT_TXT, &CMFCMemoDlg::OnExportTxt)
	ON_COMMAND(ID_SORT_LAST_EDIT, &CMFCMemoDlg::OnSortLastEdit)
	ON_COMMAND(ID_SORT_EARLIEST, &CMFCMemoDlg::OnSortEarliest)
	ON_COMMAND(ID_SORT_CONTENT, &CMFCMemoDlg::OnSortContent)
	ON_COMMAND(ID_REV, &CMFCMemoDlg::OnRev)
	ON_COMMAND(ID_AUTO_F, &CMFCMemoDlg::OnAutoF)
	ON_COMMAND(ID_MAN_F, &CMFCMemoDlg::OnManF)
	ON_COMMAND(ID_APP_HELP, &CMFCMemoDlg::OnAppHelp)
	ON_COMMAND(ID_APP_REPORT, &CMFCMemoDlg::OnAppReport)
	ON_COMMAND(ID_APP_GITHUB, &CMFCMemoDlg::OnAppGithub)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMFCMemoDlg 消息处理程序

BOOL CMFCMemoDlg::OnInitDialog()
{
#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$","w",stdout);
#endif

	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	IV = (ByteSlice)"123456789012345"; // IV is 16Byte long, 15 chars with '/0'.
	FilterChanged = false;
	AutoFilter = true;
	IsMemoSelected = IsMemoEncrypted = IsMemoUnlocked = false;
	m_MemoList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_MemoFilter.SetWindowText(TEXT("[ 您尚未打开文件 ]"));
	m_MemoFilter.EnableWindow(false);
	m_MemoList.EnableWindow(false);
	m_MemoList.InsertColumn(0, TEXT("创建时间"),0,150);
	m_MemoList.InsertColumn(1, TEXT("完成时限"), 0, 150);
	m_MemoList.InsertColumn(3, TEXT("内容"),0,500);
	m_nTimer = SetTimer(1, 10000, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCMemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCMemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCMemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCMemoDlg::OnAppAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CMFCMemoDlg::OnAppHelp()
{
	ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/FunctionSir/MFCMemo/blob/master/README.md"), nullptr, nullptr, SW_SHOWNORMAL);
}

void CMFCMemoDlg::OnAppLicense()
{
	ShellExecute(nullptr,TEXT("open"),TEXT("https://www.gnu.org/licenses/gpl-3.0.html"),nullptr,nullptr,SW_SHOWNORMAL);
}

void CMFCMemoDlg::OnAppGithub()
{
	ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/FunctionSir/MFCMemo"), nullptr, nullptr, SW_SHOWNORMAL);
}

void CMFCMemoDlg::OnAppReport()
{
	ShellExecute(nullptr, TEXT("open"), TEXT("https://github.com/FunctionSir/MFCMemo/issues"), nullptr, nullptr, SW_SHOWNORMAL);
}

void AddItem(CListCtrl &target, Int64 tsCreate,Int64 tsDDL, string content){
	time_t tsC=tsCreate;
	time_t tsD = tsDDL;
	CTime localTimeC(tsC);
	CTime localTimeD(tsD);
	int idx = target.InsertItem(0, localTimeC.Format(TEXT("%Y-%m-%d %H:%M:%S")));
	if (idx == -1) {
		return;
	}
	target.SetItemText(idx, 1, localTimeD.Format(TEXT("%Y-%m-%d %H:%M:%S")));
	target.SetItemText(idx, 2, CString(content.c_str()));
}

void UpdateList(CListCtrl& target){
	target.DeleteAllItems();
	for (size_t i=0; i<AllPlain.size();i++) {
		if (Show[i]) {
			AddItem(target, AllPlain[i].Created, AllPlain[i].Deadline, AllPlain[i].Content);
		}
	}
}

void ClearAllRaw() {
	for (auto x : AllRaws) {
		SafeFree(x.RawContent.Data, x.RawContent.Size);
	}
	AllRaws.clear();
}

void ClearAllPlain() {
	for (auto x : AllPlain) {
		SafeClean((ByteSlice)x.Content.c_str(), x.Content.size());
	}
	Show.clear();
	AllPlain.clear();
}

void ClearAll(CListCtrl &list, CEdit &filter) {
	SafeClean((ByteSlice)&MemoEnc, sizeof(MemoEnc));
	ClearAllPlain();
	ClearAllRaw();
	Show.clear();
	list.DeleteAllItems();
	IsMemoSelected = IsMemoEncrypted = IsMemoUnlocked = false;
	DbFile = "";
	filter.SetWindowText(TEXT("[ 您尚未打开文件 ]"));
	filter.EnableWindow(false);
	list.EnableWindow(false);
}

void UpdAllRaw() {
	if (!IsMemoUnlocked) {
		return;
	}
	ClearAllRaw();
	RawMemo tmp;
	ByteSlice buf = (ByteSlice)malloc(1024 * 1024 * 2);
	if (MemoEnc.Enc) {
		int i = 0;
		for (auto x : AllPlain) {
			i++;
			tmp.Id = i;
			tmp.Created = x.Created;
			tmp.Deadline = x.Deadline;
			tmp.RawContent.Size = Aes256CbcEnc((ByteSlice)x.Content.data(), x.Content.size(), buf, MemoEnc.Key, IV);
			tmp.RawContent.Data = (ByteSlice)malloc(tmp.RawContent.Size);
			if (buf!=nullptr&& tmp.RawContent.Data!=nullptr) {
				memcpy(tmp.RawContent.Data, buf, tmp.RawContent.Size);
			}else {
				throw runtime_error("fail to malloc");
			}
			AllRaws.push_back(tmp);
		}
	}
	else {
		int i = 0;
		for (auto x : AllPlain) {
			i++;
			tmp.Id = i;
			tmp.Created = x.Created;
			tmp.Deadline = x.Deadline;
			tmp.RawContent.Size = x.Content.size();
			tmp.RawContent.Data = (ByteSlice)malloc(tmp.RawContent.Size);
			if(tmp.RawContent.Data!=nullptr)
			{
				memcpy(tmp.RawContent.Data, x.Content.data(), tmp.RawContent.Size);
			}
			else {
				throw runtime_error("fail to malloc");
			}
			AllRaws.push_back(tmp);
		}
	}
	SafeFree(buf, 1024 * 1024 * 2);
}

void UpdAllPlain() {
	if (!IsMemoSelected) {
		return;
	}
	PlainMemo tmp;
	if (MemoEnc.Enc) {
		if (MemoEnc.Key == nullptr) {
			return;
		}
		ByteSlice buf;
		int len;
		buf = (ByteSlice)malloc(1024 * 1024 * 3);
		ClearAllPlain();
		Show.clear();
		for (auto& x : AllRaws) {
			tmp.Created = x.Created;
			tmp.Deadline = x.Deadline;
			len = Aes256CbcDec(x.RawContent.Data, x.RawContent.Size, buf, MemoEnc.Key, IV);
			if (buf!=nullptr)
			{
				tmp.Content = string(reinterpret_cast<const char*>(buf), len);
			}else {
				throw runtime_error("fail to malloc");
			}
			AllPlain.push_back(tmp);
			Show.push_back(true);
		}
	}
	else {
		ClearAllPlain();
		Show.clear();
		for (auto& x : AllRaws) {
			tmp.Created = x.Created;
			tmp.Deadline = x.Deadline;
			tmp.Content = string(reinterpret_cast<const char*>(x.RawContent.Data), x.RawContent.Size);
			AllPlain.push_back(tmp);
			Show.push_back(true);
		}
	}
}

void CMFCMemoDlg::OnOpen()
{
	if (IsMemoSelected && IsMemoUnlocked) {
		UpdAllRaw();
		SaveToDb();
	}
	CFileDialog openFile(true);
	if (openFile.DoModal()==IDOK){
		ClearAll(m_MemoList, m_MemoFilter);
		// CT2A is not supported by some old versions of VS.
		DbFile = CT2A(openFile.GetPathName());
		if (!IsFileGood(DbFile)){
			MessageBox(TEXT("文件状态不正常"), TEXT("消息"));
			return;
		}
		IsMemoSelected = true;
		auto res=GetRawMemos(AllRaws,MemoEnc);
		printf("CHK_ENC_UNLOCK_OPEN:");
		for (int i = 0; i < MemoEnc.ChkLen; i++) {
			printf("%X", MemoEnc.ChkEnc[i]);
		}
		puts("");
		if (res != MEMO_OP_OK) {
			IsMemoSelected = false;
			DbFile = "";
			MessageBox(TEXT("无法加载数据库（文件类型非法？）"), TEXT("消息"));
		}
		if (AllRaws.size() > 100) {
			MessageBox(TEXT("由于备忘录条目较多, 自动筛选已禁用, 请输入后按回车筛选 (若您的计算机性能足够, 可以自行手动启用)"), TEXT("消息"));
			AutoFilter = false;
		}
		IsMemoEncrypted = MemoEnc.Enc; IsMemoUnlocked = MemoEnc.Enc?false:true;
		if (IsMemoEncrypted) {
			m_MemoFilter.SetWindowText(TEXT("[ 该备忘录是被加密的 ]"));
			return;
		}
		UpdAllPlain();
		UpdateList(m_MemoList);
		m_MemoFilter.SetWindowText(TEXT(""));
		m_MemoFilter.SetCueBanner(TEXT("[ 输入文本以筛选 | 用!开头启用正则表达式 | 用+开头来防止启用特殊筛选 ]"));
		m_MemoFilter.EnableWindow(true);
		m_MemoList.EnableWindow(true);
	}
}

void CMFCMemoDlg::OnNew()
{
	if (IsMemoSelected&&IsMemoUnlocked) {
		UpdAllRaw();
		SaveToDb();
	}
	CFileDialog openFile(false);
	if (openFile.DoModal() == IDOK) {
		ClearAll(m_MemoList,m_MemoFilter);
		DbFile = CT2A(openFile.GetPathName());
		IsMemoSelected = true;
		auto res = NewMemosDb();
		if (res != MEMO_OP_OK) {
			IsMemoSelected = false;
			DbFile = "";
			MessageBox(TEXT("无法创建新数据库(文件已存在？)"), TEXT("消息"));
			return;
		}
		MemoEnc.ChkSha256 = nullptr;
		MemoEnc.ChkEnc =nullptr;
		MemoEnc.ChkLen = 0;
		MemoEnc.Key = nullptr;
		MemoEnc.Enc = false;
		IsMemoEncrypted = false; IsMemoUnlocked = true;
		m_MemoFilter.SetWindowText(TEXT(""));
		m_MemoFilter.SetCueBanner(TEXT("[ 输入文本以筛选 | 用!开头启用正则表达式 | 用+开头来防止启用特殊筛选 ]"));
		m_MemoFilter.EnableWindow(true);
		m_MemoList.EnableWindow(true);
	}
}



void CMFCMemoDlg::OnSaveAs()
{
	if (!IsMemoSelected||!IsMemoUnlocked) {
		MessageBox(TEXT("您需要首先打开且解锁文件"), TEXT("消息"));
		return;
	}
	CFileDialog openFile(false);
	CString saveas;
	string oldDbFile = DbFile;
	if (openFile.DoModal() == IDOK) {
		saveas=openFile.GetPathName();
		DbFile = CT2A(saveas);
		if (NewMemosDb() != MEMO_OP_OK) {
			DbFile = oldDbFile;
			MessageBox(TEXT("另存失败（文件已存在？）"), TEXT("消息"));
			return;
		}
		if (SaveToDb() != MEMO_OP_OK) {
			DbFile = oldDbFile;
			MessageBox(TEXT("另存失败"), TEXT("消息"));
			return;
		}
		MessageBox(TEXT("另存成功"), TEXT("消息"));
	}
}

void CMFCMemoDlg::OnNewEntry()
{
	if (!IsMemoSelected) {
		MessageBox(TEXT("您需要首先打开文件"), TEXT("消息"));
		return;
	}
	if (!IsMemoUnlocked) {
		MessageBox(TEXT("您需要先解锁文件"), TEXT("消息"));
		return;
	}
	CNewEntryDlg dlg;
	string content;
	CTime time=CTime::GetCurrentTime();
	PlainMemo tmp;
	time_t now = time.GetTime();
	if (dlg.DoModal() == IDOK) {
		time = dlg.TimeGot;
		content = CT2A(dlg.ContentGot);
		tmp = {(Int64)AllPlain.size() + 1,(Int64)now,(Int64)time.GetTime(),content};
		Show.push_back(true);
		AllPlain.push_back(tmp);
		UpdateList(m_MemoList);
	}
}

void CMFCMemoDlg::OnExportCsv()
{
	if (!IsMemoSelected) {
		MessageBox(TEXT("您需要首先打开文件"), TEXT("消息"));
		return;
	}
	if (!IsMemoUnlocked) {
		MessageBox(TEXT("您需要先解锁文件"), TEXT("消息"));
		return;
	}
	CFileDialog openFile(false);
	if (openFile.DoModal() == IDOK) {
		ofstream ofs(openFile.GetPathName());
		if (!ofs.good()) {
			MessageBox(TEXT("文件状态有问题"), TEXT("消息"));
			return;
		}
		for (auto x : AllPlain) {
			CTime createdAt(x.Created);
			CTime DDL(x.Deadline);
			ofs << CT2A(createdAt.Format(TEXT("%Y-%m-%d %H:%M:%S"))) << ","
				<< CT2A(DDL.Format(TEXT("%Y-%m-%d %H:%M:%S"))) << ","
				<< x.Content << endl;
		}
		ofs.close();
	}
}


void CMFCMemoDlg::OnExportTsv()
{
	if (!IsMemoSelected) {
		MessageBox(TEXT("您需要首先打开文件"), TEXT("消息"));
		return;
	}
	if (!IsMemoUnlocked) {
		MessageBox(TEXT("您需要先解锁文件"), TEXT("消息"));
		return;
	}
	CFileDialog openFile(false);
	if (openFile.DoModal() == IDOK) {
		ofstream ofs(openFile.GetPathName());
		if (!ofs.good()) {
			MessageBox(TEXT("文件状态有问题"), TEXT("消息"));
			return;
		}
		for (auto x : AllPlain) {
			CTime createdAt(x.Created);
			CTime DDL(x.Deadline);
			ofs << CT2A(createdAt.Format(TEXT("%Y-%m-%d %H:%M:%S"))) << "\t"
				<< CT2A(DDL.Format(TEXT("%Y-%m-%d %H:%M:%S"))) << "\t"
				<< x.Content << endl;
		}
		ofs.close();
	}

}

void CMFCMemoDlg::OnExportTxt()
{
	if (!IsMemoSelected) {
		MessageBox(TEXT("您需要首先打开文件"), TEXT("消息"));
		return;
	}
	if (!IsMemoUnlocked) {
		MessageBox(TEXT("您需要先解锁文件"), TEXT("消息"));
		return;
	}
	CFileDialog openFile(false);
	if (openFile.DoModal() == IDOK) {
		ofstream ofs(openFile.GetPathName());
		if (!ofs.good()) {
			MessageBox(TEXT("文件状态有问题"), TEXT("消息"));
			return;
		}
		for (auto x : AllPlain) {
			CTime createdAt(x.Created);
			CTime DDL(x.Deadline);
			ofs << "创建于: " << CT2A(createdAt.Format(TEXT("%Y-%m-%d %H:%M:%S"))) << "  "
				<< "截止日期：" << CT2A(DDL.Format(TEXT("%Y-%m-%d %H:%M:%S"))) << endl
				<< x.Content << endl << endl;
		}
		ofs.close();
	}
}


void CMFCMemoDlg::OnClose()
{
	if (IsMemoSelected && IsMemoUnlocked) {
		UpdAllRaw();
		SaveToDb();
	}
	ClearAll(m_MemoList,m_MemoFilter);
}



void CMFCMemoDlg::OnEncrypt() {
	if (!IsMemoSelected) {
		MessageBox(TEXT("您需要首先打开文件"), TEXT("消息"));
		return;
	}
	if (!IsMemoUnlocked) {
		MessageBox(TEXT("未解锁的文件无法重加密"), TEXT("消息"));
		return;
	}
	CEncryptDlg dlg;
	CString CstrPwd;
	string passwd;
	auto ret = dlg.DoModal();
	if (ret == IDOK) {
		CstrPwd = dlg.PasswdGot;
		passwd = CT2A(CstrPwd);
		string chkStr = GenSafeRandStr(16);
		MemoEnc.Salt = GenSafeRandStr(16);
		if (MemoEnc.Salt == "") {
			MessageBox(TEXT("盐生成失败"), TEXT("消息"));
			return;
		}
		MemoEnc.Key =Sha256PasswdWithSalt(passwd, MemoEnc.Salt);
		if (MemoEnc.ChkSha256!=nullptr){
			SafeFree(MemoEnc.ChkSha256,32);
		}
		MemoEnc.ChkSha256 = (ByteSlice)malloc(32);
		SHA256((ByteSlice)chkStr.data(), chkStr.size(), MemoEnc.ChkSha256);
		ByteSlice buf;
		buf = (ByteSlice)malloc(1024 * 1024 * 2);
		MemoEnc.ChkLen = Aes256CbcEnc((ByteSlice)chkStr.data(), chkStr.size(), buf, MemoEnc.Key, IV);
		MemoEnc.ChkEnc = (ByteSlice)malloc(MemoEnc.ChkLen);
		if(buf!=nullptr&& MemoEnc.ChkEnc!=nullptr){
			memcpy(MemoEnc.ChkEnc, buf, MemoEnc.ChkLen);
		}
		else {
			throw runtime_error("fail to malloc");
		}
		MemoEnc.Enc = true;
		SafeFree(buf,1024*1024*2);
		UpdAllRaw();
		SaveToDb();
	}
}

void CMFCMemoDlg::OnSave()
{
	if (!IsMemoSelected) {
		MessageBox(TEXT("您需要首先打开文件"), TEXT("消息"));
		return;
	}
	if (!IsMemoUnlocked) {
		MessageBox(TEXT("需要先解锁备忘录"), TEXT("消息"));
		return;
	}
	UpdAllRaw();
	SaveToDb();
}



void CMFCMemoDlg::OnEnChangeFilter()
{
	FilterChanged = true;
	if (!AutoFilter && !EnterPressed) {
		return;
	}
	CString tmp;
	m_MemoFilter.GetWindowTextW(tmp);
	if (tmp.GetLength() == 0) {
		for (int i = 0; i < Show.size();i++) {
			Show[i] = true;
		}
		UpdateList(m_MemoList);
		return;
	}
	if (tmp[0]!='!'){
		if (tmp[0]=='+') {
			tmp.Delete(0, 1);
		}
		for (int i = 0; i < Show.size();i++) {
			if (AllPlain[i].Content.find(CT2A(tmp))==string::npos){
				Show[i] = false;
			}else{
				Show[i] = true;
			}
		}
	} else {
		tmp.Delete(0, 1);
		try {
			regex pat(string(CT2A(tmp)).c_str());
			for (int i = 0; i < Show.size();i++) {
				if (!regex_search(AllPlain[i].Content, pat)) {
					Show[i] = false;
				}
				else {
					Show[i] = true;
				}
			}
		} catch (exception e) { return; }
	}
	UpdateList(m_MemoList);
	FilterChanged = false;
}

void CMFCMemoDlg::OnUnlock()
{
	if (!IsMemoSelected) {
		MessageBox(TEXT("您需要首先打开文件"), TEXT("消息"));
		return;
	}
	if (IsMemoUnlocked) {
		MessageBox(TEXT("该备忘录没有加密或已经解锁了"), TEXT("消息"));
		return;
	}
	CUnlockMemo unlock;
	auto ret = unlock.DoModal();
	CString passwd;
	if (ret == IDOK) {
		passwd = unlock.PasswdGot;
		string tmp = CT2A(passwd);
		ByteSlice keyCalced = Sha256PasswdWithSalt(tmp, MemoEnc.Salt);
		ByteSlice plainChk = (ByteSlice)malloc(1024);
		// === //
		printf("CHK_ENC_UNLOCK:");
		for (int i = 0; i < MemoEnc.ChkLen; i++) {
			printf("%X", MemoEnc.ChkEnc[i]);
		}
		puts("");
		// === //
		int plainLen = Aes256CbcDec(MemoEnc.ChkEnc, MemoEnc.ChkLen, plainChk, keyCalced, IV);
		if (plainLen == -1) {
			MessageBox(TEXT("密码错误！"), TEXT("消息"));
			free(keyCalced);
			free(plainChk);
			return;
		}
		ByteSlice sha256Plain = (ByteSlice)malloc(32);
		SHA256(plainChk, plainLen, sha256Plain);
		bool flg = true;
		for (size_t i = 0; i < 32; i++) {
			if (sha256Plain == 0 || sha256Plain[i] != MemoEnc.ChkSha256[i]) {
				flg = false;
				break;
			}
		}
		if (!flg) {
			MessageBox(TEXT("密码错误！"), TEXT("消息"));
			free(keyCalced);
			free(plainChk);
			free(sha256Plain);
			return;
		}
		MemoEnc.Key = keyCalced;
		free(plainChk);
		free(sha256Plain);
		UpdAllPlain();
		UpdateList(m_MemoList);
		m_MemoFilter.SetWindowText(TEXT(""));
		m_MemoFilter.SetCueBanner(TEXT("[ 输入文本以筛选 | 用!开头启用正则表达式 | 用+开头来防止启用特殊筛选 ]"));
		m_MemoFilter.EnableWindow(true);
		m_MemoList.EnableWindow(true);
		IsMemoUnlocked = true;
	}
}

void CMFCMemoDlg::OnNMClickMemoList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = pNMItemActivate->iItem;
	if (nItem != -1) {
		printf("CLICK_ON_NMLIST: %d\n",nItem);
		CEditEntry dlg(nItem);
		if(dlg.DoModal()==IDOK){
			if (dlg.DelSymbol) {
				AllPlain.erase(AllPlain.begin()+dlg.SelIdx);
				Show.erase(Show.begin()+dlg.SelIdx);
				UpdateList(m_MemoList);
				return;
			}
			AllPlain[dlg.SelIdx].Created = CTime::GetCurrentTime().GetTime();
			AllPlain[dlg.SelIdx].Deadline = dlg.NewDDL.GetTime();
			AllPlain[dlg.SelIdx].Content = CT2A(dlg.NewContent);
			UpdateList(m_MemoList);
		}
	}
	*pResult = 0;
}

void CMFCMemoDlg::OnOK()
{
	if (AutoFilter == false) {
		EnterPressed = true;
		OnEnChangeFilter();
	}
	EnterPressed = false;
	return;
}

void CMFCMemoDlg::OnLock()
{
	if (!IsMemoUnlocked) {
		return;
	}
	if (!MemoEnc.Enc) {
		MessageBox(TEXT("无法锁定未加密的备忘录"),TEXT("消息"));
		return;
	}
	if (IsMemoSelected && IsMemoUnlocked) {
		UpdAllRaw();
		SaveToDb();
	}
	ClearAllPlain();
	SafeFree(MemoEnc.Key,32);
	m_MemoList.DeleteAllItems();
	IsMemoUnlocked = false;
	m_MemoFilter.SetWindowText(TEXT("[ 该备忘录是被加密的 ]"));
	m_MemoFilter.EnableWindow(false);
	m_MemoList.EnableWindow(false);
}

void CMFCMemoDlg::OnCancel()
{
	if (IsMemoSelected && IsMemoUnlocked) {
		UpdAllRaw();
		SaveToDb();
	}
	CDialogEx::OnCancel();
}

void CMFCMemoDlg::OnAppExit()
{
	if (IsMemoSelected && IsMemoUnlocked) {
		UpdAllRaw();
		SaveToDb();
	}
	PostQuitMessage(0);
}


bool cmpRevLastEdit(const PlainMemo &a, const PlainMemo& b) {
	return a.Created < b.Created;
}

bool cmpRevEarlist(const PlainMemo& a, const PlainMemo& b) {
	return a.Deadline > b.Deadline;
}

bool cmpRevContent(const PlainMemo& a, const PlainMemo& b) {
	return a.Content > b.Content;
}


void CMFCMemoDlg::OnSortLastEdit()
{
	if (!IsMemoSelected || !IsMemoUnlocked) {
		MessageBox(TEXT("需要先打开并解锁备忘录"), TEXT("消息"));
		return;
	}
	sort(AllPlain.begin(), AllPlain.end(), cmpRevLastEdit);
	UpdateList(m_MemoList);
}

void CMFCMemoDlg::OnSortEarliest()
{
	if (!IsMemoSelected || !IsMemoUnlocked) {
		MessageBox(TEXT("需要先打开并解锁备忘录"), TEXT("消息"));
		return;
	}
	sort(AllPlain.begin(), AllPlain.end(), cmpRevLastEdit);
	UpdateList(m_MemoList);
}


void CMFCMemoDlg::OnSortContent()
{
	if (!IsMemoSelected || !IsMemoUnlocked) {
		MessageBox(TEXT("需要先打开并解锁备忘录"), TEXT("消息"));
		return;
	}
	sort(AllPlain.begin(), AllPlain.end(), cmpRevContent);
	UpdateList(m_MemoList);
}

void CMFCMemoDlg::OnRev()
{
	if (!IsMemoSelected || !IsMemoUnlocked) {
		MessageBox(TEXT("需要先打开并解锁备忘录"), TEXT("消息"));
		return;
	}
	reverse(AllPlain.begin(), AllPlain.end());
	UpdateList(m_MemoList);
}

void CMFCMemoDlg::OnAutoF()
{
	AutoFilter = true;
	MessageBox(TEXT("自动筛选已启用"), TEXT("消息"));
}

void CMFCMemoDlg::OnManF()
{
	AutoFilter = false;
	MessageBox(TEXT("自动筛选已禁用, 请输入后按回车筛选"), TEXT("消息"));

}
void CMFCMemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (IsMemoSelected&&IsMemoUnlocked) {
		OnSave();
	}
	CDialogEx::OnTimer(nIDEvent);
}
