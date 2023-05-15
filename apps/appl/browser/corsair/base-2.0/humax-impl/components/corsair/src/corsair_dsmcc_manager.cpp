/**************************************************************************************/
/**
 * @file corsair_dsmcc_manager.cpp
 *
 * Dsmcc function management
 *
 * @author  Sangsoo Yoo(ssyoo@humaxdigital.com)
 * @date    2011/12/29
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <hlib.h>
#include <unistd.h>

#include "hbbtv_api.h"
#include "gogi_opera_api.h"
#include "bpldsmcc.h"
#include "vkernel.h"
#include "bplglobal.h"

#include "corsair.h"
#include "corsair_dsmcc_manager.h"
#include "corsair_dsmcc_streameventlistenermanager.h"
#include "corsair_dsmcc_xmlstreameventlistener.h"
#include "corsair_dsmcc_objectcarouselmanager.h"
#include "corsair_dsmcc_objectcarousel_appid.h"
#include "corsair_dsmcc_uri.h"

using namespace std;

CorsairDsmccManager *CorsairDsmccManager::s_instance = NULL;

CorsairDsmccManager* CorsairDsmccManager::getInstance()
{
    if ( s_instance == NULL )
    {
        s_instance = new CorsairDsmccManager();
    }
    return s_instance;
}

CorsairDsmccManager::CorsairDsmccManager( )
    :m_timer_id(0)
    ,m_mtx(PTHREAD_MUTEX_INITIALIZER)
    ,m_carousel_handle(NULL)
    ,m_carousel_count(0)
    ,m_session_id(HANDLE_NULL)
    ,m_active(1)
{
	LOG_DEBUG(" (+)\n" );

	pthread_mutex_init(&m_mtx, NULL);
	BPL_Dsmcc_Init();

	LOG_DEBUG(" (-)\n" );
}

CorsairDsmccManager::~CorsairDsmccManager( )
{
	LOG_DEBUG(" (+)\n" );

	m_root_path.clear();
    m_eventlistener_manager.clear();

    if ( s_instance == this )
    {
        s_instance = NULL;
    }

	BPL_Dsmcc_Uninit();

    pthread_mutex_destroy(&m_mtx);

	LOG_DEBUG(" (-)\n" );
}
	
int CorsairDsmccManager::startApplication( unsigned long org_id, unsigned short app_id )
{
    m_organisation_id = org_id;
    m_application_id = app_id;
    return 0;
}

int CorsairDsmccManager::stopApplication( unsigned long org_id, unsigned short app_id )
{
	LOG_DEBUG(" (+) org_id 0x%x app_id 0x%x \n", org_id, app_id);

    if( deleteObjectCarousel( org_id, app_id ) == -1 )
    {
        return -1;
    }
    LOG_DEBUG(" (-)\n");
    return 0;

}

int CorsairDsmccManager::stopApplication( const char *url )
{
	LOG_DEBUG(" (+) url %s\n", url);

    string app_url = url;
     // we have to clear the dsmcc application if the url is start with "dvb" protocol
    if ( app_url.compare(0, 6, "dvb://") == 0 )
	{
	    CorsairDsmccUri carousel_uri = url;
	    unsigned int carousel_id;

	    LOG_DEBUG(" (-) clear carousel & event listener\n");
	    if ( getCarouselId( carousel_uri.getComponentTag(), &carousel_id ) )
    	{
    		LOG_DEBUG(" (-) There is not matched CarouselId\n" );
    		return 1;
    	}

    	deleteObjectCarousel( carousel_uri.getOriginalNetworkId(), carousel_uri.getTransportId(), carousel_uri.getServiceId(), carousel_id );
	}

	LOG_DEBUG(" (-)\n");
	return 0;
}

int CorsairDsmccManager::onCarouselTableUpdated( void )
{
	LOG_DEBUG(" (+)\n");

	getCarouselInfo();
	restartDownloadAll();

	return 0;
}

int CorsairDsmccManager::setRootPath( const char *szRootPath )
{
	LOG_DEBUG(" (+)\n" );

	m_root_path = szRootPath;

	LOG_DEBUG(" (-)\n" );
	return 0;
}

int CorsairDsmccManager::onCarouselDownloaded( unsigned int uiCarouselId, bool success )
{
	LOG_DEBUG(" (+)\n" );

	CorsairDsmccObjectCarouselManager::onCarouselDownloaded( uiCarouselId, success );

	LOG_DEBUG(" (-)\n" );
	return 0;
}

int CorsairDsmccManager::onStreamEventRecieved( BplDsmccStreamEvent_t *pstEvent )
{
	LOG_DEBUG(" (+)\n" );
    CorsairDsmccObjectCarousel      *object_carousel = NULL;
    CorsairDsmccStreamEventMessage	*stream_event_message = NULL;

	if ( !pstEvent )
	{
		LOG_DEBUG(" (-) Parameter is not available\n" );
		return -1;
	}

    object_carousel = getObjectCarouselHasEventId(pstEvent->nEventId);
    if ( object_carousel == NULL )
    {
        LOG_DEBUG(" (-) There is not carousel have this event Id 0x%x \n", pstEvent->nEventId );
        goto NOTIFY_ERROR;
    }

    stream_event_message = object_carousel->findEventMessageMatchingEventId(pstEvent->nEventId);
    if ( stream_event_message == NULL )
    {
        LOG_DEBUG(" (-) There is not stream_event_message\n" );
        goto NOTIFY_ERROR;
    }

	m_eventlistener_manager.onStreamEventRecieved( stream_event_message->getUrl().c_str(),
	                                               pstEvent->nEventId,
	                                               stream_event_message->getEventName(pstEvent->nEventId),
	                                               (unsigned char*)pstEvent->pucObjInfoByte,
	                                               pstEvent->ulObjInfoByteLen);
	return 0;

NOTIFY_ERROR:
#if 0
    const char *unknown = "unknown";
	m_eventlistener_manager.onStreamEventRecieved( unknown,
	                                               pstEvent->nEventId,
	                                               unknown,
	                                               (unsigned char*)pstEvent->pucObjInfoByte,
	                                               pstEvent->ulObjInfoByteLen );
#endif
	return 0;
}

int CorsairDsmccManager::onStreamEventMessageRecieved( BplDsmccStreamEvent_t *pstEvent )
{
	LOG_DEBUG(" (+)\n" );

	if ( !pstEvent )
	{
		LOG_DEBUG(" (-) Parameter is not available\n" );
		return -1;
	}

    CorsairDsmccObjectCarouselManager::onStreamEventMessageRecieved( pstEvent->ulPid, pstEvent->szUrl, pstEvent->nEventId, pstEvent->szEventName );

	return 0;
}

void CorsairDsmccManager::informStreamErrorToBrowser(const char* eventname, void* listener_id, void* listener, void* videoId)
{

	HBBTV_EventData *hbbtv_event = NULL;


	hbbtv_event = (HBBTV_EventData *)malloc(sizeof(HBBTV_EventData));
	if ( !hbbtv_event )
	{
		LOG_DEBUG(" (-) Failed to get allocation1\n" );
		return;
	}

	memset(hbbtv_event, 0, sizeof(HBBTV_EventData));

	if( eventname )
	{
		hbbtv_event->dsmcc.name = (char *)malloc( strlen( (char *)eventname ) + 1 );
		snprintf( hbbtv_event->dsmcc.name, strlen( (char *)eventname ) + 1, "%s", eventname );
	}

	hbbtv_event->dsmcc.data = NULL;
	hbbtv_event->dsmcc.text = NULL;
	hbbtv_event->dsmcc.status = (char *)malloc( strlen( (char *)"error" ) + 1 );
	snprintf( hbbtv_event->dsmcc.status, strlen( (char *)"error" ) + 1, "error" );

	hbbtv_event->dsmcc.identifier;
	hbbtv_event->dsmcc.listener_id = listener_id;
	hbbtv_event->dsmcc.event_listener = listener;
	hbbtv_event->dsmcc.video_broadcast_handle = videoId;

	opera_hbbtv_send_event(E_DSMCC_STREAM_EVENT, hbbtv_event);

	LOG_DEBUG(" (-) ok\n" );
}

int CorsairDsmccManager::onCarouselVersionUpdated( unsigned int uiCarouselId )
{
	LOG_DEBUG(" (+)\n" );

	if ( BPL_Dsmcc_Restart( uiCarouselId ) != BPL_STATUS_OK )
	{
		LOG_DEBUG(" (-) failed to restart\n" );
		return -1;
	}
	LOG_DEBUG(" (-)\n" );
	return 0;
}

void CorsairDsmccManager::callbackSystemEvent( eBplDsmccDownloadEvent eEvent, void *data )
{
	CorsairDsmccManager *DsmccMgrHandle = CorsairDsmccManager::getInstance();
	unsigned int		uiCarouselId;

	LOG_DEBUG(" (+) Event : %d\n", eEvent );

	if ( !DsmccMgrHandle->getActivation() )
	{
		LOG_DEBUG(" (-) Dsmcc Manager is not ready\n" );
		return;
	}

	switch( eEvent )
	{
		case BPLDSMCC_EVENT_SESSIONID_UPDATED:
			LOG_DEBUG("BPLDSMCC_EVENT_SESSIONID_UPDATED\n" );
			DsmccMgrHandle->setSessionId( ( unsigned int )data );
			break;

		case BPLDSMCC_EVENT_SESSIONDATA_UPDATED: /** you get the BplDsmccObjectCarouselList_t data with this event */
			{
				BplDsmccObjectCarouselList_t *pstList = (BplDsmccObjectCarouselList_t *) data;
				LOG_DEBUG("BPLDSMCC_EVENT_SESSIONDATA_UPDATED\n" );
				DsmccMgrHandle->onCarouselTableUpdated();
				DsmccMgrHandle->retryUnhandledFileRequest();
				DsmccMgrHandle->retryUnhandledStreamEventListener();
			}
			break;

		case BPLDSMCC_EVENT_DOWNLOADED: /** you get the PID value with this event */
			LOG_DEBUG("BPLDSMCC_EVENT_DOWNLOADED\n" );
			uiCarouselId = (unsigned int ) data;
			DsmccMgrHandle->onCarouselDownloaded( uiCarouselId, true );
			break;

		case BPLDSMCC_EVENT_DOWNLOAD_FAIL: /** you get the PID value with this event */
			LOG_DEBUG("BPLDSMCC_EVENT_DOWNLOAD_FAIL\n" );
			uiCarouselId = (unsigned int ) data;
			DsmccMgrHandle->onCarouselDownloaded( uiCarouselId, false );
			break;

		case BPLDSMCC_EVENT_VERSION_CHANGED: /** you get the PID value with this event */
			LOG_DEBUG("BPLDSMCC_EVENT_VERSION_CHANGED\n" );
			uiCarouselId = (unsigned int ) data;
			DsmccMgrHandle->onCarouselVersionUpdated( uiCarouselId );
			break;

		case BPLDSMCC_EVENT_STREAM_EVENT_MESSAGE_UPDATED: /** you get the id of stream event with this event */
		    LOG_DEBUG("BPLDSMCC_EVENT_STREAM_EVENT_MESSAGE_UPDATED\n" );
			{
				BplDsmccStreamEvent_t	*pstStreamEvent = (BplDsmccStreamEvent_t *)data;
				if (pstStreamEvent)
				{
					DsmccMgrHandle->onStreamEventMessageRecieved( pstStreamEvent );
				}
			}
			break;

		case BPLDSMCC_EVENT_STREAM_EVENT_DESCRIPTOR_UPDATED: /** you get the id of stream event with this event */
		    LOG_DEBUG("BPLDSMCC_EVENT_STREAM_EVENT_DESCRIPTOR_UPDATED++\n" );
			{
				BplDsmccStreamEvent_t	*pstStreamEvent = (BplDsmccStreamEvent_t *)data;
				if (pstStreamEvent)
				{
					DsmccMgrHandle->onStreamEventRecieved( pstStreamEvent );
				}
			}
			break;

		default:
			break;
	}
	LOG_DEBUG(" (-)\n" );
	return;
}

