/**************************************************************************************/
/**
 * @file opldlna.cpp
 *
 * OplDLNA
 *
 * @author  Mun-ho Park(mhpark3@humaxdigital.com)
 * @date    2011/12/08
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "opldmp.h"

#include "oapi.h"
#include <apk.h>
#include <hlib.h>
#include <vkernel.h>


#define ____________OPL_DMP_______________
static	OPL_DMP_EventCallback * s_pfn_dmp_callback = NULL;
#if defined(CONFIG_OP_JAPAN)
static	OPL_REMOVE_EventCallback * s_pfn_remove_callback = NULL;
#endif

unsigned long dmp_TimerID = 0;
const  int CDS_UPDATE_TIMEOUT = 3*1000; 

static void _OPL_DMP_TimerCallback(unsigned long timerID, void *param)
{
    /*
    ** note : this function is called on different thread, becuase of vk_timer
    */
    HxLOG_Debug("[OPL_DMP] _OPL_DMP_TimerCallback!!!\n");
    if ( (timerID == dmp_TimerID) && s_pfn_dmp_callback )
    {
        s_pfn_dmp_callback (eDMP_SERVER_SCAN_COMPLETE, 0, NULL );
    }
    else
    {
         HxLOG_Error("[OPL_DMP] _OPL_DMP_TimerCallback is null, timerid:%d\n", dmp_TimerID);
    }
    dmp_TimerID = 0;
}

static	void	_OPL_DMP_EventNotifier( HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	APKS_DLNA_DMP_ServerDevice_t *serverItems = (APKS_DLNA_DMP_ServerDevice_t *)ulParam2;
	HUINT32 ulEvent = ulParam1;
	HINT32 count = ulParam3;
	OplDMP_SERVER_Update_e eState;
	switch ( ulEvent )
	{
		case eDLNA_DMP_ServerAdd :
        {
			HxLOG_Print("[OPL_DMP] CDS Added!!!!!\n");
			eState = eDMP_SERVER_ADD;

            /*
            ** note : if timer is working, restart timer
            */
            HERROR re = ERR_OK;
            if ( dmp_TimerID != 0 )  
            {
                re = VK_TIMER_Cancel(dmp_TimerID);
                dmp_TimerID = 0;
                if ( re == ERR_OK )
                {
                    re = VK_TIMER_EventAfter(CDS_UPDATE_TIMEOUT, _OPL_DMP_TimerCallback, (void *)NULL, (int)0, &dmp_TimerID);
                }
            }
            
		}
			break;
		case eDLNA_DMP_ServerRemove :
			HxLOG_Print("[OPL_DMP] CDS Removed!!!!!\n");
			{
				eState = eDMP_SERVER_REMOVE;
				if ( NULL != OplDMSDevice::m_sCurrentDMS )
				{
					HCHAR *szCurrentUDN = OplDMSDevice::m_sCurrentDMS->getUDN();
					if ( !HxSTD_StrCmp(serverItems->szUDN, szCurrentUDN ) )
					{
						//delete OplDMSDevice::m_sCurrentDMS;
						OplDMSDevice::m_sCurrentDMS = NULL;
					}

				}
			}
			break;
		case eDLNA_DMP_ServerList :
			eState = eDMP_SERVER_ADD;
			break;

		case eDLNA_DMP_ContentAdd :
			return;

		case eDLNA_DMP_ContentEnd :
			return;

		default :
			return ;
	}

	// Send Event
	if (s_pfn_dmp_callback)
	{
		int i = 0;

#if defined(CONFIG_OP_JAPAN)
		OplDMSDevice** ppDmsList = (OplDMSDevice **)HLIB_STD_MemAlloc(sizeof(OplDMSDeviceHandle) * count);
		for ( i = 0; i < count; i++ )
		{
			ppDmsList[i] = new OplDMSDevice( (OplDMSDeviceHandle)(&serverItems[i]) );
		}

		s_pfn_dmp_callback( eState, count, ppDmsList);
#else
		for ( i = 0; i < count; i++ )
		{
			OplDMSDevice *dmpDevice = NULL;
			dmpDevice = new OplDMSDevice( (OplDMSDeviceHandle)(&serverItems[i]) );
			s_pfn_dmp_callback( eState, count, dmpDevice);
		}
#endif
	}
}



BPLStatus	OPL_DMP_Start( void )
{
    HERROR re = ERR_OK;
    
    if ( dmp_TimerID != 0 )
    {
        re = VK_TIMER_Cancel(dmp_TimerID);
        dmp_TimerID = 0;
    }
    if ( re == ERR_OK && dmp_TimerID == 0)
    {
        re = VK_TIMER_EventAfter(CDS_UPDATE_TIMEOUT , _OPL_DMP_TimerCallback, NULL, 0,  &dmp_TimerID);
    }
    if ( re == ERR_OK )
    {  
        
        if ( s_pfn_dmp_callback )
            s_pfn_dmp_callback (eDMP_SERVER_SCAN_STARTED, 0, NULL );
    	APK_DLNADMP_RegisterNotifier(_OPL_DMP_EventNotifier);
    	APK_DLNADMP_RegisterNotifier( OplDMSDevice::onEventNotifier );
    	APK_DLNADMP_Start();
    	return BPL_STATUS_OK;
    }
    return BPL_STATUS_ERROR;
}

