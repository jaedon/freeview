/*************************************************************************************************************
	File 		: NativeHDlna.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/10/24
	attention 	: 

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and 
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "NativeHDial.h"
#include "macros.h"
#include <hlib.h>

#include "opldial.h" 	// for dial 

#ifdef OIPF

typedef enum{
 	MAX_DMS_FRIENDLY_NAME_BUF_LEN =	128,
}NativeHDlna_e;

char g_szDialFriendlyName[MAX_DMS_FRIENDLY_NAME_BUF_LEN];
/*****************************************************************************
 * HDlnaDial
 ******************************************************************************/
OOIFReturnCode 	native_Dial_Start(OOIFBoolean *retval)
{
	OPL_DIAL_Start();
	OPL_DIAL_IsActivated(retval);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_Dial_Stop(OOIFBoolean *retval)
{
	OPL_DIAL_Stop();
	OPL_DIAL_IsActivated(retval);
	return OOIF_RETURN_OK;
}

bool 			native_Dial_IsActivated()
{
	HBOOL ret = FALSE;
	OPL_DIAL_IsActivated(&ret);
	return (bool)ret;
}

char* 			native_Dial_GetFriendlyName()
{
	memset(g_szDialFriendlyName, 0x00, sizeof(char)*MAX_DMS_FRIENDLY_NAME_BUF_LEN);
	OPL_DIAL_GetFriendlyName(g_szDialFriendlyName, MAX_DMS_FRIENDLY_NAME_BUF_LEN);
	return g_szDialFriendlyName;

}
OOIFReturnCode 	native_Dial_SetFriendlyName(const char *friendlyName)
{
	OPL_DIAL_SetFriendlyName(friendlyName);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dial_RegEventListener(DlnaDialEventListener listener)
{
	OPL_DIAL_RegisterListener(listener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dial_UnRegEventListener()
{
	OPL_DIAL_UnRegisterListener();
	return OOIF_RETURN_OK;
}

OOIFReturnCode	native_Dial_RegistApplication(const char *App)
{
	if(BPL_STATUS_OK != OPL_DIAL_ApplicationRegist(App))
		return  OOIF_RETURN_ERROR;

	return OOIF_RETURN_OK;
}

OOIFReturnCode	native_Dial_UnregistApplication(const char *App)
{
	if(BPL_STATUS_OK != OPL_DIAL_ApplicationUnregist(App))
		return  OOIF_RETURN_ERROR;

	return OOIF_RETURN_OK;
}

OOIFReturnCode  native_Dial_ApplicationStateUpdate(const char *App, unsigned int state)
{
	if(BPL_STATUS_OK !=	OPL_DIAL_ApplicationStateUpdate(App, state))
		return  OOIF_RETURN_ERROR;

	return OOIF_RETURN_OK;
}

#endif // OIPF