int CorsairDsmccManager::getCarouselInfo( void )
{
	BplDsmccObjectCarousel_t 	*pCarouselHandle = NULL;
	unsigned int				nCarouselCount = 0;

	LOG_DEBUG(" (+)\n" );

	if (m_carousel_handle)
	{
		LOG_DEBUG("Old Free Carousel Info\n" );
		BPL_Dsmcc_FreeCarouselList(m_carousel_handle, m_carousel_count);
		m_carousel_handle = NULL;
		m_carousel_count = 0;
	}

	if ( BPL_Dsmcc_GetCarouselList( &pCarouselHandle, &nCarouselCount ) != BPL_STATUS_OK )
	{
		LOG_DEBUG(" (-) Failed to get New Carousel Info\n" );
		return -1;
	}

#if 1
	{
		int nCount, nIdx;

		if (pCarouselHandle)
		{
			LOG_DEBUG("\t ********************************************************************************\n" );
			for (nCount=0; nCount<nCarouselCount; nCount++)
			{
				LOG_DEBUG("\t [%d]: CarouselId [0x%04X]\n", nCount , pCarouselHandle[nCount].ulCarouselId );
				LOG_DEBUG("\t --------------------------------------------------------------------------------\n" );
				for (nIdx=0; nIdx<pCarouselHandle[nCount].uiEsCount; nIdx++)
				{
					LOG_DEBUG("\t\t ES[%d]: CompTag [0x%02X], StreamType [0x%02X]\n", nIdx, pCarouselHandle[nCount].Es[nIdx].ucComponentTag, pCarouselHandle[nCount].Es[nIdx].ulStreamType );
				}
				LOG_DEBUG("\t ********************************************************************************\n" );
			}
		}
	}
#endif

	if ( nCarouselCount )
	{
		setCarouselHandle( pCarouselHandle );
		setCarouselCount( nCarouselCount );
	}

	LOG_DEBUG(" (-)\n" );

	return 0;
}

