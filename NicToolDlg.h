// NicToolDlg.h : ͷ�ļ�
//

#pragma once


// CNicToolDlg �Ի���
class CNicToolDlg : public CDialog
{
// ����
public:
	CNicToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NICTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


private:
	static UINT Thread(LPVOID lp);
	void AddLog(CString log);
	CStringA m_curPath;
	unsigned __int64 m_uAddr;
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBurn();
	afx_msg void OnBnClickedClear();
};
