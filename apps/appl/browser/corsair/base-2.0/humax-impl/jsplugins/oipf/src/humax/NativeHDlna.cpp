/*************************************************************************************************************
	File 		: NativeHDlna.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/10/24
	attention 	: 

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (Â¡Â°HumaxÂ¡Â±) and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED Â¡Â°AS ISÂ¡Â± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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
#include <typeinfo> 

#include "NativeHDlna.h"
#include "macros.h"
#include <hlib.h>


#include "opldmp.h"		// for dmp
#include "opldmr.h"		// for dmr
#include "opldms.h" 	// for dms

#ifdef OIPF


typedef enum{
 	MAX_DMS_FRIENDLY_NAME_BUF_LEN =	128,
 }NativeHDlna_e;


char 								g_szDmsFriendlyName[MAX_DMS_FRIENDLY_NAME_BUF_LEN];
char 								g_szDmrFriendlyName[MAX_DMS_FRIENDLY_NAME_BUF_LEN];

static DlnaCdsEventListener 		g_cbDlnaCds = NULL;
static DlnaCdsContentEventListener 	g_cbDlnaContent = NULL;
//static DlnaDmrEventListener			g_cbDlnaDmr = NULL;

/*****************************************************************************
 * HDlnaDmp
 ******************************************************************************/

STATIC void 						local_Dmp_StartCdsEventCheckTimer();
STATIC void* 						local_Dmp_OnCdsScan(void *pParam );
STATIC void  						local_Dmp_OplCdsScanCallback(int event, int size, CdsDeviceHandle data);

void  			local_Dmp_OplCdsScanCallback(int event, int size, CdsDeviceHandle data)
{
	switch(event)
	{
		case eDMP_SERVER_REMOVE:
			event = eDMS_SCAN_REMOVE;
			break;
		case eDMP_SERVER_ADD:
			event = eDMS_SCAN_ADD;
			break;
        case eDMP_SERVER_SCAN_STARTED:
            event = eDMS_SCAN_STARTED;
            break;
        case eDMP_SERVER_SCAN_COMPLETE:
            event = eDMS_SCAN_COMPLETE;
            break;            
		default:
			break;
	}
	if(g_cbDlnaCds)
	{
		g_cbDlnaCds(event, size, data);
	}
}

