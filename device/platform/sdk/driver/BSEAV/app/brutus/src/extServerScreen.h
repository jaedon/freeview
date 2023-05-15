/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: extServerScreen.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 4/24/07 7:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/extServerScreen.h $
 * 
 * 2   4/24/07 7:01p piyushg
 * PR28588: Add DRM-NDR blue skin support
 *
 **************************************************************************/

#include "genericscreen.h"
#include <mstringlist.h>
#include <mlistbox.h>
#include <mstringlist.h>
#include <mlabel.h>
#include <mtimer.h>
#include "video.h"
#include "control.h"

#include "upnp.h"
#include <stdio.h>
#include "bstd.h"
#include <pthread.h>
#include "upnptools.h"

#include "bdrmnd_upnp_api.h"
#include "bkni.h"
#include "bdbg.h"

//extern unsigned char g_decode_nd_key[BDRMND_KEY_SIZE];

typedef struct bdrmnd_transfer_pipe {
	bplaypump_t playpump;
	int fd, flags;
	bool eof;
} bdrmnd_transfer_pipe;

/*************/

#define MAX_DEVICES 10
#define MAX_ITEM 100
class ExtServerScreen;
struct Container;
struct Cookie;
struct MediaServerList;
struct MediaServer;

typedef struct Item
{
	char *objectID;
	char *title;
	char *resource;
	struct Container *parent;
} Item;

typedef struct Container
{
	struct Container *parent;
	char *objectID;
	char *title;
	Item item[MAX_ITEM];
	struct Container *container;
	int itemCount;
	int containerCount;
} Container;

typedef struct Cookie
{
	struct MediaServerList *serverList;
    UpnpClient_Handle clientHandle;
	ExtServerScreen *servScreen;
} Cookie;



typedef struct MediaServer 
{
	Cookie *cookie;
	Container *contents;
	char *UDN;
	char *friendlyName;
	char *contentDirectoryEventURL;
	char *contentDirectoryControlURL;
	int subscriptionTimeOut;
	Upnp_SID subscriptionID;
} MediaServer;

typedef struct MediaServerList
{
	Cookie *cookie;
	MediaServer *list;
	const char *location;
	int expire;
	IXML_Document* descriptionDoc;
} MediaServerList;
	
extern "C" int callback( Upnp_EventType eventType, void* event, void* pCookie );
extern "C" int bdrmnd_http_transfer_callback (unsigned char* data, unsigned long len, void* context);
extern "C" int bdrmnd_stage_callback (bdrmnd_license_stage stage, bdrmnd_license_status status, void *context);

class ExtServerScreen : public GenericScreen, public MListBoxEventHandler,
	public MTimerEventHandler {
public:
	ExtServerScreen(Control *control, MRect rectTVWin = SMALLTV);


	
	// events
	void onClick(MButton *self);
	void show();
	bool focus();
	void searchAddServer();
	void addToList(MediaServer *server, int index);
	void onItemClicked(MListBox *self);
	void ParseDeviceDescription( IXML_Document* doc, const char* location, int expires, Cookie* cookie );
	const char* xml_GetChildElementValue( IXML_Element* parent, const char* tagName );
	IXML_Document* browseAction( const char* pObjectID, const char* pBrowseFlag, const char* pFilter,
                       const char* pStartingIndex, const char* pRequestedCount, const char* pSortCriteria, MediaServer *server );
	bool FetchContents( MediaServer *server );
	IXML_Document* ParseBrowseResult( IXML_Document* doc );
	char *convert_xml_special_chars( const char *psz_content );
	bool addItem(Container *parent, Item *item);
	bool addContainer(Container *parent, Container *container);
	bool addServer( MediaServer* s , IXML_Document* doc, const char* location, int expires, Cookie* cookie );
	MediaServer* getServer( const char* UDN );
	bool _FetchContents(Container *parent, MediaServer *server);

protected:

	MLabel *l1, *_diskFree, *l2;
	MStringList _dirs, _labels;
	int _lastFileNumber;
	MTimer _timer;

	/**
	* listbox
	**/
	MColumnListBox *listbox;

};