int CorsairDsmccManager::getComponentTag(const char *url, unsigned char *component_tag )
{
	int i = 0;
	char szTemp[32], *pszCompTag;
	const char *p = url;

	LOG_DEBUG(" (+)\n" );

	memset( szTemp, 0x00, sizeof( szTemp ) );
	szTemp[0] = '0';
	szTemp[1] = 'x';
	pszCompTag = &szTemp[2];

	while ( *p != '\0' )
	{
		if ( *p == '.' ) i++;
		if ( i >= 3 ) break;
		p++;
	}
	p++;

	i = 0;

	while ( *p != '\0' && i<(sizeof(szTemp)-2) )
	{
		if ( *p == '/' ) break;
		pszCompTag[i++] = *p;
		p++;
	}

	*component_tag = (unsigned char )strtol(szTemp, NULL, 16);

	LOG_DEBUG(" (-) 0x%02x \n", *component_tag );
	return 0;
}

int CorsairDsmccManager::getCarouselId( unsigned char component_tag, unsigned int *carousel_id )
{
	int i, j;

	LOG_DEBUG(" (+)\n" );

	if (NULL == m_carousel_handle)
	{
		LOG_DEBUG(" (-) There is not Carousel Information\n" );
		return -1;
	}

	for( i = 0; i < m_carousel_count ; i++ )
	{
		for( j = 0; j < m_carousel_handle[i].uiEsCount ; j++ )
		{
			if ( m_carousel_handle[i].Es[j].ucComponentTag == component_tag )
			{
				*carousel_id = m_carousel_handle[i].ulCarouselId;
				LOG_DEBUG(" (-) OCID 0x%08X\n", *carousel_id );
				return 0;
			}
		}
	}
	*carousel_id = 0;

	LOG_DEBUG(" (-)\n" );

	return -1;
}

int CorsairDsmccManager::setCarouselHandle( BplDsmccObjectCarousel_t *pCarouselHandle )
{
	LOG_DEBUG(" (+)\n" );

	m_carousel_handle = pCarouselHandle;

	LOG_DEBUG(" (-)\n" );

	return 0;
}

int CorsairDsmccManager::setCarouselCount( int nCarouselCount )
{
	LOG_DEBUG(" (+) %d\n", nCarouselCount );

	m_carousel_count = nCarouselCount;

	LOG_DEBUG(" (-)\n" );

	return 0;
}

int CorsairDsmccManager::setSessionId( unsigned int session_id )
{
	LOG_DEBUG(" (+)\n" );

	if ( m_session_id != session_id )
	{
		m_session_id = session_id;

		BPL_Dsmcc_Close( );
        
		BPL_Dsmcc_Open( m_session_id );
	}

	LOG_DEBUG(" (-)\n" );

	return 0;
}

unsigned int CorsairDsmccManager::getSessionId( void )
{
	LOG_DEBUG(" (+)\n" );
	LOG_DEBUG(" (-)\n" );
	return m_session_id;
}


