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
	ABBPOS toolpos;//��е�۵�ǰλ��
	ABBPOS targetpos;//Ŀ��λ��
public:
	bool Connect();//����ABB������
	bool Disconnect();//�Ͽ�ABB������
	void MoveToolTo(ABBPOS abbp);//ABB�����˹���������ָ���ٶ��˶�������λ�ú���̬
	bool GetToolPos();//����ABB�����˹�������ϵλ�ú���̬
	void SendMsg(char* command);//��ABB�����˷��������ַ���
	bool MoveReady();//�ж��Ƿ��˶����
	//void CStringtochar(char* buf, CString str);
	//void chartoCString(CString str, char* buf);
public:
	AbbConnector();
	~AbbConnector();
protected:
	SOCKET ClientSocket;//�ͻ����׽���
};
