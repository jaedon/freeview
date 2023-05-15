/**
	@file     cmd_pal_ci.c
	@brief    pal CI test command tool

	Description: 									\n
	Module: pal/CI			 						\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <htype.h>
#include <vkernel.h>

#include <hlib.h>
#include <util.h>

#include <pal_ci.h>

//#include <mwc_drm.h>
#include <svc_cas.h>
//#include <mwc_util.h>

//#include <ci.h>

//#include <ci_mgr.h>
//#include <ci_ammi.h>

/********************************************************************
 Macro Definition
 ********************************************************************/


/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/

/********************************************************************
 External Variable Declaration
 ********************************************************************/
extern int g_PalCi_PhysDrv_Debug_Flag;

/********************************************************************
 Static Function
 ********************************************************************/

extern void CiMgr_PrintCAMInfo(void);
extern void CiMgr_PrintContextInfo(void);
extern void CIRH_PrintRes(void);

static int CMD_CI_Test(void *szArgList)
{
	HCHAR *aucArg=NULL;
	HCHAR aucStr[20], aucArg2[20], aucArg3[20];
	unsigned long p1, p2;

	/* 1st argument */
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%s", aucStr) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "INFO") == 0)
	{
		CiMgr_PrintCAMInfo();
		CiMgr_PrintContextInfo();
		CIRH_PrintRes();
		PAL_CI_PrintDemuxPath();
		PAL_CI_PrintCIStatus();
		return HxCMD_OK;
	}

	if (strcmp(aucStr, "RESET") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%s", aucArg2) == 0 )
				return HxCMD_ERR;

			p1 = atoi(aucArg2);

			PAL_CI_ResetModule(p1);
			return HxCMD_OK;
		}
	}

	if (strcmp(aucStr, "SETPATH") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%s", aucArg2) == 0 )
				return HxCMD_ERR;

			p1 = atoi(aucArg2);

			/* 3rd argument */
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%s", aucArg3) == 0 )
					return HxCMD_ERR;

				p2 = atoi(aucArg3);

				PAL_CI_SetTsPath(p1, p2);
				return HxCMD_OK;
			}
		}
	}

	if (strcmp(aucStr, "PRINTPATH") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%s", aucArg2) == 0 )
				return HxCMD_ERR;

			p1 = atoi(aucArg2);

			PAL_CI_PrintTsPath(p1);
			return HxCMD_OK;
		}
	}

	if (strcmp(aucStr, "PRINTDMXPATH") == 0)
	{
		PAL_CI_PrintDemuxPath();
	}

	if (strcmp(aucStr, "DEBUGPHYS") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%s", aucArg2) == 0 )
				return HxCMD_ERR;

			p1 = atoi(aucArg2);

			g_PalCi_PhysDrv_Debug_Flag = p1;
			return HxCMD_OK;
		}
	}

	return HxCMD_ERR;
}

#ifdef CONFIG_MW_CI_PLUS

	//#include "ci_plus.h"

//	extern void SHA_Test(void);
//	extern void RSA_Test(void);
//	extern void AES_Test(void);
//	extern void PRNG_Test(void);
//	extern void X509_ParseRawCert_Test(void);

	extern void CiCc_PrintDebugInfo(CI_SLOT_ID usSlotId, int flagPrintMoreDetail);
	extern int  CiCc_SetNegotiatedCckCipher(CI_SLOT_ID, HUINT32);
	extern int  CiAmmi_Init(void);

	#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	extern int CI_InitAuthContexts(void);
	#else
	extern int  CI_InitAuthContexts(CI_SLOT_ID usSlotId);
	#endif

	#if defined(CI_PLUS_PRODUCTION_Credentials) || defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
	extern int  CI_SaveHostLicenseConstantsForTestKitOnly(HBOOL bRetHostLicense, unsigned char *);
	#endif

	extern HERROR PAL_DSC_CloseAll(void);

