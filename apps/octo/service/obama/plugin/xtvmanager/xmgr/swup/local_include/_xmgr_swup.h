/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_sw_update.h
	@brief	  internal mplayer 헤더 파일

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_SWUP_INT_H__
#define	__XMGR_SWUP_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

BUS_Result_t		xproc_swup_Download_Dvbssu(HINT32 nMessage, Handle_t hHandle, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);

#if defined(CONFIG_OTA_ASTRA)
BUS_Result_t 		xproc_swup_Detect_Astra(HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR 				xmgr_swup_SetDefaultHomeTsInfo_Astra (void);
#endif

#if defined(CONFIG_OTA_DVBSSU)
BUS_Result_t 		xproc_swup_Detect_Dvbssu(HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR 				xmgr_swup_SetDefaultHomeTsInfo_Dvbssu (void);
#endif

#if defined(CONFIG_OTA_HUMAX)
BUS_Result_t 		xproc_swup_Detect_Humax(HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR 				xmgr_swup_SetDefaultHomeTsInfo_Humax (void);
#endif


#if defined(CONFIG_OTA_IRDETO)
BUS_Result_t		xproc_swup_Detect_Irdeto(HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR 				xmgr_swup_SetDefaultHomeTsInfo_Irdeto (void);
#endif

#if defined(CONFIG_OTA_DTT_UK) || defined(CONFIG_OTA_DTT_EU) || defined(CONFIG_OTA_DTT_UK_SSU)
BUS_Result_t 		xproc_swup_Detect_Dtt(HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR 				xmgr_swup_SetDefaultHomeTsInfo_Dtt(void);
#endif

#if defined(CONFIG_OTA_SES)
BUS_Result_t 		xproc_swup_Detect_Ses(HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
HERROR				xmgr_swup_SetDefaultHomeTsInfo_Ses(void);
HUINT32				xmgr_swup_GetStartUpdateSource_Ses(void);
#endif

#endif /* __XMGR_SWUP_INT_H__ */

