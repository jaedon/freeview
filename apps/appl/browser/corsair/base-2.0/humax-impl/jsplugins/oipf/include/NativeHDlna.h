/*************************************************************************************************************
	File 		: HContentBrowser.cpp
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


#ifndef __NATIVE_HDLNA__
#define __NATIVE_HDLNA__

#include "NativeOOIFTypes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined OIPF

typedef enum
{
	NATIVE_MAX_THREAD_STACK_SIZE = 1 * 1024 * 1024,
	NATIVE_MAX_CID_LEN	= 512,
	NATIVE_MAX_SORT_LEN	= 32,
	NATIVE_MAX_URI		= 512,
	NATIVE_MAX_UDN		= 48
} HDLNA_CONST_e;

typedef enum
{
	eDMS_SCAN_ADD		= 1,
	eDMS_SCAN_REMOVE	= 2,

	eDMS_SCAN_STARTED   = 0,
	eDMS_SCAN_COMPLETE  = 3,
	eDMS_SCAN_FAILED    = 4
} CDS_SERVER_UpdateState_e;

typedef enum
{
	eCDS_BROWSE_STARTED = 0,
	eCDS_CONTENT_ADD	= 1,
	eCDS_CONTENT_END	= 2, // COMPLETE
	eCDS_BROWSE_ERROR	= 3,

	eCDS_SEARCH_STARTED,  // NOT USE
	eCDS_SEARCH_DONE,
	eCDS_SEARCH_ERROR,
	eCDS_SEARCH_STOPPED, // NOT USE

} CDS_CONTENT_BrowseState_e;

typedef void	(*DlnaCdsEventListener)(int event, int size, void *data);
typedef void 	(*DlnaCdsContentEventListener)(int event, int reqId, int size, void **data, char *parentCid, int total);

/*****************************************************************************
 * HDlnaDmr
 *****************************************************************************
*/
typedef void	(*DlnaDmrEventListener)(unsigned int event, unsigned int result, void *cdsItem);
typedef enum
{
	eNATIVE_DMR_REQ_PLAY,
	eNATIVE_DMR_REQ_STOP,
	eNATIVE_DMR_REQ_PAUSE,
	eNATIVE_DMR_REQ_SEEK,
	eNATIVE_DMR_REQ_SETVOLUME,
	eNATIVE_DMR_REQ_SETMUTE,
	eNATIVE_DMR_REQ_BRINGIN  /* setMediaChange(uri) to webapp */
} NativeMRCP_Request_e;

OOIFReturnCode 	native_Dmr_Start(OOIFBoolean *retval);
OOIFReturnCode 	native_Dmr_Stop(OOIFBoolean *retval);
bool 			native_Dmr_IsStarted();
bool 			native_Dmr_IsActivated();
OOIFReturnCode 	native_Dmr_RegEventListener(DlnaDmrEventListener listener);
OOIFReturnCode 	native_Dmr_UnRegEventListener();
OOIFReturnCode 	native_Dmr_SetMediaChange(OOIFBoolean *retval, const char *szURI);
char* 			native_Dmr_GetFriendlyName();
OOIFReturnCode 	native_Dmr_SetFriendlyName(const char *friendlyName);


/*****************************************************************************
 * HDlnaDmp
 *****************************************************************************
*/
typedef void *CdsSearcherHandle;
typedef void *CdsDeviceHandle;
/* Methods */
OOIFReturnCode native_Dmp_Start(OOIFBoolean *retval );
OOIFReturnCode native_Dmp_Stop(OOIFBoolean *retval );
OOIFReturnCode native_Dmp_RefreshDms(OOIFBoolean *retval, const char *pUdn);
OOIFReturnCode native_Dmp_GetAvailableCdsList( OOIFNumber *retval, unsigned int *nCount, CdsDeviceHandle *cdsHandle );
OOIFReturnCode native_Dmp_CdsScanRegisterEventHandler(DlnaCdsEventListener listener );
OOIFReturnCode native_Dmp_CdsScanUnRegisterEventHandler(void);

/*****************************************************************************
 * HDlnaDms
 ******************************************************************************/
