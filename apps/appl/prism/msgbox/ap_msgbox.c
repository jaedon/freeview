/**************************************************************
*	http://www.humaxdigital.com
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

/**************************************************************************************************/
#define _________UMMA_Private_Include_________________________________________________
/**************************************************************************************************/
#include	<ondk.h>
#include	<ondk_type.h>
#include	<ondk_gfx.h>
#include	<hlib.h>
#include	<ondk.h>
#include	<oapi.h>
#include	<hapi.h>
#include	<apk.h>

#include <prism.h>
#include 	"ap_msgbox.h"


/**************************************************************************************************/
#define _________MSGBOX_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________MSGBOX_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________MSGBOX_Private_Value_________________________________________________
/**************************************************************************************************/

static	HUINT8	**s_TmpStr = NULL;
static	HINT32	s_LineNum;
static	HUINT8	s_Language[128];
static  HUINT8	*s_MsgStr = NULL;


/**************************************************************************************************/
#define _________MSGBOX_Private_prototype_functions__________________________________________________________
/**************************************************************************************************/

//static Hdmi_Contents_t*	 local_msgbox_GetContents(void);

static ONDK_Result_t	local_msgbox_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_msgbox_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_msgbox_InitData(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_msgbox_FreeData(void);
static ONDK_Result_t 	local_msgbox_SetObject(void);

static ONDK_Result_t 	local_msgbox_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_msgbox_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_msgbox_SetLanguage(void);

/**************************************************************************************************/
#define _________MSGBOX_Private_functions__________________________________________________________
/**************************************************************************************************/


static ONDK_Result_t local_msgbox_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hRet = ONDK_RET_OK;

	hRet = local_msgbox_InitData(p1,p2,p3);
	if(hRet != ONDK_RET_OK)
	{
		HxLOG_Warning("error!! local_msgbox_InitData\r\n");
	}

	hRet = local_msgbox_SetObject();
	if(hRet != ONDK_RET_OK)
	{
		HxLOG_Warning("error!! local_msgbox_SetObject\r\n");
	}

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_MSGBOX, MSGBOX_INTERVAL);

	return hRet;
}

static ONDK_Result_t local_msgbox_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_msgbox_FreeData();

	return GWM_RET_OK;
}

static ONDK_Result_t local_msgbox_InitData(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	lRet = ONDK_RET_OK;

	lRet = local_msgbox_SetLanguage();
	if(lRet != ONDK_RET_OK)
	{
		HxLOG_Warning("error!! local_msgbox_SetLanguage\r\n");
	}

	s_MsgStr = AP_Malloc(512);
	HxSTR_Memset(s_MsgStr,0,512);

	return lRet;

}

static ONDK_Result_t local_msgbox_FreeData(void)
{

	ONDK_Result_t lRet = ONDK_RET_OK;
	HINT32	i=0;

	for(i=0;i<s_LineNum;i++)
	{
		HxSTR_MemFree(s_TmpStr[i]);
	}
	HxSTR_MemFree(s_TmpStr);
	HxSTR_MemFree(s_MsgStr);

	return lRet;
}

