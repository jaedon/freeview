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
#include 	"ap_hdmi.h"


/**************************************************************************************************/
#define _________HDMI_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________HDMI_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________HDMI_Private_Value_________________________________________________
/**************************************************************************************************/

//static Hdmi_Contents_t		s_Hdmi_Contents;

static	HUINT8	**s_TmpStr = NULL;
static	HINT32	s_LineNum;
static	HUINT8	s_Language[128];
static  HUINT8	*s_MsgStr = NULL;

#define _DEF_MAX_MSGSTR_LENGTH	512

/**************************************************************************************************/
#define _________HDMI_Private_prototype_functions__________________________________________________________
/**************************************************************************************************/

//static Hdmi_Contents_t*	 local_hdmi_GetContents(void);

static ONDK_Result_t	local_hdmi_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_hdmi_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_hdmi_InitData(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_hdmi_FreeData(void);
static ONDK_Result_t 	local_hdmi_SetObject(void);

static ONDK_Result_t 	local_hdmi_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t 	local_hdmi_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_hdmi_SetLanguage(void);

/**************************************************************************************************/
#define _________HDMI_Private_functions__________________________________________________________
/**************************************************************************************************/


static ONDK_Result_t local_hdmi_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hRet = ONDK_RET_OK;

	hRet = local_hdmi_InitData(p1,p2,p3);
	if(hRet != ONDK_RET_OK)
	{
		HxLOG_Warning("error!! local_hdmi_InitData\r\n");
	}

	hRet = local_hdmi_SetObject();
	if(hRet != ONDK_RET_OK)
	{
		HxLOG_Warning("error!! local_hdmi_SetObject\r\n");
	}

	return hRet;
}

static ONDK_Result_t local_hdmi_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_hdmi_FreeData();

	return GWM_RET_OK;
}

static ONDK_Result_t local_hdmi_InitData(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	lRet = ONDK_RET_OK;

	lRet = local_hdmi_SetLanguage();
	if(lRet != ONDK_RET_OK)
	{
		HxLOG_Warning("error!! local_hdmi_SetLanguage\r\n");
	}

	s_MsgStr = AP_Malloc(_DEF_MAX_MSGSTR_LENGTH);
	HxSTR_Memset(s_MsgStr,0,_DEF_MAX_MSGSTR_LENGTH);

	return lRet;

}

static ONDK_Result_t local_hdmi_FreeData(void)
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

static ONDK_Result_t 	local_hdmi_SetObject()
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

	ONDK_GWM_APP_SetAppArea(HDMI_X, HDMI_Y, HDMI_W, HDMI_H);

	cpy_str = ONDK_GetString("STR_MESG_3191_ID");
	if(NULL == cpy_str)
	{
		cpy_str = "# STR_MESG_3191_ID #";
	}

	HxSTD_MemSet(s_MsgStr, 0, _DEF_MAX_MSGSTR_LENGTH);
	HxSTD_StrNCpy(s_MsgStr,cpy_str,(_DEF_MAX_MSGSTR_LENGTH-1));

	str_len = HxSTD_StrLen(s_MsgStr);
	s_LineNum = str_len/width_size;

	if(str_len%width_size == 0)
		s_LineNum++;

	HxLOG_Critical("[local_hdmi_SetObject] %d %d %s\r\n", str_len, s_LineNum, s_MsgStr);
	s_TmpStr = HxSTR_MemAlloc(s_LineNum*sizeof(HUINT8*));

	tok_str = HxSTD_StrTok(s_MsgStr," ", &save);

	if(s_LineNum == 4)
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
#if !defined(CONFIG_PROD_ROCKYPOM) && !defined(CONFIG_PROD_ROCKYWPOM)
		stRect = ONDK_Rect(HDMI_TEXT_X,HDMI_TEXT_Y+40*i+line_gap,HDMI_TEXT_W,HDMI_TEXT_H);
		ONDK_GWM_COM_Text_Create(HDMI_TEXT_ID_1+i,&stRect,s_TmpStr[i]);
		ONDK_GWM_COM_Text_SetSize(HDMI_TEXT_ID_1+i,HDMI_FONT_SIZE);
		ONDK_GWM_COM_Text_ChangeColor(HDMI_TEXT_ID_1+i,HDMI_FONT_COLOR,HDMI_FONT_COLOR);
		//ONDK_GWM_COM_Text_SetFont(HDMI_TEXT_ID_1+i,eFont_SystemBold);
		//ONDK_GWM_COM_Text_SetAlign(HDMI_TEXT_ID_1+10,TEXT_MULTILINED);