static int CMD_CI_Plus_Test(void *szArgList)
{
	HCHAR *aucArg=NULL;
	HCHAR aucStr[20]; //, aucArg2[20], aucArg3[20];
//	unsigned long p1, p2;

	/* 1st argument */
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%s", aucStr) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "TEST") == 0)
	{
//		unsigned long s_ulFuncStartTime, s_ulFuncEndTime;

//		s_ulFuncStartTime = VK_TIMER_GetSystemTick();
//		SHA_Test();
//		s_ulFuncEndTime = VK_TIMER_GetSystemTick();
//		HLIB_CMD_Printf("\n SHA_Test done (%ld ms) !!\n\n", s_ulFuncEndTime - s_ulFuncStartTime);

//		s_ulFuncStartTime = VK_TIMER_GetSystemTick();
//		RSA_Test();
//		s_ulFuncEndTime = VK_TIMER_GetSystemTick();
//		HLIB_CMD_Printf("\n RSA_Test done (%ld ms) !!\n\n", s_ulFuncEndTime - s_ulFuncStartTime);

//		s_ulFuncStartTime = VK_TIMER_GetSystemTick();
//		AES_Test();
//		s_ulFuncEndTime = VK_TIMER_GetSystemTick();
//		HLIB_CMD_Printf("\n AES_Test done (%ld ms) !!\n\n", s_ulFuncEndTime - s_ulFuncStartTime);

//		s_ulFuncStartTime = VK_TIMER_GetSystemTick();
//		PRNG_Test();
//		s_ulFuncEndTime = VK_TIMER_GetSystemTick();
//		HLIB_CMD_Printf("\n PRNG_Test done (%ld ms) !!\n\n", s_ulFuncEndTime - s_ulFuncStartTime);

//		s_ulFuncStartTime = VK_TIMER_GetSystemTick();
//		X509_ParseRawCert_Test();
//		s_ulFuncEndTime = VK_TIMER_GetSystemTick();
//		HLIB_CMD_Printf("\n X509_ParseRawCert_Test done (%ld ms) !!\n\n", s_ulFuncEndTime - s_ulFuncStartTime);

		return HxCMD_OK;
	}
	if (strcmp(aucStr, "INFO") == 0)
	{
		CiCc_PrintDebugInfo(0, 0);
		return HxCMD_OK;
	}
	if (strcmp(aucStr, "INFOMORE") == 0)
	{
		CiCc_PrintDebugInfo(0, 1);
		return HxCMD_OK;
	}
	if (strcmp(aucStr, "SETDES") == 0)
	{
		CiCc_SetNegotiatedCckCipher(0, 0);
		return HxCMD_OK;
	}
	if (strcmp(aucStr, "SETAES") == 0)
	{
		CiCc_SetNegotiatedCckCipher(0, 1);
		return HxCMD_OK;
	}
	if (strcmp(aucStr, "STOPDSC") == 0)
	{
		if(PAL_DSC_CloseAll())
			return HxCMD_ERR;
		else
			return HxCMD_OK;
	}
	if (strcmp(aucStr, "KEYRESET") == 0)
	{
		unsigned long s_ulFuncStartTime, s_ulFuncEndTime;

		s_ulFuncStartTime = VK_TIMER_GetSystemTick(
			);
		#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
		if (CI_InitAuthContexts())
		#else
		if (CI_InitAuthContexts(0))
		#endif
		{
			HLIB_CMD_Printf("\n failed to initialize authentication contexts !!\n\n");
			return HxCMD_ERR;
		}
		else
		{
			s_ulFuncEndTime = VK_TIMER_GetSystemTick();
			HLIB_CMD_Printf("\n Ok. Please reset now (%ld ms) !!\n\n", s_ulFuncEndTime - s_ulFuncStartTime);
			return HxCMD_OK;
		}
	}

	#if defined(CI_PLUS_PRODUCTION_Credentials) || defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
	if (strcmp(aucStr, "SETUPTESTKIT") == 0)
	{
		unsigned long s_ulFuncStartTime, s_ulFuncEndTime;

		#if defined(CI_PLUS_PRODUCTION_Credentials)
		HLIB_CMD_Printf("\n Please wait one or two minutes for setup PRODUCTION Credentials (for test only) ...\n");
		#elif defined(CI_PLUS_TEST_Technology_from_TrustCenter)
		HLIB_CMD_Printf("\n Please wait one or two minutes for setup TEST Technology from TrustCenter (for test only) ...\n");
		#elif defined(CI_PLUS_TEST_Technology_from_SmarDTV)
		HLIB_CMD_Printf("\n Please wait one or two minutes for setup TEST Technology from SmarDTV (for test only) ...\n");
		#else
		HLIB_CMD_Printf("\n No Credentials !!!\n\n");
		return HxCMD_ERR;
		#endif

		s_ulFuncStartTime = VK_TIMER_GetSystemTick();
		if (CI_SaveHostLicenseConstantsForTestKitOnly(FALSE, NULL))
		{
			HLIB_CMD_Printf("\n failed to save host license constants !!\n\n");
			return HxCMD_ERR;
		}
		else
		{
			#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
			if (CI_InitAuthContexts())
			#else
			if (CI_InitAuthContexts(0))
			#endif
			{
				HLIB_CMD_Printf("\n failed to initialize authentication contexts !!\n\n");
				return HxCMD_ERR;
			}
			else
			{
				s_ulFuncEndTime = VK_TIMER_GetSystemTick();
				HLIB_CMD_Printf("\n Ok. Please reset now (%ld ms) !!\n\n", s_ulFuncEndTime - s_ulFuncStartTime);
				return HxCMD_OK;
			}
		}
	}
	#endif

	#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_)

	if (strcmp(aucStr, "STARTAMMI") == 0)
	{
		HLIB_CMD_Printf("start ammi \n");
		CiAmmi_Emul_RequestStart(0, NULL);
		return HxCMD_OK;
	}
	#endif

	if (strcmp(aucStr, "MHEG") == 0)
	{
		CiAmmi_Init();
		return HxCMD_OK;
	}

	return HxCMD_ERR;
}