int	CorsairDsmccManager::getLocalPathOfCarousel( const char *url, string &localpath )
{
	CorsairDsmccUri carousel_url(url);
	unsigned int	carousel_id = 0;
	char			szTmpString[256];

	LOG_DEBUG(" (+)\n" );

	if ( getCarouselId( carousel_url.getComponentTag(), &carousel_id ) )
	{
		LOG_DEBUG(" (-) failed to get carousel id\n" );
		return -1;
	}
#if defined(DSMCC_MANAGING_WITH_APPID)
    snprintf(szTmpString, sizeof(szTmpString), "%s/%x.%x.%x", m_root_path.c_str(), m_organisation_id, m_application_id, carousel_id );
#else
    snprintf(szTmpString, sizeof(szTmpString), "%s/%x.%x.%x.%x", m_root_path.c_str(), carousel_url.getOriginalNetworkId(), carousel_url.getTransportId(), carousel_url.getServiceId(), carousel_id );
#endif

    localpath.clear();
    localpath = szTmpString;

	LOG_DEBUG(" (-)\n" );

	return 0;
}

int CorsairDsmccManager::addFileRequest( const char* uri, void* identifier)
{
    bool                                flag_free = true, flag_trigger_download = false;
	unsigned int 						carousel_id;
	CorsairDsmccObjectCarousel 			*object_carousel = NULL;
	CorsairDsmccFileRequest             *file_request = NULL;
	CorsairDsmccUri                     carousel_uri;
	std::string							dsmcc_local_file, localpath;
	    
	if ( uri == NULL || identifier == NULL )
	{
	    goto RETURN_ERROR;
	}
	
    carousel_uri = uri;
    
    if ( carousel_uri.getComponentTag() == 0 )
	{
		LOG_DEBUG(" (-) Failed to get component_tag\n" );
		goto RETURN_ERROR;
	}

    file_request = new CorsairDsmccFileRequest(uri, identifier);
    if ( file_request == NULL )
    {
        LOG_DEBUG(" (-) Failed to create CorsairDsmccFileRequest\n" );
		goto RETURN_ERROR;
    }

    if ( getCarouselId( carousel_uri.getComponentTag(), &carousel_id ) )
	{
		LOG_DEBUG(" (-) There is not matched CarouselId\n" );
		goto STORE_THIS_REQUEST;
	}
	
#if defined(DSMCC_MANAGING_WITH_APPID)
	object_carousel = findObjectCarousel( m_organisation_id, m_application_id, carousel_id );
#else
    object_carousel = findObjectCarousel( carousel_uri.getOriginalNetworkId(), carousel_uri.getTransportId(), carousel_uri.getServiceId(), carousel_id );
#endif

	if ( !object_carousel )
	{
		LOG_DEBUG(" (-) There is not the OC, Now Start new ObjectCarousel\n" );
		if ( getLocalPathOfCarousel( uri, localpath ) )
    	{
    		LOG_DEBUG(" (-) failed to get local path\n" );
		    goto RETURN_ERROR;
    	}
#if defined(DSMCC_MANAGING_WITH_APPID)
		object_carousel = new CorsairDsmccObjectCarouselAppId( m_organisation_id, m_application_id, carousel_id, localpath.c_str() );
#else
        object_carousel = new CorsairDsmccObjectCarousel( carousel_uri.getOriginalNetworkId(), carousel_uri.getTransportId(), carousel_uri.getServiceId(), carousel_id, localpath.c_str() );
#endif
		addObjectCarousel(object_carousel);
		flag_trigger_download = true;
	}

    if ( object_carousel->findFileMatchingIdentifier( file_request->getIdentifier()) == NULL )
    {
        if ( getLocalPathOfCarousel( file_request->getUri().c_str(), localpath ) )
    	{
    		LOG_DEBUG(" (-) failed to get local path\n" );
		    goto RETURN_ERROR;
    	}
    	localpath.append(carousel_uri.getAbsolutePath());
        file_request->setLocalPath(localpath.c_str());
        object_carousel->addFile( file_request );
        flag_free = false;
    }

    if ( flag_trigger_download == true && object_carousel->startDownload() )
	{
	    LOG_DEBUG(" (-) failed to trigger downloading carousel\n" );
	    goto RETURN_ERROR;
	}

	if ( object_carousel->downloaded() )
	{
	    if ( file_request->checkFileExist() )
    	{
    		LOG_DEBUG(" (-) The File is Ready\n" );
    		goto RETURN_OK;
    	}
    	else
    	{
	        LOG_DEBUG(" (-) The File is not Ready, but download is completed\n" );
    		goto RETURN_ERROR;
		}
	}
	
RETURN_WAIT:
    if ( flag_free && file_request != NULL )
        delete file_request;
    LOG_DEBUG(" (-) wait\n" );
    return 1;

STORE_THIS_REQUEST:
    if ( flag_free && file_request != NULL )
        delete file_request;    
    LOG_DEBUG(" (-) store this file request\n" );
	return 2;

RETURN_ERROR:
    if ( flag_free && file_request != NULL )
        delete file_request;
	LOG_DEBUG(" (-) error\n" );
	return -1;

RETURN_OK:
    if ( flag_free && file_request != NULL )
        delete file_request;
    LOG_DEBUG(" (-) ok\n" );
    return 0;
}
	
int CorsairDsmccManager::storeUnhandledFileRequest( const char* uri, void* identifier)
{
    LOG_DEBUG(" (+) file : %s(id:%p)\n", uri, identifier );
    lock();

    /* store file request to cover the case which carousel update is later than ait update */
    /* we assume that the file identifier is unique. */
    CorsairDsmccFileRequest *file_request = new CorsairDsmccFileRequest( uri, identifier );
    if ( file_request != NULL )
        m_unhandled_filerequests.push_back(file_request);
    
    unsigned long timer_id = 0;
    if ( VK_TIMER_EventAfter( DSMCC_FILEREQUEST_TIMEOUT, callbackUnhandledFileRequestTimer, NULL, 0, &timer_id) != VK_OK )
    {
        LOG_DEBUG(" (-) ERROR to Trigger Timer\n" );
    }

    unlock();
    LOG_DEBUG(" (-)\n" );

    return 0;
}

