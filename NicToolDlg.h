// NicToolDlg.h : 头文件
//

#pragma once


// CNicToolDlg 对话框
class CNicToolDlg : public CDialog
{
// 构造
public:
	CNicToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_NICTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


private:
	static UINT Thread(LPVOID lp);
	void AddLog(CString log);
	CStringA m_curPath;
	unsigned __int64 m_uAddr;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBurn();
	afx_msg void OnBnClickedClear();
};
