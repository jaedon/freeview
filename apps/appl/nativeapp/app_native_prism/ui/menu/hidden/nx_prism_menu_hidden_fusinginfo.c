/*
 * (c) 2011-2013 Humax Co., Ltd.
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

/******************************************************************************/
/**
 * @file	  		nx_prism_menu_hiddne_fusinginfo.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_common.h>
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
#define	NX_PRISM_HIDDEN_FUSING_ITEM_ID							eMenuFrame_Window_HiddenFusing

#define	NX_PRISM_HIDDEN_FUSING_WINDOW_WIDTH						720
#define	NX_PRISM_HIDDEN_FUSING_WINDOW_HEIGHT					576
#define	NX_PRISM_HIDDEN_FUSING_WINDOW_WINDOW_X					0
#define	NX_PRISM_HIDDEN_FUSING_WINDOW_WINDOW_Y					0

#define	NX_PRISM_HIDDEN_FUSING_FRAME_ID							(NX_PRISM_HIDDEN_FUSING_ITEM_ID + 1)
#define	NX_PRISM_HIDDEN_FUSING_FRAME_X							0
#define	NX_PRISM_HIDDEN_FUSING_FRAME_Y							0
#define	NX_PRISM_HIDDEN_FUSING_FRAME_W							NX_PRISM_HIDDEN_FUSING_WINDOW_WIDTH
#define	NX_PRISM_HIDDEN_FUSING_FRAME_H							NX_PRISM_HIDDEN_FUSING_WINDOW_HEIGHT

#define	NX_PRISM_HIDDEN_FUSING_TEXT_ID							(NX_PRISM_HIDDEN_FUSING_ITEM_ID + 2)
#define	NX_PRISM_HIDDEN_FUSING_TEXT_X							150
#define	NX_PRISM_HIDDEN_FUSING_TEXT_Y							100
#define	NX_PRISM_HIDDEN_FUSING_TEXT_W							(NX_PRISM_HIDDEN_FUSING_FRAME_W - NX_PRISM_HIDDEN_FUSING_TEXT_X)
#define	NX_PRISM_HIDDEN_FUSING_TEXT_H							(NX_PRISM_HIDDEN_FUSING_FRAME_H - NX_PRISM_HIDDEN_FUSING_TEXT_Y)

#define	NX_PRISM_HIDDEN_FUSING_SERIAL_NUMBER_LEN				34
#define	NX_PRISM_HIDDEN_FUSING_LANGUAGE_LEN						24


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tag_NX_MENUHIDDENFUSINGINFO_SET_CONTENT
{
	HUINT8		*pszTextMessage;

} Nx_MenuHiddenFusingInfoSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuHiddenFusingInfoSetContent_t	s_stHiddenFusingManagementContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_MenuHiddenFusingInfoSetContent_t* nx_menuHiddenFusingInfo_GetContents(void);

static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeLanguageString(HUINT8 *pszlangStr);
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeSerialNumberString(HUINT8 *pszSerialNumber);
#if defined(CONFIG_MW_CAS_IRDETO)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeCasIrdetoString(HUINT8 *pszIrdetoMsg1, HUINT8 *pszIrdetoMsg2);
#endif
#if defined(CONFIG_MW_CAS_CONAX)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeCasConaxString(HUINT8 *pszConaxCpmMsg1);
#endif
#if defined(CONFIG_MW_CAS_NAGRA)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeCasNagraString(HUINT8 *pszCasNagraStr);
#endif
#if defined(CONFIG_MW_CAS_VIACCESS)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeCasViaccessString(HUINT8 *pszCasViaccessStr);
#endif

static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeData(Nx_MenuHiddenFusingInfoSetContent_t *pstContents);

static ONDK_Result_t 	nx_menuHiddenFusingInfo_SetUiObject(Nx_MenuHiddenFusingInfoSetContent_t *pstContents);

static ONDK_Result_t	nx_menuHiddenFusingInfo_MsgGwmCreate(Nx_MenuHiddenFusingInfoSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuHiddenFusingInfo_MsgGwmDestroy(Nx_MenuHiddenFusingInfoSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHiddenFusingInfo_MsgGwmHapiMsg(Nx_MenuHiddenFusingInfoSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHiddenFusingInfo_MsgGwmKeyDown(Nx_MenuHiddenFusingInfoSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_MenuHiddenFusingInfoSetContent_t* nx_menuHiddenFusingInfo_GetContents(void)
{
	return	&s_stHiddenFusingManagementContent;
}


static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeSerialNumberString(HUINT8 *pszSerialNumber)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		i = 0, ucSerialNumber[NX_TEXT_SIZE_32] = {0,};


	HxSTD_memset(ucSerialNumber, 0, sizeof(ucSerialNumber));
	hError = NX_SYSTEM_GetSerialNumber((HCHAR*)ucSerialNumber, NX_TEXT_SIZE_32);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!! NX_SYSTEM_GetSerialNumber() Fail!! \n");
	}

	for (i = 0; i < NX_COMMON_SERIAL_NUMBER_SIZE; i++)
	{
		ucSerialNumber[i] = ucSerialNumber[i] & 0x0F;
	}

	HxSTD_snprintf((char*)pszSerialNumber, NX_PRISM_HIDDEN_FUSING_SERIAL_NUMBER_LEN, "S N : %X %X %X %X %X %X %X %X %X %X %X %X %X %X",
					ucSerialNumber[0],ucSerialNumber[1],ucSerialNumber[2], ucSerialNumber[3],ucSerialNumber[4],
					ucSerialNumber[5],ucSerialNumber[6],ucSerialNumber[7],ucSerialNumber[8],ucSerialNumber[9],
					ucSerialNumber[10],ucSerialNumber[11],ucSerialNumber[12],ucSerialNumber[13]);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeLanguageString(HUINT8 *pszlangStr)
{
	HERROR		hError = ERR_FAIL;
	HINT32		lLanageId = eLangID_NONE;
	HUINT8 		*pszLanguage = NULL;
	HUINT8		szLangCode[NX_COMMON_LANGUAGE_CODE_SIZE] = {0,};

	hError = NX_SYSTEM_GetMenuLanuage(&lLanageId);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!! NX_SYSTEM_GetMenuLanuage() Fail!! \n");
	}

	pszLanguage = (HUINT8 *)HLIB_LANG_IdTo639((HxLANG_Id_e)lLanageId);
	if (pszLanguage == NULL)
	{
		HxSTD_snprintf((char*)pszlangStr, NX_PRISM_HIDDEN_FUSING_LANGUAGE_LEN, "L a n g u a g e : ");
	}
	else
	{
		HxSTD_StrNCpy((char*)szLangCode, (char*)pszLanguage, NX_COMMON_LANGUAGE_CODE_SIZE);
		HLIB_STD_StrUpper((char*)szLangCode);

		HxSTD_snprintf((char*)pszlangStr, NX_PRISM_HIDDEN_FUSING_LANGUAGE_LEN, "L a n g u a g e : %c %c %c", szLangCode[0], szLangCode[1], szLangCode[2]);
	}

	return	ONDK_RET_OK;
}


#if defined(CONFIG_MW_CAS_IRDETO)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeCasIrdetoString(HUINT8 *pszIrdetoMsg1, HUINT8 *pszIrdetoMsg2)
{
	HUINT8		szStr1[NX_TEXT_SIZE_16] = {0,};
	HUINT8		szStr2[NX_TEXT_SIZE_16] = {0,};
	HUINT8		szStr3[NX_TEXT_SIZE_16] = {0,};

	NX_CAS_IRDETO_GetIrdetoManCode(szStr1);
	NX_CAS_IRDETO_GetIrdetoHwCode(szStr2);
	NX_CAS_IRDETO_GetIrdetoVariant(szStr3);

	HxSTD_sprintf((HUINT8*)pszIrdetoMsg1, (HUINT8 *)"I R - D E V : %s,%s,%s", szStr1, szStr2, szStr3);

	HxSTD_memset(szStr1, 0, NX_TEXT_SIZE_16);
	HxSTD_memset(szStr2, 0, NX_TEXT_SIZE_16);
	HxSTD_memset(szStr3, 0, NX_TEXT_SIZE_16);

	NX_CAS_IRDETO_GetIrdetoSysId(szStr1);
	NX_CAS_IRDETO_GetIrdetoSigVer(szStr2);
	NX_CAS_IRDETO_GetIrdetoKeyVer(szStr3);

	HxSTD_sprintf((HUINT8*)pszIrdetoMsg2, (HUINT8 *)"I R - K E Y : %s,%s,%s", szStr1, szStr2, szStr3);

	return	ONDK_RET_OK;
}
#endif


#if defined(CONFIG_MW_CAS_CONAX)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeCasConaxString(HUINT8 *pszConaxCpmMsg1)
{
	// TODO: [JHLEE] Add
	HxSTD_sprintf((HUINT8 *)pszConaxCpmMsg1, (HUINT8 *)"C - R M - F S : ");

	return	ONDK_RET_OK;
}
#endif


#if defined(CONFIG_MW_CAS_NAGRA)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeCasNagraString(HUINT8 *pszCasNagraStr)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		szCASSN[NX_CAS_NA_SYSTEM_INFO_COMPONENT_LEN] = {0,};

	hError = NX_CAS_NAGRA_GetCASN(szCASSN);
	if (hError == ERR_OK)
	{
		HxSTD_sprintf((char *)pszCasNagraStr, (char *)"C A - S N : %s", szCASSN);
	}
	else
	{
		HxSTD_sprintf((char *)pszCasNagraStr, (char *)"C A - S N : ");
	}

	return	ONDK_RET_OK;
}
#endif


#if defined(CONFIG_MW_CAS_VIACCESS)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeCasViaccessString(HUINT8 *pszCasViaccessStr)
{
	// TODO: [JHLEE] Add

	HxSTD_sprintf((HUINT8 *)pszCasViaccessStr, (HUINT8 *)"B B X : ");

	return	ONDK_RET_OK;
}
#endif


#if defined(CONFIG_MW_CAS)
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeData(Nx_MenuHiddenFusingInfoSetContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HBOOL 			bIsSCS = FALSE, bIsJTAG = FALSE, bIsOTP = FALSE, bIsCWE = FALSE;
	HCHAR			*pszLanguage = NULL;

	HUINT8			serialNumberStr[NX_TEXT_SIZE_64], langStr[NX_TEXT_SIZE_64];

	HUINT8			pszSecureInfoMsg1[NX_TEXT_SIZE_64] = {0, }, pszSecureInfoMsg2[NX_TEXT_SIZE_64] = {0, };
	HUINT8			pszConaxCpmMsg1[NX_TEXT_SIZE_64]={0,}, pszMarketID[NX_TEXT_SIZE_64]={0,};
	HUINT8			pszIrdetoMsg1[NX_TEXT_SIZE_64] = {0,}, pszIrdetoMsg2[NX_TEXT_SIZE_64] = {0,};

	HUINT8			pszBBX[NX_TEXT_SIZE_64]={0,}, pszCASN[NX_TEXT_SIZE_64]={0,};

	HINT32			lLanageId = eLangID_NONE;
	HUINT32			ulMarketID = 0;


    pstContents->pszTextMessage  = NX_APP_Calloc(sizeof(HCHAR)*NX_TEXT_SIZE_512);
	if (pstContents->pszTextMessage == NULL)
	{
		NX_APP_Error("Error!! NX_APP_Calloc() Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	// Serial number
	nx_menuHiddenFusingInfo_MakeSerialNumberString(serialNumberStr);

	// Language
	nx_menuHiddenFusingInfo_MakeLanguageString(langStr);
	hError = NX_SYSTEM_GetMenuLanuage(&lLanageId);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!! NX_SYSTEM_GetMenuLanuage() Fail!! \n");
	}
	else
	{
		pszLanguage = (HCHAR *)HLIB_LANG_IdTo639((HxLANG_Id_e)lLanageId);
	}

#if defined(CONFIG_MW_CAS_IRDETO)
	// Fusing Info
	hError = NX_CAS_IRDETO_GetFusingInfo(&bIsSCS, &bIsJTAG, &bIsOTP, &bIsCWE, &ulMarketID);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!! NX_CAS_IRDETO_GetFusingInfo() Fail!! \n");
	}
#else
	// Fusing Info
	hError = NX_SYSTEM_GetFusingInfo(&bIsSCS, &bIsJTAG, &bIsOTP, &bIsCWE, &ulMarketID);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!! NX_SYSTEM_GetFusingInfo() Fail!! \n");
	}
#endif

	HxSTD_sprintf((char *)pszSecureInfoMsg1, (char *)"S - B O O T : %c   J T A G : %c",
		(bIsSCS == TRUE) ? 'O' : 'X',	(bIsJTAG == TRUE) ? 'O' : 'X');
	HxSTD_sprintf((char *)pszSecureInfoMsg2, (char *)"S - C W : %c       O T P : %c",
		(bIsCWE == TRUE) ? 'O' : 'X',	(bIsOTP == TRUE) ? 'O' : 'X');

	// IR KEY
#if defined(CONFIG_MW_CAS_IRDETO)
	nx_menuHiddenFusingInfo_MakeCasIrdetoString(pszIrdetoMsg1, pszIrdetoMsg2);
#else
	HxSTD_sprintf((char*)pszIrdetoMsg1, (char *)"I R - D E V : ");
	HxSTD_sprintf((char*)pszIrdetoMsg2, (char *)"I R - K E Y : ");
#endif	// End of defined(CONFIG_MW_CAS_IRDETO)

#if defined(CONFIG_MW_CAS_CONAX)
	nx_menuHiddenFusingInfo_MakeCasConaxString(pszConaxCpmMsg1);
#else
	HxSTD_sprintf((char *)pszConaxCpmMsg1, (char *)"C - R M - F S : ");
#endif	// End of defined(CONFIG_MW_CAS_CONAX)

	// Market ID
	HxSTD_sprintf((char *)pszMarketID, (char *)"M - I D : 0 %X", ulMarketID);

	// BBX
#if defined(CONFIG_MW_CAS_VIACCESS)
	nx_menuHiddenFusingInfo_MakeCasViaccessString(pszBBX);
#else
	HxSTD_sprintf((char *)pszBBX, (char *)"B B X : ");
#endif

#if defined(CONFIG_MW_CAS_NAGRA)
	nx_menuHiddenFusingInfo_MakeCasNagraString(pszCASN);
#else
	// CA - SN
	HxSTD_sprintf((char *)pszCASN, (char *)"C A - S N : ");
#endif

	HxSTD_snprintf((char*)pstContents->pszTextMessage, NX_TEXT_SIZE_512, (char*)"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
		serialNumberStr, langStr, pszSecureInfoMsg1, pszSecureInfoMsg2, pszIrdetoMsg1,
		pszIrdetoMsg2, pszConaxCpmMsg1, pszMarketID, pszBBX, pszCASN);

	return	ONDK_RET_OK;
}
#else
static ONDK_Result_t	nx_menuHiddenFusingInfo_MakeData(Nx_MenuHiddenFusingInfoSetContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HBOOL 			bIsSCS = FALSE, bIsJTAG = FALSE, bIsOTP = FALSE, bIsCWE = FALSE;
	HCHAR			*pszLanguage = NULL;

	HUINT8			serialNumberStr[NX_TEXT_SIZE_64], langStr[NX_TEXT_SIZE_64];

	HUINT8			pszSecureInfoMsg1[NX_TEXT_SIZE_64] = {0, }, pszSecureInfoMsg2[NX_TEXT_SIZE_64] = {0, };
	HUINT8			pszConaxCpmMsg1[NX_TEXT_SIZE_64]={0,}, pszMarketID[NX_TEXT_SIZE_64]={0,};
	HUINT8			pszIrdetoMsg1[NX_TEXT_SIZE_64] = {0,}, pszIrdetoMsg2[NX_TEXT_SIZE_64] = {0,};

	HUINT8			pszBBX[NX_TEXT_SIZE_64]={0,}, pszCASN[NX_TEXT_SIZE_64]={0,};

	HINT32			lLanageId = eLangID_NONE;
	HUINT32			ulMarketID = 0;


    pstContents->pszTextMessage  = NX_APP_Calloc(sizeof(HCHAR)*NX_TEXT_SIZE_512);
	if (pstContents->pszTextMessage == NULL)
	{
		NX_APP_Error("Error!! NX_APP_Calloc() Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	// Serial number
	nx_menuHiddenFusingInfo_MakeSerialNumberString(serialNumberStr);

	// Language
	nx_menuHiddenFusingInfo_MakeLanguageString(langStr);
	hError = NX_SYSTEM_GetMenuLanuage(&lLanageId);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!! NX_SYSTEM_GetMenuLanuage() Fail!! \n");
	}
	else
	{
		pszLanguage = (HCHAR *)HLIB_LANG_IdTo639((HxLANG_Id_e)lLanageId);
	}

	// Fusing Info
	hError = NX_SYSTEM_GetFusingInfo(&bIsSCS, &bIsJTAG, &bIsOTP, &bIsCWE, &ulMarketID);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!! NX_SYSTEM_GetFusingInfo() Fail!! \n");
	}

	HxSTD_sprintf((char *)pszSecureInfoMsg1, (char *)"S - B O O T : %c   J T A G : %c\n",
		(bIsSCS == TRUE) ? 'O' : 'X',	(bIsJTAG == TRUE) ? 'O' : 'X');
	HxSTD_sprintf((char *)pszSecureInfoMsg2, (char *)"S - C W : %c       O T P : %c\n",
		(bIsCWE == TRUE) ? 'O' : 'X',	(bIsOTP == TRUE) ? 'O' : 'X');

	//ir-dev, ir-key
	HxSTD_sprintf((char*)pszIrdetoMsg1, (char *)"I R - D E V : ");
	HxSTD_sprintf((char*)pszIrdetoMsg2, (char *)"I R - K E Y : ");
	// Conax
	HxSTD_sprintf((char *)pszConaxCpmMsg1, (char *)"C - R M - F S : ");
	// market ID
	HxSTD_sprintf((char *)pszMarketID, (char *)"M - I D : 0 %X", ulMarketID);
	// Viaccess
	HxSTD_sprintf((char *)pszBBX, (char *)"B B X : ");
	// CA - SN
	HxSTD_sprintf((char *)pszCASN, (char *)"C A - S N : ");

	HxSTD_snprintf((char*)pstContents->pszTextMessage, NX_TEXT_SIZE_512, (char*)"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
		serialNumberStr, langStr, pszSecureInfoMsg1, pszSecureInfoMsg2, pszIrdetoMsg1,
		pszIrdetoMsg2, pszConaxCpmMsg1, pszMarketID, pszBBX, pszCASN);

	return	ONDK_RET_OK;
}
#endif


static ONDK_Result_t 	nx_menuHiddenFusingInfo_SetUiObject(Nx_MenuHiddenFusingInfoSetContent_t *pstContents)
{
	ONDK_Rect_t 		stRect;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_HIDDEN_FUSING_WINDOW_WINDOW_X, NX_PRISM_HIDDEN_FUSING_WINDOW_WINDOW_Y,
							NX_PRISM_HIDDEN_FUSING_WINDOW_WIDTH, NX_PRISM_HIDDEN_FUSING_WINDOW_HEIGHT);

	// Frame
	stRect = ONDK_Rect(NX_PRISM_HIDDEN_FUSING_FRAME_X, NX_PRISM_HIDDEN_FUSING_FRAME_Y,
						NX_PRISM_HIDDEN_FUSING_FRAME_W, NX_PRISM_HIDDEN_FUSING_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_HIDDEN_FUSING_FRAME_ID, &stRect, COL(C_G_Whistle_100));
	//ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_RSV_NOTI_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_TBLine);

	if (pstContents->pszTextMessage)
	{
		stRect = ONDK_Rect(NX_PRISM_HIDDEN_FUSING_TEXT_X, NX_PRISM_HIDDEN_FUSING_TEXT_Y,
							NX_PRISM_HIDDEN_FUSING_TEXT_W, NX_PRISM_HIDDEN_FUSING_TEXT_H);
		ONDK_GWM_COM_Text_Create(NX_PRISM_HIDDEN_FUSING_TEXT_ID, &stRect, (HCHAR*)pstContents->pszTextMessage);
		ONDK_GWM_COM_Text_SetFont(NX_PRISM_HIDDEN_FUSING_TEXT_ID, eFont_SystemBold);
		ONDK_GWM_COM_Text_SetSize(NX_PRISM_HIDDEN_FUSING_TEXT_ID, NX_PRISM_FONT_SIZE_28);
		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_HIDDEN_FUSING_TEXT_ID, (TEXT_ALIGN_LEFT | TEXT_MULTILINED | TEXT_WORDWRAP));
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_HIDDEN_FUSING_TEXT_ID, COL(C_T_Bang_100), COL(C_T_Bang_100));
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenFusingInfo_MsgGwmCreate(Nx_MenuHiddenFusingInfoSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->pszTextMessage = NULL;

	nx_menuHiddenFusingInfo_MakeData(pstContents);
	nx_menuHiddenFusingInfo_SetUiObject(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenFusingInfo_MsgGwmDestroy(Nx_MenuHiddenFusingInfoSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pszTextMessage)
	{
		NX_APP_Free(pstContents->pszTextMessage);
		pstContents->pszTextMessage = NULL;
	}

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHiddenFusingInfo_MsgGwmHapiMsg(Nx_MenuHiddenFusingInfoSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuHiddenFusingInfo_MsgGwmKeyDown(Nx_MenuHiddenFusingInfoSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!! (pstContents == NULL)!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_EXIT:
		case KEY_OK:
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuHiddenFusingInfo_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_HiddenFusingInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_MenuHiddenFusingInfoSetContent_t	*pstContents = NULL;

	pstContents = nx_menuHiddenFusingInfo_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuHiddenFusingInfo_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuHiddenFusingInfo_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_menuHiddenFusingInfo_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_menuHiddenFusingInfo_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		default:
			break;
	}
	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

