// COMTOOLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "COMTOOL.h"
#include "COMTOOLDlg.h"
#include "MyHelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EditBoxWidth 50

int BaudRate[]={300,600,1200,2400,4800,9600,14400,19200,38400,56000,57600,115200};
int ParitySelNum=5;
CString Parity[]={_T("None"),_T("Odd"),_T("Even"),_T("Mark"),_T("Space")};
int DataBits[]={5,6,7,8};
int StopBits[]={1,2};

extern int m_nComArray[20];
char ChD[2]={-1,-1};///用于暂存汉字的数组
bool SendByEnter = FALSE;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for pp About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// class wizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/ADV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCOMTOOLDlg dialog

CCOMTOOLDlg::CCOMTOOLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCOMTOOLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCOMTOOLDlg)
	m_strSend = _T("");
	m_strReceive = _T("");
	m_bHexR = false;///默认为字符
	m_bHexS = false;
	m_bAutoSend = FALSE;
	m_nSendPeriod = 1000;
	m_strSendBytes = _T("TX:0");
	m_strReceiveBytes = _T("RX:0");
	m_strPortNO = _T("");
	m_strBaudRate = _T("");
	m_strDataBits = _T("");
	m_strStopBits = _T("");
	m_strParity = _T("");
	m_bSaveConfig = TRUE;
	m_bAutoClear = TRUE;
	m_bAddEnter = FALSE;
	m_bStopDispRXData=FALSE;
	m_nSendBytes=0;
	m_nReceiveBytes=0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCOMTOOLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCOMTOOLDlg)
	DDX_Control(pDX, IDC_SendBytes, m_ctrlSendBytes);
	DDX_Control(pDX, IDC_ReceiveBytes, m_ctrlReceiveBytes);
	DDX_Control(pDX, IDC_ReceiveEdit, m_ctrlReceive);
	DDX_Control(pDX, IDC_CHECK_ADD_ENTER, m_ctrlAddEnter);
	DDX_Control(pDX, IDC_BUTTON_SEND_KEY, m_ctrlSendKey);
	DDX_Control(pDX, IDC_BHexS, m_ctrlHexS);
	DDX_Control(pDX, IDC_BHexR, m_ctrlHexR);
	DDX_Control(pDX, IDC_SAVE_CONFIGURE, m_ctrlSaveConfig);
	DDX_Control(pDX, IDC_BUTTON_STOPDISP, m_ctrlStopDisp);
	DDX_Control(pDX, IDC_CHECK_AUTOCLEAR, m_ctrlAutoClear);
	DDX_Control(pDX, IDC_ICON_S, m_IconCtrl);
	DDX_Control(pDX, IDC_Description, m_DescriptionCtrl);
	DDX_Control(pDX, IDC_Send, m_SendCtrl);
	DDX_Control(pDX, IDC_OpenClose, m_OpenCloseCtrl);
	DDX_Control(pDX, IDC_StopBits, m_StopBits);
	DDX_Control(pDX, IDC_ParityBits, m_Parity);
	DDX_Control(pDX, IDC_COM, m_PortNO);
	DDX_Control(pDX, IDC_Baudrate, m_BaudRate);
	DDX_Control(pDX, IDC_DataBits, m_DataBits);
	DDX_Control(pDX, IDC_SendPeriod, m_SendPeriodCtrl);
	DDX_Text(pDX, IDC_SendEdit, m_strSend);
	DDV_MaxChars(pDX, m_strSend, 1024);
	DDX_Text(pDX, IDC_ReceiveEdit, m_strReceive);
	DDX_Check(pDX, IDC_BHexR, m_bHexR);
	DDX_Check(pDX, IDC_BHexS, m_bHexS);
	DDX_Check(pDX, IDC_BAutoSend, m_bAutoSend);
	DDX_Text(pDX, IDC_SendPeriod, m_nSendPeriod);
	DDX_Text(pDX, IDC_SendBytes, m_strSendBytes);
	DDX_Text(pDX, IDC_ReceiveBytes, m_strReceiveBytes);
	DDX_CBString(pDX, IDC_COM, m_strPortNO);
	DDX_CBString(pDX, IDC_Baudrate, m_strBaudRate);
	DDX_CBString(pDX, IDC_DataBits, m_strDataBits);
	DDX_CBString(pDX, IDC_StopBits, m_strStopBits);
	DDX_CBString(pDX, IDC_ParityBits, m_strParity);
	DDX_Check(pDX, IDC_SAVE_CONFIGURE, m_bSaveConfig);
	DDX_Check(pDX, IDC_CHECK_AUTOCLEAR, m_bAutoClear);
	DDX_Check(pDX, IDC_CHECK_ADD_ENTER, m_bAddEnter);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCOMTOOLDlg, CDialog)
	//{{AFX_MSG_MAP(CCOMTOOLDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_QUIT, OnQuit)
	ON_BN_CLICKED(IDC_ClearS, OnClearSendEdit)
	ON_BN_CLICKED(IDC_ClearR, OnClearReceiveEdit)
	ON_BN_CLICKED(IDC_BAutoSend, OnBAutoSend)
	ON_BN_CLICKED(IDC_OpenClose, OnOpenClose)
	ON_BN_CLICKED(IDC_ClearCounter, OnClearCounter)
	ON_MESSAGE(WM_COMM_RXCHAR,OnReceiveChar)
	ON_BN_CLICKED(IDC_Send, OnSend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BHexS, OnBHexS)
	ON_BN_CLICKED(IDC_BHexR, OnBHexR)
	ON_COMMAND(IDC_MENU_ABOUT, OnMenuAbout)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_STOPDISP, OnButtonStopdisp)
	ON_BN_CLICKED(IDC_SAVE_CONFIGURE, OnSaveConfigure)
	ON_BN_CLICKED(IDC_CHECK_AUTOCLEAR, OnCheckAutoclear)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, OnButtonAbout)
	ON_CBN_SELCHANGE(IDC_COM, OnSelchangeCom)
	ON_CBN_SELCHANGE(IDC_DataBits, OnSelchangeDataBits)
	ON_CBN_SELCHANGE(IDC_ParityBits, OnSelchangeParityBits)
	ON_CBN_SELCHANGE(IDC_StopBits, OnSelchangeStopBits)
	ON_CBN_SELCHANGE(IDC_Baudrate, OnSelchangeBaudrate)
	ON_BN_CLICKED(IDC_BUTTON_SEND_KEY, OnButtonSendKey)
	ON_COMMAND(ID_MENU_SEND_ENTER, OnMenuSendEnter)
	ON_COMMAND(ID_MENU_SEND_NORMAL, OnMenuSendNormal)
	ON_BN_CLICKED(IDC_CHECK_ADD_ENTER, OnCheckAddEnter)
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnButtonHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCOMTOOLDlg message handlers

