/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

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

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>

#include <rema_int.h>

#include "RMOD_INT.h"

#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___
typedef struct RMOD_Core_t
{
    HUINT32 ulTickUpTime;
}RMOD_Core_t;

#define ___STATIC_VARIABLES___
static RMOD_Core_t *gpsRemaCore = NULL;
#define ___INTERNAL_FUNCTIONS___
#define ___PUBLIC_FUNCTIONS___
HERROR RMOD_INT_Init(void)
{   
    REM_FUNC_ENTER;

    if(!gpsRemaCore)
    {
	gpsRemaCore = (RMOD_Core_t*) HLIB_STD_MemAlloc(sizeof(RMOD_Core_t));
	if(gpsRemaCore)
	{
	    HxSTD_MemSet(gpsRemaCore, 0, sizeof(RMOD_Core_t));
	}
	gpsRemaCore->ulTickUpTime = HLIB_STD_GetSystemTick();
    }

    REM_FUNC_LEAVE;

    return (gpsRemaCore) ? ERR_OK : ERR_FAIL;
}
void RMOD_INT_Deinit(void)
{
    REM_FUNC_ENTER;

    if(gpsRemaCore)
    {
	HLIB_STD_MemFree(gpsRemaCore);
	gpsRemaCore = NULL;
    }

    REM_FUNC_LEAVE;

}


HERROR RMOD_INT_GetUpTime(HUINT32 *pulUpTime)
{
    HERROR hError = ERR_FAIL; 
    if(gpsRemaCore && pulUpTime)
    {
	*pulUpTime = gpsRemaCore->ulTickUpTime;
	hError = ERR_OK;
    }
    return hError ;
}
#define ___EXTERNAL_FUNCTIONS___
#define _____END_OF_FILE_________

