// NicToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NicTool.h"
#include "NicToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNicToolDlg �Ի���




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


// CNicToolDlg ��Ϣ�������

BOOL CNicToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
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
		//MessageBox(TEXT("MAC��ַ�ļ�������"),TEXT("���ļ�����"),MB_ICONERROR);
		//PostMessage(WM_QUIT);
	}

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNicToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
		WaitForSingleObject(pi.hThread,INFINITE);//�ȴ�������ִ�����
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
