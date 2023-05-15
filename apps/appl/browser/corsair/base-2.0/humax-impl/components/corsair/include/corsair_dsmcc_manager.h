/** **********************************************************************************************************
	@file 		corsair_dsmcc_manager.h
	
	@date		2012/09/25
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif		
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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
*********************************************************************************************************** */

#ifndef __CORSAIR_DSMCCMANAGER_H__
#define __CORSAIR_DSMCCMANAGER_H__

#include <string>
#include <map>

#include "bpldsmcc.h"

#include "corsair_config.h"
#include "corsair_dsmcc_objectcarouselmanager.h"
#include "corsair_dsmcc_streameventlistenermanager.h"

#define DSMCC_FILEREQUEST_TIMEOUT 5000// 3sec

/** **********************************************************************************************************
	@class	CorsairDsmccManager
 	@brief 	this class has the information of each downloaded file element from broadcast.
*********************************************************************************************************** */

class CorsairDsmccManager : public CorsairDsmccObjectCarouselManager{
public:
	/**
		@fn 	getInstance
		@breif 	
	*/
    static CorsairDsmccManager* getInstance();

	/**
		@fn 	CorsairDsmccManager
		@breif 	
	*/    
	virtual ~CorsairDsmccManager();

	/**
		@fn 	setRootPath
		@breif 	set the default path of dsmcc cache
		@param	root_path	[INPUT] the path of filesystem to refer
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int setRootPath( const char *root_path );
	
	/**
		@fn 	getRootPath
		@breif 	get the default path of dsmcc cache
		@param	N/A
		@return reference of root path
	*/
	std::string& getRootPath( void ) { return m_root_path; }
	
	/**
		@fn 	getSizeOfObjectCarouselListOfPMT
		@breif 	get the number of ObjectCarouselList at current channel
		@param	N/A
		@return number of ObjectCarouselList at current channel
	*/
	int getSizeOfObjectCarouselListOfPMT( void );

	/**
		@fn 	getObjectCarouselOfPMT
		@breif 	get the number of ObjectCarouselList at current channel
		@param	index	[INPUT]	the index of the ObjectCarousel Array.
		@return the pointer of the ObjectCarousel 
	*/
	BplDsmccObjectCarousel_t* getObjectCarouselOfPMT( int index );
	
	/**
		@fn 	setSessionId
		@breif 	set the session id of current channel
		@param	session_id	[INPUT]	the session value of current channel
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	setSessionId( unsigned int session_id );
	
	/**
		@fn 	getSessionId
		@breif 	get the session id of current channel
		@param	N/A
		@return current Session Id
	*/
	unsigned int getSessionId( void );
	