static ONDK_Result_t 	local_msgbox_SetObject()
{
	ONDK_Rect_t 		stRect;
	ONDK_Result_t		lRet = ONDK_RET_OK;

	HUINT8	*cpy_str = NULL;
	HUINT8	*tok_str = NULL;
	HCHAR	*save = NULL;

	HINT32	i=0;
	HINT32	str_len,tmp_len,line_gap=0;
	HINT32	width_size=0;

	if(HxSTD_StrCmp(s_Language,"Russian")==0)
		width_size = 80;
	else width_size = 45;//50; 	// shorten for Dejavu Font

	ONDK_GWM_APP_SetAppArea(MSGBOX_X, MSGBOX_Y, MSGBOX_W, MSGBOX_H);

	stRect = ONDK_Rect(MSGBOX_BG_X,MSGBOX_BG_Y,MSGBOX_BG_W,MSGBOX_BG_H);
	ONDK_GWM_COM_Rect_Create(MSGBOX_BG_ID, &stRect,0xDD000000);

	//cpy_str = ONDK_GetString("STR_MESG_3191_ID");
	cpy_str = "There is a message you need to confirm. To see the message, exit Opera TV browser.";

	HxSTD_StrNCpy(s_MsgStr,cpy_str,HxSTD_StrLen(cpy_str));
	s_MsgStr[HxSTD_StrLen(cpy_str)] = '\0';

	if(s_MsgStr == NULL)
	{
		s_MsgStr = "# STR_MESG_3191_ID #";
	}
	str_len = HxSTD_StrLen(s_MsgStr);
	s_LineNum = str_len/width_size;
	s_LineNum++;

	HxLOG_Critical("[local_msgbox_SetObject] %d %d %s\r\n", str_len, s_LineNum, s_MsgStr);
	s_TmpStr = HxSTR_MemAlloc(s_LineNum*sizeof(HUINT8*));

	tok_str = HxSTD_StrTok(s_MsgStr," ", &save);

	if(s_LineNum == 2)
		line_gap  = 20;
	else line_gap = 0;

	for(i=0;i<s_LineNum;i++)
	{
		s_TmpStr[i] = HxSTR_MemAlloc(sizeof(HUINT8)*512);
		HxSTR_Memset(s_TmpStr[i],0,sizeof(HUINT8)*512);

		while(1)
		{
			if(tok_str == NULL)
				break;
			HxSTD_StrNCat(s_TmpStr[i],tok_str,HxSTD_StrLen(tok_str));
			tmp_len = HxSTD_StrLen(s_TmpStr[i]);
			s_TmpStr[i][tmp_len] = '\0';
			if(tmp_len<width_size)
			{
				HxSTD_StrNCat(s_TmpStr[i]," ",1);
				s_TmpStr[i][tmp_len+1] = '\0';
			}
			else
			{
				tok_str = HxSTD_StrTok(NULL," ",&save);
				break;
			}

			tok_str = HxSTD_StrTok(NULL," ",&save);
			if(tok_str == NULL)
				break;
		}

		stRect = ONDK_Rect(MSGBOX_TEXT_X,MSGBOX_TEXT_Y*i+line_gap,MSGBOX_TEXT_W,MSGBOX_TEXT_H);
		ONDK_GWM_COM_Text_Create(MSGBOX_TEXT_ID_1+i,&stRect,s_TmpStr[i]);
		ONDK_GWM_COM_Text_SetSize(MSGBOX_TEXT_ID_1+i,MSGBOX_FONT_SIZE);
		ONDK_GWM_COM_Text_ChangeColor(MSGBOX_TEXT_ID_1+i,MSGBOX_FONT_COLOR,MSGBOX_FONT_COLOR);
		ONDK_GWM_COM_Text_SetFont(MSGBOX_TEXT_ID_1+i,eFont_SystemBold);
		//ONDK_GWM_COM_Text_SetAlign(MSGBOX_TEXT_ID_1+10,TEXT_MULTILINED);

	}

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return lRet;
}

static ONDK_Result_t local_msgbox_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{

	return GWM_RET_OK;
}

//MSG_GWM_TIMER
static ONDK_Result_t local_msgbox_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 ulTimerID = p1;

	if(ulTimerID == GWMPOPUP_TIMER_MSGBOX)
	{
		ONDK_GWM_APP_Destroy(0);
	}

	return GWM_RET_OK;
}

static ONDK_Result_t	local_msgbox_SetLanguage()
{
	HERROR 	hErr = ERR_OK;
	HCHAR	*pLangFullName = NULL;
	HCHAR	szLang639[8] ={0,};
	HxLANG_Id_e langId;

	hErr = APK_SCENARIO_GetMainLanguage(szLang639);

	if(hErr != ERR_OK)
	{
//		ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ONDK_RET_IO;
	}

	langId = HLIB_LANG_639ToId(szLang639);
	pLangFullName = (HCHAR *)HLIB_LANG_IdToFullName(langId);
	if(pLangFullName == NULL)
	{
//		ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ONDK_RET_IO;
	}

	HxSTD_StrNCpy(s_Language, pLangFullName, ONDK_LANGUAGE_STRING_MAX_LENGTH);
	s_Language[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';

//	ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ONDK_RET_OK;
}


/**************************************************************************************************/
#define _________MSGBOX_Public_Functions__________________________________________________________
/**************************************************************************************************/


ONDK_Result_t		AP_MSGBOX_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	ONDK_Result_t			lRet = GWM_RET_OK;

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = local_msgbox_MsgGwmCreate(p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			lRet = local_msgbox_MsgGwmTimer(p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			lRet = local_msgbox_MsgGwmKeyDown(p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			lRet = local_msgbox_MsgGwmDestroy(p1, p2, p3);
			break;
	}

	if(lRet != GWM_RET_OK)
	{
		return lRet;
	}

	lRet  =  ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
	if(lRet == GWM_RET_NO_OBJECT || lRet == GWM_MESSAGE_BREAK)
	{
		lRet = ONDK_MESSAGE_CONSUMED;
	}
	return lRet;

}


/**************************************************************************************************/
#define _________MSGBOX_End_Of_File____________________________________________________
/**************************************************************************************************/


