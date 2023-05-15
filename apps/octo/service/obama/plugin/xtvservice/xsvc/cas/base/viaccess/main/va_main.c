/*
 * (c) 2011-2012 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/*
	Viaccess Initialization API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.23
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "va_def.h"

#include "va_init.h"

#include "va_main.h"
#include "va_os_api.h"
#include "va_schip_api.h"
#include "va_nvm_api.h"
#include "va_sc_api.h"
#include "va_dscr_api.h"
#include "va_dmx_api.h"
#include "va_pi_api.h"
#include "va_ui_api.h"

#include "va_util.h"
#include "va_ctrl.h"
#if defined(CONFIG_MW_CAS_VIACCESS_NET)
#include "va_net_api.h"
#endif

#include "vkernel.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/

static UINT32 							sVaMainTid;
static UINT8 							s_aucSerialNumber[VA_STB_SERIAL_SIZE];
static tVA_INIT_ConfigurationParameters 	s_stConfigParam;
static HBOOL							s_bVaTaskInitialized = FALSE;
static HBOOL							s_bVaTaskStarted = FALSE;
/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static void VA_MAIN_Task(void * args);
void VA_MAIN_Init();

static HBOOL VA_MAIN_TaskInitialized()
{
	return s_bVaTaskInitialized;
}

HBOOL VA_MAIN_TaskStarted()
{
	return s_bVaTaskStarted;
}

void VA_Init()
{
	HxLOG_Print("[Enter]\n");

	s_bVaTaskInitialized = FALSE;
	s_bVaTaskStarted = FALSE;

	// initialize APIs
	VA_NVM_Init();
	VA_OS_Init();
	VA_SC_Init();
	VA_DSCR_Init();
	VA_DMX_Init();
	VA_PI_Adapt_Init();
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	VA_PVR_Init();
#endif
#if defined(CONFIG_MW_CAS_VIACCESS_NET)
	VA_NET_Init();
#endif
	VA_UI_Init();
	VA_UTIL_Init();
	VA_SCHIP_Init();		// dscrambler must be initialized before schip operation starts.

	// initialize ACS
	VA_MAIN_Init();

	// add event listener to ACS and...
	VA_UI_RegisterEventCallbackFunction();

#ifdef CONFIG_DEBUG
	#if defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_HIGH)
		HxLOG_Print("[01;34m CamLock is controlled (high)\n [00m");
	#elif defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_MEDIUM)
		HxLOG_Print("[01;34m CamLock is controlled (medium)\n [00m");
	#elif defined(CONFIG_MW_CAS_VIACCESS_CAMLOCK_LEVEL_LOW)
		HxLOG_Print("[01;34m CamLock is controlled (low)\n [00m");
	#else
		HxLOG_Print("[01;33m CamLock is NOT controlled !!\n [00m");
	#endif
#endif

	s_bVaTaskInitialized = TRUE;

	HxLOG_Print("[Exit]\n");
}

void VA_Start()
{
	int nErr;

	HxLOG_Print("[Enter]\n");

	// already started
	if (sVaMainTid)
		return;

	// create a task for ACS
	nErr = VK_TASK_Create(VA_MAIN_Task, VA_MAIN_TASK_PRIORITY, VA_MAIN_TASK_STACK_SIZE, VA_MAIN_TASK_NAME, NULL, &sVaMainTid, 0);
	if (nErr)
	{
		HxLOG_Info("[VA_Start] failed to create task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VK_TASK_Start(sVaMainTid);
	if (nErr)
	{
		HxLOG_Info("[VA_Start] failed to start task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	HxLOG_Print("[Exit]\n");
}

/*
	initialize each API and ACS
*/
void VA_MAIN_Init()
{
	int i;
	int nErr;

	HxLOG_Print("[Enter]\n");

	sVaMainTid = 0;

	// initialize ACS
	s_stConfigParam.uiNbAcs = VA_MAIN_ACS_NUM;
	s_stConfigParam.uiNbSmartCard = VA_MAIN_SC_NUM;

	s_stConfigParam.stStbInformation.stBoxIdentity.uiSerialNumberSize = VA_STB_SERIAL_SIZE;
	VA_GetSTBSerialNumber(s_aucSerialNumber);
	s_stConfigParam.stStbInformation.stBoxIdentity.pSerialNumber = s_aucSerialNumber;

	s_stConfigParam.stStbInformation.byConstructorIdentifier = VA_MANUFACTURER_IDENTIFIER;	// given by Viaccess
	s_stConfigParam.stStbInformation.byModelIdentifier = VA_MODEL_IDENTIFIER;			// given by Viaccess
	s_stConfigParam.stStbInformation.byHardwareType = 0;
	s_stConfigParam.stStbInformation.uiHardwareVersion = 0;
	s_stConfigParam.stStbInformation.bySoftwareType = 0;
	s_stConfigParam.stStbInformation.uiSoftwareVersion = 0;

	for (i = 0; i < s_stConfigParam.uiNbAcs; i++)
	{
		s_stConfigParam.aAcs[i].stDemux.uiMaxNumberOfFilters = kVA_SETUP_NBMAX_SECTIONFILTERS;				// 16
		s_stConfigParam.aAcs[i].stDemux.uiMaxSizeOfFilter = kVA_DMX_MAX_FILTER_SIZE;							// 10
		s_stConfigParam.aAcs[i].stDescrambler.uiMaxNumberOfChannels = kVA_SETUP_NBMAX_PID_CHANNELS;		//!! depens on h/w
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("VA_INIT_InitializeAcs() calling with uiNbAcs=%d ...\n", VA_MAIN_ACS_NUM);
#endif

#ifdef __VIACCESS50__
	nErr = VA_CTRL_Init(&s_stConfigParam);
#else
	nErr = VA_INIT_InitializeAcs(&s_stConfigParam);
#endif

#ifdef CONFIG_DEBUG
	HxLOG_Print("VA_INIT_InitializeAcs() return %d\n",nErr);
#endif
	VA_ASSERT(nErr == kVA_OK);

	HxLOG_Print("[Exit]\n");
}

/*
	ACS task
	after call VA_INIT_StartAcs(), it enters into ACS task
	after call VA_INIT_StopAcs(), it returns from ACS task and this task also be terminated
*/
static void VA_MAIN_Task(void * args)
{
	HxLOG_Print("[Enter]\n");

	while (VA_MAIN_TaskInitialized() == FALSE)
	{
		HxLOG_Info("Viaccess instance is not initialized yet!\n");
		VK_TASK_Sleep(10);
	}

	s_bVaTaskStarted = TRUE;
#ifdef __VIACCESS50__
	VA_CTRL_Start();
#else
	VA_INIT_StartAcs();
#endif
	HxLOG_Print("[Exit]\n");
}

