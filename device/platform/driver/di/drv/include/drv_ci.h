/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_ci.h
// Original Author:
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/
#ifndef	__DRV_CI_H__
#define	__DRV_CI_H__


/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#include "drv_err.h"

/*******************************************************************/
/* EXTERN VARIABLES 												*/
/*******************************************************************/


/*******************************************************************/
/* MACRO DEFINITION 												*/
/*******************************************************************/
#define CAM_MAXSLOTID		1		// Kraken �� CI Slot 1 �� ��.

#define CAM_NO_DETECT			0
#define CAM_DETECT				1

#define CAM_POWER_OFF			0
#define CAM_POWER_ON			1

#define CAM_DISABLE				1		// buffer /OE ��ȣ �̹Ƿ� '1' �� Disable
#define CAM_ENABLE				0		// buffer /OE ��ȣ �̹Ƿ� '0' �� Enable


/*******************************************************************/
/* TYPE DEFINE													*/
/*******************************************************************/
typedef struct CAMINFO_T
{
	int 				nDevId;		/* device id : this value used for DI inside */
	unsigned short		usSlotId;		/* slot id : this value used between MW and DI */
	unsigned char		ucOpen;		/* 0:not open,	1:open */
	unsigned char		ucDetect;		/* CAM_NO_DETECT or CAM_DETECT */
	unsigned short		usStatus;		/* DI_CI_STATE, event status */
}CAMINFO_t;


/*******************************************************************/
/* GLOBAL FUNCTION PROTOTYPE									*/
/*******************************************************************/

/* �̵� �Լ����� DI ���������� ���Ǵ� public �Լ����̹Ƿ� MW ���������� DI_CI_xxx ���� �̿��ϵ��� ���� */
DRV_Error DRV_CI_Init (void);
int DRV_CI_CheckSlotId (unsigned short usSlotId);
unsigned char DRV_CI_GetCAMDetect (int nDevId);
DI_ERR_CODE DRV_CI_Reset_CAM (int nDevId);
DI_ERR_CODE DRV_CI_PowerOff_CAM (int nDevId);
DI_ERR_CODE DRV_CI_PowerOn_CAM (int nDevId);
DI_ERR_CODE DRV_CI_ReadIo(int nDevId, unsigned long ulOffset, unsigned char *pucData);
DI_ERR_CODE DRV_CI_WriteIo(int nDevId, unsigned long ulOffset, unsigned char ucValue);
DI_ERR_CODE DRV_CI_ReadMem(int nDevId, unsigned long ulOffset, unsigned char *pucData);
DI_ERR_CODE DRV_CI_WriteMem(int nDevId, unsigned long ulOffset, unsigned char ucValue);
int DRV_CI_ParseCis(unsigned char *);
void DRV_CI_RegisterCallback (void (*CiCallbackFuncforDemux)(unsigned short usSlotId, unsigned short flag));
unsigned short DRV_CI_GetCAMStatus(int nDevId);

extern void (*_pfnCiCallbackFunc)(unsigned short, unsigned short);

#endif /* !__DRV_CI_H__ */

