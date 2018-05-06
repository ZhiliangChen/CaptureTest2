
// CaptureTest2Dlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CaptureTest2.h"
#include "CaptureTest2Dlg.h"
#include "afxdialogex.h"
#include "afxmsg_.h"
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <iostream> 
#include "AbbConnector.h"
#include "FindCenter.h"
#include "SerialPort.h"
#include <assert.h>
#include "PortOperation.h"
#include "InterfaceLLT_2.h"
#include "GetProfiles_Ethernet.h"



using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int Num = 0;
int Testnum = 0;
AbbConnector Abb;
CvFindCenter Cvcenter;
CSerialPort m_Com;
PortOperation m_Portoperation;
GetProfiles_Ethernet m_GetProfiles;


//static CInterfaceLLT* m_pLLT = NULL;
//extern 允许外部文件引用
unsigned int m_uiResolution = 0;
TScannerType m_tscanCONTROLType = scanCONTROL2xxx;
CInterfaceLLT* m_pLLT;
//下面这些哪些可以变成局部变量？
std::vector<unsigned int> vuiEthernetInterfaces(MAX_INTERFACE_COUNT);
std::vector<DWORD> vdwResolutions(MAX_RESOULUTIONS);
unsigned int uiEthernetInterfaceCount = 0;
unsigned int uiShutterTime = 4000;
unsigned int uiIdleTime = 1000;
bool bOK = true;
bool bConnected = false;
int GetProfiles_Ethernet::iRetValue;


//线程函数必须是全局函数或静态成员函数。
void ThreadFindCenter()
{
	Cvcenter.FindCenter();

}

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


// CCaptureTest2Dlg 对话框



CCaptureTest2Dlg::CCaptureTest2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAPTURETEST2_DIALOG, pParent)
{
	m_strMsg = _T("");
	m_strMsg_X = _T("");
	m_strMsg_Y = _T("");
	m_nOpMode = -1;
	m_strReceive = _T("");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCaptureTest2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_STATIC_MSG, m_strMsg);
	DDX_Text(pDX, IDC_STATIC_X, m_strMsg_X);
	DDX_Text(pDX, IDC_STATIC_Y, m_strMsg_Y);
	DDX_Radio(pDX, IDC_CONTINUE, m_nOpMode);
	DDX_Control(pDX, IDC_OPENCOM, m_opencom);
	DDX_Text(pDX, IDC_ReceiveEdit, m_strReceive);
	DDX_Control(pDX, IDC_ReceiveEdit, m_ctrlReceive);
	DDX_Text(pDX, IDC_TORQUE_MSG, m_strMsg_torque);
	DDX_Text(pDX, IDC_MOVE_X, m_move_X);
	DDX_Text(pDX, IDC_MOVE_Y, m_move_Y);
	DDX_Text(pDX, IDC_MOVE_Z, m_move_Z);
	DDX_Text(pDX, IDC_GAPWIDTH, m_strMsg_gapwidth);
	DDX_Control(pDX, IDC_LISTMSG, m_listmsg);
	DDX_Control(pDX, IDC_PortNO, m_PortNO);
	DDX_CBString(pDX, IDC_PortNO, m_strPortNO);

}

BEGIN_MESSAGE_MAP(CCaptureTest2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_MOVETOCENTER, OnMovetocenter)
	ON_BN_CLICKED(IDC_MOVEOFF, OnMoveoff)
	ON_BN_CLICKED(IDC_ABBDISCONNECT, OnAbbdisconnect)
	ON_BN_CLICKED(IDC_DETECTCENTER, OnDetectcenter)
	ON_BN_CLICKED(IDC_OPENCOM, OnOpencom)
	ON_BN_CLICKED(IDC_STARTTIGHTEN, OnStarttighten)
	ON_BN_CLICKED(IDC_REVERSETIGHTEN, OnReversetighten)
	
	ON_BN_CLICKED(IDC_STOPTIGHTEN, OnStoptighten)
	ON_BN_CLICKED(IDC_ABBCONNECT, OnAbbConnect)
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	ON_BN_CLICKED(IDC_TRIGGER, OnTrigger)

	ON_BN_CLICKED(IDC_STARTLLT, OnStartLLT)
	ON_BN_CLICKED(IDC_STOPLLT, OnStopLLT)
	ON_BN_CLICKED(IDC_STARTTEST, OnStarttest)

	ON_WM_TIMER()
	ON_MESSAGE(WM_COMM_RXCHAR, OnReceiveChar)
	//ON_MESSAGE(WM_COMM_RXCHAR, OnCommunication)


END_MESSAGE_MAP()


