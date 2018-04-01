#pragma once
#define NOMINMAX
#include <Winsock2.h>
#include <assert.h>
#include <algorithm>
#include "stdlib.h"
typedef struct tagABBPOS{
	double position[3];
	double euler[3];
	int conf[4];
}ABBPOS;


class AbbConnector
{
public:
	ABBPOS toolpos;//机械臂当前位置
	ABBPOS targetpos;//目标位置
public:
	bool Connect();//连接ABB机器人
	bool Disconnect();//断开ABB机器人
	void MoveToolTo(ABBPOS abbp);//ABB机器人工具坐标以指定速度运动到期望位置和姿态
	bool GetToolPos();//返回ABB机器人工具坐标系位置和姿态
	void SendMsg(char* command);//向ABB机器人发送命令字符串
	bool MoveReady();//判断是否运动完毕
	//void CStringtochar(char* buf, CString str);
	//void chartoCString(CString str, char* buf);
public:
	AbbConnector();
	~AbbConnector();
protected:
	SOCKET ClientSocket;//客户端套接字
};