BOOL CCOMTOOLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here


	m_strSend = "http://blog.csdn.net/itas109";
	UpdateData(FALSE);

	m_SendPeriodCtrl.EnableWindow(m_bAutoSend);
	m_OpenCloseCtrl.SetWindowText(_T("打开串口"));
	m_DescriptionCtrl.SetWindowText("串口未打开");
	if(m_bHexS)
		GetDlgItem(IDC_SendEdit)->ModifyStyle(0,ES_UPPERCASE,0);
	else
		GetDlgItem(IDC_SendEdit)->ModifyStyle(ES_UPPERCASE,0,0);

	if(m_bHexR)
		GetDlgItem(IDC_ReceiveEdit)->ModifyStyle(0,ES_UPPERCASE,0);
	else
		GetDlgItem(IDC_ReceiveEdit)->ModifyStyle(ES_UPPERCASE,0,0);

	CString temp;
	
	//添加波特率到下拉列表
	for(int i=0;i<sizeof(BaudRate)/sizeof(int);i++)
	{
		temp.Format("%d",BaudRate[i]);
		m_BaudRate.AddString((LPCTSTR)temp);
	}
		
	
	//添加显示奇偶校验到下拉列表
	for (i=0;i<ParitySelNum;i++)
	{
		m_Parity.AddString((LPCTSTR) Parity[i]);
	}
	
	//添加显示停止位到下拉列表
	for(i=0;i<sizeof(StopBits)/sizeof(int);i++)
	{
		temp.Format("%d",StopBits[i]);
		m_StopBits.AddString((LPCTSTR)temp);
	}
	
	//添加显示数据位到下拉列表
	for(i=0;i<sizeof(DataBits)/sizeof(int);i++)
	{
		temp.Format("%d",DataBits[i]);
		m_DataBits.AddString((LPCTSTR)temp);
	}

