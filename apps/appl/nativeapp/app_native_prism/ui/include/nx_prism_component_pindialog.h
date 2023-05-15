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
 * @file	  		nx_prism_component_pindialog.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_COMP_PINCODE_DIALOG_H__
#define __NX_PRISM_COMP_PINCODE_DIALOG_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <nx_common.h>
#include <nx_prism_app.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_PRISM_UI_PINDLG_BG_ID						0x00000001
#define	NX_PRISM_UI_PINDLG_FRAME_ID						0x00000002

#define	NX_PRISM_UI_PIN_DIGIT_MAX						5

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef enum
{
	eNxPinCodeDataType_ParentalControl = 0,
	eNxPinCodeDataType_Installation,
	eNxPinCodeDataType_ChannelSetting,
	eNxPinCodeDataType_LockMaturityRating,
	eNxPinCodeDataType_LockChannel,
	eNxPinCodeDataType_End
} NX_PINCODE_DATA_TYPE_t;

typedef struct
{
	/* pin Dialog를 사용하기 전 꼭 채워주여야 하는 값들. */
	HUINT32					ulPinDlgWhoseId;							// 동일한 app 에서 pincode 창을 2개 용도로 보여줘야 할 경우에 사용, ex> medialist 에서 파일 재생시 권한 체크 용도, 파일에 대한 Lock 변경 용도
	HUINT32					ulDlgAttributes;
	HUINT8					*pszDlgTitle;
	HUINT8					*pszDlgMsg;
	ONDK_GWM_Callback_t		parentAppCallback;

	/* 자동으로 지정됨. 만질 필요없음. */
	ONDK_GWM_Callback_t		thisAppCallback;
	ONDK_Rect_t				appArea;
	HUINT32					ulInputMax;
	ONDK_Color_t			colorMessageText;
	HBOOL					bConfirmFlag;								/* verify drawing flag and re-confirm*/
	HINT32					nPinInputPos;
	HUINT8					szPinCodeStr[NX_PRISM_UI_PIN_DIGIT_MAX];
	HUINT8					szNewPinCodeStr[NX_PRISM_UI_PIN_DIGIT_MAX];			/* new pin code */
	HUINT8					szVerifyPinCodeStr[NX_PRISM_UI_PIN_DIGIT_MAX];		/* verify pin code */
	HUINT8					szSavedPinCode[NX_PRISM_UI_PIN_DIGIT_MAX];		/* verify pin code */
	HUINT8					*pszNewPincodeUIStr;
	HUINT8					*pszVerifyPincodeUIStr;
	HUINT32					ulActionId;
	HUINT32					ulAppPriority;
	HBOOL					bPinInputLock;
	NX_PINCODE_DATA_TYPE_t	pinType;
} NX_PinDialogInstance_t;

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
extern ONDK_Result_t	NX_PRISM_COMP_PinDlalog_SetDialog(NX_PinDialogInstance_t *pstContents, ONDK_GWM_Callback_t parentAppCallback, ONDK_Rect_t *pRect,
														HUINT32 ulDlgAttr, ONDK_Color_t colorMessageText, HUINT8 *pszDlgMsg, HUINT32 ulPinCodeType);
extern ONDK_Result_t	NX_PRISM_COMP_PinDlalog_ProtoProc(NX_PinDialogInstance_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_COMP_ChangePinDlalog_ProtoProc(NX_PinDialogInstance_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#endif // __NX_PRISM_COMP_PINCODE_DIALOG_H__

