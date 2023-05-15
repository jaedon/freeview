/**************************************************************************************/
/**
 * @file NativeHmxDlna.cpp
 *
 * DLNA/DMP module
 *
 * @author  Munho Park(mhpark3@humaxdigital.com)
 * @date    2011/12/08
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NativeHmxDLNA.h"
#include "macros.h"
#include "opldmp.h"

#ifdef OIPF

/*****************************************************************************
 * HMX_DMSSearcher
 *****************************************************************************
*/

#define	DMP_ACTION_STOP		0
#define DMP_ACTION_START	1
OOIFReturnCode native_DMPAction( int actionType, OOIFNumber *retval )
{
	switch ( actionType )
	{
		case DMP_ACTION_START :
			OPL_DMP_Start();
			break;
		case DMP_ACTION_STOP :
			OPL_DMP_Stop();
			break;
		default :
			*retval = FALSE;
			return OOIF_RETURN_OK;

	}
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DMSSearchGetAvailableDMSList( OOIFNumber *retval )
{
	//OPL_DMP_GetListOfDMS();
	//*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DMSSearchGetCurrentDMS( DMSDeviceHandle *identifier )
{
	OPL_DMP_GetCurrentDMS(identifier);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DMSSearchRegisterEventHandler(OPL_DMP_EventCallback cbFunc )
{
	OPL_DMP_RegisterListener( cbFunc );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DMSSearchUnRegisterEventHandler(void)
{
	OPL_DMP_UnRegisterListener();
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * HMX_DMS
 *****************************************************************************
*/
/* Properties */
OOIFReturnCode native_DMSDeviceGetUDN( DMSDeviceHandle identifier, const char **retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getUDN();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DMSDeviceGetFriendlyName( DMSDeviceHandle identifier, const char **retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getFriendlyName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DMSDeviceGetIpAddress( DMSDeviceHandle identifier, const char **retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getIpAddress();
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_DMSDeviceRegisterEventHandler( void (cbFunc)(int event, char *udn, int reqId, int size, void **data, int total) )
{
	OplDMSDevice::registerEventListener( cbFunc );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DMSDeviceUnRegisterEventHandler( void )
{
	OplDMSDevice::unRegisterEventListener();
	return OOIF_RETURN_OK;
}

/* Methods */
OOIFReturnCode native_DMSDeviceReleaseHandle( DMSDeviceHandle id )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(id);
	if ( NULL != dmsDevice )
	{
		delete dmsDevice;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DMSDeviceBrowseDirectChildren( DMSDeviceHandle identifier, char *cid, int pageNum, OOIFNumber *retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	dmsDevice->browsingChildItem( (const char *)cid, NULL, 0, pageNum);
	*retval = 0;
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * HMX_DLNAContent
 *****************************************************************************
*/
OOIFReturnCode native_DLNAContentGetObjectID( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szCID;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetParentID( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szParentId;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetClass( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szClass;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetRestricted( DLNAContentHandle identifier, OOIFBoolean *retval)
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = (OOIFBoolean)content->bRestricted;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetObjectType( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szObjectType;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetImportURI( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szImportURI;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetThumbnailURI( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	if ( 0 == strlen(content->szThumbnailURI) )
		*retval = NULL;
	else
		*retval = content->szThumbnailURI;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetTitle( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szTitle;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetDate( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szDate;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetDuration( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szDuration;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetResolution( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szResolution;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetDigitalCopyControl( DLNAContentHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szDigitalCopyControl;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_DLNAContentGetChildCount( DLNAContentHandle identifier, OOIFNumber *retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = (OOIFNumber)content->ulChildCount;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentGetGenre( DLNAContentMetaHandle identifier, const char **retval )
{
	OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
	*retval = content->szGenre;
	return OOIF_RETURN_OK;
}


DLNAContentMetaHandle  native_DLNAContentGetMetaData( DLNAContentHandle identifier )
{
	if ( NULL == identifier )
		return NULL;

	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	return (DLNAContentHandle)(&content->stMetaData);
}

OOIFReturnCode native_DLNAContentReleaseHandle( DLNAContentHandle id )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(id);
	if ( NULL != content )
	{
		delete content;
		content = NULL;
	}
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * HMX_DLNAContentMetaData
 *****************************************************************************
*/
OOIFReturnCode native_DLNAContentMetaGetAlbum( DLNAContentMetaHandle identifier, const char **retval )
{
	OplDLNAContentMeta *meta = static_cast<OplDLNAContentMeta *>(identifier);
	*retval = meta->szAlbumTitle;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DLNAContentMetaGetArtist( DLNAContentMetaHandle identifier, const char **retval )
{
	OplDLNAContentMeta *meta = static_cast<OplDLNAContentMeta *>(identifier);
	*retval = meta->szArtist;
	return OOIF_RETURN_OK;
}

#endif // OIPF