// CCaptureTest2Dlg 消息处理程序
/////////////////////////////////////////////////////////////////////////////
// CCaptureTest2Dlg message handlers
void CALLBACK RawCallBack(LPVOID lpParam, LPVOID lpUser)
{
	BYTE *pDataBuffer = (BYTE*)lpParam;
	CCaptureTest2Dlg *pDemoDlg = (CCaptureTest2Dlg*)lpUser;

	if (pDemoDlg->m_bRawSave)
	{
		errno_t err;
		FILE * fp;
		err = fopen_s(&fp,"RawData.raw", "wb+");
		if (err == 0)
			fwrite(pDataBuffer, sizeof(BYTE), pDemoDlg->m_CapInfo.Width*pDemoDlg->m_CapInfo.Height, fp);
		fclose(fp);
		pDemoDlg->m_bRawSave = FALSE;
	}
	if (pDemoDlg->m_bRawToRGB) {

		string   str = "RawToRGBData.bmp";
		LPCTSTR   lpstr = (LPCTSTR)str.c_str();
		MV_Usb2ConvertRawToRgb(pDemoDlg->m_hMVC1000, pDataBuffer, pDemoDlg->m_CapInfo.Width, pDemoDlg->m_CapInfo.Height, pDemoDlg->m_pRGBData);
		//pDemoDlg->SaveRGBAsBmp(pDemoDlg->m_pRGBData, "RawToRGBData.bmp", pDemoDlg->m_CapInfo.Width, pDemoDlg->m_CapInfo.Height);
		MV_Usb2SaveFrameAsBmp(pDemoDlg->m_hMVC1000, &pDemoDlg->m_CapInfo, pDemoDlg->m_pRGBData, lpstr);
		
		pDemoDlg->m_bRawToRGB = FALSE;
		SetEvent(pDemoDlg->m_savebmp);
	}
}

void CALLBACK FrameCallBack(LPVOID lpParam, LPVOID lpUser)
{
	BYTE *pDataBuffer = (BYTE*)lpParam;
	CCaptureTest2Dlg *pDemoDlg = (CCaptureTest2Dlg*)lpUser;
	if (pDemoDlg->m_bRGBSave) {
		pDemoDlg->SaveRGBAsBmp(pDataBuffer, L"RGBData.bmp", pDemoDlg->m_CapInfo.Width, pDemoDlg->m_CapInfo.Height);
		pDemoDlg->m_bRGBSave = FALSE;
	}

}

