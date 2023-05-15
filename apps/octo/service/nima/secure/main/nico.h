/*************************************************************************************************************
	File 		: nico.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/15
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/


#ifndef __NICO_H_
#define __NICO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vkernel.h>

#include <papi.h>
#include <hapi.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "drv_err.h"
#include "di_err.h"
#include "drv_hotplug.h"
#include "drv_wifi.h"
#include "drv_pppoe.h"

#include "nico_msg.h"
#include "nico_operator_common.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define ENTRY	HxLOG_Debug("%s:%s: (%d) ++\n", __FILE__, __FUNCTION__, __LINE__)
//#define EXIT	HxLOG_Debug("%s:%s: (%d) --\n", __FILE__, __FUNCTION__, __LINE__)

#if 0 //Enable for debugging in flash mode (vdkmai)
#define ENTRY printf("[%s][%s][%d]  (+)Enter!!\n",HLIB_STD_GetCurProcessName(),__FUNCTION__, __LINE__);
#define EXIT printf("[%s][%s][%d]  (-)Leave!!\n",HLIB_STD_GetCurProcessName(),__FUNCTION__, __LINE__);
#else
#define ENTRY   HxLOG_Debug("(+)++\n")
#define EXIT   	HxLOG_Debug("(-)--\n")
#endif

#define SAFE_FREE(p)			{if(p){ HLIB_STD_MemFree(p); (p)=NULL;}}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define OBJ_NAME_LEN 64
/*******************************************************************/
/*****************          function_param       *******************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
#ifdef __cplusplug
extern "C"{
#endif

void nico_init(void);

#ifdef __cplusplug
}
#endif


#ifdef __cplusplus
}
#endif
#endif
