/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <apk.h>
#include <nova_cas.h>
#include "nova_errcode.h"
#include "nova_utils.h"

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____IMPLEMENTATION_______________________
/**********************************************************************************************************/

void NOVA_CAS_PrintHexDump(HUINT8 *pusTitle, HUINT8 *pusData, HUINT32 ulSize)
{
	int i = 0;

	HxLOG_Print("\n\t\t\t* Hex Dump Title(%s), Size(%d)\n", pusTitle, ulSize);

	if(!pusData || ulSize<=0)
	{
		HxLOG_Error("[%s:%d] -%s- pusData is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return;
	}

	for(i=0;i<ulSize;i++)
	{
		if(i%16 == 0)
		{
			printf("\n\t\t\t* %08xh: ", i);
		}
		printf("%02X ", pusData[i]);
	}
	printf("\n");
}

NOVA_CAS_t * NOVA_CAS_GetCas(void)
{
	return &stNovaCas;
}

void NOVA_CAS_Init(void)
{
	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO);

	HxSTD_MemSet(&stNovaCas, 0x00, sizeof(NOVA_CAS_t));

	(void)APK_CAS_NAGRA_NOVA_Init();

	NOVA_CAS_TransportSessionInfoInit();	
	NOVA_CAS_CA_Init();
	NOVA_CAS_DRM_init();
}

void	NOVA_CMDRegister_CAS(const HCHAR *hCmdHandle)
{
#if defined(CONFIG_DEBUG)
	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO);
	NOVA_CAS_Cmd(hCmdHandle);
#endif
}
