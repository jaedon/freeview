/*************************************************************************************************************
	File 		: NativeHSatip.cpp
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

#include "NativeHSatip.h"
#include "macros.h"
#include <hlib.h>

#include "oplsatip.h" 

#ifdef OIPF

typedef enum{
 	MAX_DMS_FRIENDLY_NAME_BUF_LEN =	128,
}NativeHDlna_e;

char g_szSatipFriendlyName[MAX_DMS_FRIENDLY_NAME_BUF_LEN];

/*****************************************************************************
 * HDlnaSatip
 ******************************************************************************/
OOIFReturnCode 	native_Satip_Start(OOIFBoolean *retval)
{
	OPL_SATIP_Start();
	OPL_SATIP_IsActivated(retval);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_Satip_Stop(OOIFBoolean *retval)
{
	OPL_SATIP_Stop();
	OPL_SATIP_IsActivated(retval);
	return OOIF_RETURN_OK;
}

bool 			native_Satip_IsActivated()
{
	HBOOL ret = FALSE;
	OPL_SATIP_IsActivated(&ret);
	return (bool)ret;
}

char* 			native_Satip_GetFriendlyName()
{
	memset(g_szSatipFriendlyName, 0x00, sizeof(char)*MAX_DMS_FRIENDLY_NAME_BUF_LEN);
	OPL_SATIP_GetFriendlyName(g_szSatipFriendlyName, MAX_DMS_FRIENDLY_NAME_BUF_LEN);
	return g_szSatipFriendlyName;

}
OOIFReturnCode 	native_Satip_SetFriendlyName(const char *friendlyName)
{
	OPL_SATIP_SetFriendlyName(friendlyName);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Satip_RegEventListener(DlnaSatipEventListener listener)
{
	OPL_SATIP_RegisterListener(listener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Satip_UnRegEventListener()
{
	OPL_SATIP_UnRegisterListener();
	return OOIF_RETURN_OK;
}
#endif // OIPF
