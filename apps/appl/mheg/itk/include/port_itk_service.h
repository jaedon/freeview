/**
	@file     port_itk_service.h
	@brief    ITK Host Service 모듈 헤더파일.

	Description: Porinting Layer 함수 선언을 포함 함.						\n
	Module: mw/mheg/itk												\n

	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/
#ifndef _PORT_ITK_SERVICE_H_
#define _PORT_ITK_SERVICE_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include "itk.h"
#include "itk_service.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
/* definition of MPEG-2 system Stream Type  */
#define ITK_ISO_TYPE_VIDEO_11172					0x01
#define ITK_ISO_TYPE_VIDEO_13818					0x02
#define ITK_ISO_TYPE_AUDIO_11172					0x03
#define ITK_ISO_TYPE_AUDIO_13818					0x04
#define ITK_ISO_PRIVDATA_1							0x05	// AC3
#define ITK_ISO_PRIVDATA_2							0x06	// AC3
#define ITK_ISO_AUDIO_H_264							0x11	/** < MPEG4 High Efficiency AAC audio coding */
#define ITK_ISO_AUDIO_13818_7						0x0f	/* ADTS */
#define ITK_ISO_VIDEO_H_264							0x1B	/** < H264/AVC video coding */

#define ITK_ELEMENT_INDEX_AUDIO						1
#define ITK_ELEMENT_INDEX_VIDEO						2
#define ITK_ELEMENT_INDEX_PCR						3
#define ITK_ELEMENT_INDEX_DATA						4

#define	ITK_SVC_INVALID_ID			0xffffffff

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/** @brief WatchTV application state enumerates */

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern void			PORT_ITK_SVC_Init(void);
extern void			PORT_ITK_SVC_CreateObject(redkey_profile_t eItkProfile);
extern HERROR		PORT_ITK_SVC_Start(void);
extern HERROR		PORT_ITK_SVC_Stop(itk_service_tune_event_t eTuneEvent);
extern ITK_RefSvc_t	port_itk_svc_getReferenceService(const HCHAR* pszSourceUrl, HINT32 *pnOnId, HINT32 *pnTsId, HINT32 *pnSvcId, HINT32 *pnLcn, HUINT32 *pulFsi);
extern HERROR		port_itk_svc_getSvcHandleByTripleIds(HINT32 nOrgNetId, HINT32 nTsId, HINT32 nSvcId, HINT32 *pnSvcUid);
extern HERROR		PORT_ITK_SVC_Retune(HINT32 slIndex, HUINT32 flag);

#ifdef CONFIG_DEBUG

void 	PORT_ITK_SVC_DEBUG_ShowSvcIndexList(void);

#endif	/* CONFIG_DEBUG */



#endif	/* _PORT_ITK_SERVICE_H_ */




