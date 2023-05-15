/*************************************************************************************************************
	File 		: NativeHWoon.cpp
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


#ifndef __NATIVE_HWOON__
#define __NATIVE_HWOON__

#include "NativeOOIFTypes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined OIPF

/*****************************************************************************
 * HWoon
 *************************************************************************** */
typedef void *WoonServerHandle;
typedef void *WoonClientHandle;
/* Properties */

/* Methods */

/*****************************************************************************
 * HWoonClient
 ***************************************************************************** */
typedef void 	*WoonFriendHandle;
typedef void	(*WoonClientEventListener)(unsigned event, unsigned int state, WoonFriendHandle *data);

OOIFReturnCode native_WoonClient_RegEventListener(WoonClientEventListener listener);
OOIFReturnCode native_WoonClient_UnRegEventListener( void );

OOIFReturnCode native_WoonClient_StartScan(OOIFBoolean *retval);
OOIFReturnCode native_WoonClient_StopScan(OOIFBoolean *retval);
OOIFReturnCode native_WoonClient_AddFriend(const char *wid, OOIFBoolean *retval);
OOIFReturnCode native_WoonClient_RemoveFriend(const char *wid, OOIFBoolean *retval);
OOIFReturnCode native_WoonClient_GetWoonFriendList( OOIFNumber *retval, int *nCount, WoonFriendHandle *friendHandle );

/*****************************************************************************
 * HWoonFriend
 ***************************************************************************** */
typedef void *WoonContentHandle;
typedef void 	(*WoonFriendEventListener)(int event, int size, void *data);

/* Properties */


/* Methods */
/*****************************************************************************
 * HWoonFriend
 ***************************************************************************** */
	OOIFReturnCode native_WoonFriend_BrowseContent(WoonFriendHandle identifier, char *szCid, int nReqCount, char *szFileExt, int nSortType);

/*****************************************************************************
 * HWoonServer
 ***************************************************************************** */
typedef void 	(*WoonServerEventListener)(unsigned event, unsigned int state, void *data);
typedef enum
{
	eNATIVE_WOONS_STARTED,
	eNATIVE_WOONS_STOPPED,
	eNATIVE_WOONS_RENAMED,
	eNATIVE_WOONS_TESTCONNECTION,
	eNATIVE_WOONS_NOTIFYPINCODE,
	eNATIVE_WOONS_UNKNOWN,
} NATIVE_WOONS_EVENT_e;

/* Properties */

/* Methods */
OOIFReturnCode native_WoonServer_Start(const char *szFriendlyName);  
OOIFReturnCode native_WoonServer_Stop();	
OOIFReturnCode native_WoonServer_RegEventListener(WoonServerEventListener listener);
OOIFReturnCode native_WoonServer_UnRegEventListener();
OOIFReturnCode native_WoonServer_TestConnection(OOIFBoolean *retval);
bool 		   native_WoonServer_IsActivated();
char* 		   native_WoonServer_GetServerName();
OOIFReturnCode native_WoonServer_SetServerName(const char *szFriendlyName);

/*****************************************************************************
 * HWoonServerInfo
 ***************************************************************************** */
typedef void *WoonServerInfoHandle;
OOIFReturnCode native_WoonServerInfo_GetState( WoonServerInfoHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_WoonServerInfo_GetLocalIp( WoonServerInfoHandle identifier,const char **retval );
OOIFReturnCode native_WoonServerInfo_GetPublicIp( WoonServerInfoHandle identifier, const char **retval );
OOIFReturnCode native_WoonServerInfo_GetExternalIp( WoonServerInfoHandle identifier, const char **retval );
OOIFReturnCode native_WoonServerInfo_GetPublicPort( WoonServerInfoHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_WoonServerInfo_GetExternalPort( WoonServerInfoHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_WoonServerInfo_ReleaseHandle( WoonServerInfoHandle id );

/*****************************************************************************
 * HWoonContent
 ***************************************************************************** */
/* Properties */
OOIFReturnCode native_WoonContent_GetContentType(WoonContentHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_WoonContent_GetCid(WoonContentHandle identifier, const char** retval);
OOIFReturnCode native_WoonContent_GetParentCid(WoonContentHandle identifier, const char** retval);
OOIFReturnCode native_WoonContent_GetTitle(WoonContentHandle identifier, const char** retval);
OOIFReturnCode native_WoonContent_GetCreator(WoonContentHandle identifier, const char** retval);
OOIFReturnCode native_WoonContent_GetResolution(WoonContentHandle identifier, const char** retval);
OOIFReturnCode native_WoonContent_GetDuraion(WoonContentHandle identifier,  const char** retval);
OOIFReturnCode natvie_WoonContent_GetBitrate(WoonContentHandle identifier, OOIFNumber *retval);
OOIFReturnCode natvie_WoonContent_GetColordepth(WoonContentHandle identifier, OOIFNumber *retval);

/* Methods */



#endif /* OIPF */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HWOON__*/