int CorsairDsmccManager::retryUnhandledFileRequest( )
{
    LOG_DEBUG(" (+)\n");
    lock();

    if ( m_unhandled_filerequests.empty() == false )
    {
        std::vector<CorsairDsmccFileRequest *>::iterator it;
        for( it=m_unhandled_filerequests.begin(); it != m_unhandled_filerequests.end(); it++ )
        {
            addFileRequest((*it)->getUri().c_str(), (*it)->getIdentifier());
            delete (*it);
        }
        m_unhandled_filerequests.clear();
    }

    unlock();
    LOG_DEBUG(" (-)\n" );

    return 0;
}

int CorsairDsmccManager::onTimeoutUnhandledFileRequest()
{
    LOG_DEBUG(" (+)\n");
    lock();

    if ( m_unhandled_filerequests.empty() == false )
    {
        std::vector<CorsairDsmccFileRequest *>::iterator it;
        for( it=m_unhandled_filerequests.begin(); it != m_unhandled_filerequests.end(); it++ )
        {
            (*it)->onFileDownloaded();
            delete (*it);
        }
        m_unhandled_filerequests.clear();
    }

    unlock();
	
    LOG_DEBUG(" (-)\n" );

    return 0;
}

void CorsairDsmccManager::callbackUnhandledFileRequestTimer( unsigned long timer_id, void *param)
{
    LOG_DEBUG(" (+)\n" );
    CorsairDsmccManager *dsmcc_manager = CorsairDsmccManager::getInstance();//static_cast<CorsairDsmccManager*>(param);
    dsmcc_manager->onTimeoutUnhandledFileRequest();
    LOG_DEBUG(" (-)\n" );
}

int CorsairDsmccManager::subscribeStreamEventListener(const char* target_url, const char* eventname, void* listener_id, void* listener, void* videoId)
{
	LOG_DEBUG(" (+)\n" );

	lock();

    int ret = -1;
	CorsairDsmccStreamEventListener *new_listener = NULL;
	
	if ( !m_active )
	{
	    LOG_DEBUG(" (-) DSMCC manager did not enable\n" );
		goto RETURN;
	}
    
	ret = addStreamEventListener( target_url, eventname, listener_id, listener, videoId );
    
RETURN:
    unlock();
	if ( ret == 1 )
	{
		LOG_DEBUG(" (-) store\n" );
    	storeUnhandledStreamEventListener( target_url, eventname, listener_id, listener, videoId );
	}
    LOG_DEBUG(" (-) ret %d\n", ret );
	return ret;
}

int CorsairDsmccManager::subscribeXmlStreamEventListener(const char* target_url, const char* eventname, unsigned short event_id,
													unsigned short service_id, unsigned short component_tag,
								  					void* listener_id, void* listener, void* videoId)
{
	LOG_DEBUG(" (+)\n" );

	lock();

    int ret = -1;
	CorsairDsmccStreamEventListener *new_listener = NULL;
	
	if ( !m_active )
	{
	    LOG_DEBUG(" (-) DSMCC manager did not enable\n" );
		goto RETURN;
	}
    
	ret = addXmlStreamEventListener( target_url, eventname, event_id, service_id, component_tag, listener_id, listener, videoId );
	
RETURN:
    unlock();
    if ( ret == 1 )
	{
		LOG_DEBUG(" (-) store\n" );
    	storeUnhandledXmlStreamEventListener( target_url, eventname, event_id, service_id, component_tag, listener_id, listener, videoId );
	}
    
    LOG_DEBUG(" (-) ret %d\n", ret );
	return ret;
}

int CorsairDsmccManager::storeUnhandledStreamEventListener(const char* target_url, const char* eventname, void* listener_id, void* listener, void* videoId)
{
    LOG_DEBUG(" (+) target url : %s\n", target_url );
    lock();

    /* store file request to cover the case which carousel update is later than ait update */
    /* we assume that the file identifier is unique. */
    CorsairDsmccStreamEventListener *new_listener = new CorsairDsmccStreamEventListener( (const char *)target_url, eventname, listener_id, listener, videoId );
    if ( new_listener != NULL )
        m_unhandled_streamevent_listeners.push_back(new_listener);

    unlock();
    LOG_DEBUG(" (-)\n" );

    return 0;
}

int CorsairDsmccManager::storeUnhandledXmlStreamEventListener(const char* target_url, const char* eventname, unsigned short event_id,
													unsigned short service_id, unsigned short component_tag,
								  					void* listener_id, void* listener, void* videoId)
{
    LOG_DEBUG(" (+) target url : %s\n", target_url );
    lock();

    /* store file request to cover the case which carousel update is later than ait update */
    /* we assume that the file identifier is unique. */
    CorsairDsmccStreamEventListener *new_listener = \
            new CorsairDsmccXMLStreamEventListener( (const char *)target_url, eventname, event_id, service_id, component_tag, listener_id, listener, videoId );
    if ( new_listener != NULL )
        m_unhandled_streamevent_listeners.push_back(new_listener);

    unlock();
    LOG_DEBUG(" (-)\n" );

    return 0;
}

