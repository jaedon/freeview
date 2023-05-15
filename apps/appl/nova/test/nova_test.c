/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include "tuner_api.h"


void* g_NovaContext = 0x1004;

// @ Novaspread's Listener APIs
void NOVA_TunerAllocationFinishedListener	(void* aContext, void* aTuner);
void NOVA_TunerReleaseRequestedListener		(void* aContext, void* aTuner);
void NOVA_DataAvailableListener				(void* aContext);
void NOVA_TunerStateChangeListener			(void* aContext, int aNewState);

bool Play_RTL_Television(void);
bool Play_ZDFHD(void);


bool TEST_Init(void)
{
	printf("[%s:%d](+)\n", __FUNCTION__, __LINE__);

	if (TUNERAPI_Init() == false)
	{
		printf("TUNER_MANAGER_Init failed\n");
		return false;
	}

	printf("[%s:%d](-)\n", __FUNCTION__, __LINE__);

	return true;
}

bool TEST_DeInit(void)
{
	printf("[%s:%d](+)\n", __FUNCTION__, __LINE__);

	if (TUNERAPI_DeInit() == false)
	{
		printf("TUNER_MANAGER_DeInit failed\n");
		return false;
	}
	printf("[%s:%d](-)\n", __FUNCTION__, __LINE__);

	return true;
}

bool TEST_Do(void)
{
	int i;

	usleep(3000000); //  After TUNERAPI_Init is called, sleep several sec. if not , octo occurs error.

	for (i = 0 ; i < 1; i++)
	{
		Play_RTL_Television();
		//Play_ZDFHD();
	}

	return true;
}



bool Play_RTL_Television(void)
{
	void* pTuner = NULL;

	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	TUNER_PARAM_t TunerParam =	{TT_SAT, 3, 12188, 27500,  eDxSAT_CODERATE_3_4, eDxSAT_POLAR_HOR, eDxSAT_TRANS_DVBS , eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035};
	unsigned short pids[7]	= {0, 44	, 104 , 105 , 106 , 110 , 163};


	printf(HxANSI_COLOR_GREEN("**********************************************************************\n"));
	printf(HxANSI_COLOR_GREEN("**********************************************************************\n"));
	printf(HxANSI_COLOR_GREEN("                  [TEST] %s\n *"), __FUNCTION__);
	printf(HxANSI_COLOR_GREEN("**********************************************************************\n"));
	printf(HxANSI_COLOR_GREEN("**********************************************************************\n"));

	// @AllocateTuner
	pTuner = TUNERAPI_AllocateTuner(TunerParam, NOVA_TunerAllocationFinishedListener); // RTL Television
	if (pTuner == NULL)
	{
		printf("TUNER_MANAGER_AllocateTuner failed\n");
		return false;
	}

	// @Register listener
	TUNERAPI_SetDataAvailableListner(pTuner, NOVA_DataAvailableListener, g_NovaContext);
	TUNERAPI_SetStateChangeListner(pTuner, NOVA_TunerStateChangeListener, g_NovaContext);


	TUNERAPI_Start(pTuner);

	usleep(3000000);
	ErrCode = TUNERAPI_SetPids(pTuner, pids, 7);
	if (ErrCode != NERR_OK)
	{
		printf("TUNER_MANAGER_SetPids failed\n");
		goto ErrBlock;
	}


	if (1)
	{
		unsigned char* pBuffer;
		FILE* fp = fopen("/receive.ts", "wb");
		int n;
		int readSize;
		size_t wSize;

		pBuffer = malloc(7 * 188 * 10);

		for (n = 0 ; n < 500 ; n++)
		{
			readSize = TUNERAPI_ReadPackets(pTuner, pBuffer, 7 * 188 * 10);
			if (readSize > 0)
			{
				wSize = (size_t)readSize;
				fwrite(pBuffer, 1, wSize, fp);
				printf("readSize:%d\n", readSize);
				usleep(100000);
			}
		}
		fclose(fp);

		free(pBuffer);
		pBuffer = NULL;
	}



	usleep(3000000);
	ErrCode = TUNERAPI_SetPids(pTuner, pids, 1);
	if (ErrCode != NERR_OK)
	{
		printf("TUNER_MANAGER_SetPids failed\n");
		goto ErrBlock;
	}

	usleep(1000000);

	TUNERAPI_Stop(pTuner);

	TUNERAPI_Start(pTuner);

	TUNERAPI_Stop(pTuner);

ErrBlock:
	// @ReleaseTuner
	TUNERAPI_ReleaseTuner(pTuner);

	usleep(1000000); // if it does not sleep octo occurs error!!. to restore , reboot stb.

	printf("[%s:%d](-)\n", __FUNCTION__, __LINE__);

	return true;
}

