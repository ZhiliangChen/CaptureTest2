
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

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int Num = 0;
int Testnum = 0;
AbbConnector Abb;
CvFindCenter Cvcenter;

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
	DDX_Text(pDX, IDC_STATIC_MSG, m_strMsg);
	DDX_Text(pDX, IDC_STATIC_X, m_strMsg_X);
	DDX_Text(pDX, IDC_STATIC_Y, m_strMsg_Y);
	DDX_Radio(pDX, IDC_CONTINUE, m_nOpMode);
	DDX_Control(pDX, IDC_OPENCOM, m_opencom);
	DDX_Text(pDX, IDC_ReceiveEdit, m_strReceive);
	DDX_Control(pDX, IDC_ReceiveEdit, m_ctrlReceive);
}

BEGIN_MESSAGE_MAP(CCaptureTest2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_MOVETOCENTER, OnMovetocenter)
	ON_BN_CLICKED(IDC_GETTOOLPOS, OnGettoolpos)
	ON_BN_CLICKED(IDC_ABBDISCONNECT, OnAbbdisconnect)
	ON_BN_CLICKED(IDC_DETECTCENTER, OnDetectcenter)
	ON_BN_CLICKED(IDC_OPENCOM, OnOpencom)
	ON_BN_CLICKED(IDC_STARTTIGHTEN, OnStarttighten)
	ON_BN_CLICKED(IDC_STOPTIGHTEN, OnStoptighten)
	ON_BN_CLICKED(IDC_ABBCONNECT, OnAbbConnect)
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	ON_BN_CLICKED(IDC_TRIGGER, OnTrigger)
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
		//cvNamedWindow("circles", 1);
		//wchar_t  *wchar = L"RawToRGBData.bmp";
		//char     chr[128];
		//wchar = (wchar_t *)malloc(sizeof(wchar_t) * 64);
		//WideCharToMultiByte(CP_ACP, 0, wchar, -1,chr, sizeof(wchar_t) * 64, NULL, NULL);
		string   str = "RawToRGBData.bmp";
		LPCTSTR   lpstr = (LPCTSTR)str.c_str();
		MV_Usb2ConvertRawToRgb(pDemoDlg->m_hMVC1000, pDataBuffer, pDemoDlg->m_CapInfo.Width, pDemoDlg->m_CapInfo.Height, pDemoDlg->m_pRGBData);
		//pDemoDlg->SaveRGBAsBmp(pDemoDlg->m_pRGBData, "RawToRGBData.bmp", pDemoDlg->m_CapInfo.Width, pDemoDlg->m_CapInfo.Height);
		MV_Usb2SaveFrameAsBmp(pDemoDlg->m_hMVC1000, &pDemoDlg->m_CapInfo, pDemoDlg->m_pRGBData, lpstr);
		
		pDemoDlg->m_bRawToRGB = FALSE;
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
	while ((abs(Cvcenter.x_center - 640) > 10) || (abs(Cvcenter.y_center - 512) > 10)) {
		//检测一下坐标对不对
		Abb.GetToolPos();
		Abb.targetpos = Abb.toolpos;
		if (abs(Cvcenter.x_center - 640) > 10) {
			Abb.targetpos.position[1] = Abb.toolpos.position[1] - (Cvcenter.x_center - 640) / 4;
			Abb.MoveToolTo(Abb.targetpos);
			Sleep(3000);
		}
		
		if (abs(Cvcenter.y_center - 512) > 10) {
			Abb.targetpos.position[0] = Abb.toolpos.position[0] - (Cvcenter.y_center - 512) / 4;
			Abb.MoveToolTo(Abb.targetpos);
			Sleep(3000);
		}
		//将速度改小！！！
		//Abb.MoveToolTo(Abb.targetpos);
		
		m_strMsg.Format(L"继续向圆心移动");
		UpdateData(FALSE);
		m_bRawToRGB = TRUE;
		//此处的sleep3秒必须加，否则图片来不及保存就会执行findcenter
		Sleep(3000);
		Cvcenter.FindCenter();
		m_strMsg_X.Format(L"%d", Cvcenter.x_center);
		UpdateData(FALSE);
		m_strMsg_Y.Format(L"%d", Cvcenter.y_center);
		UpdateData(FALSE);
		m_strMsg.Format(L"%d",Testnum);
		UpdateData(FALSE);
		Testnum++;
		//BUG：屏幕显示跟不上，Sleep无效，对于最后结果没有影响。
		//Sleep(6000);
		//m_strMsg.Format(L"休眠结束");
		//UpdateData(FALSE);
		
	}
	m_strMsg.Format(L"到达圆心，停止移动");
	UpdateData(FALSE);



}

