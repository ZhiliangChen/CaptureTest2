#include "stdafx.h"
#include "PortOperation.h"

CStringA PortOperation::ChangeCharstr2Hexstr(CStringA Charstr)///字符转为十六进制
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

void PortOperation::HexStringFilter(CStringA &str)///十六进制过滤0-9 a-f A-F
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


char PortOperation::CombineHexChar(char CharH, char CharL) ///十六进制转为字符  CombineHexChar(A,B) result=1011;
{
	char result;
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


CStringA PortOperation::DevideHexChar(char HexChar)///分离十六进制和字符？
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