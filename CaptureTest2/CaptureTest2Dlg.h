
// CaptureTest2Dlg.h: 头文件
//#include "SerialPort.h"
//
#pragma once
#include "InterfaceLLT_2.h"

// CCaptureTest2Dlg 对话框
class CCaptureTest2Dlg : public CDialogEx
{
// 构造
public:
	CCaptureTest2Dlg(CWnd* pParent = nullptr);	// 标准构造函数
	//添加部分
	HANDLE m_hMVC1000;
	struct CapInfoStruct m_CapInfo;		//视频属性
	BYTE   *m_pRGBData;					//24bitRGB数据指针
	BYTE   *m_pRawData;					//用于存放RawData数据
	BOOL	m_bRawSave;
	BOOL	m_bRawToRGB;
	BOOL	m_bRGBSave;
	BITMAPINFOHEADER FrameBmi;
	HDC m_hPreviewDC;
	CRect m_clientSize;
	void InitImageParam();
	int SaveRGBAsBmp(BYTE *pSrc, LPCTSTR FileName, DWORD dwWidth, DWORD dwHeight);
	void OnTimer(UINT_PTR nIDEvent);
	//CSerialPort m_Com;


	//多线程部分
	//void ThreadFindCenter();
	//事件句柄
	HANDLE m_savebmp;
	HANDLE hThread_findcenter;
	DWORD ThreadID_findcenter;



// 对话框数据，需要去掉ifdef的结构吗
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAPTURETEST2_DIALOG };
#endif
	CString	m_strMsg;
	CString	m_strMsg_X;
	CString	m_strMsg_Y;
	int		m_nOpMode;
	CEdit m_opencom;
	BOOL m_IsOpenCom = FALSE;
	CString	m_strReceive;
	CEdit	m_ctrlReceive;
	CString m_torque;
	double	m_move_X;
	double	m_move_Y;
	double	m_move_Z;
	CString m_strMsg_gapwidth;
	CListBox m_listmsg;
	int      num_CurSel;

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

	afx_msg void OnDestroy();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnMovetocenter();
	afx_msg void OnMoveoff();
	afx_msg void OnAbbdisconnect();
	afx_msg void OnDetectcenter();
	
	afx_msg void OnUninitcamera();
	afx_msg void OnAbbConnect();
	afx_msg void OnContinue();
	afx_msg void OnTrigger();
	
	afx_msg void OnOpencom();
	afx_msg void OnStarttighten();
	afx_msg void OnStoptighten(); 
	afx_msg void OnReversetighten();

	afx_msg void OnStartLLT();
	afx_msg void OnStopLLT();
	afx_msg void OnStarttest();

	//afx_msg LRESULT OnCommunication(WPARAM ch, WPARAM port);//串口接收处理函数
	afx_msg LRESULT OnReceiveChar(WPARAM ch, LPARAM  port);
	//afx_msg LRESULT OnReceiveChar(LPCSTR nstrText, LPCSTR nstrIP, UINT nPort, unsigned nlength);

	DECLARE_MESSAGE_MAP()
};