static int CMD_CI_Plus_URI_Test(void *szArgList)
{
	HCHAR *aucArg=NULL;
	HCHAR aucStr[20];
	HCHAR aucArgN[20];
	CIPLUS_UriInfo_t stUriInfo;
//	DxCopyrightControl_t drm_info;
//	HINT32 p1 = 0, p2 = 0, p3 = 0;

	/* 1st argument */
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		goto ERROR_EXIT;
	if( sscanf(aucArg, "%s", aucStr) == 0 )
		goto ERROR_EXIT;

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "INFO") == 0)
	{

		return HxCMD_OK;
	}

	if ( (strcmp(aucStr, "SET") == 0)
		|| (strcmp(aucStr, "SETAP") == 0) )
	{
		HxSTD_memset(&stUriInfo, 0x00, sizeof(CIPLUS_UriInfo_t));
//		HxSTD_memset(&drm_info, 0x00, sizeof(DxCopyrightControl_t));

		/* 2nd argument - APS */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL )
			goto ERROR_EXIT;
		if( sscanf(aucArg, "%s", aucArgN) == 0 )
			goto ERROR_EXIT;
		stUriInfo.ucAps = atoi(aucArgN);
		if( stUriInfo.ucAps > CI_CC_URI_APS_ON_3 )
			goto ERROR_EXIT;

		/* 3rd argument - EMI */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL )
			goto ERROR_EXIT;
		if( sscanf(aucArg, "%s", aucArgN) == 0 )
			goto ERROR_EXIT;
		stUriInfo.ucEmi = atoi(aucArgN);
		if( stUriInfo.ucEmi > CI_CC_URI_EMI_COPY_NEVER )
			goto ERROR_EXIT;

		/* 4th argument - ICT */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL )
			goto ERROR_EXIT;
		if( sscanf(aucArg, "%s", aucArgN) == 0 )
			goto ERROR_EXIT;
		stUriInfo.ucIct= atoi(aucArgN);
		if( stUriInfo.ucIct > CI_CC_URI_ICT_ON )
			goto ERROR_EXIT;

		/* 5th argument - RCT */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL )
			goto ERROR_EXIT;
		if( sscanf(aucArg, "%s", aucArgN) == 0 )
			goto ERROR_EXIT;
		stUriInfo.ucRct= atoi(aucArgN);
		if( stUriInfo.ucRct > CI_CC_URI_RCT_ON )
			goto ERROR_EXIT;

		stUriInfo.ucMode = CI_CC_URI_MODE_FROMCAM;
		stUriInfo.ulMinutesForRL = 90;
		stUriInfo.usServiceId = 0x00;	// 사용하지 않아요~

		// set debug URI
		if (strcmp(aucStr, "SETAP") == 0)
		{
#if 0
			// 직접 ap로 msg 전달하여 uri setting하는 방법. 강제 테스트용
			drm_info.eDrmType = eDxCopyrightControl_CasCcType_CiPlus;
			drm_info.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucMode = stUriInfo.ucMode;
			drm_info.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucAps = stUriInfo.ucAps;
			drm_info.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucEmi = stUriInfo.ucEmi;
			drm_info.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucIct = stUriInfo.ucIct;
			drm_info.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ucRct = stUriInfo.ucRct;
			drm_info.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL = stUriInfo.ulMinutesForRL;

			MGR_COPYRIGHTCONTROL_ConvertCcInfoToCcMsgArg (drm_info, &p1, &p2, &p3);
#endif
		}
		else
		{

		}

		return HxCMD_OK;
	}

