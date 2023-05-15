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

/* CI ���� Ȯ�ο� : CI ���� ���ο����� ����� �� */
//#define	_CI_DEBUG_

/* CI+ �ֿ� ���� Ȯ�ο� : CI ���� ���ο����� ����� �� */
//#define	_CI_PLUS_DEBUG_

/*--- CI+ specific defines ---*/

/* 2013.01.22. === NOTE by KKKIM ===																							*/
/* ������ ���̻� CONFIG_MW_CI_PLUS_PVR CONFIG�� ������� �ʰ�, �׻� DISABLE �� �����Ѵ�. config.cas ������ ���� �� 				*/
/* ������ CI+ v1.2 ���ؿ��� ����� CAM�� �� �ϳ��� ��������(�������� ���� �׸�), CI+ v1.3 ������ ���ŵ� feature �̱� �����̴�.	*/
/* ��, PVR resource�� �������� �ʾҰ�, ������ ������� ���� resource�̱� ������ history�� �������� ���̻� ENABLE�� ���� ����.	*/
//#if defined(CONFIG_MW_CI_PLUS_PVR) && !defined(CONFIG_MW_CI_PLUS_V_1_3) /* CI+ v1.3 ���� ���ŵ� feature ��. ���絵 ���������� ������� �ʰ� ������ �����ε� ����� ���� ����. */


/*--- H/W platform specific defines ---*/

#ifdef CONFIG_DOUGLAS

/* douglas�� read/write memory functions�� �������� �ʰ� CIS/COR ó���� ���� functions���� �����ϱ� ������ �Ʒ� define�� ����ϵ��� �Ѵ� */
	#define USE_DOUGLAS_PRIMITIVE_FUNC
	#ifdef USE_DOUGLAS_PRIMITIVE_FUNC
		extern int DRV_CI_ReadCIS (unsigned short usSlotId, unsigned char *pcis, unsigned short *plen);
		extern int DRV_CI_WriteCOR (unsigned short usSlotId);
	#endif

/* 2009.03.29 WorkAround For Douglas Platform (H/W ECO �ȵ� Douglas �÷���) :
   ���۷��� ������ ķ �ʱ�ȭ�� ���ҽ��� �ö���� �ʴ� ��� (Resource Manager ���Ŀ� Application Information �� ������ �ٸ� ���ҽ��� �ȿö���� ��찡 ����)
   ķ ������ �ɱ� ���� work-around �� */
	#define CI_WORKAROUND_FOR_DOUGLAS
	#ifdef CI_WORKAROUND_FOR_DOUGLAS
		#define CI_WORKAROUND_FOR_DOUGLAS_TIMEOUT	20000
		#define CI_WORKAROUND_FOR_DOUGLAS_LIMIT		3
	#endif

#endif

typedef enum
{
	ePAL_CI_EXTRACTED = 0,	/* ķ�� ���ŵ� ��� �Ǵ� ķ ����� power off �� ��� */
	ePAL_CI_INSERTED		/* ķ�� ���Ե� ���� */
} PalCi_State_e;			/* DI_CI_STATE �� ���ε� */

typedef enum
{
	ePAL_CI_BYPASS = 0,
	ePAL_CI_PASSTHROUGH
} PalCi_Path_e;			/* DI_CI_PATH �� ���ε� */

typedef enum
{
	ePAL_CI_CI_V1 = 0, /*Legacy CAM*/
	ePAL_CI_CI_PLUS
} PalCi_CamType_e;

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
// DI_CI_CAM_SOURCE�� ����.
typedef enum
{
	ePAL_CI_CAM_INPUT0 = 0,
	ePAL_CI_CAM_INPUT1,
	ePAL_CI_CAM_OTHERMODULE,
	ePAL_CI_CAM_SOURCE_MAX
} PalCi_CamSource_e;
#endif

/*	DI_CI_CONNECT_WAY �� ����.
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


HUINT8	connectStatus		==> ���� CAM�� insert/extract ��Ȳ ǥ��
HUINT8	connectWay		==> ���� CI Path ���� ��Ȳ 	ePAL_CI_CONNECT_DAISYCHAIN :: slot 0�� �Ϲ������� ���� �� ��Ȳ���� slot1�� input�� slot0�� output���� ���� �ϰ�, 1�� slot output ���� �ȵ� ��� , �ƴϸ� �� �ݴ� ��Ȳ
*/
typedef struct PAL_CI_ConnectStatus
{
	PalCi_State_e			ePalCiState;
	PalCi_ConnectWay_e		ePalConnectWay;
}PalCi_ConnectStatus_e;


/* PAL_CI_PATHWAY_t�� DI�� DI_CI_PATHWAY�� ���� ����.
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

unsigned short PAL_CI_GetNumberOfModule (void); /* �����ϴ� CI ���� ���� ������ */
unsigned short PAL_CI_GetMaxSlotNumber (void); /* OCTO�� ������ �� �ִ� �ִ� CI ���� ���� ������ */

HERROR PAL_CI_RegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag)); /* ķ�� ���¿� ���� CI ����̹��κ��� ȣ��Ǳ⸦ ���ϴ� �ݹ��� ����ϱ� ���� �Լ� */

PalCi_State_e PAL_CI_CheckModule (CI_SLOT_ID usSlotId); /* ķ ���� ������ ������ */

HERROR PAL_CI_ResetModule (CI_SLOT_ID usSlotId); /* ķ ����� ������ */

HERROR PAL_CI_SetTsPath (CI_SLOT_ID usSlotId, PalCi_Path_e path); /* bypass �Ǵ� pass-thru�� �����ϱ� ���� �Լ� */
HERROR PAL_CI_SetPath(HUINT32 ulSrcId, HINT32 nSlotId, HUINT32 ulDemuxId); /* ���� �������� nSlotId �� -1 ���� ���� �� ������ ���� */
HERROR PAL_CI_GetPath(HUINT32 ulDemuxId, HINT32 *pnCI_Id);

#ifdef CONFIG_DEBUG
void PAL_CI_PrintTsPath (CI_SLOT_ID usSlotId); /* debugging �뵵�θ� ����ϱ� ���� �Լ� */
void PAL_CI_PrintTsPathAll(void); /* debugging �뵵�θ� ����ϱ� ���� �Լ� */
void PAL_CI_PrintDemuxPath(void); /* debugging �뵵�θ� ����ϱ� ���� �Լ� */
void PAL_CI_PrintCIStatus(void);
#endif

//HERROR PAL_CI_GetVersion(HUINT8 *ucMajorVer, HUINT8 *ucMinorVer);

/* B�� CI Plus ��������� Ǯ������ �߰� API */
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