BOOL CCaptureTest2Dlg::OnInitDialog()
{
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
	DWORD RGBDataSize = 1280 * 1024 * 3;
	m_pRGBData = (BYTE*)malloc(RGBDataSize * sizeof(BYTE));
	memset(m_pRGBData, 0, RGBDataSize);
	DWORD RawDataSize = 1280 * 1024;
	m_pRawData = (BYTE*)malloc(RawDataSize * sizeof(BYTE));
	memset(m_pRawData, 0, RawDataSize);
	m_bRawSave = FALSE;
	m_bRawToRGB = FALSE;
	m_bRGBSave = FALSE;
	m_hMVC1000 = NULL;
	InitImageParam();

	FrameBmi.biSize = sizeof(BITMAPINFOHEADER);
	FrameBmi.biPlanes = 1;
	FrameBmi.biCompression = BI_RGB;
	FrameBmi.biClrImportant = 0;
	FrameBmi.biSizeImage = 0;
	FrameBmi.biClrUsed = 0;//use biBitCount value
	FrameBmi.biBitCount = 24;
	FrameBmi.biWidth = m_CapInfo.Width;
	FrameBmi.biHeight = m_CapInfo.Height;
	FrameBmi.biXPelsPerMeter = 0;
	FrameBmi.biYPelsPerMeter = 0;


	m_nOpMode = 0;
	UpdateData(FALSE);

	//自动复位事件状态，初始状态未触发
	m_savebmp = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_Com.Hkey2ComboBox(m_PortNO);///COM口初始化到CComboBox


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCaptureTest2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCaptureTest2Dlg::OnPaint()
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
HCURSOR CCaptureTest2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*=====================================================================
Initial video parameter
=====================================================================*/
void CCaptureTest2Dlg::InitImageParam()
{
	memset(&m_CapInfo, 0, sizeof(CapInfoStruct));
	m_CapInfo.Buffer = m_pRawData;

	m_CapInfo.Width = 1280;
	m_CapInfo.Height = 1024;
	m_CapInfo.HorizontalOffset = 0;
	m_CapInfo.VerticalOffset = 0;
	m_CapInfo.Exposure = 1024;
	m_CapInfo.Gain[0] = 17;
	m_CapInfo.Gain[1] = 9;
	m_CapInfo.Gain[2] = 15;
	m_CapInfo.Control = 0;
	memset(m_CapInfo.Reserved, 0, 8);
	m_CapInfo.Reserved[0] = 2;

}

void CCaptureTest2Dlg::OnDestroy()
{
	
	CDialog::OnDestroy();
	
	if (m_hMVC1000 != NULL) {
		MV_Usb2Uninit(&m_hMVC1000);
		m_hMVC1000 = NULL;
	}

	if (m_pRawData) {
		free(m_pRawData);
		m_pRawData = NULL;
	}
	if (m_pRGBData) {
		free(m_pRGBData);
		m_pRGBData = NULL;
	}
	//PostQuitMessage(0);

}

void CCaptureTest2Dlg::OnAbbConnect()
{
	Abb.Connect();
	//Sleep(3000);
	Abb.MoveReady();
	//int nIndex = 0;
	//ULONG i = 0;
	//int rt = MV_Usb2Init("MVC-F", &nIndex, &m_CapInfo, &m_hMVC1000);
	//if (ResSuccess != rt)
	//{
	//	AfxMessageBox("Can not open USB camera! ");
	//	MV_Usb2Uninit(&m_hMVC1000);
	//	m_hMVC1000 = NULL;
	//	return;
	//}
	//MV_Usb2SetOpMode(m_hMVC1000, m_nOpMode, FALSE);
	//MV_Usb2SetRawCallBack(m_hMVC1000, RawCallBack, this);
	//MV_Usb2SetFrameCallBack(m_hMVC1000, FrameCallBack, this);
	m_strMsg.Format(L"Abb.Connect");
	UpdateData(FALSE);
	m_listmsg.AddString(L"Abb.Connect");
}

void CCaptureTest2Dlg::OnStart()
{
	//Init camera
	int nIndex = 0;
    	ULONG i = 0;
	int rt = MV_Usb2Init("MVC-F", &nIndex, &m_CapInfo, &m_hMVC1000);
	if (ResSuccess != rt)
	{
		MessageBoxA(NULL, "Can not open USB camera!", "Error", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
		MV_Usb2Uninit(&m_hMVC1000);
		m_hMVC1000 = NULL;
		return;
	}
	MV_Usb2SetOpMode(m_hMVC1000, m_nOpMode, FALSE);
	MV_Usb2SetRawCallBack(m_hMVC1000, RawCallBack, this);
	MV_Usb2SetFrameCallBack(m_hMVC1000, FrameCallBack, this);
	//start capture
	MV_Usb2StartCapture(m_hMVC1000, TRUE);
	//Save raw
	m_bRawSave = TRUE;
	m_strMsg.Format(L"USB camera start capture");
	UpdateData(FALSE);
	m_listmsg.AddString(L"USB camera start capture");
	//	DWORD nRet=MV_Usb2SetThreadAffinityMask(m_hMVC1000,2);
}

void CCaptureTest2Dlg::OnStop()
{
	MV_Usb2StartCapture(m_hMVC1000, FALSE);
	//uninit camera
	if (m_hMVC1000 != NULL) {
		MV_Usb2Uninit(&m_hMVC1000);
		m_hMVC1000 = NULL;
		
	}
	//销毁窗口
	//cvDestroyWindow("circles");
	cvDestroyAllWindows();
	m_strMsg.Format(L"USB camera stop capture");
	UpdateData(FALSE);
	m_listmsg.AddString(L"USB camera stop capture");

}
int CCaptureTest2Dlg::SaveRGBAsBmp(BYTE *pSrc, LPCTSTR FileName, DWORD dwWidth, DWORD dwHeight)
{
	FILE *fp;
	BITMAPFILEHEADER pf;
	BITMAPINFOHEADER FrameBmi;
	errno_t err;

	if (pSrc == NULL)
		return 0;

	DWORD			Width = dwWidth;
	DWORD			Height = dwHeight;
	DWORD RowLength;
	RowLength = 4 * ((Width * 24 + 31) / 32);

	DWORD RGBFrameSize = RowLength * Height;


	FrameBmi.biSize = sizeof(BITMAPINFOHEADER);
	FrameBmi.biPlanes = 1;
	FrameBmi.biCompression = BI_RGB;
	FrameBmi.biClrImportant = 0;
	FrameBmi.biSizeImage = 0;
	FrameBmi.biClrUsed = 0;//use biBitCount value
	FrameBmi.biBitCount = 24;
	FrameBmi.biWidth = Width;
	FrameBmi.biHeight = Height;
	FrameBmi.biXPelsPerMeter = 0;
	FrameBmi.biYPelsPerMeter = 0;

	err = _wfopen_s(&fp,FileName, L"wb");
	if (err != 0) {
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
		);
		::MessageBox(NULL, (LPCTSTR)lpMsgBuf, L"Error", MB_OK | MB_ICONINFORMATION);
		LocalFree(lpMsgBuf);
		return -1;
	}

	pf.bfType = 0x4d42; //"BM"
	pf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + RGBFrameSize;
	pf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	pf.bfReserved1 = 0;
	pf.bfReserved2 = 0;

	RGBQUAD *pPallete = (RGBQUAD*)malloc(256 * sizeof(RGBQUAD));
	for (int i = 0; i<256; i++) {
		pPallete[i].rgbRed = (BYTE)i;
		pPallete[i].rgbBlue = (BYTE)i;
		pPallete[i].rgbGreen = (BYTE)i;
		pPallete[i].rgbReserved = (BYTE)0;
	}
	fwrite(&pf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&FrameBmi, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(pSrc, sizeof(BYTE), RGBFrameSize, fp);
	fclose(fp);
	free(pPallete);
	return 0;
}
//自动运行到圆心位置
void CCaptureTest2Dlg::OnMovetocenter()
{

	//计算圆心距离（640,512）的距离，计算屏幕上移动一个像素对应真实mm关系
	//重新设定机械臂的工件坐标，改变X Y方向与图片相同
	while ((abs(Cvcenter.x_center - 640) > 0) || (abs(Cvcenter.y_center - 512) > 0)) {
		//检测一下坐标对不对
		Abb.GetToolPos();
		Abb.targetpos = Abb.toolpos;
		if (abs(Cvcenter.x_center - 640) > 0) 
		{
			if (abs(Cvcenter.x_center - 640) > 4) 
			{
				Abb.targetpos.position[1] = Abb.toolpos.position[1] - (Cvcenter.x_center - 640) / 8.0;
			}
			else {
				Abb.targetpos.position[1] = Abb.toolpos.position[1] - (Cvcenter.x_center - 640) / 10.0;
			}
			
			//Abb.MoveToolTo(Abb.targetpos);
			//Sleep(3000);
			//Abb.MoveReady();
		}
		
		if (abs(Cvcenter.y_center - 512) > 0) 
		{
			if (abs(Cvcenter.y_center - 512) > 4)
			{
				Abb.targetpos.position[0] = Abb.toolpos.position[0] - (Cvcenter.y_center - 512) / 8.0;
			}
			else {
				Abb.targetpos.position[0] = Abb.toolpos.position[0] - (Cvcenter.y_center - 512) / 10.0;
			}
			
			//Abb.MoveToolTo(Abb.targetpos);
			//Sleep(3000);
			//Abb.MoveReady();
		}
		//不在两个if之间分别移动就会导致一直在move
		Abb.MoveToolTo(Abb.targetpos);
		Abb.MoveReady();
		//Sleep(3000);
		m_strMsg.Format(L"继续向圆心移动");
		m_listmsg.AddString(m_strMsg);
		m_bRawToRGB = TRUE;
		//此处的sleep3秒必须加，否则图片来不及保存就会执行findcenter
		Sleep(3000);
		
		//等待事件触发
		//WaitForSingleObject(m_savebmp, INFINITE);
		Cvcenter.FindCenter();
		//hThread_findcenter = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadFindCenter,NULL,0,&ThreadID_findcenter);
		//WaitForSingleObject(hThread_findcenter, INFINITE);
		m_strMsg_X.Format(L"%f", Cvcenter.x_center);
		UpdateData(FALSE);
		m_strMsg_Y.Format(L"%f", Cvcenter.y_center);
		UpdateData(FALSE);
		m_strMsg.Format(L"循环次数：%d",Testnum);
		m_listmsg.AddString(m_strMsg);
		//选中listbox中的最后一行
		num_CurSel = m_listmsg.GetCount();
		m_listmsg.SetCurSel(num_CurSel - 1);
		//UpdateData(FALSE);
		Testnum++;
		//BUG：屏幕显示跟不上，Sleep无效，对于最后结果没有影响。
		//m_strMsg.Format(L"休眠结束");
		//UpdateData(FALSE);
		
	}
	m_strMsg.Format(L"到达圆心，停止移动");;
	m_listmsg.AddString(m_strMsg);

	m_strMsg.Format(L"%f,%f,%f", Abb.targetpos.position[0], Abb.targetpos.position[1], Abb.targetpos.position[2]);
	m_listmsg.AddString(m_strMsg);
	//选中listbox中的最后一行
	num_CurSel = m_listmsg.GetCount();
	m_listmsg.SetCurSel(num_CurSel - 1);

}

//BUG：点完gettoolpos后，再点detectcenter就没反应了，必须重新拔插相机USB。点完disconnectAbb后可以解决
void CCaptureTest2Dlg::OnMoveoff()
{
	//double move_x, move_y, move_z;
	UpdateData(TRUE);

	Abb.GetToolPos();
	Abb.targetpos = Abb.toolpos;
	Abb.targetpos.position[0] += m_move_X;
	Abb.targetpos.position[1] += m_move_Y;
	//暂时将moveff改为Z轴上升5
	Abb.targetpos.position[2] += m_move_Z;
	//Abb.targetpos.position[2] += 5;
	Abb.MoveToolTo(Abb.targetpos);
	m_strMsg.Format(L"%f,%f,%f",Abb.targetpos.position[0], Abb.targetpos.position[1], Abb.targetpos.position[2]);
	m_listmsg.AddString(m_strMsg);
	m_move_X = m_move_Y = m_move_Z = 0;
	UpdateData(FALSE);
	//选中listbox中的最后一行
	num_CurSel = m_listmsg.GetCount();
	m_listmsg.SetCurSel(num_CurSel - 1);

}

void CCaptureTest2Dlg::OnAbbdisconnect()
{
	//for color camera
	Abb.Disconnect();
	m_strMsg.Format(L"Abb.Disconnect");
	UpdateData(FALSE);
	m_listmsg.AddString(m_strMsg);
}

void CCaptureTest2Dlg::OnTimer(UINT_PTR nIDEvent)

{

	// TODO: Add your message handler code hereand/or call default   

	Num++;
	if (Num == 100)
		Num = 0;



}
void CCaptureTest2Dlg::OnDetectcenter()
{
	m_bRawToRGB = TRUE;
	//此处的sleep3秒必须加，否则图片来不及保存就会执行findcenter
	Sleep(3000);
	Cvcenter.FindCenter();
	m_strMsg_X.Format(L"%f", Cvcenter.x_center);
	UpdateData(FALSE);
	m_strMsg_Y.Format(L"%f", Cvcenter.y_center);
	UpdateData(FALSE);


	//SetTimer(1, 1000, NULL);

	//MSG msg;
	//BOOL bRet;
	//while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	//{
	//	if (bRet == -1)
	//	{
	//		//handle the error and possibly exit
	//		KillTimer(1);//关闭定时器1
	//		break;
	//	}
	//	else//如果(msg.message==WM_TIMER)
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//		if (Testnum != Num) {
	//			m_bRawToRGB = TRUE;
	//			Cvcenter.FindCenter();
	//			m_strMsg_X.Format(L"%d", Cvcenter.x_center);
	//			UpdateData(FALSE);
	//			m_strMsg_Y.Format(L"%d", Cvcenter.y_center);
	//			UpdateData(FALSE);
	//			Testnum = Num;
	//		
	//		}
	//	

	//		m_strMsg.Format(L"%d", Num);
	//		UpdateData(FALSE);
	//	}
	//}

}


void CCaptureTest2Dlg::OnUninitcamera()
{
	/*if (m_hMVC1000 != NULL) {
		MV_Usb2Uninit(&m_hMVC1000);
		m_hMVC1000 = NULL;
		m_strMsg.Format("USB camera uninit successfully");
		UpdateData(FALSE);
	}*/
}


void CCaptureTest2Dlg::OnContinue()
{
	UpdateData(TRUE);
	MV_Usb2SetOpMode(m_hMVC1000, m_nOpMode, FALSE);
}

void CCaptureTest2Dlg::OnTrigger()
{
	UpdateData(TRUE);
	MV_Usb2SetOpMode(m_hMVC1000, m_nOpMode, FALSE);
}



void CCaptureTest2Dlg::OnOpencom()
{
	//打开&关闭 串口

	//m_PortName = m_Combox.GetCurSel() + 1;                       //获取串口号
	//m_Baud = m_Baud_Group[m_Combox_Baud.GetCurSel()];    //获取波特率
	//m_Parity = m_Parity_Group[m_Combox_Parity.GetCurSel()];//获取校验位
	//m_DataBit = m_DataBit_Group[m_Combox_Data.GetCurSel()]; //获取数据位
	//m_StopBit = m_StopBit_Group[m_Combox_Stop.GetCurSel()]; //获取停止位
	//OL m_IsOpenCom = FALSE;//默认串口是关闭，最好先查询一下串口状态
	if (m_IsOpenCom)//串口已经打开
	{
		//关闭串口
		m_Com.ClosePort();
		m_IsOpenCom = FALSE;
		m_opencom.SetWindowText(L"Open Com");//说明已经关闭了串口
		 //修改状态
		GetDlgItem(IDC_PortNO)->EnableWindow(TRUE);  //允许改
	}
	else//串口已经关闭
	{
		//if (m_Com.InitPort(this, m_PortName, 9600 ,'N',8, 0))
		//CWnd *pWnd = GetDlgItem(IDC_OPENCOM); // 取得控件的指针  
		//HWND hwnd = pWnd->GetSafeHwnd(); // 取得控件的句柄
		int SelPortNO;
		UpdateData(true);
		CString temp = m_strPortNO;
		temp.Delete(0, 3);
		//函数不支持COM口大于10的情况，可以在设备管理器->端口->属性->port setting->advanced中修改COM口
		SelPortNO = _ttoi(temp);//Cstring转整型

		HWND hwnd;
		hwnd = AfxGetMainWnd()->GetSafeHwnd();
		if (m_Com.InitPort(hwnd, SelPortNO, 115200, 'N', 8, 0))
									//将COM付给Open Com按钮的句柄？这个赋值有什么用？输出串口信息的？
		{                           //串口号，波特率，校验位，数据位，停止位为1(在此输入0，代表停止位为1)
									//打开串口成功
			m_Com.StartMonitoring();
			m_IsOpenCom = TRUE;
			m_opencom.SetWindowText(L"Close Com");//说明已经打开了串口
		    //修改状态          
			GetDlgItem(IDC_PortNO)->EnableWindow(FALSE);  //不许改
		}
		else
		{//串口打开失败           
			MessageBoxA(NULL, "没有发现此串口或被占用!", "串口打开失败", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
		}
	}

}
void CCaptureTest2Dlg::OnStarttighten()
{
	CStringA temp = "01 10 00 00 00 02 04 02 01 00 00 A3 D7" ;
	temp = m_Portoperation.ChangeCharstr2Hexstr(temp);
	m_Com.WriteToPort(temp.GetBuffer(temp.GetLength()), temp.GetLength());
	num_sendchar = 1;
}
void CCaptureTest2Dlg::OnReversetighten()
{
	CStringA temp = "01 10 00 00 00 02 04 02 01 08 00 A4 17";
	temp = m_Portoperation.ChangeCharstr2Hexstr(temp);
	m_Com.WriteToPort(temp.GetBuffer(temp.GetLength()), temp.GetLength());
	num_sendchar = 2;
	num_reverse = 0;
}
void CCaptureTest2Dlg::OnStoptighten()
{
	CStringA temp = "01 10 00 00 00 02 04 80 01 00 00 8B AF";
	temp = m_Portoperation.ChangeCharstr2Hexstr(temp);
	m_Com.WriteToPort(temp.GetBuffer(temp.GetLength()), temp.GetLength());
	num_sendchar = 0;
}

LRESULT CCaptureTest2Dlg::OnReceiveChar(WPARAM ch, LPARAM port)///接收消息响应函数
{
	CStringA hexchar;
	int len;
	hexchar = m_Portoperation.DevideHexChar((char)ch);
	len = MultiByteToWideChar(CP_ACP, 0, hexchar, strlen(hexchar), NULL, 0);
	TCHAR *buf = new TCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, 0, hexchar, strlen(hexchar), buf, len);
	buf[len] = '\0';
	
	m_strReceive.Append(buf);
	UpdateData(FALSE);

	//显示实时扭矩，待添加返回7个字节的情况
	//思路：新建一个进程，模拟串口通信的进程；一直查询status判断何时停止查询实时扭矩。
	//通过num_receivechar和01 03 10判断一个字符串的长度范围
	//将查询函数放到这个函数里面？收一个发一个？
	//循环里面的向串口发送，会不会和面板上按钮所发送的串口数据有冲突？？？
	
	CStringA temp;
	CStringA temp_status = "01 03 00 04 00 01 C5 CB";
	CStringA temp_torque = "01 03 00 08 00 02 45 C9";
	CStringA temp_stop = "01 10 00 00 00 02 04 80 01 00 00 8B AF";
	temp = buf;
	if (num_receivechar == 0) {
		if (temp == "01") {
		}
		else {
			return 0;
		}
	}

	if (num_receivechar == 1) {
		if (temp == "03") {
			num_receivesize = 5;
		}
		//不能直接发送查询扭矩，应该先查询status，然后再发送扭矩查询;扭矩接受完，发送status

		if (temp == "10") {
			if (num_sendchar != 0) {
				num_receivesize = 8;
				temp_status = m_Portoperation.ChangeCharstr2Hexstr(temp_status);
				m_Com.WriteToPort(temp_status.GetBuffer(temp_status.GetLength()), temp_status.GetLength());
			}
		}
	}

	if (num_receivechar == 2) {
		if (temp == "04"|| temp == "02") {
			if (temp == "04") {
				num_receivesize = 9;
			}
			if (temp == "02") {
				num_receivesize = 7;
			}
		}
		else {
			return 0;
		}
	}

	if (num_receivechar == 3) {
		m_torque.Append(temp);
		if (num_receivesize == 7) {
			if (temp == "81") {
				//此时不再发送扭矩查询命令
				num_sendchar = 0;
			}
			if (temp == "01") {
				//此时不再发送扭矩查询命令
				num_sendchar = 0;
			}
			if (temp == "03") {
				num_sendchar = 1;
			}
		}
		
	}
	if (num_receivechar == 4) {
		m_torque.Append(temp);

	}

	if (num_receivechar == 5 && num_receivesize == 9) {
		m_torque.Append(temp);
	}

	if (num_receivechar == 6) {
		if (num_receivesize == 9) {
			m_torque.Append(temp);
			unsigned char value[4];
			int i;
			int a;
			int b;
			//不对啊，为什么？因为是宽字符吗
			for (i = 3; i >= 0; i--)
			{
				if (m_torque[i * 2]>'9')
				{
					a = m_torque[i * 2] - 'A' + 10;
				}
				else
				{
					a = m_torque[i * 2] - '0';
				}
				if (m_torque[i * 2 + 1]>'9')
				{
					b = m_torque[ i * 2 + 1] - 'A' + 10;
				}
				else
				{
					b = m_torque[i * 2 + 1] - '0';
				}
				//低字节在前，高字节在后的16进制
				value[i] = (unsigned char)(a * 16 + b);
				//正常顺序的16进制
				//value[3 - i] = (unsigned char)(a * 16 + b);
			}
			float x = *((float*)value);
			m_strMsg_torque.Format(L"%f", x);
			UpdateData(FALSE);
			//接受到扭矩之后，发送查询status
			temp_status = m_Portoperation.ChangeCharstr2Hexstr(temp_status);
			m_Com.WriteToPort(temp_status.GetBuffer(temp_status.GetLength()), temp_status.GetLength());

			//同一个函数中，或者说同一个循环中，若有两个m_Com.WriteToPort函数，只会执行后写出的那个。
			if ( x < -3.0) {
				//num_reverse >180 &&
				m_listmsg.AddString(L"11111");
				//反转时停止
				temp_stop = m_Portoperation.ChangeCharstr2Hexstr(temp_stop);
				m_Com.WriteToPort(temp_stop.GetBuffer(temp_stop.GetLength()), temp_stop.GetLength());
			
			}
			
			
		}
		
		if (num_receivesize == 7) {
			num_receivechar = -1;
			m_torque.Empty();
			//接受到status为03拧紧运行时，发送查询扭矩
			if (num_sendchar != 0) {
				temp_torque = m_Portoperation.ChangeCharstr2Hexstr(temp_torque);
				m_Com.WriteToPort(temp_torque.GetBuffer(temp_torque.GetLength()), temp_torque.GetLength());
			}
		}
	}

	if (num_receivechar == 7 && num_receivesize == 8) {
		num_receivechar = -1;
		m_torque.Empty();
	}

	if (num_receivechar == 8) {
		
		num_receivechar = -1;
		m_torque.Empty();
		}
	
	

	
	num_receivechar++;
	num_reverse++;
	delete[] buf;
	return 0;
}



void CCaptureTest2Dlg::OnStartLLT()
{
	bool bLoadError;
	char acErrorString[200];
	// Creating a LLT-object
	// The LLT-Object will load the LLT.dll automaticly and give us a error if ther no LLT.dll
	m_pLLT = new CInterfaceLLT("LLT.dll", &bLoadError);
	if (bLoadError)
	{
		MessageBoxA(NULL, "Error loading LLT.dll", "加载LLT.dll失败", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
	}


	// Test if the LLT.dll supports GetLLTType (Version 3.0.0.0 or higher)
	if (m_pLLT->m_pFunctions->CreateLLTDevice == NULL)
	{
		MessageBoxA(NULL, "Please use a LLT.dll version 3.0.0.0 or higher!", "ERROR", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
	}
	else
	{
		// Create a Firewire Device
		if (m_pLLT->CreateLLTDevice(INTF_TYPE_ETHERNET))
		{
			m_strMsg.Format(L"CreateLLTDevice Ethernet OK");
			UpdateData(FALSE);
			m_listmsg.AddString(m_strMsg);
		}
		else
			MessageBoxA(NULL, "Error during CreateLLTDevice", "ERROR", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);

		// Gets the available interfaces from the scanCONTROL-device
		m_GetProfiles.iRetValue = m_pLLT->GetDeviceInterfacesFast(&vuiEthernetInterfaces[0], (unsigned int)vuiEthernetInterfaces.size());

		if (m_GetProfiles.iRetValue == ERROR_GETDEVINTERFACES_REQUEST_COUNT)
		{
			uiEthernetInterfaceCount = vuiEthernetInterfaces.size();
		}
		else if (m_GetProfiles.iRetValue < 0)
		{
			MessageBoxA(NULL, "A error occured during searching for connected scanCONTROL", "ERROR", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
			uiEthernetInterfaceCount = 0;
		}
		else
		{
			uiEthernetInterfaceCount = m_GetProfiles.iRetValue;
		}

		if (uiEthernetInterfaceCount == 0)
			MessageBoxA(NULL, "There is no scanCONTROL connected", "ERROR", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);

		else if (uiEthernetInterfaceCount == 1)
		{
			m_strMsg.Format(L"There is 1 scanCONTROL connected");
			UpdateData(FALSE);
			m_listmsg.AddString(m_strMsg);
		}
		
		else
		{
			m_strMsg.Format(L"There are %d scanCONTROL's connected", uiEthernetInterfaceCount);
			m_listmsg.AddString(m_strMsg);
		}

		if (uiEthernetInterfaceCount >= 1)
		{
			m_strMsg.Format(L"Select the device interface %d ", vuiEthernetInterfaces[0]);
			m_listmsg.AddString(m_strMsg);
			if ((m_GetProfiles.iRetValue = m_pLLT->SetDeviceInterface(vuiEthernetInterfaces[0], 0)) < GENERAL_FUNCTION_OK)
			{
				if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
					//m_strMsg.Format(L"%s", acErrorString);
					MessageBoxA(NULL, acErrorString, "Error during SetDeviceInterfac", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
				memset(acErrorString, 0, sizeof(acErrorString));
				bOK = false;
			}

			if (bOK)
			{
				
				if ((m_GetProfiles.iRetValue = m_pLLT->Connect()) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during Connect", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}
				else
				{
					m_strMsg.Format(L"Connecting to scanCONTROL");
					m_listmsg.AddString(m_strMsg);
				}	
			}

			if (bOK)
			{
				//cout << "Get scanCONTROL type\n";
				if ((m_GetProfiles.iRetValue = m_pLLT->GetLLTType(&m_tscanCONTROLType)) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during GetLLTType", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}

				if (m_GetProfiles.iRetValue == GENERAL_FUNCTION_DEVICE_NAME_NOT_SUPPORTED)
				{
					m_strMsg.Format(L"Can't decode scanCONTROL type. Please contact Micro-Epsilon for a newer version of the LLT.dll.");
					m_listmsg.AddString(m_strMsg);
				}

				if (m_tscanCONTROLType >= scanCONTROL27xx_25 && m_tscanCONTROLType <= scanCONTROL27xx_xxx)
				{
					m_strMsg.Format(L"The scanCONTROL is a scanCONTROL27xx");
					m_listmsg.AddString(m_strMsg);
				}
				else if (m_tscanCONTROLType >= scanCONTROL26xx_25 && m_tscanCONTROLType <= scanCONTROL26xx_xxx)
				{
					m_strMsg.Format(L"The scanCONTROL is a scanCONTROL26xx");
					m_listmsg.AddString(m_strMsg);
				}
				else if (m_tscanCONTROLType >= scanCONTROL29xx_25 && m_tscanCONTROLType <= scanCONTROL29xx_xxx)
				{
					m_strMsg.Format(L"The scanCONTROL is a scanCONTROL29xx");
					m_listmsg.AddString(m_strMsg);
				}
				else
				{
					m_strMsg.Format(L"The scanCONTROL is a undefined type\nPlease contact Micro - Epsilon for a newer SDK");
					m_listmsg.AddString(m_strMsg);
				}

				//cout << "Get all possible resolutions\n";
				if ((m_GetProfiles.iRetValue = m_pLLT->GetResolutions(&vdwResolutions[0], vdwResolutions.size())) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during GetResolutions", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}

				m_uiResolution = vdwResolutions[0];
			}

			if (bOK)
			{
				m_strMsg.Format(L"Set resolution to %d", m_uiResolution);
				m_listmsg.AddString(m_strMsg);
				if ((m_GetProfiles.iRetValue = m_pLLT->SetResolution(m_uiResolution)) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during SetResolution", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}
			}

			if (bOK)
			{
				//cout << "Set trigger to internal\n";
				if ((m_GetProfiles.iRetValue = m_pLLT->SetFeature(FEATURE_FUNCTION_TRIGGER, 0x00000000)) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during SetFeature(FEATURE_FUNCTION_TRIGGER)", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}
			}

			if (bOK)
			{
				//cout << "Profile config set to PROFILE\n";
				if ((m_GetProfiles.iRetValue = m_pLLT->SetProfileConfig(PROFILE)) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during SetProfileConfig", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}
			}

			if (bOK)
			{
				m_strMsg.Format(L"Set shutter time to %d", uiShutterTime);
				m_listmsg.AddString(m_strMsg);
				if ((m_GetProfiles.iRetValue = m_pLLT->SetFeature(FEATURE_FUNCTION_SHUTTERTIME, uiShutterTime)) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during SetFeature(FEATURE_FUNCTION_SHUTTERTIME)", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}
			}

			if (bOK)
			{
				m_strMsg.Format(L"Set idle time to %d", uiIdleTime);
				m_listmsg.AddString(m_strMsg);
				if ((m_GetProfiles.iRetValue = m_pLLT->SetFeature(FEATURE_FUNCTION_IDLETIME, uiIdleTime)) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during SetFeature(FEATURE_FUNCTION_IDLETIME)", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}
			}

			if (bOK)
			{
				m_strMsg.Format(L"Set measuring field to large");
				m_listmsg.AddString(m_strMsg);
				if ((m_GetProfiles.iRetValue = m_pLLT->SetFeature(FEATURE_FUNCTION_MEASURINGFIELD, 0x82000000)) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during SetFeature(FEATURE_FUNCTION_MEASURINGFIELD)", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}
			}

			if (bOK)
			{
				m_strMsg.Format(L"Set THRESHOLD to 128");
				m_listmsg.AddString(m_strMsg);
				if ((m_GetProfiles.iRetValue = m_pLLT->SetFeature(FEATURE_FUNCTION_THRESHOLD, 0x82000c80)) < GENERAL_FUNCTION_OK)
				{
					if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
						MessageBoxA(NULL, acErrorString, "Error during SetFeature(FEATURE_FUNCTION_THRESHOLD)", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
					memset(acErrorString, 0, sizeof(acErrorString));
					bOK = false;
				}
			}
			//if (bOK)
			//{
			//	m_GetProfiles.GetProfiles();
			//}

			
		}
	}
}
void CCaptureTest2Dlg::OnStopLLT()
{
	m_pLLT->SetFeature(FEATURE_FUNCTION_LASERPOWER,0x82000000);
	char acErrorString[200];
	m_strMsg.Format(L"Disconnect the scanCONTROL");

		if ((m_GetProfiles.iRetValue = m_pLLT->Disconnect()) < GENERAL_FUNCTION_OK)
		{
			if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
				MessageBoxA(NULL, acErrorString, "Error during Disconnect", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
			memset(acErrorString, 0, sizeof(acErrorString));
		}
	// Deletes the LLT-object
	delete m_pLLT;
}
void CCaptureTest2Dlg::OnStarttest()
{
	char acErrorString[200];
	m_pLLT->SetFeature(FEATURE_FUNCTION_LASERPOWER, 0x82000002);
	m_GetProfiles.GetProfiles();
	if (m_GetProfiles.iRetValue < GENERAL_FUNCTION_OK)
	{
		if (m_pLLT->TranslateErrorValue(m_GetProfiles.iRetValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
			MessageBoxA(NULL, acErrorString, "Error during GetProfiles", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
		memset(acErrorString, 0, sizeof(acErrorString));
		delete m_pLLT;
	}
	m_strMsg.Format(L"ShutterOpen: %f ShutterClose: %f", m_GetProfiles.dShutterOpen, m_GetProfiles.dShutterClose);
	m_listmsg.AddString(m_strMsg);
	m_strMsg.Format(L"ProfileCount: %d", m_GetProfiles.uiProfileCount);
	m_listmsg.AddString(m_strMsg);
	m_strMsg.Format(L"gap: X_left =: %f X_right: %f", m_GetProfiles.remakex[m_GetProfiles.gapnum_left], m_GetProfiles.remakex[m_GetProfiles.gapnum_right]);
	m_listmsg.AddString(m_strMsg);
	m_strMsg.Format(L"num_left: %d num_right: %d num_valid: %d", m_GetProfiles.num_left, m_GetProfiles.num_right, m_GetProfiles.num_valid);
	m_GetProfiles.num_valid = 0;
	m_listmsg.AddString(m_strMsg);
	//选中listbox中的最后一行
	num_CurSel = m_listmsg.GetCount();
	m_listmsg.SetCurSel(num_CurSel - 1);
	m_strMsg_gapwidth.Format(L"%f", m_GetProfiles.m_gap);
	UpdateData(FALSE);
}