/************************************************************************/
	///自动保存配置

	int tempKey,IsSaveConfig;

	IsSaveConfig = AfxGetApp()->GetProfileInt("SerialPortInfo","SaveConfig",-1);///没有该键值时，返回-1

	m_ctrlSaveConfig.SetCheck(m_bSaveConfig);

	if (IsSaveConfig == 0)///如果不保存配置，把控件值改为0
	{
		m_bSaveConfig = 0;

		m_ctrlSaveConfig.SetCheck(m_bSaveConfig);
	}

	if(IsSaveConfig == 1)///如果保存配置，按照上次配置信息设置
	{
		tempKey = AfxGetApp()->GetProfileInt("SerialPortInfo","BaudRate",5);///波特率 返回5，应该不会出现。第一次运行时，不执行读操作
		m_BaudRate.SetCurSel(tempKey);
		
		tempKey = AfxGetApp()->GetProfileInt("SerialPortInfo","Parity",0);///奇偶校验
		m_Parity.SetCurSel(tempKey);
		
		tempKey = AfxGetApp()->GetProfileInt("SerialPortInfo","StopBits",0);///停止位
		m_StopBits.SetCurSel(tempKey);
		
		tempKey = AfxGetApp()->GetProfileInt("SerialPortInfo","DataBits",3);///数据位
		m_DataBits.SetCurSel(tempKey);


		m_bHexS = AfxGetApp()->GetProfileInt("SerialPortInfo","HexS",0);///十六进制发送
		m_ctrlHexS.SetCheck(m_bHexS);
			
		m_bHexR = AfxGetApp()->GetProfileInt("SerialPortInfo","HexR",0);///十六进制接收
		m_ctrlHexR.SetCheck(m_bHexR);
		
		m_bAutoClear = AfxGetApp()->GetProfileInt("SerialPortInfo","AutoClear",1);///自动清空
		m_ctrlAutoClear.SetCheck(m_bAutoClear);

		SendByEnter = AfxGetApp()->GetProfileInt("SerialPortInfo","SendEnter",0);///按回车发送数据
		if (SendByEnter)
			OnMenuSendEnter();

	}
	else/// IsSaveConfig = 1和IsSaveConfig = 0的情况，也就是第一次运行和没有保存配置的情况，按照默认进行配置
	{
		temp.Format("%d",9600);
		m_BaudRate.SetCurSel(m_BaudRate.FindString(0,temp));
		
		m_Parity.SetCurSel(m_Parity.FindString(0,_T("None")));
		
		temp.Format("%d",1);
		m_StopBits.SetCurSel(m_StopBits.FindString(0,(LPCTSTR)temp));
		
		temp.Format("%d",8);
		m_DataBits.SetCurSel(m_DataBits.FindString(0,(LPCTSTR)temp));

		m_ctrlHexS.SetCheck(m_bHexS);

		m_ctrlHexR.SetCheck(m_bHexR);

		m_ctrlAutoClear.SetCheck(m_bAutoClear);///自动清空


		
	}
/************************************************************************/	
	m_SerialPort.Hkey2ComboBox(m_PortNO);///COM口初始化到CComboBox

	OnOpenClose();///开启时，自动打开第一个端口

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCOMTOOLDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCOMTOOLDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCOMTOOLDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CCOMTOOLDlg::OnQuit() ///程序退出
{
	// TODO: Add your control notification handler code here
	m_SerialPort.InitPort(this,MaxSerialPortNum);
	m_SerialPort.ClosePort();
	PostQuitMessage(0);
	
}

void CCOMTOOLDlg::OnClearSendEdit() ///清除发送区内容
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	UpdateData(true);
	m_strSend=_T("");
	UpdateData(false);
}

void CCOMTOOLDlg::OnClearReceiveEdit() ///清楚接收区内容
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	UpdateData(true);
	m_strReceive=_T("");
	UpdateData(false);
}

void CCOMTOOLDlg::OnBAutoSend() ///自动发送复选框
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	UpdateData(true);
	m_SendPeriodCtrl.EnableWindow(m_bAutoSend);///当自动发送复选框选定时，发送周期有效
	if(m_bAutoSend)	
	{
		m_SendCtrl.SetWindowText("自动发送");
	}
	else
	{
		m_SendCtrl.SetWindowText("手动发送");
		KillTimer(1);
	}
}