int CorsairDsmccManager::retryUnhandledStreamEventListener( )
{
    LOG_DEBUG(" (+)\n");
    lock();

    if ( m_unhandled_streamevent_listeners.empty() == false )
    {
        std::vector<CorsairDsmccStreamEventListener *>::iterator it;
        for( it=m_unhandled_streamevent_listeners.begin(); it != m_unhandled_streamevent_listeners.end(); it++ )
        {
            switch( (*it)->getListenerType() )
            {
                case CorsairDsmccStreamEventListener::StreamEventListener:
                    {
                        CorsairDsmccStreamEventListener *stream_event_listener = \
                                static_cast<CorsairDsmccStreamEventListener *>((*it));
                        addStreamEventListener(stream_event_listener->getTargetUrl().c_str()
                                               ,stream_event_listener->getEventName().c_str()
                                               ,stream_event_listener->getListenerId()
                                               ,stream_event_listener->getListener()
                                               ,stream_event_listener->getVideoId());
                    }
                    break;
            
                case CorsairDsmccStreamEventListener::XMLStreamEventListener:
                    {
                        CorsairDsmccXMLStreamEventListener *xml_stream_event_listener = \
                                static_cast<CorsairDsmccXMLStreamEventListener *>((*it));
                        addXmlStreamEventListener( xml_stream_event_listener->getTargetUrl().c_str()
                                                  ,xml_stream_event_listener->getEventName().c_str()
                                                  ,xml_stream_event_listener->getEventId()
                                                  ,xml_stream_event_listener->getServiceId()
                                                  ,xml_stream_event_listener->getComponentTag()
                                                  ,xml_stream_event_listener->getListenerId()
                                                  ,xml_stream_event_listener->getListener()
                                                  ,xml_stream_event_listener->getVideoId());
                    }
                    break;
            }
            delete (*it);
        }
        m_unhandled_streamevent_listeners.clear();
    }
    
    unlock();
    LOG_DEBUG(" (-)\n" );

    return 0;
}

int CorsairDsmccManager::onTimeoutUnhandledStreamEventListener()
{
    LOG_DEBUG(" (+)\n");
    lock();

    if ( m_unhandled_filerequests.empty() == false )
    {
        std::vector<CorsairDsmccStreamEventListener *>::iterator it;
        for( it=m_unhandled_streamevent_listeners.begin(); it != m_unhandled_streamevent_listeners.end(); it++ )
        {
            delete (*it);
        }
        m_unhandled_streamevent_listeners.clear();
    }

    unlock();
    LOG_DEBUG(" (-)\n" );

    return 0;
}

/*************************************************************************************************************
	 functions are called by opera core directly
*************************************************************************************************************/

int CorsairDsmccManager::start( void )
{
	char	path[256]={0,};

	LOG_DEBUG(" (+)\n" );

    lock();

#if defined(CONFIG_OP_JCOM)
	setActivation(false);
#endif

	if ( !m_active )
	{
	    unlock();
		return -1;
	}
    
	BPL_Dsmcc_SetListener( CorsairDsmccManager::callbackSystemEvent );

	//memset( path, 0x00, sizeof( path ) );
	if ( BPL_Dsmcc_GetCachePath( &path[0], sizeof( path ) ) != BPL_STATUS_OK )
	{
		setRootPath( "/tmp/cache/" );
	}
	else
	{
		setRootPath( (const char *)path );
	}

	LOG_DEBUG("Caching Path : %s\n", getRootPath().c_str() );

    unlock();

	LOG_DEBUG(" (-)\n");

	return 0;
}

int CorsairDsmccManager::stop( void )
{
	LOG_DEBUG(" (+)\n");

    lock();

	if ( !m_active )
	{
	    unlock();
		return -1;
	}

	BPL_Dsmcc_SetListener( NULL );

    unlock();

	LOG_DEBUG(" (-)\n");

	return 0;
}

int CorsairDsmccManager::getFile( const char* uri, void* identifier)
{
	LOG_DEBUG(" (+) %s(%08x)\n", uri, identifier );
	
	int ret = -1;
    
    lock();
    
	if ( !m_active )
	{
	    LOG_DEBUG(" (-) DSMCC manager did not enable\n" );
	    ret = -1;
		goto RETURN;
	}

    ret = addFileRequest( uri, identifier );
    
RETURN:
    unlock();
    
    if ( ret == 2 )
        storeUnhandledFileRequest( uri, identifier );
        
	LOG_DEBUG(" (-) ret %d\n", ret );
	return ret;

};

int CorsairDsmccManager::getMimeType( void* identifier, char** mime_type)
{
	CorsairDsmccObjectCarousel	*pObjectCarousel;
	CorsairDsmccFileRequest		*pFile;

	LOG_DEBUG(" (+)\n" );

	if ( !m_active )
	{
		return -1;
	}

	pObjectCarousel = getObjectCarouselHasFile( identifier );
	if ( !pObjectCarousel )
	{
		LOG_DEBUG(" (-) failed to get object carousel has 0x%08x\n", identifier );
		return -1;
	}

	pFile = pObjectCarousel->findFileMatchingIdentifier( identifier );
	if ( pFile )
	{
		if( pFile->getMimeType().size() == 0 )
		{
			*mime_type = NULL;
		}
		else
		{
			/**
			 *@note  Set to NULL if mime type is unknown, causing Opera to try determining it. \n
			 * in case of "application/vnd.hbbtv.xhtml+xml", HTML5 apps over DSMCC doesn't works in opera. \n
			 * ralated link : https://bugs.opera.com/browse/KAYO-1374
			 */ 
			if ( strcmp(pFile->getMimeType().c_str(),"application/vnd.hbbtv.xhtml+xml") == 0 )
			{
				*mime_type = NULL;
			}
			else
			{
				*mime_type = strdup(pFile->getMimeType().c_str());
			}
		}
		LOG_DEBUG(" (-) Got MimeType\n" );
		return 0;
	}

	LOG_DEBUG(" (-) Failed to get MimeType\n" );
	return -1;
}

int CorsairDsmccManager::readFile( void* identifier, void** data, int* length)
{
	CorsairDsmccObjectCarousel				*pObjectCarousel;
	CorsairDsmccFileRequest						*pFile;

	LOG_DEBUG(" (+)\n" );

	if ( !m_active )
	{
		return -1;
	}

	pObjectCarousel = getObjectCarouselHasFile( identifier );
	if ( !pObjectCarousel )
	{
		LOG_DEBUG(" (-) There is not the OC has this file(0x%08x)\n", identifier );
		return -1;
	}

	pFile = pObjectCarousel->findFileMatchingIdentifier( identifier );
	if ( pFile )
	{
		if( pFile->read( data, length ) )
		{
			LOG_DEBUG(" (-) Failed to read\n" );
			return -1;
		}
		else
		{
			LOG_DEBUG(" (-) Success to read (%d)\n", *length );
			return 0;
		}
	}

	LOG_DEBUG(" (-) There is not this file\n" );
	return -1;
}