OOIFReturnCode 	native_Dmp_Start(OOIFBoolean *retval)
{
	if ( OPL_DMP_Start() == BPL_STATUS_OK )
	{
    	*retval = TRUE;	
	}
    else
    {
       	*retval = FALSE;	
    }
  	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_Dmp_Stop(OOIFBoolean *retval)
{
	OPL_DMP_Stop();
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dmp_RefreshDms(OOIFBoolean *retval, const char *pUdn)
{
	OPL_DMP_RefreshDms( pUdn );
	*retval = TRUE;
	return OOIF_RETURN_OK;
}


OOIFReturnCode 	native_Dmp_CdsScanRegisterEventHandler(OPL_DMP_EventCallback cbFunc)
{
	g_cbDlnaCds = (DlnaCdsEventListener)cbFunc;
	OPL_DMP_RegisterListener( local_Dmp_OplCdsScanCallback);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dmp_CdsScanUnRegisterEventHandler()
{
	OPL_DMP_UnRegisterListener();
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dmp_GetAvailableCdsList( OOIFNumber *retval, HUINT32 *nCount, CdsDeviceHandle *cdsHandle )
{
	OplDMSDeviceHandle _dmsDevice;
	OPL_DMP_GetListOfDMS(nCount, &_dmsDevice);
	*cdsHandle = _dmsDevice;
	*retval = TRUE;
	return OOIF_RETURN_OK;	
}

/*************************************************************************/

/*****************************************************************************
 * HDlnaDmr
 ******************************************************************************/
OOIFReturnCode 	native_Dmr_Start(OOIFBoolean *retval)
{
	OPL_DMR_Start();
	OPL_DMR_IsActivated(retval);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_Dmr_Stop(OOIFBoolean *retval)
{
	OPL_DMR_Stop();
	OPL_DMR_IsActivated(retval);
	return OOIF_RETURN_OK;
}

bool 			native_Dmr_IsStarted() // same with IsActivated???
{
	HBOOL ret = FALSE;
	//1 ## ¹º°¡ Á» ÀÌ»óÇÏÁö ¾Ê³ª??
	OPL_DMR_IsActivated(&ret);
	return (bool)ret;
}

bool 			native_Dmr_IsActivated()
{
	HBOOL ret = FALSE;
	//1 ## ¹º°¡ Á» ÀÌ»óÇÏÁö ¾Ê³ª??	
	OPL_DMR_IsActivated(&ret);
	return (bool)ret;
}

char* 			native_Dmr_GetFriendlyName()
{
	memset(g_szDmrFriendlyName, 0x00, sizeof(char)*MAX_DMS_FRIENDLY_NAME_BUF_LEN);
	OPL_DMR_GetDmrFriendlyName(g_szDmrFriendlyName, MAX_DMS_FRIENDLY_NAME_BUF_LEN);
	return g_szDmrFriendlyName;

}
OOIFReturnCode 	native_Dmr_SetFriendlyName(const char *friendlyName)
{
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );
	OOIF_LOG_PRINT ( "[%s:%s:%d] %s \n", __FILE__, __FUNCTION__, __LINE__, friendlyName );

	OPL_DMR_SetDmrFriendlyName(friendlyName);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dmr_RegEventListener(DlnaDmrEventListener listener)
{
	OPL_DMR_RegisterListener(listener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dmr_UnRegEventListener()
{
	OPL_DMR_UnRegisterListener();
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dmr_SetMediaChange(OOIFBoolean *retval, const char *szURI)
{
	OPL_DMR_SetBringInUri( szURI );
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * HDlnaDms
 ******************************************************************************/

OOIFReturnCode 	native_Dms_Start(OOIFBoolean *retval)
{
	OPL_DMS_Start();
	
	OPL_DMS_IsActivated((HBOOL*)retval);	
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dms_Stop(OOIFBoolean *retval)
{
	OPL_DMS_Stop();
	
	OPL_DMS_IsActivated((HBOOL*)retval);	
		
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Dms_GetState(OOIFBoolean *retval)
{
	OPL_DMS_IsActivated((HBOOL*)retval);	
	return OOIF_RETURN_OK;
}
bool 			native_Dms_IsActivated()
{
	HBOOL ret = FALSE;
	OPL_DMS_IsActivated( &ret );	
	return ( bool )ret;
}
OOIFReturnCode 	native_Dms_GetCurrentDMS( CdsDeviceHandle *identifier )
{
	OPL_DMP_GetCurrentDMS(identifier);
	return OOIF_RETURN_OK;
}

char* 			native_Dms_GetFriendlyName()
{
	memset(g_szDmsFriendlyName, 0x00, sizeof(char)*MAX_DMS_FRIENDLY_NAME_BUF_LEN);
	OPL_DMS_GetDmsFriendlyName(g_szDmsFriendlyName, MAX_DMS_FRIENDLY_NAME_BUF_LEN);
	return g_szDmsFriendlyName;
}

OOIFReturnCode 	native_Dms_SetFriendlyName(const char *friendlyName)
{
	OPL_DMS_SetDmsFriendlyName(friendlyName);
	return OOIF_RETURN_OK;
}


/*****************************************************************************
 * HDlnaCds
 ******************************************************************************/
void  			local_Cds_OplContentScanCallback(int event, char *udn, int reqId, int size, CdsDeviceHandle *data, int total )
{	
	if(NULL != g_cbDlnaContent)
	{
		switch(event){
			case eDMP_CONTENT_ADD:
				event = eCDS_CONTENT_ADD;
				break;
			case eDMP_CONTENT_END:
				event = eCDS_CONTENT_END;
				break;
			case eDMP_CONTENT_SEARCH_DONE :
				event = eCDS_SEARCH_DONE;
				break;
			case eDMP_CONTENT_SEARCH_ERROR :
				event = eCDS_SEARCH_ERROR;
				break;
			case eDMP_CONTENT_BROWSE_ERROR:
				event = eCDS_BROWSE_ERROR;
				break;
			case eDMP_RECORD_DESTINATIONS :
			case eDMP_RECORD_DISKINFO :
				return ;
			default:
				event = eCDS_BROWSE_ERROR;
				break;
		}
		g_cbDlnaContent(event, reqId, size, data, udn, total);
	}
}
OOIFReturnCode 	native_Cds_RegisterEventHandler(DlnaCdsContentEventListener cbFunc)
{
	g_cbDlnaContent= (DlnaCdsContentEventListener)cbFunc;
	OplDMSDevice::registerEventListener( local_Cds_OplContentScanCallback);
	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_Cds_UnRegisterEventHandler()
{
	OplDMSDevice::unRegisterEventListener();
	return OOIF_RETURN_OK;
}

//OOIFReturnCode 	native_Cds_BrowseContent( CdsDeviceHandle identifier, char *szCid, int nReqItemCount, char*szFileExt, int nSortType, OOIFNumber *retval )
OOIFReturnCode 	native_Cds_BrowseContent( CdsDeviceHandle identifier, char *szCid, char *szSort, int nStartIdx, int nReqCount, OOIFNumber *retval )
{
	if(identifier)
	{
		OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);

		dmsDevice->browsingChildItem( (const char *)szCid, ( const char * )szSort, nStartIdx, nReqCount);	
		if(NULL != g_cbDlnaContent)
		{
			g_cbDlnaContent(eCDS_BROWSE_STARTED, 0, 0, NULL, szCid, 0);
		}
		*retval = 0;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Cds_SearchContent( CdsDeviceHandle identifier, const char *pszCid, const char *pszCategory,  
					const char *pszKeyword, const char *pszSort, int nStartIdx, int nReqCount, OOIFNumber *retval )
{
	if(identifier)
	{
		OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
		*retval = dmsDevice->searchingItem( pszCid, pszCategory, pszKeyword, pszSort, nStartIdx, nReqCount);	
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Cds_GetUDN( CdsDeviceHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
		*retval = dmsDevice->getUDN();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode 	native_Cds_GetFriendlyName( CdsDeviceHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
		*retval = dmsDevice->getFriendlyName();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode 	native_Cds_GetIpAddress( CdsDeviceHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
		*retval = dmsDevice->getIpAddress();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode 	native_Cds_ReleaseHandle( CdsDeviceHandle id )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(id)
	{
		OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(id);
		if ( NULL != dmsDevice && typeid(OplDMSDevice) == typeid(*dmsDevice) )
		{
			delete dmsDevice;
			dmsDevice = NULL;
		}
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

/*****************************************************************************
 * HDlnaCdsContent
 *****************************************************************************
*/

OOIFReturnCode native_CdsContent_GetObjectID( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szCID;
	}
	else
	{	
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetParentID( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szParentId;
	}
	else
	{	
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetClass( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szClass;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetRestricted( CdsContentHandle identifier, OOIFBoolean *retval)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = (OOIFBoolean)content->bRestricted;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetObjectType( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szObjectType;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetImportURI( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szImportURI;
	}
	else
	{	
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetThumbnailURI( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		if ( 0 == strlen(content->szThumbnailURI) )
			*retval = NULL;
		else
			*retval = content->szThumbnailURI;
	}
	else
	{	
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetByteSeekAble( CdsContentHandle identifier, OOIFNumber *retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = (OOIFNumber)content->bByteSeekAble;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetTimeSeekAble( CdsContentHandle identifier, OOIFNumber *retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = (OOIFNumber)content->bTimeSeekAble;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetTitle( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szTitle;
	}
	else
	{	
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetDate( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szDate;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetDuration( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szDuration;
	}
	else
	{	
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetResolution( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szResolution;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetDigitalCopyControl( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = content->szDigitalCopyControl;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode native_CdsContent_GetChildCount( CdsContentHandle identifier, OOIFNumber *retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
		*retval = (OOIFNumber)content->ulChildCount;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetGenre( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
		*retval = content->szGenre;
	}
	else
	{	
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode native_CdsContent_GetAlbum( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
		*retval = content->stMetaData.szAlbumTitle;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_GetArtist( CdsContentHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
		*retval = content->stMetaData.szArtist;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_CdsContent_ReleaseHandle( CdsContentHandle id )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(id)
	{
		OplDLNAContent *content =  static_cast<OplDLNAContent *>(id);
		if ( NULL != content )
		{
			delete content;
			content = NULL;
		}
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}


/*****************************************************************************
 * HMX_DLNAContentMetaData
 *****************************************************************************
*/

#endif // OIPF