	/**
		@fn 	onCarouselDownloaded
		@breif 	inform that the file is ready to load for opera
		@param	dsmcc_manager_hadnel	[INPUT] the handle of dsmcc manager
		@param	carousel_id	[INPUT] the path of filesystem to refer
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int onCarouselDownloaded( unsigned int, bool );
	
	/**
		@fn 	onStreamEventRecieved
		@breif 	inform that the file has an error to load for opera
		@param	dsmcc_manager_hadnel	[INPUT] the handle of dsmcc manager
		@param	event			[INPUT] the path of filesystem to refer
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int onStreamEventRecieved( BplDsmccStreamEvent_t *event );

	/**
		@fn 	addStreamEventMessage
		@breif 	inform that the file has an error to load for opera
		@param	dsmcc_manager_hadnel	[INPUT] the handle of dsmcc manager
		@param	event			[INPUT] the path of filesystem to refer
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int onStreamEventMessageRecieved( BplDsmccStreamEvent_t *pstEvent );
	
	/**
		@fn 	onCarouselVersionUpdated
		@breif 	restart the Object Carousel has uiCarouselId 
		@param	dsmcc_manager_hadnel	[INPUT] the handle of dsmcc manager
		@param	carousel_id	[INPUT] the path of filesystem to refer
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	onCarouselVersionUpdated( unsigned int carousel_id );
		
	/**
		@fn 	getCarouselInfo
		@breif 	get the carsoursel information of current channel
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int getCarouselInfo( void );
	
	/**
		@fn 	getComponentTag
		@breif 	get the carsoursel information of current channel
		@param	url			[INPUT]		the Url to get Component tag value
		@param	component_tag	[OUTPUT]	the pointer to store Component Tag
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int getComponentTag( const char *url, unsigned char *component_tag );
	
	/**
		@fn 	getCarouselId
		@breif 	get the carsoursel id from component tag
		@param	component_tag	[INPUT]		the Component to get CarouselId value
		@param	carousel_id	[OUTPUT]	the pointer to store CarouselId
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	getCarouselId( unsigned char component_tag, unsigned int *carousel_id );

	/**
		@fn 	setCarouselHandle
		@breif 	set the Carousel handle
		@param	carousel_handle	[INPUT]	the handle of Carousel to set
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int setCarouselHandle( BplDsmccObjectCarousel_t *carousel_handle );
	
	/**
		@fn 	setCarouselCount
		@breif 	set the value how many carousel is in the current channel
		@param	carousel_count	[INPUT]	the number of carousel in the current channel
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int setCarouselCount( int carousel_count );
	
	/*************************************************************************************************************
	 functions is direct to corsair and bpl
	*************************************************************************************************************/
	/**
		@fn 	startApplication
		@breif 	start the url by dsmcc module
		@param	url	[INPUT]	the url to run by dsmcc
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int startApplication( unsigned long org_id, unsigned short app_id );
	
	/**
		@fn 	stopApplication
		@breif 	stop all application
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int stopApplication( unsigned long org_id, unsigned short app_id );
	int stopApplication( const char *url );
	
	/**
		@fn 	onCarouselTableUpdated
		@breif 	notify the information of carousel list
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int onCarouselTableUpdated(void);
	
	/**
		@fn 	callbackUnhandledFileRequestTimer
		@breif 	this function will be called after timeout of VK_TIMER_After
		@param	N/A
		@return 1	- Actived
		@return 0	- Deactived
	*/
	static void callbackUnhandledFileRequestTimer( unsigned long , void *);
		
	/**
		@fn 	callbackSystemEvent
		@breif 	this function is for the callback from platform to process dsmcc event
		@param	event	[INPUT] the event type
		@param	data	[INPUT] the data of event
		@return N/A
	*/
	static void	callbackSystemEvent( eBplDsmccDownloadEvent event, void *data );

	/*************************************************************************************************************
	 functions are called by opera core directly
	*************************************************************************************************************/
	/**
		@fn 	start
		@breif 	ready to start dsmcc module
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int start( void );
	
	/**
		@fn 	stop
		@breif 	free all resources of dsmcc module
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int stop( void );
	
	/**
		@fn 	getFile
		@breif 	get the file has filename and file id.
		@param	uri	[INPUT] the location of resource
		@param	identifier	[INPUT] the data of event
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int getFile( const char* uri, void* identifier);
	
	/**
		@fn 	getMimeType
		@breif 	get the mime type of indicated file
		@param	identifier		[INPUT] the Id of the file
		@param	mimetype	[OUTPUT] the pointer to get mime type string
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int getMimeType( void* identifier, char** mimetype);
	
	/**
		@fn 	readFile
		@breif 	read the data of indicated file
		@param	identifier		[INPUT] the Id of the file
		@param	data			[OUTPUT] the pointer to get the data of the file
		@param	length		[OUTPUT] the pointer to get length of the file data
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int readFile( void* identifier, void** data, int* length);
	
	/**
		@fn 	closeFile
		@breif 	close the file , and release the file handle
		@param	identifier		[INPUT] the Id of the file
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int closeFile( void* identifier);
	
	/**
		@fn 	clearCache
		@breif 	clear all data of this manager
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int clearCache ( void );

	/**
		@fn 	subscribeStreamEventListener
		@breif 	add the stream event on this manager
		@param	target_url	[INPUT]	the url of the page that it use this event listener.
		@param	event_name	[INPUT]	the event name to use on this listener
		@param	listener_id	[INPUT]	the Id of this Listener
		@param	listener	[INPUT]	the handle of this Listener
		@param	video_id	[INPUT]	the VideoId of this Listener
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int subscribeStreamEventListener(const char* target_url, const char* event_name, void* listener_id, void* listener, void* video_id);
	
	/**
		@fn 	subscribeXmlStreamEventListener
		@breif 	add the stream event on this manager
		@param	event_name		[INPUT]	the event name to use on this listener
		@param	event_id		[INPUT]	the event id to use on this listener
		@param	service_id		[INPUT]	the id of current service
		@param	component_tag	[INPUT]	the component tag value that this listen can accept
		@param	listener_id		[INPUT]	the Id of this Listener
		@param	pListener		[INPUT]	the handle of this Listener
		@param	video_id		[INPUT]	the VideoId of this Listener
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	subscribeXmlStreamEventListener(const char* target_url, const char* event_name, unsigned short event_id,
								   		  unsigned short service_id, unsigned short component_tag,
								   		  void* listener_id, void* listener, void* video_id);

	/**
		@fn 	deleteStreamEventListener
		@breif 	delete the indicated Streamevent listener
		@param	event_name		[INPUT]	the event name to use on this listener
		@param	listener_id		[INPUT]	the Id of this Listener
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/					   
	int	deleteStreamEventListener( const char* target_url, const char* event_name, void* listener_id );
	
	/**
		@fn 	deleteStreamEventListenerAll
		@breif 	delete all Streamevent listener has the video Id
		@param	video_id		[INPUT]	the Id of Current Video
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/				
	int	deleteStreamEventListenerAll( void* video_id);
	
	/**
		@fn 	isSameCarousel
		@breif 	compare two url is matched or not
		@param	url1		[INPUT]	the first Url to compare
		@param	url2		[INPUT]	the second Url to compare
		@param	match		[OUTPUT] the return value of compared result
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/	
	int	isSameCarousel(const char* url1, const char* url2, int* match);
		
	/**
		@fn 	setActivation
		@breif 	get the session id of current channel
		@param	active [INPUT] set the active status of dsmcc manager
		@return N/A
	*/
	void setActivation(bool active ) { m_active = active; }
private:
	/**
		@fn 	CorsairDsmccManager
		@breif 	
	*/
    CorsairDsmccManager();
    
