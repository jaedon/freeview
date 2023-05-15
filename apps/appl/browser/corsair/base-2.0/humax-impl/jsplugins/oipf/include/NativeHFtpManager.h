/*************************************************************************************************************
	File 		: NativeHFtpManager.cpp
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

#ifndef __NATIVE_HFTP__
#define __NATIVE_HFTP__

#include "NativeOOIFTypes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined OIPF
typedef enum
{
	eNATIVE_FTPS_STARTED,
	eNATIVE_FTPS_STOPED, 

	eNATIVE_FTPS_CONFIG_CHANGED,	
	eNATIVE_FTPS_STATUS_CHANGED,	

} NativeSamba_Event_e;

typedef void (*NativeFtpServerCallback)( int event, int retErr );
typedef void* FtpServerHandle;

/*****************************************************************************
 * HFtpManager
 *****************************************************************************
*/
FtpServerHandle	native_FtpManager_GetServerHandle(void);

/*****************************************************************************
 * HFtpServer
 *****************************************************************************
*/
/* Methods */
OOIFReturnCode native_FtpServer_AddListener(NativeFtpServerCallback listener);
OOIFReturnCode native_FtpServer_RemoveListener(NativeFtpServerCallback listener);

OOIFReturnCode native_FtpServer_Start( FtpServerHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_FtpServer_Stop( FtpServerHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_FtpServer_IsStarted( FtpServerHandle identifier, OOIFBoolean *retval);

/* Properties */
OOIFReturnCode native_FtpServer_GetName( FtpServerHandle identifier, const char **retval );
OOIFReturnCode native_FtpServer_SetName( FtpServerHandle identifier, const char *serverName, OOIFNumber*retval);

OOIFReturnCode native_FtpServer_GetPort( FtpServerHandle identifier, OOIFNumber*retval );
OOIFReturnCode native_FtpServer_SetPort( FtpServerHandle identifier, int  nPort, OOIFNumber*retval );

OOIFReturnCode native_FtpServer_GetShareFolder( FtpServerHandle identifier, const char*userName, int *nCount, FtpServerHandle *SharedList);
OOIFReturnCode native_FtpServer_SetShareFolder( FtpServerHandle identifier, const char *userName, const char *path, const char *opt, OOIFNumber*retval);
OOIFReturnCode native_FtpServer_GetPassword( FtpServerHandle identifier, const char **retval);
OOIFReturnCode native_FtpServer_SetPassword( FtpServerHandle identifier, const char *userName, const char *curPassword, const char *newPasword, OOIFNumber*retval);
OOIFReturnCode native_FtpServer_IsValidPassword( FtpServerHandle identifier, const char *userName, const char *curPassword, OOIFBoolean*retval);
OOIFReturnCode native_FtpServer_SetPublicAccess( FtpServerHandle identifier, const char *userName, OOIFBoolean bAccess, OOIFNumber*retval);
OOIFReturnCode native_FtpServer_GetPublicAccess( FtpServerHandle identifier, const char *userName, OOIFBoolean*retval);

/*****************************************************************************
 * HShareFolder
 *****************************************************************************
*/
OOIFReturnCode native_FtpsShareFolder_GetPath( FtpServerHandle identifier, const char **retval );
OOIFReturnCode native_FtpsShareFolder_GetName( FtpServerHandle identifier, const char **retval );
OOIFReturnCode native_FtpsShareFolder_GetUser( FtpServerHandle identifier, const char **retval );
OOIFReturnCode native_FtpsShareFolder_GetWritable( FtpServerHandle identifier, OOIFBoolean *retval );
OOIFReturnCode native_FtpsShareFolder_SetWritable( FtpServerHandle identifier, OOIFBoolean retval );
OOIFReturnCode native_FtpsShareFolder_IsValid( FtpServerHandle identifier,  OOIFBoolean *retval );
OOIFReturnCode native_FtpsShareFolder_ReleaseHandle( FtpServerHandle id );

#endif /* OIPF */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HFTP__ */