ERROR_EXIT:

	HLIB_CMD_Printf("\n#################################################################################### \n");
	HLIB_CMD_Printf("URI command usage ::\n\n");
	HLIB_CMD_Printf("URI Info : print latest URI setting \n");
	HLIB_CMD_Printf("URI Set <APS > <EMI> <ICT> <RCT> : msg를 ci mgr->cas mgr->ap wtv 의 순서로 전달한다 \n");
	HLIB_CMD_Printf("URI SetAp <APS > <EMI> <ICT> <RCT> : msg를 ap wtv로 직접 전달하여 output control 을 한다 \n");
	HLIB_CMD_Printf("\t APS : (%d)Off, (%d)APS_ON_1, (%d)APS_ON_2, (%d)APS_ON_3\n",
				CI_CC_URI_APS_OFF, CI_CC_URI_APS_ON_1, CI_CC_URI_APS_ON_2, CI_CC_URI_APS_ON_3);
	HLIB_CMD_Printf("\t EMI : (%d)COPY_FREE, (%d)COPY_NO_MORE, (%d)COPY_ONCE, (%d)COPY_NEVER\n",
				CI_CC_URI_EMI_COPY_FREE, CI_CC_URI_EMI_COPY_NO_MORE, CI_CC_URI_EMI_COPY_ONCE, CI_CC_URI_EMI_COPY_NEVER);
	HLIB_CMD_Printf("\t ICT : (%d)Off, (%d)On\n",
				CI_CC_URI_ICT_OFF, CI_CC_URI_ICT_ON);
	HLIB_CMD_Printf("\t RCT : (%d)Off, (%d)On\n",
				CI_CC_URI_RCT_OFF, CI_CC_URI_RCT_ON);
	HLIB_CMD_Printf("#################################################################################### \n\n");

	return HxCMD_OK;
}
#endif // CONFIG_MW_CI_PLUS

#ifdef WIN32

// #include "pal_ci_emul.h"

static int CMD_CI_Emul_Test(void *szArgList)
{
	HCHAR *aucArg=NULL;
	HCHAR aucStr[20], aucArg2[20], aucArg3[20];
	unsigned long p1, p2;
	HUINT32 ulRet=0;

	/* 1st argument */
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%s", &aucStr) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "IO") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p1 ) == 0 )
				return HxCMD_ERR;

			EMUL_CI_CamState(p1);
			return HxCMD_OK;
		}
	}
	else if (strcmp(aucStr, "APP") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p1 ) == 0 )
				return HxCMD_ERR;

			EMUL_CI_AppInfo(p1);
			return HxCMD_OK;
		}
	}
	else if (strcmp(aucStr, "ID") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p1 ) == 0 )
				return HxCMD_ERR;

			EMUL_CI_CaSysId(p1);
			return HxCMD_OK;
		}
	}
	else if (strcmp(aucStr, "MMITEST") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p1 ) == 0 )
				return HxCMD_ERR;

			EMUL_CI_CamState(p1);
			EMUL_CI_CamState(p1);
			EMUL_CI_CamState(p1);
			EMUL_CI_CamState(p1);

			EMUL_CI_AppInfo(p1);
			return HxCMD_OK;
		}
	}
	else if (strcmp(aucStr, "ENQ") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p1 ) == 0 )
				return HxCMD_ERR;

			EMUL_CI_ReqMmiEnq(p1);
			return HxCMD_OK;
		}
	}

	return HxCMD_ERR;
}

#endif

/********************************************************************
 Global Function
 ********************************************************************/
#define	hCmdHandle		"pal_ci"

void CMD_Register_Pal_Ci(void)
{
#if defined(CONFIG_MW_CI) && defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord(hCmdHandle, CMD_CI_Test,
			"CIV1", /* keyword */
			"CIV1 TEST", /* help */
			"CIV1 Info | Reset <slot> | SetPath <slot> <0:bypass/1:passthru> | PrintPath <slot> | PrintDmxPath <DmxId> | DebugPhys <0:1>"); /* usage */
	#if defined(CONFIG_MW_CI_PLUS)
	HLIB_CMD_RegisterWord(hCmdHandle, CMD_CI_Plus_Test,
			"CI+", /* keyword */
			"CI+ TEST", /* help */
			#if defined(CI_PLUS_PRODUCTION_Credentials) || defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
			"CI+ Info | InfoMore | SetDes | SetAes | StopDsc | KeyReset | SetupTestKit | MHEG"); /* usage */
			#else
			"CI+ Info | InfoMore | SetDes | SetAes | StopDsc | KeyReset | MHEG"); /* usage */
			#endif
	HLIB_CMD_RegisterWord(hCmdHandle, CMD_CI_Plus_URI_Test,
			"URI", /* keyword */
			"Print URI Info, Set URI", /* help */
			"URI Info | Set <APS > <EMI> <ICT> <RCT>"); /* usage */
	#endif	// CONFIG_MW_CI_PLUS
#endif

	return;
}