void CCOMTOOLDlg::OnOpenClose() ///打开/关闭串口按钮
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	UpdateData(true);
	if(temp==_T("关闭串口"))///表示点击后是"关闭串口"，也就是已经关闭了串口
	{
		m_IconCtrl.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_OFF));
		m_SerialPort.InitPort(this,MaxSerialPortNum);///串口初始化
		m_OpenCloseCtrl.SetWindowText(_T("打开串口"));///设置按钮文字为"打开串口"
		m_DescriptionCtrl.SetWindowText("串口未打开");///底下的窗口信息栏设置为空
		
		m_SendCtrl.GetWindowText(temp);///若自动发送时，关闭串口，则停止自动发送
		if(temp==_T("停止发送"))
		{
			KillTimer(1);
			m_SendCtrl.SetWindowText("自动发送");
		}
	}
	///打开串口操作
	else if( m_PortNO.GetCount())///当前列表的内容个数
	{	
		
		int SelPortNO,SelBaudRate,SelDataBits,SelStopBits;
		char SelParity;
		UpdateData(true);
		temp=m_strPortNO;///CString temp
		temp.Delete(0,3);
		SelPortNO=atoi(temp);
		SelBaudRate=atoi(m_strBaudRate);
		SelDataBits=atoi(m_strDataBits);
		SelParity=m_strParity.GetAt(0);
		SelStopBits=atoi(m_strStopBits);
		if(m_SerialPort.InitPort(this,SelPortNO,SelBaudRate,SelParity,SelDataBits,SelStopBits,EV_RXCHAR|EV_CTS,512))		
		{
			m_IconCtrl.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ON));
			m_SerialPort.StartMonitoring();
			m_OpenCloseCtrl.SetWindowText(_T("关闭串口"));
			temp=m_strPortNO+",打开"+","+m_strBaudRate+","+m_strParity+
				","+m_strDataBits+","+m_strStopBits;
			m_DescriptionCtrl.SetWindowText(temp);
		}
		else
		{
			AfxMessageBox("串口已被占用！");
		}
	}
	else 
	{
		m_IconCtrl.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_OFF));
		AfxMessageBox("没有发现串口！");
	}
}

void CCOMTOOLDlg::OnClearCounter() ///清除计数信息
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	UpdateData(true);
	m_nSendBytes=0;
	m_nReceiveBytes=0;
	m_strSendBytes=_T("TX:0");
	m_strReceiveBytes=_T("RX:0");
	UpdateData(false);
}


void CCOMTOOLDlg::OnSend() ///发送
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	CString temp;
	UpdateData(true);
	m_OpenCloseCtrl.GetWindowText(temp);
	if(_T(temp)=="打开串口")///没有打开串口
	{
		AfxMessageBox("请首先打开串口");
		return;
	}

	if(!m_bAutoSend)///不是自动发送
	{	
		temp=m_strSend;
		if(m_bHexS)
			temp=ChangeCharstr2Hexstr(temp);
		if(m_bAddEnter)///若设置字符追加回车，HEX时，不能追加
			temp=m_strSend+'\n';
		m_SerialPort.WriteToPort(temp.GetBuffer(temp.GetLength()),temp.GetLength());
		m_nSendBytes+=temp.GetLength();
		m_strSendBytes.Format("TX:%d",m_nSendBytes);
		m_ctrlSendBytes.SetWindowText(m_strSendBytes);
	}
	else 
	{
		m_SendCtrl.GetWindowText(temp);
		if(temp=="停止发送")
		{
			KillTimer(1);
			m_SendCtrl.SetWindowText("自动发送");
			///UpdateData(false);
		}
		else
		{
			SetTimer(1,m_nSendPeriod,NULL);
			m_SendCtrl.SetWindowText("停止发送");
		}
	}
}