bool Play_ZDFHD(void)
{
	void* pTuner = NULL;

	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	TUNER_PARAM_t TunerParam = {TT_SAT, 3, 11362, 22000,  eDxSAT_CODERATE_2_3, eDxSAT_POLAR_HOR, eDxSAT_TRANS_DVBS2, eDxSAT_PSK_8PSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035};
	unsigned short pids[9]= {0, 6100, 6110, 6120, 6121, 6123, 6122, 6130 ,6131};


	printf(HxANSI_COLOR_GREEN("**********************************************************************\n"));
	printf(HxANSI_COLOR_GREEN("**********************************************************************\n"));
	printf(HxANSI_COLOR_GREEN("                  [TEST] %s\n *"), __FUNCTION__);
	printf(HxANSI_COLOR_GREEN("**********************************************************************\n"));
	printf(HxANSI_COLOR_GREEN("**********************************************************************\n"));

	// @AllocateTuner
	pTuner = TUNERAPI_AllocateTuner(TunerParam, NOVA_TunerAllocationFinishedListener); // RTL Television
	if (pTuner == NULL)
	{
		printf("TUNER_MANAGER_AllocateTuner failed\n");
		return false;
	}

	// @Register listener
	TUNERAPI_SetDataAvailableListner(pTuner, NOVA_DataAvailableListener, g_NovaContext);
	TUNERAPI_SetStateChangeListner(pTuner, NOVA_TunerStateChangeListener, g_NovaContext);


	TUNERAPI_Start(pTuner);

	usleep(3000000);
	ErrCode = TUNERAPI_SetPids(pTuner, pids, 9);
	if (ErrCode != NERR_OK)
	{
		printf("TUNER_MANAGER_SetPids failed\n");
		goto ErrBlock;
	}

	usleep(3000000);
	ErrCode = TUNERAPI_SetPids(pTuner, pids, 1);
	if (ErrCode != NERR_OK)
	{
		printf("TUNER_MANAGER_SetPids failed\n");
		goto ErrBlock;
	}

	usleep(1000000);

	TUNERAPI_Stop(pTuner);

	TUNERAPI_Start(pTuner);

	TUNERAPI_Stop(pTuner);

ErrBlock:
	// @ReleaseTuner
	TUNERAPI_ReleaseTuner(pTuner);

	usleep(1000000); // if it does not sleep octo occurs error!!. to restore , reboot stb.

	return true;
}





void NOVA_TunerAllocationFinishedListener(void* aContext, void* aTuner)
{
	printf(HxANSI_COLOR_WHITE("***********************************************\n"));
	printf(HxANSI_COLOR_WHITE("Call %s(Context:%p Tuner:%p)\n"), __FUNCTION__, aContext, aTuner);
	printf(HxANSI_COLOR_WHITE("***********************************************\n"));
}

void NOVA_TunerReleaseRequestedListener(void* aContext, void* aTuner)
{
	printf(HxANSI_COLOR_WHITE("***********************************************\n"));
	printf(HxANSI_COLOR_WHITE("Call %s(Context:%p Tuner:%p)\n"), __FUNCTION__, aContext, aTuner);
	printf(HxANSI_COLOR_WHITE("***********************************************\n"));
}

void NOVA_DataAvailableListener(void* aContext)
{
	printf(HxANSI_COLOR_WHITE("***********************************************\n"));
	printf(HxANSI_COLOR_WHITE("Call %s(Context:%p)\n"), __FUNCTION__ , aContext);
	printf(HxANSI_COLOR_WHITE("***********************************************\n"));
}

void NOVA_TunerStateChangeListener(void* aContext, int aNewState)
{
	printf(HxANSI_COLOR_WHITE("***********************************************\n"));
	printf(HxANSI_COLOR_WHITE("Call %s(Context:%p NewState:%d)\n"), __FUNCTION__ , aContext, aNewState);
	printf(HxANSI_COLOR_WHITE("***********************************************\n"));
}