int CorsairDsmccManager::closeFile( void* identifier )
{
	CorsairDsmccObjectCarousel  *pObjectCarousel;
	CorsairDsmccFileRequest	    *pFile;

	LOG_DEBUG(" (+)\n" );

    lock();

	if ( !m_active )
	{
	    LOG_DEBUG(" (-) DSMCC manager did not enable\n" );
		goto RETURN_ERROR;
	}

	pObjectCarousel = getObjectCarouselHasFile( identifier );
	if ( !pObjectCarousel )
	{
		LOG_DEBUG(" (-) There is not the OC has this file(0x%08x)\n", identifier );
		goto RETURN_ERROR;
	}

	pFile = pObjectCarousel->findFileMatchingIdentifier( identifier );
	if ( !pFile )
	{
		LOG_DEBUG(" (-) There is not this file\n" );
		goto RETURN_ERROR;
	}

	pObjectCarousel->deleteFile(identifier);
	LOG_DEBUG(" (-) Success to close, deleteFile\n" );
	unlock();
	return 0;

RETURN_ERROR:
	LOG_DEBUG(" (-) error\n" );
    unlock();
	return -1;
}

int CorsairDsmccManager::clearCache( void )
{
	LOG_DEBUG(" (+)\n" );

    if ( !m_active )
	{
		return -1;
	}

	if (m_carousel_handle)
	{
		LOG_DEBUG(" Free Carousel Info\n" );
		BPL_Dsmcc_FreeCarouselList(m_carousel_handle, m_carousel_count);
		m_carousel_handle = NULL;
		m_carousel_count = 0;
	}

    onTimeoutUnhandledFileRequest();
    onTimeoutUnhandledStreamEventListener();
	CorsairDsmccObjectCarouselManager::clear();
	m_eventlistener_manager.clear();

	LOG_DEBUG(" (-)\n" );

	return 0;
}

int CorsairDsmccManager::addStreamEventListener(const char* target_url, const char* eventname, void* listener_id, void* listener, void* videoId)
{
	CorsairDsmccStreamEventListener *pListener = NULL;
    CorsairDsmccUri                 carousel_uri(target_url);
    CorsairDsmccObjectCarousel      *object_carousel;
    string                          localpath;
    unsigned int                    carousel_id = 0;

	LOG_DEBUG(" (+)\n" );

	if ( !m_active )
	{
	    LOG_DEBUG(" (-) DSMCC manager did not enable\n" );
		goto RETURN_ERROR;
	}

	pListener = m_eventlistener_manager.findListenerMatchingId( listener_id );
	if ( !pListener )
	{
		pListener = new CorsairDsmccStreamEventListener( (const char *)target_url, eventname, listener_id, listener, videoId );
		if ( !pListener )
        {
			LOG_DEBUG(" (-) Failed to get new Listner\n" );
			goto RETURN_ERROR;
		}
    }
    
	if ( carousel_uri.getComponentTag() == 0 )
	{
	    LOG_DEBUG(" (-) Maybe the target Url is malformed.\n" );
	    informStreamErrorToBrowser(eventname, listener_id, listener, videoId);
	    goto RETURN_ERROR;
	}

    if ( getCarouselId( carousel_uri.getComponentTag(), &carousel_id ) )
	{
		LOG_DEBUG(" (-) There is not matched CarouselId\n" );
		goto STORE_STREAMEVENTLISTENER;
	}
	else
	{
	    m_eventlistener_manager.addListener( pListener );
		LOG_DEBUG("\n\nAdd New Listener Url[%s], eventname [%s], listener_id [0x%08x], listener [0x%08x], videoId [0x%08x]\n\n", target_url, eventname, listener_id, listener, videoId );
    }
#if defined(DSMCC_MANAGING_WITH_APPID)
	object_carousel = findObjectCarousel( m_organisation_id, m_application_id, carousel_id );
#else
	object_carousel = findObjectCarousel( carousel_uri.getOriginalNetworkId(), carousel_uri.getTransportId(), carousel_uri.getServiceId(), carousel_id );
#endif
	if ( !object_carousel )
	{
		LOG_DEBUG(" (-) There is not the OC, Now Start new ObjectCarousel\n" );
		if ( getLocalPathOfCarousel( target_url, localpath ) )
    	{
    		LOG_DEBUG(" (-) failed to get local path\n" );
		    goto RETURN_ERROR;
    	}
#if defined(DSMCC_MANAGING_WITH_APPID)
		object_carousel = new CorsairDsmccObjectCarouselAppId( m_organisation_id, m_application_id, carousel_id, localpath.c_str() );
#else
        object_carousel = new CorsairDsmccObjectCarousel( carousel_uri.getOriginalNetworkId(), carousel_uri.getTransportId(), carousel_uri.getServiceId(), carousel_id, localpath.c_str() );
#endif
		addObjectCarousel(object_carousel);
		if ( object_carousel->startDownload() )
		{
		    LOG_DEBUG(" (-) failed to download carousel\n" );
		    goto RETURN_ERROR;
		}
	}
    BPL_Dsmcc_RequestStreamEventStart();

	LOG_DEBUG(" (-) ok\n" );
	return 0;

STORE_STREAMEVENTLISTENER:
    if ( pListener != NULL )
        delete pListener;
	LOG_DEBUG(" (-) store\n" );
	return 1;

RETURN_ERROR:
    if ( pListener != NULL )
        delete pListener;
	LOG_DEBUG(" (-) error\n" );
	return -1;
}