OOIFReturnCode 	native_Dms_Start(OOIFBoolean *retval);
OOIFReturnCode 	native_Dms_Stop(OOIFBoolean *retval);
OOIFReturnCode 	native_Dms_GetState(OOIFBoolean *retval);
OOIFReturnCode 	native_Dms_GetCurrentDMS( CdsDeviceHandle *identifier );
bool 			native_Dms_IsActivated();
char* 			native_Dms_GetFriendlyName();
OOIFReturnCode 	native_Dms_SetFriendlyName(const char *friendlyName);

/*****************************************************************************
 * HDlnaCds
 *****************************************************************************
*/
/* Properties */
OOIFReturnCode native_Cds_GetUDN( CdsDeviceHandle identifier, const char **retval );
OOIFReturnCode native_Cds_GetFriendlyName( CdsDeviceHandle identifier, const char **retval);
OOIFReturnCode native_Cds_GetIpAddress( CdsDeviceHandle identifier, const char **retval);

/* Methods */
OOIFReturnCode native_Cds_ReleaseHandle( CdsDeviceHandle id );
//OOIFReturnCode native_Cds_BrowseContent( CdsDeviceHandle identifier, char *szCid, int nReqItemCount, char*szFileExt, int nSortType, OOIFNumber *retval );
OOIFReturnCode 	native_Cds_BrowseContent( CdsDeviceHandle identifier, char *szCid, char *szSort, int nStartIdx, int nReqCount, OOIFNumber *retval );
OOIFReturnCode 	native_Cds_SearchContent( CdsDeviceHandle identifier, const char *pszCid, const char *pszCategory, 
					const char *pszKeyword, const char *pszSort, int nStartIdx, int nReqCount, OOIFNumber *retval );

OOIFReturnCode native_Cds_RegisterEventHandler( DlnaCdsContentEventListener listener );
OOIFReturnCode native_Cds_UnRegisterEventHandler( void );

/*****************************************************************************
 * HDlnaSearchedCds
 *****************************************************************************
*/
/* Properties */
OOIFReturnCode 	native_SearchedCds_GetUDN( CdsDeviceHandle identifier, const char **retval );
OOIFReturnCode 	native_SearchedCds_GetFriendlyName( CdsDeviceHandle identifier, const char **retval );
OOIFReturnCode 	native_SearchedCds_GetIpAddress( CdsDeviceHandle identifier, const char **retval );
OOIFReturnCode 	native_SearchedCds_GetCategory( CdsDeviceHandle identifier, const char **retval );
OOIFReturnCode 	native_SearchedCds_GetKeyword( CdsDeviceHandle identifier, const char **retval );
OOIFReturnCode 	native_SearchedCds_GetMatchCount( CdsDeviceHandle identifier, OOIFNumber *retval );

/* Methods */
OOIFReturnCode 	native_SearchedCds_RequestContent( CdsDeviceHandle identifier, char *szSort, int nStartIdx, int nReqCount, OOIFNumber *retval );
OOIFReturnCode 	native_SearchedCds_RegisterEventHandler(DlnaCdsContentEventListener cbFunc);
OOIFReturnCode 	native_SearchedCds_UnRegisterEventHandler();
OOIFReturnCode 	native_SearchedCds_ReleaseHandle( CdsDeviceHandle id );

/*****************************************************************************
 * HDlnaCdsContent
 *****************************************************************************
*/
typedef void *CdsContentHandle;

OOIFReturnCode native_CdsContent_GetObjectID( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetParentID( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetClass( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetRestricted( CdsContentHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_CdsContent_GetObjectType( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetImportURI( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetThumbnailURI( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetByteSeekAble( CdsContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_CdsContent_GetTimeSeekAble( CdsContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_CdsContent_GetTitle( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetDate( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetDuration( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetResolution( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetDigitalCopyControl( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetChildCount( CdsContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_CdsContent_GetGenre( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetAlbum( CdsContentHandle identifier, const char **retval );
OOIFReturnCode native_CdsContent_GetArtist( CdsContentHandle identifier, const char **retval );

/* Methods */
OOIFReturnCode native_CdsContent_ReleaseHandle( CdsContentHandle id );

#endif /* OIPF */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HDLNA__ */
