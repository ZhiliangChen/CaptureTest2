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
#include <vector>


extern unsigned int m_uiResolution;
extern TScannerType m_tscanCONTROLType;
extern CInterfaceLLT* m_pLLT;

int GetProfiles_Ethernet::num_valid;
int GetProfiles_Ethernet::num_right;
int GetProfiles_Ethernet::num_left;
double GetProfiles_Ethernet::m_gap;
double GetProfiles_Ethernet::remakex[1280];
double GetProfiles_Ethernet::remakez[1280];
int GetProfiles_Ethernet::gapnum_right;
int GetProfiles_Ethernet::gapnum_left;
double GetProfiles_Ethernet::dShutterOpen;
double GetProfiles_Ethernet::dShutterClose;
unsigned int GetProfiles_Ethernet::uiProfileCount;

double gapwidth;

void GetProfiles_Ethernet::GetProfiles()
{
	
  std::vector<double> vdValueX(m_uiResolution);
  std::vector<double> vdValueZ(m_uiResolution);
  // Resize the profile buffer to the maximal profile size
  std::vector<unsigned char> vucProfileBuffer(m_uiResolution * 4 + 16);
  int divisor_average = 20;
  m_gap = 0;
  //ȡ10�����ܾ�����ȡƽ��ֵ
  for (int m_average = 0; m_average < 20; m_average++) {


	  //Demonstrate the profile transfer via poll function
	  //Gets the type of the scanCONTROL (measurement range)
	  //Enable the measurement
	  if ((iRetValue = m_pLLT->TransferProfiles(NORMAL_TRANSFER, true)) < GENERAL_FUNCTION_OK)
	  {
		  //OnError("Error during TransferProfiles", iRetValue);
		  return;
	  }
	  // Sleep for a while to warm up the transfer
	  Sleep(100);

	  // Gets 1 profile in "polling-mode" and PURE_PROFILE configuration
	  if ((iRetValue = m_pLLT->GetActualProfile(&vucProfileBuffer[0], (unsigned int)vucProfileBuffer.size(), PURE_PROFILE, NULL)) !=
		  vucProfileBuffer.size())
	  {
		  // OnError("Error during GetActualProfile", iRetValue);
		  return;
	  }
	  //Get profile in polling-mode and PURE_PROFILE configuration OK 
	 //Converting of profile data from the first reflection
	  iRetValue = m_pLLT->ConvertProfile2Values(&vucProfileBuffer[0], m_uiResolution, PURE_PROFILE, m_tscanCONTROLType, 0, true, NULL,
		  NULL, NULL, &vdValueX[0], &vdValueZ[0], NULL, NULL);

	  if (((iRetValue & CONVERT_X) == 0) || ((iRetValue & CONVERT_Z) == 0))
	  {
		  //OnError("Error during Converting of profile data", iRetValue);
		  return;
	  }

	  DisplayProfile(&vdValueX[0], &vdValueZ[0], m_uiResolution);
	  if ((num_left == 0) || (num_right == 0))
	  {
		  divisor_average --;
		  gapwidth = 0;
	  }
	  //Ϊʲô���漸��ѭ������0��
	  m_gap += gapwidth;
	 
	  //Sleep(100);

	  //Disable the measurement
	  if ((iRetValue = m_pLLT->TransferProfiles(NORMAL_TRANSFER, false)) < GENERAL_FUNCTION_OK)
	  {
		  //OnError("Error during TransferProfiles", iRetValue);
		  return;
	  }
  }
  //�������յĽ��ܾ���
	 m_gap = m_gap / divisor_average;
	  //Display the timestamp from the profile
	  DisplayTimestamp(&vucProfileBuffer[m_uiResolution * 4]);
  

}


// Displays one profile
void GetProfiles_Ethernet::DisplayProfile(double* pdValueX, double* pdValueZ, unsigned int uiResolution)
{
//  size_t tNumberSize;
  double valuex[1280];
  double valuez[1280];
  int j = 0;
  num_left = num_right = 0;
  num_valid = 0;
  memset(remakex, 0, 1280);
  memset(remakez, 0, 1280);
  gapnum_right = gapnum_left = 0;
  for(unsigned int i = 0; i < uiResolution; i++)
  {
	 
    // Prints the X- and Z-values
    //tNumberSize = Double2Str(*pdValueX).size();
	  valuex[i] = *pdValueX++;
	  valuez[i] = *pdValueZ++;

	
	if (valuez[i] != 0)
	{
		remakex[j] = valuex[i];
		remakez[j] = valuez[i];
		if (j > 0)
		{
			gapwidth = remakez[j] - remakez[j - 1];
			//�ӹ��⾶����������9mm����Щ������ڽӹ��ϱ��淴���ȥ�����ȡ��Сֵ4mm�����ж�
			if ((gapwidth >5) && (gapwidth < 10))
			{
				gapnum_left = num_valid;
				num_left++;
			}
			if ((gapwidth>-2.8) &&(gapwidth < -2.6))
			//if (gapwidth < -2)
			{
				gapnum_right = num_valid - 1;
				num_right++;
			}
		}
		num_valid++;
		j++;	
	}
  }
  gapwidth = remakex[gapnum_right] - remakex[gapnum_left];
  //��һ��Ч��ȡ����1280������᲻�������⣿Ϊʲô�������������
  
  //memset
}

// Displays the timestamp
void GetProfiles_Ethernet::DisplayTimestamp(unsigned char* pucTimestamp)
{
  // Decode the timestamp
  m_pLLT->Timestamp2TimeAndCount(pucTimestamp, &dShutterOpen, &dShutterClose, &uiProfileCount);

}