int CorsairDsmccManager::addXmlStreamEventListener(const char* target_url, const char* eventname, unsigned short event_id,
													unsigned short service_id, unsigned short component_tag,
								  					void* listener_id, void* listener, void* videoId)
{
	CorsairDsmccStreamEventListener *pListener = NULL;
    CorsairDsmccUri                 carousel_uri(target_url);
    CorsairDsmccObjectCarousel      *object_carousel;
    string                          localpath;
    unsigned int                    carousel_id = 0;
    BplDsmccService_t	            service;

	LOG_DEBUG(" (+)\n" );

	if ( !m_active )
	{
	    LOG_DEBUG(" (-) DSMCC manager did not enable\n" );
		goto RETURN_ERROR;
	}

	pListener = m_eventlistener_manager.findListenerMatchingId( listener_id );
	if ( !pListener )
	{
		pListener = new CorsairDsmccXMLStreamEventListener( target_url, eventname, event_id, service_id, component_tag, listener_id, listener, videoId );
		if ( pListener == NULL )
		{
			LOG_DEBUG(" (-) Failed to get new Listener\n" );
			goto RETURN_ERROR;
		}
	}

    if ( getCarouselId( component_tag, &carousel_id ) )
	{
		LOG_DEBUG(" (-) There is not matched CarouselId\n" );
		goto STORE_STREAMEVENTLISTENER;
	}
	else
	{
	    m_eventlistener_manager.addListener( pListener );
		LOG_DEBUG("\n\nAdd New Listener Url[%s], eventname [%s], listener_id [0x%08x], listener [0x%08x], videoId [0x%08x]\n\n", target_url, eventname, listener_id, listener, videoId );
    }

    if ( BPL_Dsmcc_GetServiceInfo(&service) != BPL_STATUS_OK )
	{
	    goto RETURN_ERROR;
	}
#if defined(DSMCC_MANAGING_WITH_APPID)
	object_carousel = findObjectCarousel( m_organisation_id, m_application_id, carousel_id );
#else
	object_carousel = findObjectCarousel( service.usOnID, service.usTSID, service_id, carousel_id );
#endif
	if ( !object_carousel )
	{
	    LOG_DEBUG(" (-) There is not the OC, Now Start new ObjectCarousel\n" );

        char dvb_url[64] = {};
	    snprintf(dvb_url, sizeof(dvb_url), "%s/%x.%x.%x.%x/", m_root_path.c_str(), service.usOnID, service.usTSID, service_id, carousel_id );
		localpath.clear();
		localpath = dvb_url;
#if defined(DSMCC_MANAGING_WITH_APPID)
		object_carousel = new CorsairDsmccObjectCarouselAppId( m_organisation_id, m_application_id, carousel_id, localpath.c_str() );
#else
		object_carousel = new CorsairDsmccObjectCarousel( service.usOnID, service.usTSID, service_id, carousel_id, localpath.c_str() );
#endif
		addObjectCarousel(object_carousel);
		if ( object_carousel->startDownload() )
		{
		    LOG_DEBUG(" (-) failed to download carousel\n" );
		    goto RETURN_ERROR;
		}
	}

	BPL_Dsmcc_RequestStreamEventStart();
	LOG_DEBUG(" (-) ok\n" );
	return 0;

STORE_STREAMEVENTLISTENER:
    if ( pListener != NULL )
        delete pListener;
	LOG_DEBUG(" (-) store\n" );
	return 1;

RETURN_ERROR:
    if ( pListener != NULL )
        delete pListener;
	LOG_DEBUG(" (-) error\n" );
	return -1;
}

int CorsairDsmccManager::deleteStreamEventListener( const char* target_url, const char* event_name, void* listener_id )
{
	LOG_DEBUG(" (+) target_url %s event_name %s listener_id %p\n", target_url, event_name, listener_id );

    lock();

	if ( !m_active )
	{
	    unlock();
		return -1;
	}

	if ( m_eventlistener_manager.deleteListener( target_url, event_name, listener_id ) )
	{
	    LOG_DEBUG(" (-) There is not matched listener\n" );
	    unlock();
	    return 1;
	}

    unlock();
	LOG_DEBUG(" (-)\n" );

	return 0;
}

int CorsairDsmccManager::deleteStreamEventListenerAll( void* videoId )
{
	LOG_DEBUG(" (+)\n" );

    lock();

	if ( !m_active )
	{
	    unlock();
		return -1;
	}

	m_eventlistener_manager.deleteAllListener( videoId );

    //clear();

    unlock();

	LOG_DEBUG(" (-)\n" );

	return 0;
}

int CorsairDsmccManager::isSameCarousel(const char* url1, const char* url2, int* match)
{
	LOG_DEBUG(" (+)\n" );
	unsigned int carousel_id_of_url1 = 0, carousel_id_of_url2 = 0;
	unsigned char component_tag_of_url1 = 0,component_tag_of_url2 = 0;

	if ( getComponentTag( url1, &component_tag_of_url1 ) )
	{
		LOG_DEBUG(" (-) Failed to get component_tag\n" );
		return -1;
	}

	if ( getCarouselId( component_tag_of_url1, &carousel_id_of_url1 ) )
	{
		LOG_DEBUG(" (-) Failed to get CarouselId\n" );
		return -1;
	}

	if ( getComponentTag( url2, &component_tag_of_url2 ) )
	{
		LOG_DEBUG(" (-) Failed to get component_tag\n" );
		return -1;
	}

	if ( getCarouselId( component_tag_of_url2, &carousel_id_of_url2 ) )
	{
		LOG_DEBUG(" (-) Failed to get CarouselId\n" );
		return -1;
	}

	if ( carousel_id_of_url1 != carousel_id_of_url2 )
	{
		LOG_DEBUG(" (-) These are not matched\n" );
		*match = 0;
	}
	else
	{
		LOG_DEBUG(" (-) These are matched\n" );
		*match = 1;
	}

	LOG_DEBUG(" (-)\n" );
	return true;
};
