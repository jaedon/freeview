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

#include "rema_MOD.h"
#include "RMOD_SVC_Obama.h"

#include <oapi.h>




void RMOD_SVC_Obama_Deinit(void);

HINT32 RMOD_SVC_Obama_resetUpdate(void);


#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___


#define ___STATIC_VARIABLES___

#define ___INTERNAL_FUNCTIONS___
#define ___INTERNAL_FUNCTIONS___
static void Stb_Notifier_t(void)
{
    REM_FUNC_ENTER;

    REM_FUNC_LEAVE;
}

#define ___PUBLIC_FUNCTIONS___
HERROR RMOD_SVC_Obama_Init(void)
{
    REM_FUNC_ENTER;

    OAPI_SYSINFO_STB_Init(Stb_Notifier_t);
    REM_FUNC_LEAVE;

    return ERR_OK;
}
void RMOD_SVC_Obama_Deinit(void)
{
    REM_FUNC_ENTER;

    OAPI_SYSINFO_STB_DeInit();
    REM_FUNC_LEAVE;
    return;
}

HINT32 RMOD_SVC_Obama_resetUpdate(void)
{
    return 0 ; 
}


#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

