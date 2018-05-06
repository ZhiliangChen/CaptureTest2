//   GetProfiles_Ethernet.h: demo-application for using the LLT.dll
//
//   Version 3.0.0.0
//
//   Copyright 2009
//
//   Sebastian Lueth
//   MICRO-EPSILON Optronic GmbH
//   Lessingstrasse 14
//   01465 Dresden OT Langebrueck
//   Germany
//---------------------------------------------------------------------------
//#include "InterfaceLLT_2.h"
class GetProfiles_Ethernet
{

#ifndef LLTGetProfilesEthernetH
#define LLTGetProfilesEthernetH

#define MAX_INTERFACE_COUNT    5
#define MAX_RESOULUTIONS       6

public:

	void GetProfiles();
	void DisplayProfile(double* pdValueX, double* pdValueZ, unsigned int uiResolution);
	void DisplayTimestamp(unsigned char* pucTimestamp);
	//std::string Double2Str(double dValue);
	//CInterfaceLLT InterfaceLLT;

	static int iRetValue;
	static int num_valid;
	static int num_right;
	static int num_left;
	static double m_gap;
	static double remakex[1280];
	static double remakez[1280];
	static int gapnum_right;
	static int gapnum_left;
	static double dShutterOpen;
	static double dShutterClose;
	static unsigned int uiProfileCount;

#endif

};


