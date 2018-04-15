//   GetProfiles_Ethernet.cpp: demo-application for using the LLT.dll
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

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include "InterfaceLLT_2.h"
#include "GetProfiles_Ethernet.h"

CInterfaceLLT InterfaceLLT;

void GetProGetProfiles_Ethernet::GetProfiles_Ethernet()
{
  int iRetValue;
  std::vector<double> vdValueX(m_uiResolution);
  std::vector<double> vdValueZ(m_uiResolution);
  // Resize the profile buffer to the maximal profile size
  std::vector<unsigned char> vucProfileBuffer(m_uiResolution * 4 + 16);

  cout << "\nDemonstrate the profile transfer via poll function\n";

  cout << "Gets the type of the scanCONTROL (measurement range)\n";

  cout << "Enable the measurement\n";
  if((iRetValue = InterfaceLLT.TransferProfiles(NORMAL_TRANSFER, true)) < GENERAL_FUNCTION_OK)
  {
    OnError("Error during TransferProfiles", iRetValue);
    return;
  }

  // Sleep for a while to warm up the transfer
  Sleep(100);

  // Gets 1 profile in "polling-mode" and PURE_PROFILE configuration
  if((iRetValue = InterfaceLLT.GetActualProfile(&vucProfileBuffer[0], (unsigned int)vucProfileBuffer.size(), PURE_PROFILE, NULL)) !=
     vucProfileBuffer.size())
  {
    OnError("Error during GetActualProfile", iRetValue);
    return;
  }
  cout << "Get profile in polling-mode and PURE_PROFILE configuration OK \n";

  cout << "Converting of profile data from the first reflection\n";
  iRetValue = InterfaceLLT.ConvertProfile2Values(&vucProfileBuffer[0], m_uiResolution, PURE_PROFILE, m_tscanCONTROLType, 0, true, NULL,
                                            NULL, NULL, &vdValueX[0], &vdValueZ[0], NULL, NULL);
  if(((iRetValue & CONVERT_X) == 0) || ((iRetValue & CONVERT_Z) == 0))
  {
    OnError("Error during Converting of profile data", iRetValue);
    return;
  }

  DisplayProfile(&vdValueX[0], &vdValueZ[0], m_uiResolution);

  cout << "\n\nDisplay the timestamp from the profile:";
  DisplayTimestamp(&vucProfileBuffer[m_uiResolution * 4]);

  cout << "Disable the measurement\n";
  if((iRetValue = InterfaceLLT.TransferProfiles(NORMAL_TRANSFER, false)) < GENERAL_FUNCTION_OK)
  {
    OnError("Error during TransferProfiles", iRetValue);
    return;
  }
}

// Displaying the error text
void GetProGetProfiles_Ethernet::OnError(const char* szErrorTxt, int iErrorValue)
{
  char acErrorString[200];

  cout << szErrorTxt << "\n";
  if(InterfaceLLT.TranslateErrorValue(iErrorValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
    cout << acErrorString << "\n\n";
}

// Displays one profile
void GetProGetProfiles_Ethernet::DisplayProfile(double* pdValueX, double* pdValueZ, unsigned int uiResolution)
{
  size_t tNumberSize;
  int testnum = 0;
  int gapnum1 = 0;
  int gapnum2 = 0;
  double gapwidth;
  double valuex[1280];
  double valuez[1280];
  int testnum2 = 0;
  int j = 0;
  double remakex[1280];
  double remakez[1280];

  for(unsigned int i = 0; i < uiResolution; i++)
  {
	 
    // Prints the X- and Z-values
    //tNumberSize = Double2Str(*pdValueX).size();
	  valuex[i] = *pdValueX++;
	  valuez[i] = *pdValueZ++;

    cout << "\r"
         << "Profiledata: X = " << valuex[i];
    //for(; tNumberSize < 8; tNumberSize++)
    //{
    //  cout << " ";
    //}

   // tNumberSize = Double2Str(*pdValueZ).size();
    cout << " Z = " << valuez[i];
    //for(; tNumberSize < 8; tNumberSize++)
    //{
    //  cout << " ";
    //}
	
	if (valuez[i] != 0)
	{
		remakex[j] = valuex[i];
		remakez[j] = valuez[i];
		if (j > 0)
		{
			gapwidth = remakez[j] - remakez[j - 1];
			if (gapwidth >5)
			{
				gapnum2 = testnum;
				
				//cout << "\n" << testnum;
			}
			if ((gapwidth>-3.1) &&(gapwidth < -2.5))
			{
				gapnum1 = testnum - 1;
				//cout << "\n" << testnum;
				testnum2++;
			}
		}
		testnum++;
		j++;
		
	}
	//gapwidth = valuex[gapnum1] - valuex[gapnum2];

	
	

    // Somtimes wait a short time (only for display)
	if (i % 8 == 0) 
	{
		Sleep(10);
	}
      
   
  }
  gapwidth = remakex[gapnum1] - remakex[gapnum2];

  cout << "\n" << "gap: X = " << gapwidth <<" "<< remakex[gapnum1] << " " << remakex[gapnum2] << " " << testnum2;
}

// Displays the timestamp
void GetProGetProfiles_Ethernet::DisplayTimestamp(unsigned char* pucTimestamp)
{
  double dShutterOpen, dShutterClose;
  unsigned int uiProfileCount;

  // Decode the timestamp
  InterfaceLLT.Timestamp2TimeAndCount(pucTimestamp, &dShutterOpen, &dShutterClose, &uiProfileCount);
  cout << "\nShutterOpen: " << dShutterOpen << " ShutterClose: " << dShutterClose << "\n";
  cout << "ProfileCount: " << uiProfileCount << "\n";
  cout << "\n";
}


