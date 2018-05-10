#include "StdAfx.h"
#include "AbbConnector.h"
#include "stdio.h"
#include <atlstr.h>
#include "math.h"
#include "stdlib.h"
#pragma comment(lib,"ws2_32.lib")
#define pi 3.141592653589793
AbbConnector::AbbConnector()
{
}

AbbConnector::~AbbConnector()
{
}


/*
通信的几个步骤
client端：1 WSAStartup；2 socket；3connect 4send/recv； 5closesocket
*/
bool AbbConnector::Connect()
{
	WSAData wsadata;
	WORD wVersionRequested;
	wVersionRequested = MAKEWORD(2, 2);

	//step 1每一个使用windows socket的应用程序必须先进行WSAstartup函数加载套接字库
	if (WSAStartup(wVersionRequested, &wsadata) != 0)
	{
		MessageBoxA(NULL, "Socket Stack Error!", "Error", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
		return false;
	}

	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}

	sockaddr_in addr;
	//step 2 所有通信之前都要调用socket函数创建套接字
	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	//默认地址为192.168.125.1，TCP调试助手的地址为127.0.0.1
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.125.1"); 
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1502);
	int rc;
	
	if (rc = connect(ClientSocket, (SOCKADDR*)&addr, sizeof(addr)) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AbbConnector::SendMsg(char* command)
{
	//向ABB机器人发送命令字符串
	//WideCharToMultiByte(CP_ACP, 0, command, -1, command, NULL, NULL, FALSE);
	send(ClientSocket, command, strlen(command), 0);
}

void AbbConnector::MoveToolTo(ABBPOS abbp)
{
	//double vel = 50;
	CString abbmes;	
	CString temp;
	char recvbuf[80];

	//欧拉角转换为四元数
	double x, y, z;
	double ax, ay, az;//长度制
	//double q1, q2, q3, q4;//四元数各分量

	int c1, c4, c6, cx;
	c1 = abbp.conf[0];
	c4 = abbp.conf[1];
	c6 = abbp.conf[2];
	cx = abbp.conf[3];
	x = abbp.position[0];
	y = abbp.position[1];
	z = abbp.position[2];
	//弧度表示
	ax = abbp.euler[0] * 180 / pi;
	ay = abbp.euler[1] * 180 / pi;
	az = abbp.euler[2] * 180 / pi;


	//向下位机发一个目标点数据（double[3]），目标点姿态（euler[3]），机械臂参数（conf[3]）
	
	abbmes = "#mov@";	
	abbmes += "X";
	temp.Format(_T("%.3lf"), x);
	abbmes = abbmes + temp;
	abbmes += "Y";
	temp.Format(_T("%.3lf"), y);
	abbmes = abbmes + temp;
	abbmes += "Z";
	temp.Format(_T("%.3lf"), z);
	abbmes = abbmes + temp;
	abbmes += "Q";
	temp.Format(_T("%.3lf"), ax);
	abbmes = abbmes + temp;
	abbmes += "W";
	temp.Format(_T("%.3lf"), ay);
	abbmes = abbmes + temp;
	abbmes += "E";
	temp.Format(_T("%.3lf"), az);
	abbmes = abbmes + temp;
	abbmes += "R";
	temp.Format(_T("%d"), c1);
	abbmes = abbmes + temp;
	abbmes += "U";
	temp.Format(_T("%d"), c4);
	abbmes = abbmes + temp;
	abbmes += "I";
	temp.Format(_T("%d"), c6);
	abbmes = abbmes + temp;
	abbmes += "O";
	temp.Format(_T("%d"), cx);
	abbmes = abbmes + temp;
	abbmes += "P";

	/*
	lstrcpy(abbmes, L"#mov@");
	lstrcat(abbmes, L"X");
	wsprintf(temp, L"%.6lf", abbp.position[0]);
	lstrcat(abbmes,temp);
	lstrcat(abbmes, L"Y");
	wsprintf(temp, L"%.6lf", abbp.position[1]);
	lstrcat(abbmes, temp);
	lstrcat(abbmes, L"Z");
	wsprintf(temp, L"%.6lf", abbp.position[2]);
	lstrcat(abbmes, temp);
	lstrcat(abbmes, L"Q");
	wsprintf(temp, L"%.6lf", q1);
	lstrcat(abbmes, temp);
	lstrcat(abbmes, L"W");
	wsprintf(temp, L"%.6lf", q2);
	lstrcat(abbmes, temp);
	lstrcat(abbmes, L"E");
	wsprintf(temp, L"%.6lf", q3);
	lstrcat(abbmes, temp);
	lstrcat(abbmes, L"R");
	wsprintf(temp, L"%.6lf", q4);
	lstrcat(abbmes, temp);
	lstrcat(abbmes, L"T");*/
	

	//CString to char*
	int len = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)abbmes, -1, NULL, 0, NULL, NULL);
	char *cabbmes = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, abbmes, -1, cabbmes, len, NULL, FALSE);
	cabbmes[len] = '\0';
	SendMsg(cabbmes);
	//这一部分为什么会导致程序卡死？
	//Sleep(500);

	//char* ptr;
	//while ((ptr = strstr(recvbuf, "finish")) == NULL)
	//{
	//	Sleep(500);
	//	recv(ClientSocket, recvbuf, 100, 0);
	//}
	//delete[] ptr;
	//delete[] recvbuf;
	delete[] cabbmes;
	

}