//BUG：点完gettoolpos后，再点detectcenter就没反应了，必须重新拔插相机USB。点完disconnectAbb后可以解决
void CCaptureTest2Dlg::OnGettoolpos()
{
	Abb.GetToolPos();
	Abb.targetpos = Abb.toolpos;
	Abb.targetpos.position[0] += 50;
	Abb.targetpos.position[1] -= 50;
	//Abb.targetpos.conf[3] = 1;
	Abb.MoveToolTo(Abb.targetpos);
	m_strMsg.Format(L"%f,%f,%f",Abb.targetpos.euler[0], Abb.targetpos.euler[1], Abb.targetpos.euler[2]);
	UpdateData(FALSE);
}

void CCaptureTest2Dlg::OnAbbdisconnect()
{
	//for color camera
	Abb.Disconnect();
	m_strMsg.Format(L"Abb.Disconnect");
	UpdateData(FALSE);
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
	m_strMsg_X.Format(L"%d", Cvcenter.x_center);
	UpdateData(FALSE);
	m_strMsg_Y.Format(L"%d", Cvcenter.y_center);
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
		//GetDlgItem(IDC_COMBO_PORT)->EnableWindow(TRUE);  //允许改
		//GetDlgItem(IDC_COMBO_BAUD)->EnableWindow(TRUE);  //允许改
		//GetDlgItem(IDC_COMBO_PARITY)->EnableWindow(TRUE);//允许改
		//GetDlgItem(IDC_COMBO_DATA)->EnableWindow(TRUE);  //允许改
		//GetDlgItem(IDC_COMBO_STOP)->EnableWindow(TRUE);  //允许改

	}
	else//串口已经关闭
	{
		//if (m_Com.InitPort(this, m_PortName, 9600 ,'N',8, 0))
		//CWnd *pWnd = GetDlgItem(IDC_OPENCOM); // 取得控件的指针  
		//HWND hwnd = pWnd->GetSafeHwnd(); // 取得控件的句柄  
		HWND hwnd;
		hwnd = AfxGetMainWnd()->GetSafeHwnd();
		if (m_Com.InitPort(hwnd, 1, 115200, 'N', 8, 0))
									//将COM付给Open Com按钮的句柄？这个赋值有什么用？输出串口信息的？
		{                           //串口号，波特率，校验位，数据位，停止位为1(在此输入0，代表停止位为1)
									//打开串口成功
			m_Com.StartMonitoring();
			m_IsOpenCom = TRUE;
			m_opencom.SetWindowText(L"Close Com");//说明已经打开了串口


											 //修改状态          
			//GetDlgItem(IDC_COMBO_PORT)->EnableWindow(FALSE);  //不许改
			//GetDlgItem(IDC_COMBO_BAUD)->EnableWindow(FALSE);  //不许改
			//GetDlgItem(IDC_COMBO_PARITY)->EnableWindow(FALSE);//不许改
			//GetDlgItem(IDC_COMBO_DATA)->EnableWindow(FALSE);  //不许改
			//GetDlgItem(IDC_COMBO_STOP)->EnableWindow(FALSE);  //不许改

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
	temp = ChangeCharstr2Hexstr(temp);
	m_Com.WriteToPort(temp.GetBuffer(temp.GetLength()), temp.GetLength());

}
void CCaptureTest2Dlg::OnStoptighten()
{
	CStringA temp = "01 10 00 00 00 02 04 80 01 00 00 8B AF";
	temp = ChangeCharstr2Hexstr(temp);
	m_Com.WriteToPort(temp.GetBuffer(temp.GetLength()), temp.GetLength());

}



CStringA CCaptureTest2Dlg::ChangeCharstr2Hexstr(CStringA Charstr)///字符转为十六进制
{
	CStringA Hexstr = ("");
	Charstr.MakeUpper();///将字符串转为大写

	HexStringFilter(Charstr);///过滤非十六进制字符

	int Length = Charstr.GetLength();
	if (Length % 2)///若不是偶数，删除最后一个字符
		Charstr.Delete(Length - 1);

	Length = Charstr.GetLength();
	for (int i = 0; i<Length / 2; i++)///?
	{
		Hexstr += CombineHexChar(Charstr.GetAt(i * 2), Charstr.GetAt(i * 2 + 1));
	}
	return Hexstr;
}

void CCaptureTest2Dlg::HexStringFilter(CStringA &str)///十六进制过滤0-9 a-f A-F
{
	BOOL bOK;
	for (int i = 0; i<str.GetLength();)///若该字符不是0-9或a-f或A-F，则删除
	{
		bOK = ((str.GetAt(i) >= '0') && (str.GetAt(i) <= '9')) ||
			((str.GetAt(i) >= 'A') && (str.GetAt(i) <= 'F')) ||
			((str.GetAt(i) >= 'a') && (str.GetAt(i) <= 'f'));
		if (!bOK)
			str.Delete(i);
		else i++;
	}
}


char CCaptureTest2Dlg::CombineHexChar(char CharH, char CharL) ///十六进制转为字符  CombineHexChar(A,B) result=1011;
{
	char result;
	CString temp;
	if (CharH >= '0'&&CharH <= '9')			result = (CharH - '0');
	else if (CharH >= 'a'&&CharH <= 'f')		result = (CharH - 'a' + 10);
	else if (CharH >= 'A'&&CharH <= 'F')		result = (CharH - 'A' + 10);
	else								result = 0;///执行HexStringFilter之后应该不会出现

	result <<= 4;	///把数据左移4位
	if (CharL >= '0'&&CharL <= '9')			result += (CharL - '0');
	else if (CharL >= 'a'&&CharL <= 'f')		result += (CharL - 'a' + 10);
	else if (CharL >= 'A'&&CharL <= 'F')		result += (CharL - 'A' + 10);
	else								result += 0;
	return result;
}

LRESULT CCaptureTest2Dlg::OnReceiveChar(WPARAM ch, LPARAM port)///接收消息响应函数
{
	CStringA hexchar;
	int len;
	hexchar = DevideHexChar((char)ch);
	len = MultiByteToWideChar(CP_ACP, 0, hexchar, strlen(hexchar), NULL, 0);
	TCHAR *buf = new TCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, 0, hexchar, strlen(hexchar), buf, len);
	buf[len] = '\0';

	m_strReceive.Append(buf);
	UpdateData(FALSE);
	delete[] buf;

	return 0;
}
CStringA CCaptureTest2Dlg::DevideHexChar(char HexChar)///分离十六进制和字符？
{
	
	CStringA result = ("");
	int temp = (HexChar & 0xF0) >> 4;
	if (temp < 10)
		result += (char)(temp + '0');
	else
		result += (char)(temp + 'A' - 10);
	temp = HexChar & 0x0F;
	if (temp<10)
		result += (char)(temp + '0');
	else
		result += (char)(temp + 'A' - 10);
	return result;
}