#endif

	}

	stRect = ONDK_Rect(HDMI_YELLOWLINE_UP_X,HDMI_YELLOWLINE_UP_Y,HDMI_YELLOWLINE_UP_W,HDMI_YELLOWLINE_UP_H);
	ONDK_GWM_COM_Image_Create(HDMI_YELLOWLINE_UP_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(HDMI_YELLOWLINE_UP_ID,HDMI_IMG_YELLOWLINE);

	stRect = ONDK_Rect(HDMI_ICON_X,HDMI_ICON_Y,HDMI_ICON_W,HDMI_ICON_H);
	ONDK_GWM_COM_Image_Create(HDMI_ICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(HDMI_ICON_ID,HDMI_IMG_ICON);

	stRect = ONDK_Rect(HDMI_YELLOWLINE_DOWN_X,HDMI_YELLOWLINE_DOWN_Y,HDMI_YELLOWLINE_DOWN_W,HDMI_YELLOWLINE_DOWN_H);
	ONDK_GWM_COM_Image_Create(HDMI_YELLOWLINE_DOWN_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(HDMI_YELLOWLINE_DOWN_ID,HDMI_IMG_YELLOWLINE);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());

	return lRet;
}

static ONDK_Result_t local_hdmi_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{


	return GWM_RET_OK;
}

//MSG_GWM_TIMER
static ONDK_Result_t local_hdmi_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3)
{
	return GWM_RET_OK;
}

static ONDK_Result_t	local_hdmi_SetLanguage()
{
	HERROR 	hErr = ERR_OK;
	HCHAR	*pLangFullName = NULL;
	HCHAR	szLang639[8] ={0,};
	HxLANG_Id_e langId;

	hErr = APK_SCENARIO_GetMainLanguage(szLang639);

	ONDK_Print("[%s:%d] szLang639 : [%s]\n", __FUNCTION__, __LINE__, szLang639);

	if(hErr != ERR_OK)
	{
//		ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ONDK_RET_IO;
	}

	langId = HLIB_LANG_639ToId(szLang639);
	pLangFullName = (HCHAR *)HLIB_LANG_IdToFullName(langId);
	if(pLangFullName == NULL)
	{
		ONDK_Error("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
		return ONDK_RET_IO;
	}

	HxSTD_StrNCpy(s_Language, pLangFullName, ONDK_LANGUAGE_STRING_MAX_LENGTH);
	s_Language[ONDK_LANGUAGE_STRING_MAX_LENGTH-1] = '\0';

	ONDK_Print("[%s:%d] pLangFullName : [%s]\n", __FUNCTION__, __LINE__, s_Language);

	return ONDK_RET_OK;
}


/**************************************************************************************************/
#define _________HDMI_Public_Functions__________________________________________________________
/**************************************************************************************************/


ONDK_Result_t		AP_HDMI_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	ONDK_Result_t			lRet = GWM_RET_OK;

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = local_hdmi_MsgGwmCreate(p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			lRet = local_hdmi_MsgGwmTimer(p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			lRet = local_hdmi_MsgGwmKeyDown(p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			lRet = local_hdmi_MsgGwmDestroy(p1, p2, p3);
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
#define _________HDMI_End_Of_File____________________________________________________
/**************************************************************************************************/