void CCOMTOOLDlg::OnTimer(UINT nIDEvent)  ///自动发送
{
	// TODO: Add your message handler code here and/or call default
	UpdateData(true);
	CString temp;
	temp=m_strSend;
	if(m_bHexS)///十六进制发送
		temp=ChangeCharstr2Hexstr(temp);
	if(m_bAddEnter)
		temp=m_strSend+'\n';
	m_SerialPort.WriteToPort(temp.GetBuffer(temp.GetLength()),temp.GetLength());
	m_nSendBytes+=temp.GetLength();
	m_strSendBytes.Format("TX:%d",m_nSendBytes);///TX计数
	m_ctrlSendBytes.SetWindowText(m_strSendBytes);
	//UpdateData(false);
	CDialog::OnTimer(nIDEvent);
}

void CCOMTOOLDlg::OnReceiveChar(UINT ch, LONG port)///接收消息响应函数
{
	UpdateData(true);
	m_nReceiveBytes++;
	CString temp;
	temp.Format("RX:%d",m_nReceiveBytes);///RX计数
	m_ctrlReceiveBytes.SetWindowText(temp);
	//UpdateData(false);
/************************************************************************/
	/*
	*参考于龚建伟
	*/

	if(m_bStopDispRXData)   //如果选择了停止显示接收数据，则返回
		return;          //注意，这种情况下，计数仍在继续，只是不显示
	
	//若设置了自动清空，则达到50行后，自动清空接收编辑框中显示的数据
	if((m_ctrlAutoClear.GetCheck())&&(m_ctrlReceive.GetLineCount()>=50))
	{
		m_strReceive.Empty();
		UpdateData(FALSE);
	}
	//如果没有自动清空，数据行达到100后，也自动清空
	//因为数据过多，影响接收速度，显示是最费CPU时间的操作
	if(m_ctrlReceive.GetLineCount()>100)
	{
		m_strReceive.Empty();
		m_strReceive="***The Length of the Text is too long, Emptied Automaticly!!!***\r\n";        
		UpdateData(FALSE);
	}

/************************************************************************/
	if(m_bHexR)///若为十六进制接收，则分离Hex char
		m_strReceive=DevideHexChar(ch)+_T(" ");
	else
	{
		
		if(ch == '\n')///回车时保存两个
		{
			m_strReceive=0x0D;
			m_strReceive+=0x0a;
		}
		/************************************************************************/
		///汉字处理
		///无法校验汉字前后是否匹配
		else
		{
			//ch的ASCII 汉字的ASCII是大于127的，因为最高位为1
			if(ch > 127 && ChD[0]== -1)///遇到的汉字的前半部分
			{
				ChD[0] = ch;
				return;
			}
			else if(ch > 127 && ChD[1]== -1)///遇到的汉字的后半部分
			{	
				ChD[1] = ch;///
				m_strReceive=ChD;///汉字赋给文本框
				ChD[0]=ChD[1]=-1;///汉字数组初始化
			}
			else
				m_strReceive=ch;
			/************************************************************************/
		}
	}

	///以下是将接收的字符加在字符串的最后，这里费时很多
	///不过采用末尾追加的方式，相对于UpdateData来说，闪烁不是很明显。因为UpdateData是全部更新使用会比较明显。
	//int nLen=m_ctrlReceive.GetWindowTextLength();
	m_ctrlReceive.SetSel(-1,-1);
	m_ctrlReceive.ReplaceSel(m_strReceive);


	if(!((CEdit*)GetDlgItem(IDC_ReceiveEdit))->LineLength())///防止除数为0 add by itas109 2014-01-14
		return;
	((CEdit*)GetDlgItem(IDC_ReceiveEdit))->LineScroll(
		m_strReceive.GetLength()/(((CEdit*)GetDlgItem(IDC_ReceiveEdit))->LineLength()));///首次发送汉字时，除数为0
}

CString CCOMTOOLDlg::ChangeCharstr2Hexstr(CString Charstr)///字符转为十六进制
{
	CString Hexstr=_T("");
	Charstr.MakeUpper();///将字符串转为大写

	HexStringFilter(Charstr);///过滤非十六进制字符

	int Length=Charstr.GetLength();
	if(Length%2)///若不是偶数，删除最后一个字符
		Charstr.Delete(Length-1);

	Length=Charstr.GetLength();
	for(int i=0;i<Length/2;i++)///?
	{
		Hexstr+=CombineHexChar(Charstr.GetAt(i*2),Charstr.GetAt(i*2+1));
	}
	return Hexstr;
}

