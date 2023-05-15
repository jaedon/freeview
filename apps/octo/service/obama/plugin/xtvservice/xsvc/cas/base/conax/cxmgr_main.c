/*******************************************************************
	File Description
*******************************************************************/
/*++
    @file        cxmgr_main.c
    @brief       Conax Manager

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
/*******************************************************************/
#define ________CXMGR_MAIN_Private_Include____________________________________
/*******************************************************************/
#include <conax.h>
#include <cxmgr_action.h>
#include <conax_ctrl.h>

/*******************************************************************/
#define ________CXMGR_MAIN_Golbal_Value_______________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXMGR_MAIN_Private_Define_____________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXMGR_MAIN_Private_Static_Value_______________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXMGR_MAIN_Global_Value_______________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXMGR_MAIN_Private_Static_Prototype___________________________
/*******************************************************************/
STATIC void local_cxmgr_Init(void);

/*******************************************************************/
#define ________CXMGR_MAIN_Private_Static_Prototype_Body______________________
/*******************************************************************/
STATIC void local_cxmgr_Init(void)
{
	ENTER_FUNCTION;

	// cxcore init
	CXAPI_Initialize();

	// casmgr에 연결
	CXACT_Init();

	CA_CX_RegisterCtrlList();

	LEAVE_FUNCTION;
}
/*******************************************************************/
#define ________CXMGR_MAIN_Public_Functions_Body______________________________
/*******************************************************************/
HERROR xsvc_cas_CxMgrInit(void)
{
	ENTER_FUNCTION;

	local_cxmgr_Init();

	LEAVE_FUNCTION;
	return ERR_OK;
}

void xsvc_cas_CxCmdInit(HxCMD_Handle_t hCmdHandle)
{
#if defined (CONFIG_DEBUG)
	HLIB_CMD_RegisterWord(hCmdHandle,
						CXCMD_Execute,
						"cx",						/* keyword */
						"CONAX Debug\n cx [cmd] (test number)\n",	/* help   */
						"cx [UI]");	/* usage  */
#endif
}
