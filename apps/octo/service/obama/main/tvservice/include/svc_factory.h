/*******************************************************************
	File Description
********************************************************************/
/**
	@file     mw_factory.c
	@brief    Factory 지원 기능을 관리하는 모듈을 제어하는 Service

	Description: \n
	Module: MW/FACTORY			 	\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MW_FACTORY_H__
#define	__MW_FACTORY_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

enum
{
	EVT_FACTORY = 0xF0000000,

	/* - Description : FACTORY COMMAND가 도착했음을  알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	EVT_FACTORY_COMMAND_RECEIVED,

	/* - Description : EDID VERIFY가 완료했음을  알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	EVT_FACTORY_EDID_VERIFIED,

	/* - Description : FRC DOWNLOAD과 완료했음을  알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	EVT_FACTORY_FRC_DOWNLOAD_FINISHED,
	
	/* - Description : FRONTKEY Test가 완료했음을  알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Result / 0 : OK 1: FAIL
		   p2 : TBD
		   p3 : TBD */
	EVT_FACTORY_FRONT_KEY_FINISHED,

	EVT_FACTORY_END
}; /* Message to GWM */

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR MW_FACTORY_Init(void);

HERROR MW_FACTORY_TASK_Create(void);

HERROR MW_FACTORY_GetParam(HUINT32 *param, HUINT32 nParamNum);

HERROR MW_FACTORY_GetString(HUINT8 *pString, HUINT32 nParamNum);

HERROR MW_FACTORY_SendCmdResult(HUINT32 ulCommand, HBOOL bResult, HUINT8 numOfParam , HUINT8 *ucParam1, HUINT8 sizeOfParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4);

void MW_FACTORY_EDIDVerify(HUINT8 *pString, HUINT32 nLength, HUINT32 nParamNum);

void MW_FACTORY_FRCDownload(void);

HERROR MW_FACTORY_CH_SAT_LNBOut(HUINT32 ulTunerID, HUINT32 ulPolarization, HUINT32 ulToneOnOff, HUINT32 ulLnbOnOff);

HERROR MW_FACTORY_CH_SAT_Lock(HUINT32 ulTunerID, HUINT32 ulTransSpec, HUINT32 ulModulation, HUINT32 ulCodeRate, HUINT32 ulFrequency, HUINT32 ulSymbolrate, HUINT32 ulDiseq, HUINT32 ulPilot, HUINT32 ulRollOff);

HERROR MW_FACTORY_CH_TER_AntVoltage(HBOOL bOnOff);

HERROR MW_FACTORY_CH_TER_Lock(HUINT32 ulTunerID, HUINT32 ulFrequency, HUINT32 ulSymbolate, HUINT32 ulModulation);

HERROR MW_FACTORY_CH_CAB_Lock(HUINT32 ulTunerID, HUINT32 ulFrequency, HUINT32 ulSymbolate, HUINT32 ulModulation);

HERROR MW_FAC_ETH_ARP_Test(HUINT32 nServerIp);
HERROR MW_FAC_AV_PLAY(HUINT32 ulVideoFormat, HUINT32 ulAudioFormat, HUINT32 ulVideoPID, HUINT32 ulAudioPID, HUINT32 ulPcrPID, HUINT8 eChType, HINT32 withCI);
HERROR MW_FAC_EraseEntireArea(void);
HERROR MW_FACTORY_Start(void);
HERROR MW_FACTORY_Flash_Dump(void);

#endif		/* __MW_FACTORY_H__ */
