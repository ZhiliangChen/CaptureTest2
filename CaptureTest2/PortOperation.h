#pragma once
class PortOperation
{

public:

	char CombineHexChar(char CharH, char CharL);
	CStringA ChangeCharstr2Hexstr(CStringA Charstr);
	void HexStringFilter(CStringA &str);
	CStringA DevideHexChar(char HexChar);


};
