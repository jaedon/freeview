/**
	@file	  pal_ci.h
	@brief	  																					\n

	Description: 																				\n
	Module: PAL/CI				 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.															\n
	All rights reserved.																		\n
*/


#ifndef	__PAL_CI_H__
#define	__PAL_CI_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "octo_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*--- debugging defines ---*/

/* CI 동작 확인용 : CI 스택 내부에서만 사용할 것 */
//#define	_CI_DEBUG_

/* CI+ 주요 동작 확인용 : CI 스택 내부에서만 사용할 것 */
//#define	_CI_PLUS_DEBUG_

/*--- CI+ specific defines ---*/

/* 2013.01.22. === NOTE by KKKIM ===																							*/
/* 앞으로 더이상 CONFIG_MW_CI_PLUS_PVR CONFIG는 사용하지 않고, 항상 DISABLE 로 고정한다. config.cas 에서도 제외 함 				*/
/* 이유는 CI+ v1.2 기준에서 사용한 CAM이 단 하나도 없었으며(인증에도 제외 항목), CI+ v1.3 에서는 제거된 feature 이기 때문이다.	*/
/* 즉, PVR resource는 사용되지도 않았고, 앞으로 사용하지 않을 resource이기 때문에 history는 남기지만 더이상 ENABLE될 일은 없음.	*/
//#if defined(CONFIG_MW_CI_PLUS_PVR) && !defined(CONFIG_MW_CI_PLUS_V_1_3) /* CI+ v1.3 에서 제거된 feature 임. 현재도 본격적으로 사용하지 않고 있지만 앞으로도 사용할 일이 없다. */


/*--- H/W platform specific defines ---*/

#ifdef CONFIG_DOUGLAS

/* douglas가 read/write memory functions을 지원하지 않고 CIS/COR 처리를 별도 functions으로 지원하기 때문에 아래 define을 사용하도록 한다 */
	#define USE_DOUGLAS_PRIMITIVE_FUNC
	#ifdef USE_DOUGLAS_PRIMITIVE_FUNC
		extern int DRV_CI_ReadCIS (unsigned short usSlotId, unsigned char *pcis, unsigned short *plen);
		extern int DRV_CI_WriteCOR (unsigned short usSlotId);
	#endif

/* 2009.03.29 WorkAround For Douglas Platform (H/W ECO 안된 Douglas 플랫폼) :
   더글러스 문제로 캠 초기화시 리소스가 올라오지 않는 경우 (Resource Manager 이후에 Application Information 를 포함한 다른 리소스가 안올라오는 경우가 있음)
   캠 리셋을 걸기 위한 work-around 임 */
	#define CI_WORKAROUND_FOR_DOUGLAS
	#ifdef CI_WORKAROUND_FOR_DOUGLAS
		#define CI_WORKAROUND_FOR_DOUGLAS_TIMEOUT	20000
		#define CI_WORKAROUND_FOR_DOUGLAS_LIMIT		3
	#endif

#endif

typedef enum
{
	ePAL_CI_EXTRACTED = 0,	/* 캠이 제거된 경우 또는 캠 모듈을 power off 한 경우 */
	ePAL_CI_INSERTED		/* 캠이 삽입된 직후 */
} PalCi_State_e;			/* DI_CI_STATE 와 매핑됨 */

typedef enum
{
	ePAL_CI_BYPASS = 0,
	ePAL_CI_PASSTHROUGH
} PalCi_Path_e;			/* DI_CI_PATH 와 매핑됨 */

typedef enum
{
	ePAL_CI_CI_V1 = 0, /*Legacy CAM*/
	ePAL_CI_CI_PLUS
} PalCi_CamType_e;

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
// DI_CI_CAM_SOURCE와 같다.
typedef enum
{
	ePAL_CI_CAM_INPUT0 = 0,
	ePAL_CI_CAM_INPUT1,
	ePAL_CI_CAM_OTHERMODULE,
	ePAL_CI_CAM_SOURCE_MAX
} PalCi_CamSource_e;
#endif

/*	DI_CI_CONNECT_WAY 와 같음.
*/
typedef enum
{
	ePAL_CI_CONNECT_NORMAL = 0,
	ePAL_CI_CONNECT_DAISYCHAIN,
	ePAL_CI_CONNECT_SPECIAL,
	ePAL_CI_CONNECT_PLAYBACK,
	ePAL_CI_CONNECT_MAX
}PalCi_ConnectWay_e;

/*
typedef struct DI_CI_ConnectStatus
{
	DI_CI_STATE		connectStatus;
	DI_CI_CONNECT_WAY	connectWay;
}DI_CI_CONNECT_STATUS_t;


HUINT8	connectStatus		==> 현재 CAM이 insert/extract 상황 표현
HUINT8	connectWay		==> 현재 CI Path 연결 상황 	ePAL_CI_CONNECT_DAISYCHAIN :: slot 0에 일반적으로 연결 된 상황에서 slot1에 input을 slot0의 output으로 연결 하고, 1번 slot output 연결 안된 경우 , 아니면 그 반대 상황
*/
typedef struct PAL_CI_ConnectStatus
{
	PalCi_State_e			ePalCiState;
	PalCi_ConnectWay_e		ePalConnectWay;
}PalCi_ConnectStatus_e;