BPLStatus	OPL_DMP_Stop( void )
{    
    if ( dmp_TimerID != 0 )
    {
        VK_TIMER_Cancel(dmp_TimerID);
        dmp_TimerID = 0;
    }
	APK_DLNADMP_UnRegisterNotifier(_OPL_DMP_EventNotifier);
	APK_DLNADMP_UnRegisterNotifier( OplDMSDevice::onEventNotifier );

	if( NULL != OplDMSDevice::m_sCurrentDMS )
	{
		delete  OplDMSDevice::m_sCurrentDMS;
		OplDMSDevice::m_sCurrentDMS = NULL;
	}

	APK_DLNADMP_Stop();
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMP_RefreshDms( const HCHAR *pUdn )
{
	APK_DLNADMP_RefreshDms( pUdn );
	return BPL_STATUS_OK;
}


BPLStatus	OPL_DMP_GetListOfDMS( HUINT32 *nCount, OplDMSDeviceHandle *dmpDevice)
{
	APKS_DLNA_DMP_ServerDevice_t *pResultData = NULL;
	OplDMSDevice **dmpDeviceList = NULL;
	OplDMSDevice *_dmpDevice = NULL;
	HUINT32 _nCount = 0, i = 0;

	APK_DLNADMP_getListOfDMS(&_nCount, &pResultData);

	if(_nCount > 0)
	{
		dmpDeviceList = (OplDMSDevice **)HLIB_STD_MemAlloc(sizeof(OplDMSDeviceHandle)*_nCount);
		if(dmpDeviceList != NULL)
		{
			HxSTD_MemSet(dmpDeviceList, 0x00, sizeof(OplDMSDeviceHandle)*_nCount);

			for ( i = 0; i < _nCount; i++ )
			{
				_dmpDevice = new OplDMSDevice((OplDMSDeviceHandle)(&(pResultData[i])) );
				dmpDeviceList[i] = _dmpDevice;
			}
		}
		else
		{
			*dmpDevice = (OplDMSDeviceHandle)NULL;
			*nCount = 0;
			HxLOG_Critical("[%s:%d] memory allocate fail... !! nCount : [%d] \n", __FUNCTION__, __LINE__, _nCount);
			return BPL_STATUS_ERROR;
		}
	}

	*dmpDevice = (OplDMSDeviceHandle)dmpDeviceList;
	*nCount = _nCount;

	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMP_GetCurrentDMS( OplDMSDeviceHandle *identifier )
{
	if ( NULL == identifier )
	{
		return BPL_STATUS_ERROR;
	}
	*identifier = (OplDMSDeviceHandle)OplDMSDevice::m_sCurrentDMS;
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMP_RegisterListener( OPL_DMP_EventCallback callback )
{
	s_pfn_dmp_callback = callback;
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMP_UnRegisterListener( void )
{
	s_pfn_dmp_callback = NULL;
	return BPL_STATUS_OK;
}

#if defined(CONFIG_OP_JAPAN)
#define BOOKMARK_MAX_NAME_LEN	48
#define BOOKMARK_MAX_COUNT		8

HUINT16     usNumOfBookmark;
typedef	struct
{
	HUINT32 ulPos;
	HCHAR 	szName[BOOKMARK_MAX_NAME_LEN];
}DlnaRecList_Bookmark_t;

DlnaRecList_Bookmark_t astBookmarks[BOOKMARK_MAX_COUNT];

OPL_Bookmark_t*     OPL_CDSRecording_Bookmark_New (unsigned long   time, const char *name)
{
	OPL_Bookmark_t  *self;

	HxLOG_Trace();

	self = (OPL_Bookmark_t *)HLIB_STD_MemCalloc(sizeof(OPL_Bookmark_t));
	if (self)
	{
		self->time  = time;
		self->name  = HLIB_STD_StrDup(name);
	}

	return self;
}

void	OPL_CDSRecording_Bookmark_Delete (OPL_Bookmark_t *self)
{
	HxLOG_Trace();

	if (self == NULL)
		return;

	if (self->name)
	{
		HLIB_STD_MemFree(self->name);
	}

	HLIB_STD_MemFree(self);
}

OPL_Bookmark_t *    OPL_CDSRecording_Bookmark_Clone (const OPL_Bookmark_t *self)
{
	if (self)
	{
		return OPL_CDSRecording_Bookmark_New(self->time, self->name);
	}
	return NULL;
}

static  OPL_Bookmark_t*     opl_CDSRecording_MakeBookmark(HUINT16 index)
{
	OPL_Bookmark_t  *bookmark = NULL;
	if (usNumOfBookmark <= index)
	{
		return NULL;
	}

	bookmark = OPL_CDSRecording_Bookmark_New(astBookmarks[index].ulPos, astBookmarks[index].szName);
	return bookmark;
}

static  OPL_Collection_t* opl_CDSRecording_MakeBookmarkCollection()
{
	OPL_Collection_t    *c;
	OPL_Bookmark_t      *b;
	int i;

	c = OPL_Collection_NewEx((void (*)(void *))OPL_CDSRecording_Bookmark_Delete, (void *(*)(void *))OPL_CDSRecording_Bookmark_Clone);
	if (c == NULL)
	{
		HxLOG_Critical("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	for (i = 0 ; i < usNumOfBookmark ; i++)
	{

		b = opl_CDSRecording_MakeBookmark(i);
		if (b == NULL)
			continue;

		OPL_Collection_Add(c, (void *)b);
	}
	return c;
}

BPLStatus	OPL_REMOVE_RegisterListener( OPL_REMOVE_EventCallback callback )
{
	s_pfn_remove_callback = callback;
	return BPL_STATUS_OK;
}

BPLStatus	OPL_REMOVE_UnRegisterListener( void )
{
	s_pfn_remove_callback = NULL;
	return BPL_STATUS_OK;
}

void		OPL_CDSRecording_Remove (Opl_CDSRecording_t *item, char **retVal)
{
	HxLOG_Trace();

	APK_DLNADMP_RemoveItem(item->szUDN, item->szCID, retVal);
}

#endif

/******************************************************************
	class OplDMSDevice
******************************************************************/
//static
OPL_CONTENT_EventCallback * OplDMSDevice::s_pfn_contentSearch_callback = NULL;

OplDMSDevice* 	OplDMSDevice:: m_sCurrentDMS = NULL;
char* 			OplDMSDevice:: m_sCurrentCID = NULL;
HINT32 			OplDMSDevice:: m_nReqIdCnt = 0;

#if defined(CONFIG_OP_JAPAN)
char * OplDMSDevice:: m_szCurrentDestinationId = NULL;
#endif

OplDMSDevice::OplDMSDevice( OplDMSDeviceHandle handle )
{
	APKS_DLNA_DMP_ServerDevice_t *dmsDevice = NULL;
	m_szUDN = m_szFriendlyName = m_szIpAddr = NULL;

#if defined(CONFIG_OP_JAPAN)
   	m_szMAC = m_szTotalSize= NULL;
#endif

	m_szUDN = new char[OPL_DMP_MAX_UDN];
	m_szFriendlyName = new char[OPL_DMP_MAX_TITLE];
	m_szIpAddr = new char[OPL_DMP_MAX_IP4ADDR];

	if ( NULL == handle )
		return ;

	dmsDevice = static_cast<APKS_DLNA_DMP_ServerDevice_t *>(handle);

	HxSTD_StrNCpy( m_szUDN, dmsDevice->szUDN, OPL_DMP_MAX_UDN -1 );
	HxSTD_StrNCpy( m_szFriendlyName, dmsDevice->szTitle, OPL_DMP_MAX_TITLE -1 ); /* FriendlyName */
	HxSTD_StrNCpy( m_szIpAddr, dmsDevice->szIP, OPL_DMP_MAX_IP4ADDR -1 );

#if defined(CONFIG_OP_JAPAN)
	ulDeviceHandle = (HUINT32) handle;

	bIsSupportSrs = dmsDevice->bIsSupportSrs;

	nSortCap = dmsDevice->ucSortCapNum;
	nSearchCap = dmsDevice->ucSearchCapNum;
	nDlanCap = dmsDevice->ucDlnaCapNum;
	nJlabsCap = dmsDevice->ucJlabsCapNum;

	if(nSortCap)
	{
		if(nSortCap > OPL_DMP_MAX_CAP) 	nSortCap = OPL_DMP_MAX_CAP;
		for(int i=0; i<nSortCap; i++)
		{
			m_szSortCAP[i] = new char[OPL_DMP_MAX_TITLE];
			HxSTD_StrNCpy(m_szSortCAP[i], dmsDevice->szSortCAP[i] ,  OPL_DMP_MAX_TITLE -1 );
		}
	}

	if(nSearchCap)
	{
		if(nSearchCap > OPL_DMP_MAX_CAP) nSearchCap = OPL_DMP_MAX_CAP;
		for(int i=0; i<nSearchCap; i++)
		{
			m_szSearchCAP[i] = new char[OPL_DMP_MAX_TITLE];
			HxSTD_StrNCpy(m_szSearchCAP[i], dmsDevice->szSearchCAP[i] ,  OPL_DMP_MAX_TITLE -1 );
		}
	}

	if(nDlanCap)
	{
		if(nDlanCap>OPL_DMP_MAX_CAP) nDlanCap = OPL_DMP_MAX_CAP;
		for(int i=0; i<nDlanCap; i++)
		{
			m_szDlnaCAP[i] = new char[OPL_DMP_MAX_TITLE];
			HxSTD_StrNCpy(m_szDlnaCAP[i], dmsDevice->szDlnaCAP[i] ,  OPL_DMP_MAX_TITLE -1 );
		}
	}

	if(nJlabsCap)
	{
		if(nJlabsCap>OPL_DMP_MAX_CAP) nJlabsCap = OPL_DMP_MAX_CAP;
		for(int i=0; i<nJlabsCap; i++)
		{
			m_szJlabsCAP[i] = new char[OPL_DMP_MAX_TITLE];
			HxSTD_StrNCpy(m_szJlabsCAP[i], dmsDevice->szJlabsCAP[i] ,  OPL_DMP_MAX_TITLE -1 );
		}
	}

	m_szMAC = new char[OPL_DMP_MAX_TITLE];
	m_szTotalSize = new char[OPL_DMP_MAX_TITLE];

	//ulDeviceHandle = dmsDevice->nDeviceHandle;

	HxSTD_StrNCpy(m_szMAC, dmsDevice->szMACAddress, OPL_DMP_MAX_TITLE -1 );
	HxSTD_StrNCpy(m_szTotalSize, dmsDevice->szTotalStroageSize, OPL_DMP_MAX_TITLE -1);

#endif
}

OplDMSDevice::OplDMSDevice( const OplDMSDevice &original )
{
	m_szUDN = new char[OPL_DMP_MAX_UDN];
	m_szFriendlyName = new char[OPL_DMP_MAX_TITLE];
	m_szIpAddr = new char[OPL_DMP_MAX_IP4ADDR];

	HxSTD_StrNCpy( m_szUDN, original.m_szUDN, OPL_DMP_MAX_UDN -1 );
	HxSTD_StrNCpy( m_szFriendlyName, original.m_szFriendlyName, OPL_DMP_MAX_TITLE -1 );
	HxSTD_StrNCpy( m_szIpAddr, original.m_szIpAddr, OPL_DMP_MAX_IP4ADDR -1 );

#if defined(CONFIG_OP_JAPAN)
	ulDeviceHandle = original.ulDeviceHandle;
	bIsSupportSrs = original.bIsSupportSrs;
	nSortCap = original.nSortCap;
	nSearchCap = original.nSearchCap;
	nDlanCap = original.nDlanCap;
	nJlabsCap = original.nJlabsCap;

	if(nSortCap)
	{
		if(nSortCap > OPL_DMP_MAX_CAP) 	nSortCap = OPL_DMP_MAX_CAP;
		for(int i=0; i<nSortCap; i++)
		{
			m_szSortCAP[i] = new char[OPL_DMP_MAX_TITLE];
			HxSTD_StrNCpy(m_szSortCAP[i], original.m_szSortCAP[i],  OPL_DMP_MAX_TITLE -1 );
		}
	}

	if(nSearchCap)
	{
		if(nSearchCap > OPL_DMP_MAX_CAP) nSearchCap = OPL_DMP_MAX_CAP;
		for(int i=0; i<nSearchCap; i++)
		{
			m_szSearchCAP[i] = new char[OPL_DMP_MAX_TITLE];
			HxSTD_StrNCpy(m_szSearchCAP[i], original.m_szSearchCAP[i] ,  OPL_DMP_MAX_TITLE -1 );
		}
	}

	if(nDlanCap)
	{
		if(nDlanCap>OPL_DMP_MAX_CAP) nDlanCap = OPL_DMP_MAX_CAP;
		for(int i=0; i<nDlanCap; i++)
		{
			m_szDlnaCAP[i] = new char[OPL_DMP_MAX_TITLE];
			HxSTD_StrNCpy(m_szDlnaCAP[i], original.m_szDlnaCAP[i] ,  OPL_DMP_MAX_TITLE -1 );
		}
	}

	if(nJlabsCap)
	{
		if(nJlabsCap>OPL_DMP_MAX_CAP) nJlabsCap = OPL_DMP_MAX_CAP;
		for(int i=0; i<nJlabsCap; i++)
		{
			m_szJlabsCAP[i] = new char[OPL_DMP_MAX_TITLE];
			HxSTD_StrNCpy(m_szJlabsCAP[i], original.m_szJlabsCAP[i] ,  OPL_DMP_MAX_TITLE -1 );
		}
	}

	m_szMAC = new char[OPL_DMP_MAX_TITLE];
	m_szTotalSize = new char[OPL_DMP_MAX_TITLE];

	HxSTD_StrNCpy(m_szMAC, original.m_szMAC, OPL_DMP_MAX_TITLE -1 );
	HxSTD_StrNCpy(m_szTotalSize, original.m_szTotalSize, OPL_DMP_MAX_TITLE -1);
#endif
}

OplDMSDevice::~OplDMSDevice()
{
	if ( m_szUDN )
	{
		delete []m_szUDN;
		m_szUDN = NULL;
	}
	if ( m_szFriendlyName )
	{
		delete []m_szFriendlyName;
		m_szFriendlyName = NULL;
	}
	if ( m_szIpAddr )
	{
		delete []m_szIpAddr;
		m_szIpAddr = NULL;
	}

#if 0
	if (m_sCurrentCID)
	{
		delete m_sCurrentCID;
		m_sCurrentCID = NULL;
	}
#endif

#if defined(CONFIG_OP_JAPAN)
	if ( m_szMAC )
	{
		delete []m_szMAC;
		m_szMAC = NULL;
	}
	if ( m_szTotalSize )
	{
		delete []m_szTotalSize;
		m_szTotalSize = NULL;
	}

	for(int i=0; i< nSortCap; i++)
	{
	   if(m_szSortCAP[i]) {	delete []m_szSortCAP[i]; m_szSortCAP[i] = NULL; }
	}
	for(int i=0; i< nSearchCap;i++)
	{
		if(m_szSearchCAP[i]) { delete []m_szSearchCAP[i]; m_szSearchCAP[i] = NULL;}
	}
	for(int i=0; i< nDlanCap;i++)
	{
		if(m_szDlnaCAP[i]) {delete  []m_szDlnaCAP[i]; m_szDlnaCAP[i] = NULL;}
	}
	for(int i=0; i< nJlabsCap;i++)
	{
		if(m_szJlabsCAP[i]) {delete []m_szJlabsCAP[i]; m_szJlabsCAP[i] = NULL;}
	}

	if(m_szCurrentDestinationId)
		HLIB_STD_MemFree(m_szCurrentDestinationId);
#endif
}

//static
void	OplDMSDevice::onEventNotifier(  HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 )
{
	APKS_DLNA_DMP_ContentItem_t *contentItems = NULL;

	OplDLNAContent ** 		contentList = NULL;
	OplDMP_CONTENT_Update_e eUpdateState = eDMP_CONTENT_END;

	OplDLNAContentMeta *pstMeta = NULL;
	OplDLNAContent * content = NULL;
	HUINT32 		ulEvent	= ulParam1;
	HINT32 			count = ulParam3;
	HBOOL			bIsCurrentList = FALSE;
	HINT32 			i = 0;
	if ( NULL == s_pfn_contentSearch_callback )
	{
		HxLOG_Error("[%s:%d] s_pfn_contentSearch_callback is null \n", __FUNCTION__, __LINE__);
		return ;
	}

	switch ( ulEvent )
		{
			case eDLNA_DMP_BrowseError :
				contentList  = NULL;
				eUpdateState = eDMP_CONTENT_BROWSE_ERROR;
				HxLOG_Print("[ OPL_DMP ] eDLNA_DMP_BrowseError  \n");
				s_pfn_contentSearch_callback( eUpdateState, NULL, 0, 0, NULL, 0 );
				break;
			case eDLNA_DMP_ContentAdd :
			contentItems = (APKS_DLNA_DMP_ContentItem_t *)ulParam2;
				eUpdateState = eDMP_CONTENT_ADD;

				if(count > 0)
				{
					if(m_sCurrentCID == NULL || HxSTD_StrEmpty(contentItems[0].szParentId))
					{
						HxLOG_Error("[%s:%d] m_sCurrentCID or szParentId is null \n", __FUNCTION__, __LINE__);
						return;
					}
					else
					{
						if(HxSTD_StrNCmp(contentItems[i].szParentId, m_sCurrentCID, OPL_DMP_MAX_TITLE-1) != 0)
						{
							HxLOG_Error("[%s:%d] this is not contencts list of current cid... ! \n", __FUNCTION__, __LINE__	);
							return;
						}
					}
				}

			if ( 0 < count )
			{
				contentList = (OplDLNAContent **)HLIB_STD_MemAlloc( sizeof(OplDLNAContent *) * count  );
			}

				for ( i = 0; i < count; i++ )
				{
					content = (OplDLNAContent *)HLIB_STD_MemAlloc( sizeof(OplDLNAContent) );
					HxSTD_MemSet( content, 0, sizeof(OplDLNAContent) );
					HxSTD_StrNCpy( content->szTitle, contentItems[i].szTitle, OPL_DMP_MAX_TITLE -1 );

					if(HxSTD_StrNCmp(contentItems[i].szParentId, m_sCurrentCID, OPL_DMP_MAX_TITLE-1) == 0)
						bIsCurrentList = TRUE;

					HxSTD_StrNCpy( content->szCID, contentItems[i].szCID, OPL_DMP_MAX_CID -1 );
					HxSTD_StrNCpy( content->szClass, contentItems[i].szClass, OPL_DMP_MAX_CLASS -1 );
					HxSTD_StrNCpy( content->szObjectType, contentItems[i].szObjectType, OPL_DMP_MAX_OBJECTTYPE -1 );
					HxSTD_StrNCpy( content->szImportURI, contentItems[i].szURI, OPL_DMP_MAX_URI -1 );
					HxSTD_StrNCpy( content->szDuration, contentItems[i].szDuration, OPL_DMP_MAX_DURATION -1 );
					HxSTD_StrNCpy( content->szThumbnailURI, contentItems[i].szThumnail, OPL_DMP_MAX_URI -1 );
					HxSTD_StrNCpy( content->szGenre, contentItems[i].szGenre, OPL_DMP_MAX_GENRE -1 );
					HxSTD_StrNCpy( content->szResolution, contentItems[i].szResolution, OPL_DMP_MAX_RESOLUTION -1 );
					HxSTD_StrNCpy( content->szUDN, contentItems[i].szUDN, OPL_DMP_MAX_UDN-1 );
					HxSTD_StrNCpy( content->szParentId, contentItems[i].szParentId, OPL_DMP_MAX_TITLE-1 );

					pstMeta =  &content->stMetaData;
					HxSTD_StrNCpy( pstMeta->szAlbumTitle, contentItems[i].szAlbumTitle, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy( pstMeta->szArtist, contentItems[i].szArtist, OPL_DMP_MAX_TITLE -1 );

					/* Seek ???? ???? */
					content->bByteSeekAble = contentItems[i].bByteSeekAble;
					content->bTimeSeekAble = contentItems[i].bTimeSeekAble;

					HxLOG_Debug( "[ OPL_DMP ] szCID = %s\n", content->szCID );
					HxLOG_Debug( "[ OPL_DMP ] szClass = %s\n", content->szClass );
					HxLOG_Debug( "[ OPL_DMP ] szObjectType = %s\n", content->szObjectType );
					HxLOG_Debug( "[ OPL_DMP ] szImportURI = %s\n", content->szImportURI );
					HxLOG_Debug( "[ OPL_DMP ] szDuration = %s\n", content->szDuration );
					HxLOG_Debug( "[ OPL_DMP ] szThumbnailURI = %s\n", content->szThumbnailURI );
					HxLOG_Debug( "[ OPL_DMP ] szGenre = %s\n", content->szGenre );
					HxLOG_Debug( "[ OPL_DMP ] szResolution = %s\n", content->szResolution );
					HxLOG_Debug( "[ OPL_DMP ] szUDN = %s\n", content->szUDN );
					HxLOG_Debug( "[ OPL_DMP ] szAlbumTitle = %s\n", pstMeta->szAlbumTitle );
					HxLOG_Debug( "[ OPL_DMP ] szArtist = %s\n", pstMeta->szArtist );
					HxLOG_Debug( "[ OPL_DMP ] bByteSeekAble = %ds\n", content->bByteSeekAble );
					HxLOG_Debug( "[ OPL_DMP ] bTimeSeekAble = %ds\n", content->bTimeSeekAble );



#if defined(CONFIG_OP_JAPAN)
					//HCHAR   szObjectID[APKD_DLNA_DMP_MAX_UNKNOWN];
					usNumOfBookmark = contentItems[i].usNumOfBookmark;
					content->ulPRNum = contentItems[i].ucPRNum;

					for(int j=0; j<usNumOfBookmark; j++)
					{
						astBookmarks[j].ulPos = contentItems[i].astBookmarks[j].ulPos;
						HxSTD_StrNCpy(astBookmarks[j].szName,  contentItems[i].astBookmarks[j].szName, BOOKMARK_MAX_NAME_LEN);
					}
					content->bookmarks = opl_CDSRecording_MakeBookmarkCollection();

					for(HUINT32 j=0; j<content->ulPRNum; j++)
					{
						HxSTD_StrNCpy(content->stParentalRating[j].szName, contentItems[i].astParentalRating[j].aucIsoCountryCode, 3 );
						content->stParentalRating[j].ucParentalRating = contentItems[i].astParentalRating[j].ucParentalRating;
					}

					HxSTD_StrNCpy(content->szChannelName, contentItems[i].szChannelName, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szChannelNr, contentItems[i].szChannelNum, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szRadioBand, contentItems[i].szRadioBand, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szAnalogCopyControl, contentItems[i].szAnalogCopyControl, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szDigitalCopyControl, contentItems[i].szDigitalCopyControl, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szProfile, contentItems[i].szProfile, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szDate, contentItems[i].szDate, OPL_DMP_MAX_DATE -1 );
					content->bRestricted = contentItems[i].restricted;

					HxSTD_StrNCpy( pstMeta->szAuthor, contentItems[i].szAuthor, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy( pstMeta->szDescription, contentItems[i].szDescription, OPL_DMP_MAX_DESC -1 );
					HxSTD_StrNCpy( pstMeta->szLongDescription, contentItems[i].szLongDescription, OPL_DMP_MAX_LONGDESC -1 );
					content->ulTotalMatches = contentItems[i].ulTotalMatches;
#endif
					pstMeta = NULL;

					content->ulChildCount = contentItems[i].ulChildCount;
					contentList[i] = content;
				}

					HxLOG_Print("[ OPL_DMP ] eDLNA_DMP_ContentAdd( count - %d )\n", count );
			s_pfn_contentSearch_callback( eUpdateState, NULL, 0, count, (OplDLNAContentHandle)contentList, 0 );

				/* contentList 메모???? ?????????????????OIPF????Event???dispatch ????문제가 발생????
				 * 그래?? ????메모????????OIPF?????Event처리?????????????
				 *	if ( NULL != contentList )
				 *		HLIB_STD_MemFree(contentList);
				 */

				return;

			case eDLNA_DMP_ContentEnd :
				contentList  = NULL;
				eUpdateState = eDMP_CONTENT_END;
				HxLOG_Print("[ OPL_DMP ] eDLNA_DMP_ContentEnd  \n");
				s_pfn_contentSearch_callback( eUpdateState, NULL, 0, 0, NULL, 0 );
				return;

		case eDLNA_DMP_SearchResult :
			{
				APKS_DLNA_DMP_ContentResult_t *pReqResult = NULL;
				contentItems = (APKS_DLNA_DMP_ContentItem_t *)ulParam2;
				eUpdateState = eDMP_CONTENT_SEARCH_DONE;
				pReqResult = (APKS_DLNA_DMP_ContentResult_t *)ulParam3;
				count = pReqResult->nReturnCnt;

				if ( 0 < count )
				{
					contentList = (OplDLNAContent **)HLIB_STD_MemAlloc( sizeof(OplDLNAContent *) * count  );
				}

				for ( i = 0; i < count; i++ )
				{
					content = (OplDLNAContent *)HLIB_STD_MemAlloc( sizeof(OplDLNAContent) );
					HxSTD_MemSet( content, 0, sizeof(OplDLNAContent) );
					HxSTD_StrNCpy( content->szTitle, contentItems[i].szTitle, sizeof(content->szTitle)-1 );

					HxSTD_StrNCpy( content->szCID, contentItems[i].szCID, sizeof(content->szCID)-1 );
					HxSTD_StrNCpy( content->szClass, contentItems[i].szClass, sizeof(content->szClass)-1 );
					HxSTD_StrNCpy( content->szObjectType, contentItems[i].szObjectType, sizeof(content->szObjectType)-1 );
					HxSTD_StrNCpy( content->szImportURI, contentItems[i].szURI, sizeof(content->szImportURI) -1 );
					HxSTD_StrNCpy( content->szDuration, contentItems[i].szDuration, sizeof(content->szDuration)-1 );
					HxSTD_StrNCpy( content->szThumbnailURI, contentItems[i].szThumnail, sizeof(content->szThumbnailURI)-1 );
					HxSTD_StrNCpy( content->szGenre, contentItems[i].szGenre, sizeof(content->szGenre)-1 );
					HxSTD_StrNCpy( content->szResolution, contentItems[i].szResolution, sizeof(content->szResolution)-1 );
					HxSTD_StrNCpy( content->szUDN, pReqResult->szUDN, sizeof(content->szUDN)-1 );
					HxSTD_StrNCpy( content->szParentId, contentItems[i].szParentId, sizeof(content->szParentId)-1 );

					pstMeta =  &content->stMetaData;
					HxSTD_StrNCpy( pstMeta->szAlbumTitle, contentItems[i].szAlbumTitle, sizeof(pstMeta->szAlbumTitle)-1 );
					HxSTD_StrNCpy( pstMeta->szArtist, contentItems[i].szArtist, sizeof(pstMeta->szArtist)-1 );

					/* Seek ???? ???? */
					content->bByteSeekAble = contentItems[i].bByteSeekAble;
					content->bTimeSeekAble = contentItems[i].bTimeSeekAble;

					HxLOG_Debug( "[ OPL_DMP ] szCID = %s\n", content->szCID );
					HxLOG_Debug( "[ OPL_DMP ] szClass = %s\n", content->szClass );
					HxLOG_Debug( "[ OPL_DMP ] szObjectType = %s\n", content->szObjectType );
					HxLOG_Debug( "[ OPL_DMP ] szImportURI = %s\n", content->szImportURI );
					HxLOG_Debug( "[ OPL_DMP ] szDuration = %s\n", content->szDuration );
					HxLOG_Debug( "[ OPL_DMP ] szThumbnailURI = %s\n", content->szThumbnailURI );
					HxLOG_Debug( "[ OPL_DMP ] szGenre = %s\n", content->szGenre );
					HxLOG_Debug( "[ OPL_DMP ] szResolution = %s\n", content->szResolution );
					HxLOG_Debug( "[ OPL_DMP ] szUDN = %s\n", content->szUDN );
					HxLOG_Debug( "[ OPL_DMP ] szAlbumTitle = %s\n", pstMeta->szAlbumTitle );
					HxLOG_Debug( "[ OPL_DMP ] szArtist = %s\n", pstMeta->szArtist );
					HxLOG_Debug( "[ OPL_DMP ] bByteSeekAble = %ds\n", content->bByteSeekAble );
					HxLOG_Debug( "[ OPL_DMP ] bTimeSeekAble = %ds\n", content->bTimeSeekAble );

#if defined(CONFIG_OP_JAPAN)
					//HCHAR   szObjectID[APKD_DLNA_DMP_MAX_UNKNOWN];
					usNumOfBookmark = contentItems[i].usNumOfBookmark;
					content->ulPRNum = contentItems[i].ucPRNum;

					for(int j=0; j<usNumOfBookmark; j++)
					{
						astBookmarks[j].ulPos = contentItems[i].astBookmarks[j].ulPos;
						HxSTD_StrNCpy(astBookmarks[j].szName,  contentItems[i].astBookmarks[j].szName, BOOKMARK_MAX_NAME_LEN);
					}
					content->bookmarks = opl_CDSRecording_MakeBookmarkCollection();

					for(HUINT32 j=0; j<content->ulPRNum; j++)
					{
						HxSTD_StrNCpy(content->stParentalRating[j].szName, contentItems[i].astParentalRating[j].aucIsoCountryCode, 3 );
						content->stParentalRating[j].ucParentalRating = contentItems[i].astParentalRating[j].ucParentalRating;
					}

					HxSTD_StrNCpy(content->szChannelName, contentItems[i].szChannelName, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szChannelNr, contentItems[i].szChannelNum, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szRadioBand, contentItems[i].szRadioBand, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szAnalogCopyControl, contentItems[i].szAnalogCopyControl, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szDigitalCopyControl, contentItems[i].szDigitalCopyControl, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szProfile, contentItems[i].szProfile, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy(content->szDate, contentItems[i].szDate, OPL_DMP_MAX_DATE -1 );
					content->bRestricted = contentItems[i].restricted;

					HxSTD_StrNCpy( pstMeta->szAuthor, contentItems[i].szAuthor, OPL_DMP_MAX_TITLE -1 );
					HxSTD_StrNCpy( pstMeta->szDescription, contentItems[i].szDescription, OPL_DMP_MAX_DESC -1 );
					HxSTD_StrNCpy( pstMeta->szLongDescription, contentItems[i].szLongDescription, OPL_DMP_MAX_LONGDESC -1 );
					content->ulTotalMatches = contentItems[i].ulTotalMatches;
#endif
					pstMeta = NULL;

					content->ulChildCount = contentItems[i].ulChildCount;
					contentList[i] = content;
				}

				HxLOG_Print("[ OPL_DMP ] eDLNA_DMP_ContentSearch( count - %d )\n", count );
				s_pfn_contentSearch_callback( eUpdateState, pReqResult->szUDN, pReqResult->nReqId, count,
						(OplDLNAContentHandle)contentList, pReqResult->nTotalMatch );
			}

				return;

#if defined(CONFIG_OP_JAPAN)
			case  eDLNA_DMP_GetRecordDestination:
				{
					HINT32	i, cnt = ulParam3;
					APKS_DLNA_DMP_RecordDestination_t *dsts = NULL;

					OplDLNADestination **DestinationList = NULL;
					OplDLNADestination *item= NULL;
				if ( cnt > 0 )
				{
					DestinationList = (OplDLNADestination **)HLIB_STD_MemAlloc( sizeof(OplDLNADestination *) * cnt);
				}
					eUpdateState = eDMP_RECORD_DESTINATIONS;

					dsts = (APKS_DLNA_DMP_RecordDestination_t *)ulParam2;

					for(i=0; i<cnt; i++)
					{
						item = (OplDLNADestination *)HLIB_STD_MemAlloc( sizeof(OplDLNADestination) );
						HxSTD_StrNCpy(item->szName, dsts[i].szName, OPL_DMP_MAX_TITLE -1 );
						HxSTD_StrNCpy(item->szId, dsts[i].szId, OPL_DMP_MAX_TITLE -1 );
						DestinationList[i] = item;
					}
				s_pfn_contentSearch_callback( eUpdateState, NULL, 0, count, (OplDLNADestinationHandle)DestinationList, 0);
				}
				return;

			case eDLNA_DMP_GetDmsDiskInfo:
				{
					APKS_DLNA_DMP_Storage_t *pInfo = NULL;
					Opl_DmsStorage_t *item= NULL;
					item = (Opl_DmsStorage_t *)HLIB_STD_MemAlloc( sizeof(Opl_DmsStorage_t));

					eUpdateState = eDMP_RECORD_DISKINFO;
					pInfo = (APKS_DLNA_DMP_Storage_t *)ulParam2;

					if(pInfo)
					{
						item->Total = pInfo->Total;
						item->Available = pInfo->Available;
						item->pDstId = HLIB_STD_StrDup(m_szCurrentDestinationId);
						s_pfn_contentSearch_callback( eUpdateState, NULL, 0, 1, (void **)item, 0);
					}

				}
				return;

			case eDLNA_DMP_RemoveCmd:
				if( s_pfn_remove_callback )
				{
					HCHAR szRet[512] = {0, };
					APKS_DLNA_DMP_RemoveItemResult_t *pInfo = (APKS_DLNA_DMP_RemoveItemResult_t *)ulParam2;

					if(pInfo)
					{
						HxSTD_snprintf( szRet, sizeof(szRet), "%s&%s", pInfo->szUDN, pInfo->szCID);
						s_pfn_remove_callback(HLIB_STD_StrDup(szRet), pInfo->nRetVal);
					}
					else
						s_pfn_remove_callback(NULL, ERR_FAIL);

				}
				return;
#endif

			default :
				return;
		}
}

BPLStatus	OplDMSDevice::browsingChildItem( const char *cid, const char *sort, int nStart , int nRequest)
{
	HUINT32 hErr;

	if ( NULL == m_sCurrentDMS )
	{
		m_sCurrentDMS = new OplDMSDevice((OplDMSDevice)*this);
	}
	else if  ( HxSTD_StrCmp( m_sCurrentDMS->m_szUDN, m_szUDN) )
	{
		delete m_sCurrentDMS;
		m_sCurrentDMS = new OplDMSDevice((OplDMSDevice)*this);
	}

	if( NULL == m_sCurrentCID )
	{
		m_sCurrentCID = new char[OPL_DMP_MAX_TITLE];
		HxSTD_StrNCpy(m_sCurrentCID, cid,  OPL_DMP_MAX_TITLE -1 );
	}
	else
	{
		HxSTD_StrNCpy(m_sCurrentCID, cid,  OPL_DMP_MAX_TITLE -1 );
	}

	hErr = APK_DLNADMP_BrowsingChildItem( (const char *)m_szUDN, cid, sort, nStart, nRequest);
	if(hErr != ERR_OK)
		return BPL_STATUS_ERROR;

	return BPL_STATUS_OK;
}

HINT32	OplDMSDevice::searchingItem( const char *cid, const char *category, const char *keyword,  const char *sort, int nStart , int nRequest)
{
	HERROR  hErr;
	HINT32 nID = OplDMSDevice::m_nReqIdCnt;


	hErr = APK_DLNADMP_SearchingItem((const char *)m_szUDN, cid, category, keyword, sort, nStart, nRequest, nID+1 );
	if (hErr != ERR_OK)
		return 0;

	return (++OplDMSDevice::m_nReqIdCnt);
}

void	OplDMSDevice::registerEventListener( OPL_CONTENT_EventCallback callback )
{
	s_pfn_contentSearch_callback = callback;
}

//static
void	OplDMSDevice::unRegisterEventListener( void )
{
	s_pfn_contentSearch_callback = NULL;
}

#if defined(CONFIG_OP_JAPAN)
BPLStatus	OplDMSDevice::getRecordDestination( const char *udn)
{
	HUINT32 hErr;

	if ( NULL == m_sCurrentDMS )
	{
		m_sCurrentDMS = new OplDMSDevice((OplDMSDevice)*this);
	}
	else if  ( HxSTD_StrCmp( m_sCurrentDMS->m_szUDN, m_szUDN) )
	{
		delete m_sCurrentDMS;
		m_sCurrentDMS = new OplDMSDevice((OplDMSDevice)*this);
	}

	hErr = APK_DLNADMP_GetRecordDestinations( (const char *)udn);
	if(hErr != ERR_OK)
		return BPL_STATUS_ERROR;

	return BPL_STATUS_OK;
}

BPLStatus	OplDMSDevice::getAvailableStorageSize( const char *dst )
{
	HUINT32 hErr;

	if ( NULL == m_sCurrentDMS )
	{
		m_sCurrentDMS = new OplDMSDevice((OplDMSDevice)*this);
	}
	else if  ( HxSTD_StrCmp( m_sCurrentDMS->m_szUDN, m_szUDN) )
	{
		delete m_sCurrentDMS;
		m_sCurrentDMS = new OplDMSDevice((OplDMSDevice)*this);
	}

	if (NULL == m_szCurrentDestinationId)
	{
		m_szCurrentDestinationId = HLIB_STD_StrDup(dst);
	}
	else
	{
		HLIB_STD_MemFree(m_szCurrentDestinationId);
		m_szCurrentDestinationId = HLIB_STD_StrDup(dst);
	}

	hErr = APK_DLNADMP_GetRecordDiskInfo( (const char *)m_szUDN, dst);
	if(hErr != ERR_OK)
		return BPL_STATUS_ERROR;

	return BPL_STATUS_OK;
}
#endif
