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

class GetProGetProfiles_Ethernet
{

#ifndef LLTGetProfilesEthernetH
#define LLTGetProfilesEthernetH

#define MAX_INTERFACE_COUNT    5
#define MAX_RESOULUTIONS       6

public:

	void GetProfiles_Ethernet();
	void OnError(const char* szErrorTxt, int iErrorValue);
	void DisplayProfile(double* pdValueX, double* pdValueZ, unsigned int uiResolution);
	void DisplayTimestamp(unsigned char* pucTimestamp);
	std::string Double2Str(double dValue);

#endif

};