void CCOMTOOLDlg::OnBHexS() ///十六进制发送复选框
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	UpdateData(true);
	if(m_bHexS)
	{
		GetDlgItem(IDC_SendEdit)->ModifyStyle(0,ES_UPPERCASE,0);///发送框显示大写
		GetDlgItem(IDC_CHECK_ADD_ENTER)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_SendEdit)->ModifyStyle(ES_UPPERCASE,0,0);///发送框显示正常
		GetDlgItem(IDC_CHECK_ADD_ENTER)->EnableWindow(TRUE);
	}
}

void CCOMTOOLDlg::OnBHexR() ///十六进制接收复选框
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	UpdateData(true);
	if(m_bHexR)
		GetDlgItem(IDC_ReceiveEdit)->ModifyStyle(0,ES_UPPERCASE,0);///接收框显示大写
	else
		GetDlgItem(IDC_ReceiveEdit)->ModifyStyle(ES_UPPERCASE,0,0);///接收框显示正常
}

void CCOMTOOLDlg::HexStringFilter(CString &str)///十六进制过滤0-9 a-f A-F
{
	BOOL bOK;
	for(int i=0;i<str.GetLength();)///若该字符不是0-9或a-f或A-F，则删除
	{
		bOK=((str.GetAt(i)>='0')&&(str.GetAt(i)<='9'))||
			((str.GetAt(i)>='A')&&(str.GetAt(i)<='F'))||
			((str.GetAt(i)>='a')&&(str.GetAt(i)<='f'));
		if(!bOK)
			str.Delete(i);
		else i++;	
	}
}
char CCOMTOOLDlg::CombineHexChar(char CharH,char CharL) ///十六进制转为字符  CombineHexChar(A,B) result=1011;
{
	char result;
	CString temp;
	if(CharH>='0'&&CharH<='9')			result=(CharH-'0');
	else if(CharH>='a'&&CharH<='f')		result=(CharH-'a'+10);
	else if(CharH>='A'&&CharH<='F')		result=(CharH-'A'+10);
	else								result=0;///执行HexStringFilter之后应该不会出现

	result<<=4;	///把数据左移4位
	if(CharL>='0'&&CharL<='9')			result+=(CharL-'0');
	else if(CharL>='a'&&CharL<='f')		result+=(CharL-'a'+10);
	else if(CharL>='A'&&CharL<='F')		result+=(CharL-'A'+10);
	else								result+=0;
	return result;
}


CString CCOMTOOLDlg::DevideHexChar(char HexChar)///分离十六进制和字符？
{
	CString result=_T("");
	int temp=(HexChar&0xF0)>>4;
	if (temp < 10)
		result += (temp + '0');
	else 
		result+=(temp+'A'-10);
	temp=HexChar&0x0F;
	if(temp<10)
		result+=(temp+'0');
	else 
		result+=(temp+'A'-10);
	return result;
}

void CCOMTOOLDlg::OnMenuAbout() ///关于
{
	// TODO: Add your command handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	CAboutDlg dlg;
	dlg.DoModal();
}

void CCOMTOOLDlg::OnButtonRefresh() ///刷新当前串口信息
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	if(temp==_T("关闭串口"))
	{
		OnOpenClose();
	}
	m_SerialPort.Hkey2ComboBox(m_PortNO);
}

void CCOMTOOLDlg::OnButtonStopdisp() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	m_bStopDispRXData=!m_bStopDispRXData;
	if(m_bStopDispRXData)
		m_ctrlStopDisp.SetWindowText("继续显示");
	else
		m_ctrlStopDisp.SetWindowText("停止显示");
}

BOOL CCOMTOOLDlg::DestroyWindow() ///当程序结束时，自动保存配置
{
	// TODO: Add your specialized code here and/or call the base class
	AfxGetApp()->WriteProfileInt("SerialPortInfo","SaveConfig",m_bSaveConfig);///自动保存配置，先存于注册表中
	if (m_bSaveConfig)
	{
		AfxGetApp()->WriteProfileInt("SerialPortInfo","BaudRate",m_BaudRate.GetCurSel());///波特率

		AfxGetApp()->WriteProfileInt("SerialPortInfo","Parity",m_Parity.GetCurSel());///奇偶校验

		AfxGetApp()->WriteProfileInt("SerialPortInfo","StopBits",m_StopBits.GetCurSel());///停止位

		AfxGetApp()->WriteProfileInt("SerialPortInfo","DataBits",m_DataBits.GetCurSel());///数据位

		/************************************************************************/

		AfxGetApp()->WriteProfileInt("SerialPortInfo","HexS",m_bHexS);///十六进制发送

		AfxGetApp()->WriteProfileInt("SerialPortInfo","HexR",m_bHexR);///十六进制接收

		AfxGetApp()->WriteProfileInt("SerialPortInfo","AutoClear",m_bAutoClear);///自动清空

		AfxGetApp()->WriteProfileInt("SerialPortInfo","SendEnter",SendByEnter);///按回车发送数据

	}
	return CDialog::DestroyWindow();
}

