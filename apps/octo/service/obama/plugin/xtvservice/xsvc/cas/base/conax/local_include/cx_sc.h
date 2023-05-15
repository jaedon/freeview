/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cx_sc.h
	@brief  	 Smartcard Driver Adaptation Layer for Conax

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* �� 2011-2012 Humax Co., Ltd.
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
#ifndef __CX_SC_H__
#define __CX_SC_H__
/*******************************************************************/
/********************     Include files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************    Types and structures   ********************/
/*******************************************************************/
#define CX_SC_SLOT_NUM					(CONFIG_MW_CAS_NUM_SC_SLOT)
#define CX_SC_T0_COMMAND_LEN			(5)
#define CX_SC_INVALID_SLOT				(-1)
#define CX_SC_INVALID_HANDLE			(-1)

typedef enum
{
	eCx_SC_EXTRACTED,
	eCx_SC_INSERTED
} Cx_SciEvent_e;

typedef HERROR (* Cx_SciEventNotifyCB_t)(Cx_SciEvent_e eSciEvent, HUINT8 ucSlotId);

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
void CXSC_Init(Cx_SciEventNotifyCB_t cbFunc);
HERROR CXSC_Reset(HINT32 nScSlot, HUINT8* pucAtr, HUINT32* pulAtrLen, HUINT8* pucProtocol);
HERROR CXSC_Write(HINT32 nScSlot, HUINT8* pucSndData, HUINT32 ulSndLen, HUINT8* pucRcvData, HUINT32* pulNumRead);
HERROR CXSC_Read(HINT32 nScSlot, HUINT8* pucSndData, HUINT8* pucRcvData, HUINT32* pulNumRead);
HERROR CXSC_ValidCard(HINT32 nScSlot, HBOOL bIsValid, HUINT16 usCaSysId, HUINT8 ucNumSession);
HERROR CXSC_Deactivate(HINT32 nScSlot);

#endif	/* !__CX_SC_H__ */
