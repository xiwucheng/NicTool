// NicToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NicTool.h"
#include "NicToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNicToolDlg 对话框




CNicToolDlg::CNicToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNicToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNicToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNicToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BURN, &CNicToolDlg::OnBnClickedBurn)
	ON_BN_CLICKED(IDC_CLEAR, &CNicToolDlg::OnBnClickedClear)
END_MESSAGE_MAP()


// CNicToolDlg 消息处理程序

BOOL CNicToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	char curPath[2048]={0};
	m_uAddr = 0;
	GetCurrentDirectoryA(2048,curPath);
	m_curPath=curPath;
	CStringA szPathName=m_curPath;
	szPathName += "\\mac.txt";
	FILE* fp;
	fp=fopen((LPSTR)(LPCSTR)szPathName,"r+");
	char buff[16]={0};
	wchar_t wbuff[16]={0};
	if (fp)
	{
		fgets(buff,16,fp);
		fclose(fp);
	}
	if (strlen(buff))
	{
		sscanf(buff,"%I64x",&m_uAddr);
		swprintf(wbuff,16,TEXT("%012I64X"),m_uAddr);
		SetDlgItemText(IDC_EDIT1,wbuff);
	}
	else
	{
		//MessageBox(TEXT("MAC地址文件不存在"),TEXT("读文件错误"),MB_ICONERROR);
		//PostMessage(WM_QUIT);
	}

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNicToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CNicToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT CNicToolDlg::Thread(LPVOID lp)
{
	CNicToolDlg* p = (CNicToolDlg*)lp;

	BOOL retval;
	PROCESS_INFORMATION pi={0};
	STARTUPINFOA si={0};
	SECURITY_ATTRIBUTES sa={0};
	HANDLE hReadPipe,hWritePipe;
	sa.bInheritHandle=TRUE;
	sa.nLength=sizeof SECURITY_ATTRIBUTES;
	sa.lpSecurityDescriptor=NULL;
	retval=CreatePipe(&hReadPipe,&hWritePipe,&sa,0);
	si.cb=sizeof STARTUPINFO;
	si.wShowWindow=SW_HIDE;
	si.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.hStdOutput=si.hStdError=hWritePipe;
	DWORD dwLen,dwRead,retCode=-1;
	char* fBuff;
	wchar_t * wBuff;
	char cmd[255]={0};
	CStringA szPathName=p->m_curPath;
	if (p->m_uAddr == 0)
	{
		goto end;
	}
	szPathName += "\\x86";
	sprintf(cmd,"cmd.exe /c RTUNicPG32.exe /efuse /# 0 /nodeid %012I64X",p->m_uAddr);
	retval=CreateProcessA(NULL,cmd,&sa,&sa,TRUE,0,NULL,szPathName,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//等待命令行执行完毕
		GetExitCodeProcess(pi.hProcess,&retCode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen=GetFileSize(hReadPipe,NULL);
		fBuff = new char[dwLen+1];
		wBuff = new wchar_t[dwLen+1];
		memset(fBuff,0,dwLen+1);
		memset(wBuff,0,(dwLen+1)*2);
		ReadFile(hReadPipe,fBuff,dwLen,&dwRead,NULL);
		if (dwRead)
		{
			mbstowcs(wBuff,fBuff,dwLen);
			p->AddLog(wBuff);
		}
		delete fBuff;
		CloseHandle(hWritePipe);
		CloseHandle(hReadPipe);

		if (retCode)
		{
			//strcpy(szErrMsg,"fptw.exe not found or not support current platform!");
			goto end;
		}
		FILE* fp;
		szPathName=p->m_curPath;
		szPathName += "\\mac.txt";
		fp=fopen(szPathName,"w+");
		char buff[16]={0};
		wchar_t wbuff[16]={0};
		if (fp)
		{
			p->m_uAddr++;
			sprintf(buff,"%012I64X",p->m_uAddr);
			swprintf(wbuff,16,TEXT("%012I64X"),p->m_uAddr);
			p->SetDlgItemText(IDC_EDIT1,wbuff);
			fputs(buff,fp);
			fclose(fp);
		}
	}
end:
	EnableMenuItem(::GetSystemMenu(p->m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
	p->GetDlgItem(IDC_BURN)->EnableWindow();
	return 0;
}

void CNicToolDlg::OnBnClickedBurn()
{
	// TODO: Add your control notification handler code here
	EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_DISABLED);
	GetDlgItem(IDC_BURN)->EnableWindow(0);
	CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Thread,this,0,NULL));
}

void CNicToolDlg::AddLog(CString log)
{
	CString buff,tmp;
	GetDlgItem(IDC_EDIT2)->GetWindowText(buff);
	tmp=log;
	//tmp+=TEXT("\r\n");
	buff+=tmp;
	CEdit* output=(CEdit*)GetDlgItem(IDC_EDIT2);
	output->SetWindowText(buff);
	output->LineScroll(output->GetLineCount());
}
void CNicToolDlg::OnBnClickedClear()
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_EDIT2,TEXT(""));
}
