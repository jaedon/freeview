/*************************************************************************************************************
	File 		: NativeHSamba.cpp
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

#ifndef __NATIVE_HSAMBA__
#define __NATIVE_HSAMBA__

#include "NativeOOIFTypes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined OIPF

/*****************************************************************************
 * HSambe
 *****************************************************************************
*/
typedef void* SambaServerHandle;

typedef enum
{
	eNATIVE_SAMBAC_SCAN_STARTED,
	eNATIVE_SAMBAC_SCAN_ADD,
	eNATIVE_SAMBAC_SCAN_REMOVE,
	eNATIVE_SAMBAC_SCAN_COMPLETE, 
	eNATIVE_SAMBAC_SCAN_FAIL, 

	eNATIVE_SAMBAC_MOUNT_OK			= 10,
	eNATIVE_SAMBAC_MOUNT_FAIL,	
	eNATIVE_SAMBAC_UMOUNT_OK,
	eNATIVE_SAMBAC_UMOUNT_FAIL,
} NativeSamba_Event_e;

typedef void (*NativeSambaManagerCallback)(int event, void *contentList);
typedef void (*NativeSambaServerCallback)(int event, int result);


/*****************************************************************************
 * HSambaManager
 *****************************************************************************
 */
SambaServerHandle	native_SambaManager_GetServerHandle(void);

/*****************************************************************************
 * HSambaServer
 *****************************************************************************
 */
OOIFReturnCode native_SambaServer_AddListener(NativeSambaServerCallback listener);
OOIFReturnCode native_SambaServer_RemoveListener(NativeSambaServerCallback listener);
OOIFReturnCode native_SambaServer_Start( SambaServerHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_SambaServer_Stop( SambaServerHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_SambaServer_IsStarted( SambaServerHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_SambaServer_GetName( SambaServerHandle identifier, const char **retval );
OOIFReturnCode native_SambaServer_SetName( SambaServerHandle identifier, const char *serverName, OOIFNumber*retval);
OOIFReturnCode native_SambaServer_GetWorkgroup( SambaServerHandle identifier, const char **retval );
OOIFReturnCode native_SambaServer_SetWorkgroup( SambaServerHandle identifier, const char *workgroup, OOIFNumber*retval);
OOIFReturnCode native_SambaServer_GetShareFolder( SambaServerHandle identifier, const char*userName, int *nCount, SambaServerHandle *SharedList );
OOIFReturnCode native_SambaServer_SetShareFolder( SambaServerHandle identifier, const char *userName, const char *path, const char *opt, OOIFNumber*retval);
OOIFReturnCode native_SambaServer_GetPassword( SambaServerHandle identifier, const char **retval);
OOIFReturnCode native_SambaServer_SetPassword( SambaServerHandle identifier, const char *userName, const char *curPassword, const char *newPasword, OOIFNumber*retval);
OOIFReturnCode native_SambaServer_IsValidPassword( SambaServerHandle identifier, const char *userName, const char *curPassword, OOIFBoolean*retval);
OOIFReturnCode native_SambaServer_SetPublicAccess( SambaServerHandle identifier, const char *userName, OOIFBoolean bAccess, OOIFNumber*retval);
OOIFReturnCode native_SambaServer_GetPublicAccess( SambaServerHandle identifier, const char *userName, OOIFBoolean*retval);

/*****************************************************************************
 * HSmbsShareFolder
 *****************************************************************************
 */

OOIFReturnCode native_SmbsShareFolder_GetPath( SambaServerHandle identifier, const char **retval );
OOIFReturnCode native_SmbsShareFolder_GetName( SambaServerHandle identifier, const char **retval );
OOIFReturnCode native_SmbsShareFolder_GetUser( SambaServerHandle identifier, const char **retval );
OOIFReturnCode native_SmbsShareFolder_GetWritable( SambaServerHandle identifier, OOIFBoolean *retval );
OOIFReturnCode native_SmbsShareFolder_SetWritable( SambaServerHandle identifier, OOIFBoolean retval );
OOIFReturnCode native_SmbsShareFolder_IsValid( SambaServerHandle identifier,  OOIFBoolean *retval );
OOIFReturnCode native_SmbsShareFolder_ReleaseHandle( SambaServerHandle id );


/*****************************************************************************
 * HSambaClient
 *****************************************************************************
*/
/* Methods */
OOIFReturnCode native_Sambac_AddListener(NativeSambaManagerCallback listener);
OOIFReturnCode native_Sambac_RemoveListener( void );

OOIFReturnCode native_Sambac_StartScan(OOIFBoolean *retval);
OOIFReturnCode native_Sambac_StopScan(OOIFBoolean *retval);
OOIFReturnCode native_Sambac_Mount(SambaServerHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_Sambac_Unmount(SambaServerHandle identifier, OOIFBoolean *retval); 
OOIFReturnCode native_Sambac_UnmountAll( OOIFBoolean *retval);

OOIFReturnCode native_Sambac_GetAvailableServerList(int *size, SambaServerHandle **sharedList, OOIFBoolean *retval);


/* Properties */
OOIFReturnCode native_Sambac_GetServerName( SambaServerHandle identifier, const char **retval );
OOIFReturnCode native_Sambac_GetSharedName( SambaServerHandle identifier, const char**retval);

OOIFReturnCode native_Sambac_GetServerIp( SambaServerHandle identifier, const char **retval );

OOIFReturnCode native_Sambac_GetMountPath( SambaServerHandle identifier, const char **retval );
OOIFReturnCode native_Sambac_GetLogMsg(SambaServerHandle identifier, const char **retval);

OOIFReturnCode native_Sambac_GetId( SambaServerHandle identifier, const char **retval );
OOIFReturnCode native_Sambac_GetPassword( SambaServerHandle identifier, const char **retval );

OOIFReturnCode natvie_Sambac_GetMountState( SambaServerHandle identifier, int *retval);
OOIFReturnCode natvie_Sambac_GetState( SambaServerHandle identifier, OOIFNumber *retval);

OOIFReturnCode native_Sambac_SetId( SambaServerHandle identifier, const char *newId);
OOIFReturnCode native_Sambac_SetPassword( SambaServerHandle identifier, const char *newPasword);
OOIFReturnCode native_Sambac_ReleaseHandle( SambaServerHandle id );


#endif /* OIPF */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HSAMBA__ */