/* PAL_CI_PATHWAY_t는 DI의 DI_CI_PATHWAY와 값이 같음.
typedef enum
{
           DI_CI_BYPASS = 0,
           DI_CI_NORMAL_PASSTHROUGH,
           DI_CI_DAISYCHAIN_PASSTHROUGH,
           DI_CI_CUSTOM_PASSTHROUGH,
           DI_CI_PATHWAY_MAX
}DI_CI_PATHWAY;
*/
typedef enum
{
	ePAL_CI_PATHWAY_BYPASS = 0,
	ePAL_CI_PATHWAY_NORMAL_PASSTHROUGH,
	ePAL_CI_PATHWAY_DAISYCHAIN_PASSTHROUGH,
	ePAL_CI_PATHWAY_CUSTOM_PASSTHROUGH,
	ePAL_CI_PATHWAY_MAX
}PalCi_PathWay_e;

/*
typedef enum
{
           DI_CI_LIVE0 = 1,
           DI_CI_LIVE1,
           DI_CI_PLAYBACK0,
           DI_CI_PLAYBACK1,
           DI_CI_INPUT_MAX
}DI_CI_INPUT;
*/
typedef enum
{
	ePAL_CI_LIVE0 = 0,
	ePAL_CI_LIVE1,
	ePAL_CI_PLAYBACK0,
	ePAL_CI_PLAYBACK1,
	ePAL_CI_INPUT_MAX
}PalCi_Input_e;


/*
typedef enum
{
           DI_CI_OUTPUT0 = 0,
           DI_CI_OUTPUT1,
           DI_CI_OUTPUT_MAX
}DI_CI_OUTPUT;
*/
typedef enum
{
	ePAL_CI_OUTPUT0 = 0,
	ePAL_CI_OUTPUT1,
	ePAL_CI_OUTPUT_MAX
}PalCi_Output_e;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

HERROR PAL_CI_Init(void);

unsigned short PAL_CI_GetNumberOfModule (void); /* 지원하는 CI 슬롯 수를 리턴함 */
unsigned short PAL_CI_GetMaxSlotNumber (void); /* OCTO가 지원할 수 있는 최대 CI 슬롯 수를 리턴함 */

HERROR PAL_CI_RegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag)); /* 캠의 상태에 따라 CI 드라이버로부터 호출되기를 원하는 콜백을 등록하기 위한 함수 */

PalCi_State_e PAL_CI_CheckModule (CI_SLOT_ID usSlotId); /* 캠 삽입 유무를 리턴함 */

HERROR PAL_CI_ResetModule (CI_SLOT_ID usSlotId); /* 캠 모듈을 리셋함 */

HERROR PAL_CI_SetTsPath (CI_SLOT_ID usSlotId, PalCi_Path_e path); /* bypass 또는 pass-thru를 지정하기 위한 함수 */
HERROR PAL_CI_SetPath(HUINT32 ulSrcId, HINT32 nSlotId, HUINT32 ulDemuxId); /* 현재 구현으로 nSlotId 은 -1 값을 취할 수 있음에 유의 */
HERROR PAL_CI_GetPath(HUINT32 ulDemuxId, HINT32 *pnCI_Id);

#ifdef CONFIG_DEBUG
void PAL_CI_PrintTsPath (CI_SLOT_ID usSlotId); /* debugging 용도로만 사용하기 위한 함수 */
void PAL_CI_PrintTsPathAll(void); /* debugging 용도로만 사용하기 위한 함수 */
void PAL_CI_PrintDemuxPath(void); /* debugging 용도로만 사용하기 위한 함수 */
void PAL_CI_PrintCIStatus(void);
#endif

//HERROR PAL_CI_GetVersion(HUINT8 *ucMajorVer, HUINT8 *ucMinorVer);

/* B사 CI Plus 제약사항을 풀기위한 추가 API */
HERROR PAL_CI_SetCamType(CI_SLOT_ID usSlotId, PalCi_CamType_e eType);

HINT32 PAL_CI_Open (HUINT8 *device_name);
HINT32 PAL_CI_Close (HUINT16 usSlotId);
HINT32 PAL_CI_Write (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes);
HINT32 PAL_CI_Read (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes);
HINT32 PAL_CI_Ioctl (HUINT16 usSlotId, HINT32 command, void *parg);
#ifdef USE_DOUGLAS_PRIMITIVE_FUNC
HINT32 PAL_CI_ReadCIS (HUINT16 usSlotId, HUINT8 *pcis, HUINT16 *plen);
HINT32 PAL_CI_WriteCOR (HUINT16 usSlotId);
#endif

HERROR PAL_CI_GetStatusCI(unsigned short usSlotId, PalCi_ConnectStatus_e *pstCIStatus);

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
HERROR PAL_CI_GetCurrentPathWay(HUINT32 unSlotId, PalCi_PathWay_e *pePalCiPathWay);
HERROR PAL_CI_SetExtendedTsPath(HUINT32 unTunerId, HUINT32 unSlotId, HUINT32 unDemuxId, PalCi_PathWay_e ePalCiPathWay);
#endif

#endif	//__PAL_CI_H__