	/**
		@fn 	getLocalPathOfCarousel
		@breif 	get the session id of current channel
		@param	input		[INPUT]		the pointer has caching path
		@param  output	[OUTPUT]	the pointer to store changed string
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int getLocalPathOfCarousel( const char *url, string &localpath );

	/**
		@fn 	getActivation
		@breif 	get the active status of dsmcc manager
		@param	N/A
		@return 1	- Actived
		@return 0	- Deactived
	*/
	bool getActivation(void) { return m_active; }
	
	/**
		@fn 	addFileRequest
		@breif 	this function will store unhandled file request to m_unhandled_files;
		@param	N/A
		@return 1	- Actived
		@return 0	- Deactived
	*/
	int addFileRequest( const char* uri, void* identifier);
	
	/**
		@fn 	storeFileRequest
		@breif 	this function will store unhandled file request to m_unhandled_files;
		@param	N/A
		@return 1	- Actived
		@return 0	- Deactived
	*/
	int storeUnhandledFileRequest( const char* uri, void* identifier);
	
	/**
		@fn 	retryFileRequest
		@breif 	this function to request again when the platform is ready.
		@param	N/A
		@return 1	- Actived
		@return 0	- Deactived
	*/
	int retryUnhandledFileRequest( );
	
	/**
		@fn 	onTimeoutFileRequest
		@breif 	to notify file requests have timeout
		@param	N/A
		@return 1	- Actived
		@return 0	- Deactived
	*/
	int onTimeoutUnhandledFileRequest();

