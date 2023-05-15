/* $Header:   //BASE/archives/PDR_LIGHT/mw/cas/ci/ci_res/ci_mmi.h-arc   1.5   Jan 13 2007 00:18:38   htson  $ */
/***************************************************************
* $Workfile:   ci_mmi.h  $
* $Modtime:   Jan 12 2007 06:19:44  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_MMI_H_
#define _CI_MMI_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "ci.h"


/***************************************************************
* definition
***************************************************************/
#define CI_MMI_CLOSE_IMMEDIATE		0x00
#define CI_MMI_CLOSE_DELAY			0x01

// display_control_cmd in display_control()
#define SET_MMI_MODE										0x01
#define GET_DISPLAY_CHAR_TABLE_LIST						0x02
#define GET_INPUT_CHAR_TABLE_LIST						0x03
#define GET_OVERLAY_GRAPHICS_CHARACTERISTICS			0x04
#define GET_FULL_SCREEN_GRAPHICS_CHARACTERISTICS		0x05

// mmi_mode in display_control()
#define CI_MMI_HIGH_LEVEL									0x01
#define CI_MMI_LOW_LEVEL_OVERLAY_GRAPHICS				0x02
#define CI_MMI_LOW_LEVEL_FULL_SCREEN_GRAPHICS			0x03

// display_reply_id in display_reply()
#define CI_MMI_MODE_ACK									0x01
#define CI_LIST_DISPLAY_CHAR_TABLES						0x02
#define CI_LIST_INPUT_CHAR_TABLES						0x03
#define CI_LIST_GRAPHIC_OVERLAY_CHARACTERISTICS		0x04
#define CI_LIST_FULL_SCREEN_GRAPHIC_CHARACTERISTICS	0x05
#define CI_UNKNOWN_DISPLAY_CONTROL_CMD					0xF0
#define CI_UNKNOWN_MMI_MODE								0xF1
#define CI_UNKNOWN_CHAR_TABLE							0xF2

// Answ object answ_id in answ()
#define CI_MMI_ANSW_ANSWER		0x01
#define CI_MMI_ANSW_CANCEL		0x00


/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI MMI (Man Machine Interface) */
int CiMmi_Init(void);
int CiMmi_SsOpenedProc(unsigned short usSsNo);
int CiMmi_SsClosedProc(unsigned short usSsNo);
int CiMmi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen);
unsigned short CiMmi_GetSsNo(CI_SLOT_ID usSlotId);
int CiMmi_SendCloseMmi(unsigned short usSsNo);
int CiMmi_SendMenuAnsw(unsigned short usSsNo, MMI_REQ nMmiReq, unsigned char ucChoiceNo);
int CiMmi_SendAnswString(unsigned short usSsNo, unsigned char *pucStr, unsigned short usStrSize);
#ifdef SUPPORT_SMART_CI_MMI_PINCODE
int CiMmi_CheckWaitingUserInput(CI_SLOT_ID usSlotId, unsigned long* IsWaiting);
#endif


#ifdef __cplusplus
}
#endif

#endif /* _CI_MMI_H_ */

