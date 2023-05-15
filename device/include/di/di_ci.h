/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_ci.h
// Original Author:
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/

#ifndef __DI_CI_H__
#define __DI_CI_H__

/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#include "htype.h"
#include "di_err.h"


/*******************************************************************/
/* EXTERN VARIABLES 												*/
/*******************************************************************/



/*******************************************************************/
/* MACRO DEFINITION 												*/
/*******************************************************************/



/*******************************************************************/
/* TYPE DEFINE													*/
/*******************************************************************/
typedef enum
{
	DI_CI_EXTRACTED = 0,	/* ķ�� ���ŵ� ��� �Ǵ� ķ ����� power off �� ��� */
	DI_CI_INSERTED,		/* ķ�� ���Ե� ��� */
	DI_CI_ENABLED		/* ķ�� ���ԵǾ� reset �� access �����ϰ� �� ��� (DI ���������θ� ���) */
}DI_CI_STATE;		/* PAL_CI_STATE �� ���ε� */

typedef enum
{
	DI_CI_BYPASS = 0,
	DI_CI_PASSTHROUGH
}DI_CI_PATH;				/* PAL_CI_PATH �� ���ε� */

typedef enum
{
           DI_CI_CI_V1 = 0, /*Legacy CAM*/
           DI_CI_CI_PLUS,
           DI_CI_TYPE_MAX
}DI_CI_CAM_TYPE;                                         

typedef enum
{
	DI_CI_INPUT0 = 0,
	DI_CI_INPUT1,
	DI_CI_OTHERMODULE,
	DI_CI_SOURCE_MAX
}DI_CI_CAM_SOURCE;                                         

/*
DI_CI_CONNECT_STATUS_t

DI_CI_STATE	connectStatus		==> ���� CAM�� insert/extract ��Ȳ ǥ�� 	(0 ::  extract , 1:: insert )
DI_CI_CONNECT_WAY	connectWay		==> ���� CI Path ���� ��Ȳ 			(0::  normal , 1:: daisychain , 2 :: slot 0�� �Ϲ������� ���� �� ��Ȳ���� slot1�� input�� slot0�� output���� ���� �ϰ�, 1�� slot output ���� �ȵ� ��� , �ƴϸ� �� �ݴ� ��Ȳ , 3::playback)
*/

typedef enum
{
	DI_CI_CONNECT_NORMAL = 0,
	DI_CI_CONNECT_DAISYCHAIN,
	DI_CI_CONNECT_SPECIAL,
	DI_CI_CONNECT_PLAYBACK,
	DI_CI_CONNECT_MAX
}DI_CI_CONNECT_WAY;                                         

typedef enum
{
	DI_CI_NORMAL_BYPASS = 0,
	DI_CI_NORMAL_PASSTHROUGH,
	DI_CI_DAISYCHAIN_PASSTHROUGH,
	DI_CI_CUSTOM_PASSTHROUGH,      
	DI_CI_PATHWAY_MAX
}DI_CI_PATHWAY;

typedef enum
{
	DI_CI_LIVE0 = 1,
	DI_CI_LIVE1,
	DI_CI_PLAYBACK0,
	DI_CI_PLAYBACK1,
	DI_CI_INPUT_MAX
}DI_CI_INPUT;


typedef enum
{
	DI_CI_OUTPUT0 = 0,
	DI_CI_OUTPUT1,
	DI_CI_OUTPUT_MAX
}DI_CI_OUTPUT;

typedef struct DI_CI_ConnectStatus
{
	DI_CI_STATE		connectStatus;	
	DI_CI_CONNECT_WAY	connectWay;
}DI_CI_CONNECT_STATUS_t;


/*******************************************************************/
/* GLOBAL FUNCTION PROTOTYPE									*/
/*******************************************************************/
unsigned short DI_CI_GetNumberOfModule (void);
DI_ERR_CODE DI_CI_RegisterCallback (void (*CiCallbackFunc)(unsigned short usSlotId, unsigned short flag));
DI_CI_STATE DI_CI_CheckModule (unsigned short usSlotId);
DI_ERR_CODE DI_CI_ResetModule (unsigned short usSlotId);
DI_ERR_CODE DI_CI_PowerOffModule (unsigned short usSlotId);
DI_ERR_CODE DI_CI_SetTsPath (unsigned short usSlotId, DI_CI_PATH path);
DI_ERR_CODE DI_CI_SetCAMType(unsigned short usSlotId, DI_CI_CAM_TYPE eCamType);
DI_ERR_CODE DI_CI_GetStatusCI(unsigned short usSlotId, DI_CI_CONNECT_STATUS_t *pCIStatus);
DI_ERR_CODE DI_CI_SetExtendedTsPath(unsigned int tunerId, DI_CI_INPUT input, unsigned int slotId, DI_CI_OUTPUT output, DI_CI_PATHWAY pathway );

#endif /* !__DI_CI_H__ */