    /**
		@fn 	addStreamEventListener
		@breif 	add the stream event on this manager
		@param	target_url	[INPUT]	the url of the page that it use this event listener.
		@param	event_name	[INPUT]	the event name to use on this listener
		@param	listener_id	[INPUT]	the Id of this Listener
		@param	listener	[INPUT]	the handle of this Listener
		@param	video_id	[INPUT]	the VideoId of this Listener
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int addStreamEventListener(const char* target_url, const char* event_name, void* listener_id, void* listener, void* video_id);;

	/**
		@fn 	addXmlStreamEventListener
		@breif 	add the stream event on this manager
		@param	event_name		[INPUT]	the event name to use on this listener
		@param	event_id		[INPUT]	the event id to use on this listener
		@param	service_id		[INPUT]	the id of current service
		@param	component_tag	[INPUT]	the component tag value that this listen can accept
		@param	listener_id		[INPUT]	the Id of this Listener
		@param	pListener		[INPUT]	the handle of this Listener
		@param	video_id		[INPUT]	the VideoId of this Listener
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int	addXmlStreamEventListener(const char* target_url, const char* event_name, unsigned short event_id,
								   		  unsigned short service_id, unsigned short component_tag,
								   		  void* listener_id, void* listener, void* video_id);
									   		  
    /**
		@fn 	storeUnhandledStreamEventListener
		@breif 	add the stream event on this manager
		@param	target_url	[INPUT]	the url of the page that it use this event listener.
		@param	event_name	[INPUT]	the event name to use on this listener
		@param	listener_id	[INPUT]	the Id of this Listener
		@param	listener	[INPUT]	the handle of this Listener
		@param	video_id	[INPUT]	the VideoId of this Listener
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int storeUnhandledStreamEventListener(const char* target_url, const char* event_name, void* listener_id, void* listener, void* video_id);;

	/**
		@fn 	storeUnhandledXmlStreamEventListener
		@breif 	add the stream event on this manager
		@param	target_url	[INPUT]	the url of the page that it use this event listener.
		@param	event_name	[INPUT]	the event name to use on this listener
		@param	listener_id	[INPUT]	the Id of this Listener
		@param	listener	[INPUT]	the handle of this Listener
		@param	video_id	[INPUT]	the VideoId of this Listener
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int storeUnhandledXmlStreamEventListener( const char* target_url, const char* event_name, unsigned short event_id,
								   		  unsigned short service_id, unsigned short component_tag,
								   		  void* listener_id, void* listener, void* video_id);
	
	/**
		@fn 	retryUnhandledStreamEventListener
		@breif 	this function to request handling of stream event listener again when the platform is ready.
		@param	N/A
		@return 1	- Actived
		@return 0	- Deactived
	*/
	int retryUnhandledStreamEventListener( );
	
	/**
		@fn 	onTimeoutUnhandledStreamEventListener
		@breif 	to notify StreamEventListener have timeout
		@param	N/A
		@return 1	- Actived
		@return 0	- Deactived
	*/
	int onTimeoutUnhandledStreamEventListener();
	
	/**
		@fn 	lock
		@breif 	nlock the mutex to prevent unexpected access to the context data
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	void lock() { pthread_mutex_lock(&m_mtx);
   	}
	
	/**
		@fn 	unlock
		@breif 	unlock the mutex to prevent unexpected access to the context data
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	void unlock() { pthread_mutex_unlock(&m_mtx);
   	}
	
	/**
		@fn 	setTimerId
		@breif 	to store the parameter as internal timer handle.
		@param	timer_id	[INPUT] timer handle
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	int setTimerId( unsigned long timer_id ){m_timer_id = timer_id;}
	
	/**
		@fn 	getTimerId
		@breif 	return internal timer id to measure the filerequest timeout
		@param	N/A
		@return the value of timer id;
	*/
	unsigned long getTimerId(){return m_timer_id;}
	
	/**
		@fn 	informStreamErrorToBrowser
		@breif 	inform that the file has an error to load for opera
		@param	dsmcc_manager_hadnel	[INPUT] the handle of dsmcc manager
		@param	event			[INPUT] the path of filesystem to refer
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	void informStreamErrorToBrowser(const char* eventname, void* listener_id, void* listener, void* videoId);

private:
    static CorsairDsmccManager* s_instance;
	
	/* members to handle which file requests failed to handle due to carousel table information.
	m_timer_id and m_mtx is added to cover the Ligada Testsuite TC.
	in the case of oirg.Hbbtv_00000250, it requires that we return DSMCC_ERROR if there is not carousel id information.
	but in the other case like org.hbbtv_DSMCC046, this TC requires that we have to request again file requests. */
	std::vector<CorsairDsmccFileRequest*> m_unhandled_filerequests;
	std::vector<CorsairDsmccStreamEventListener*> m_unhandled_streamevent_listeners;
	unsigned long m_timer_id;
	pthread_mutex_t m_mtx;	

	CorsairDsmccStreamEventListenerManager m_eventlistener_manager;
	std::string m_root_path;

	BplDsmccObjectCarousel_t *m_carousel_handle;
	unsigned int m_carousel_count;
	unsigned int m_session_id;
	int	m_active;
	
	/* store the current application's id */
	unsigned long m_organisation_id;
	unsigned short m_application_id;
};

#endif	// __CORSAIR_DSMCCMANAGER_H__

