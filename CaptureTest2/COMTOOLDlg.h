//Download by http://www.NewXing.com
// COMTOOLDlg.h : header file
//
#include"SerialPort.h"
#if !defined(AFX_COMTOOLDLG_H__5384AAC6_E9BC_453F_89C6_00BE9259A9DF__INCLUDED_)
#define AFX_COMTOOLDLG_H__5384AAC6_E9BC_453F_89C6_00BE9259A9DF__INCLUDED_

#include "SerialPort.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCOMTOOLDlg dialog

class CCOMTOOLDlg : public CDialog
{
// Construction
public:
	CString DevideHexChar(char HexChar);
	char CombineHexChar(char CharH,char CharL);
	void HexStringFilter(CString &str);
	CString ChangeCharstr2Hexstr(CString Charstr);
	int m_nReceiveBytes;
	int m_nSendBytes;
	BOOL m_bStopDispRXData; //是否显示接收字符
	CSerialPort m_SerialPort;
	CCOMTOOLDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCOMTOOLDlg)
	enum { IDD = IDD_COMTOOL_DIALOG };
	CStatic	m_ctrlSendBytes;
	CStatic	m_ctrlReceiveBytes;
	CEdit	m_ctrlReceive;
	CButton	m_ctrlAddEnter;
	CButton	m_ctrlSendKey;
	CButton	m_ctrlHexS;
	CButton	m_ctrlHexR;
	CButton	m_ctrlSaveConfig;
	CButton	m_ctrlStopDisp;
	CButton	m_ctrlAutoClear;
	CStatic	m_IconCtrl;
	CStatic	m_DescriptionCtrl;
	CButton	m_SendCtrl;
	CButton	m_OpenCloseCtrl;
	CComboBox	m_StopBits;
	CComboBox	m_Parity;
	CComboBox	m_PortNO;
	CComboBox	m_BaudRate;
	CComboBox	m_DataBits;
	CEdit	m_SendPeriodCtrl;
	CString	m_strSend;
	CString	m_strReceive;
	BOOL	m_bHexR;
	BOOL	m_bHexS;
	BOOL	m_bAutoSend;
	long	m_nSendPeriod;
	CString	m_strSendBytes;
	CString	m_strReceiveBytes;
	CString	m_strPortNO;
	CString	m_strBaudRate;
	CString	m_strDataBits;
	CString	m_strStopBits;
	CString	m_strParity;
	BOOL	m_bSaveConfig;
	BOOL	m_bAutoClear;
	BOOL	m_bAddEnter;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCOMTOOLDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCOMTOOLDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnQuit();
	afx_msg void OnClearSendEdit();
	afx_msg void OnClearReceiveEdit();
	afx_msg void OnBAutoSend();
	afx_msg void OnOpenClose();
	afx_msg void OnClearCounter();
	afx_msg void OnReceiveChar(UINT ch, LONG port);
	afx_msg void OnSend();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBHexS();
	afx_msg void OnBHexR();
	afx_msg void OnMenuAbout();
	afx_msg void OnButtonRefresh();
	afx_msg void OnButtonStopdisp();
	afx_msg void OnSaveConfigure();
	afx_msg void OnCheckAutoclear();
	afx_msg void OnButtonAbout();
	afx_msg void OnSelchangeCom();
	afx_msg void OnSelchangeDataBits();
	afx_msg void OnSelchangeParityBits();
	afx_msg void OnSelchangeStopBits();
	afx_msg void OnSelchangeBaudrate();
	afx_msg void OnButtonSendKey();
	afx_msg void OnMenuSendEnter();
	afx_msg void OnMenuSendNormal();
	afx_msg void OnCheckAddEnter();
	afx_msg void OnButtonHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMTOOLDLG_H__5384AAC6_E9BC_453F_89C6_00BE9259A9DF__INCLUDED_)





