void CCOMTOOLDlg::OnSaveConfigure() ///自动保存配置复选框
{
	// TODO: Add your control notification handler code here
	m_bSaveConfig=!m_bSaveConfig;
}

void CCOMTOOLDlg::OnCheckAutoclear() ///自动清空复选框
{
	// TODO: Add your control notification handler code here
	m_bAutoClear=!m_bAutoClear;
}

void CCOMTOOLDlg::OnButtonAbout() ///关于
{
	// TODO: Add your control notification handler code here
	CAboutDlg dlg;
	dlg.DoModal();
}



void CCOMTOOLDlg::OnSelchangeCom() ///下拉列表选定值改变时，关闭串口
{
	// TODO: Add your control notification handler code here
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	if(temp==_T("关闭串口"))
	{
		OnOpenClose();
	}
}

void CCOMTOOLDlg::OnSelchangeDataBits() 
{
	// TODO: Add your control notification handler code here
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	if(temp==_T("关闭串口"))
	{
		OnOpenClose();
	}
}

void CCOMTOOLDlg::OnSelchangeParityBits() 
{
	// TODO: Add your control notification handler code here
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	if(temp==_T("关闭串口"))
	{
		OnOpenClose();
	}
}

void CCOMTOOLDlg::OnSelchangeStopBits() 
{
	// TODO: Add your control notification handler code here
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	if(temp==_T("关闭串口"))
	{
		OnOpenClose();
	}
}

void CCOMTOOLDlg::OnSelchangeBaudrate() 
{
	// TODO: Add your control notification handler code here
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	if(temp==_T("关闭串口"))
	{
		OnOpenClose();
	}
}

void CCOMTOOLDlg::OnButtonSendKey() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	CRect rect;
	CPoint point;
	CButton * mybt = (CButton *)GetDlgItem(IDC_BUTTON_SEND_KEY);
	mybt->GetWindowRect(rect);
    point.x = rect.right;
    point.y = rect.bottom;
	
	CMenu menu;
	VERIFY( menu.LoadMenu(IDR_MENU_SENDKEY));
	CMenu* popup = menu.GetSubMenu(0);
	ASSERT( popup != NULL );
            popup->TrackPopupMenu(TPM_RIGHTALIGN| TPM_RIGHTBUTTON, point.x, point.y, this );
}

void CCOMTOOLDlg::OnMenuSendNormal() ///回车发送无效
{
	// TODO: Add your command handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	SendByEnter = FALSE;
	m_ctrlSendKey.SetWindowText("↓");
}

void CCOMTOOLDlg::OnMenuSendEnter() ///回车发送有效
{
	// TODO: Add your command handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	SendByEnter = TRUE;
	m_ctrlSendKey.SetWindowText("E");
}

BOOL CCOMTOOLDlg::PreTranslateMessage(MSG* pMsg) ///按下回车并选择回车发送时，执行发送函数
{
	// TODO: Add your specialized code here and/or call the base class


	///在其他的可用空间添加GetDlgItem(IDC_SendEdit)->SetFocus();
	///使控件的焦点始终在发送给文本框上
	///如果不这样，可能会引发未知的错误
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN &&SendByEnter)
		OnSend();

	return CDialog::PreTranslateMessage(pMsg);
}



void CCOMTOOLDlg::OnCheckAddEnter() ///是否在字符后追加回车
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	m_bAddEnter = !m_bAddEnter;
	m_ctrlAddEnter.SetCheck(m_bAddEnter);
}

void CCOMTOOLDlg::OnButtonHelp() ///帮助文档
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_SendEdit)->SetFocus();
	CMyHelpDlg dlg;
	dlg.DoModal();
}