bool AbbConnector::GetToolPos()
{
	ABBPOS abbp;
	bool IsGetCorrect = FALSE;
	int rc;
	char recvbuf[100] = "";
	
	CString str;
	CString temp;
	while (!IsGetCorrect)
	{
		IsGetCorrect = TRUE;
		SendMsg("#get@");

		Sleep(500);
		if (strlen(recvbuf) == 0)
			rc = recv(ClientSocket, recvbuf, 100, 0);

		//char to CString
		int charLen = strlen(recvbuf);
		int len = MultiByteToWideChar(CP_ACP, 0, recvbuf, charLen, NULL, 0);
		TCHAR *buf = new TCHAR[len + 1];
		MultiByteToWideChar(CP_ACP, 0, recvbuf, charLen, buf, len);
		buf[len] = '\0';
		str.Append(buf);
		delete[] buf;
		//str.Format(_T("%s"),recvbuf);

		if (str.Find(L'#') == -1)
		{
			IsGetCorrect = FALSE;
		}
		else
		{
			//分割提取信息
			int min;
			int max;
			double num;
			int cnum;
			//X
			min = str.Find(L'X');
			max = str.Find(L'Y');
			temp = str.Mid(min + 1, max - min - 1);
			num = _wtof(temp.GetBuffer());
			abbp.position[0] = num;
			//Y
			min = str.Find(L'Y');
			max = str.Find(L'Z');
			temp = str.Mid(min + 1, max - min - 1);
			num = _wtof(temp.GetBuffer());
			abbp.position[1] = num;
			//Z
			min = str.Find(L'Z');
			max = str.Find(L'Q');
			temp = str.Mid(min + 1, max - min - 1);
			num = _wtof(temp.GetBuffer());
			abbp.position[2] = num;
			//E1
			min = str.Find(L'Q');
			max = str.Find(L'W');
			temp = str.Mid(min + 1, max - min - 1);
			num = _wtof(temp.GetBuffer());
			abbp.euler[0] = num * pi / 180.0;
			//E2
			min = str.Find(L'W');
			max = str.Find(L'E');
			temp = str.Mid(min + 1, max - min - 1);
			num = _wtof(temp.GetBuffer());
			abbp.euler[1] = num * pi / 180.0;
			//E3
			min = str.Find(L'E');
			max = str.Find(L'R');
			temp = str.Mid(min + 1, max - min - 1);
			num = _wtof(temp.GetBuffer());
			abbp.euler[2] = num * pi / 180.0;
			//C1
			min = str.Find(L'R');
			max = str.Find(L'U');
			temp = str.Mid(min + 1, max - min - 1);
			cnum = _wtoi(temp.GetBuffer());
			abbp.conf[0] = cnum;
			//C1
			min = str.Find(L'U');
			max = str.Find(L'I');
			temp = str.Mid(min + 1, max - min - 1);
			cnum = _wtoi(temp.GetBuffer());
			abbp.conf[1] = cnum;
			//C1
			min = str.Find(L'I');
			max = str.Find(L'O');
			temp = str.Mid(min + 1, max - min - 1);
			cnum = _wtoi(temp.GetBuffer());
			abbp.conf[2] = cnum;
			IsGetCorrect = TRUE;
		}
	}

	//判断是否获取成功
	//if (rc == SOCKET_ERROR){
		//return FALSE;
	//}
	//else{

		//获取的坐标存在toolpos中
		toolpos.euler[0] = abbp.euler[0];
		toolpos.euler[1] = abbp.euler[1];
		toolpos.euler[2] = abbp.euler[2];
		toolpos.position[0] = abbp.position[0];
		toolpos.position[1] = abbp.position[1];
		toolpos.position[2] = abbp.position[2];
		toolpos.conf[0] = abbp.conf[0];
		toolpos.conf[1] = abbp.conf[1];
		toolpos.conf[2] = abbp.conf[2];
		return TRUE;
	//}
}

bool AbbConnector::Disconnect()
{
	SendMsg("#end@");
	Sleep(500);
	if (closesocket(ClientSocket) == 0)
	{

		return TRUE;
	}
	else
		return FALSE;
}

bool AbbConnector::MoveReady(){
	char recvbuf[100]="";
	//char* ptr;
	/*while ((ptr = strstr(recvbuf, "f")) == NULL)
	{
		Sleep(100);
		recv(ClientSocket, recvbuf, 100, 0);
	}*/
	
	
	CString str;
	bool flag = TRUE;
	int charLen;
	int len;

	while (flag){

		SendMsg("#wai@");

		recv(ClientSocket, recvbuf, 100, 0);
		charLen = strlen(recvbuf);
		len = MultiByteToWideChar(CP_ACP, 0, recvbuf, charLen, NULL, 0);
		TCHAR *buf = new TCHAR[len + 1];
		MultiByteToWideChar(CP_ACP, 0, recvbuf, charLen, buf, len);
		buf[len] = '\0';
		str.Append(buf);
		delete[] buf;
		
		if (str.Find(L'rea') >= 0)
		{	
			flag = FALSE;
		}
		Sleep(3000);

	}
	//delete[] recvbuf;
	//SendMsg("#fin@");
	return TRUE;
}