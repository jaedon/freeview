/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: extServerScreen.cpp $
 * $brcm_Revision: 6 $
 * $brcm_Date: 4/9/10 1:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/extServerScreen.cpp $
 * 
 * 6   4/9/10 1:55p katrep
 * SW7405-4188:decode must be stopped before new decode is started
 * 
 * 5   7/15/08 2:16p katrep
 * PR43636: Fixed compilar warnings,fixed segmentation fault due to
 * playpump handle corruption,added debug messages
 * 
 * 4   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 * 
 * 3   12/6/07 12:52p erickson
 * PR37967: rename functions for consistency
 * 
 * 2   4/24/07 7:00p piyushg
 * PR28588: Add DRM-NDR blue skin support
 * 
 * 1   3/9/07 2:12p piyushg
 * PR28588: Add DRM-NDR support in brutus
 *
 **************************************************************************/




#include "extServerScreen.h"
#include "videoscreen.h"
#include "playengine.h"
#include "control.h"
#include "playengine.h"
#ifndef _WIN32_WCE
    #include <sys/stat.h>
    #include <dirent.h>
#endif
#include <mmessagebox.h>
#include "cfgsettings.h"
#include "bstd.h"


BDBG_MODULE(extServerScreen);


#define BIG_BUFSIZE 256
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
const char* MEDIA_SERVER_DEVICE = "urn:schemas-upnp-org:device:MediaServer:1";
const char* CONTENT_DIRECTORY_SERVICE = "urn:schemas-upnp-org:service:ContentDirectory:1";
MediaServerList _serverList[MAX_DEVICES];
Item ItemList[1000];
unsigned char _g_overflow_buff[BDRMND_OBUFF_SIZE];
extern unsigned char g_decode_nd_key[BDRMND_KEY_SIZE];

bdrmnd_stream_context nd_stream;
bdrmnd_session_context nd_session;
bdrmnd_transfer_pipe nd_pipe;

Video *vid;
PlayEngine *playEngine;
bstream_mpeg mpeg;
bplaypump_t playpump;
bstream_t stream;
bdisplay_t display=NULL;
bdecode_t decode=NULL;
bdecode_window_t window=NULL;
static int itemCount = 0;
static int serverCount = 0;
int currentServerIndex = 0;

void b_print_xml(IXML_Node *xml);

static int frsttime=0;

class _LibraryListBox : public MColumnListBox
{
public:
    _LibraryListBox(MWidget *parent, Config *cfg, const MRect &rect, int scrollBarWidth, const char *name = NULL) :
    MColumnListBox(parent, rect, scrollBarWidth, name)
    {
        _cfg = cfg;
    }
protected:
    class _LibraryListItem : public MColumnListItem
    {
    public:
        _LibraryListItem(_LibraryListBox *listbox, const char *name = NULL) :
        MColumnListItem(listbox, name)
        {
            _cfg = listbox->_cfg;
        }
    protected:
        void drawColumn(
                       MPainter &ptr, int col, const char *text, int textlen, const MRect &rect,
                       MPainter::Alignment al, MPainter::VAlignment val);
        Config *_cfg;
    };
    MListItem *createItem();
    Config *_cfg;
};

void _LibraryListBox::_LibraryListItem::drawColumn(
                                                  MPainter &ptr, int col, const char *text, int textlen, const MRect &rect,
                                                  MPainter::Alignment al, MPainter::VAlignment val)
{
    MColumnListItem::drawColumn(ptr, col, text, textlen, rect, al, val);
}

MListBox::MListItem *_LibraryListBox::createItem()
{
    return new _LibraryListItem(this, "libraryitem");
}

ExtServerScreen::ExtServerScreen(Control *acontrol, MRect rectTVWin) :
GenericScreen(acontrol, rectTVWin),//SMALLTV),
_timer(acontrol->fb()->app())
{
    MRect                rectListBox(17, 200, 600, 200);
    uint32_t             nBevelListBox        = 2;
    uint32_t             nRows                = 5;
    MPainter::BevelStyle styleBevel           = MPainter::bsRaised;
    int32_t              nListTextColor       = -1;
    int32_t              nListTextColorFocus  = -1;
    MString              listFocusImage;
    MString              listScrollbarImage;
    MString              listScrollIndImage;
    uint32_t             listDropShadow       = 0;

    _timer.addHandler(this);
    _timer.start(1000, MTimer::eForever); // update the UI for recording files

    _lastFileNumber = 0;

    Skin * pSkin = control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_SERVER))
    {

        pSkin->createWidgets(XML_SCREEN_SERVER, this);
        //return; //do not return here...we must continue creating required widgets

        //get list box attributes from skin
        MString         listBevelStyle; 
        MString         listColumns; 
        MString         listTextColor;
        MString         listTextColorFocus;
        uint32_t        listX               = 0;
        uint32_t        listY               = 0;
        uint32_t        listWidth           = 0;
        uint32_t        listHeight          = 0;
        uint32_t        listBevelWidth      = 0;
        uint32_t        listRows            = 0;

        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_GEOM_LIST_X,            listX);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_GEOM_LIST_Y,            listY);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_GEOM_LIST_H,            listHeight);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_GEOM_LIST_W,            listWidth);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_BEVEL_LIST_WIDTH,       listBevelWidth);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_BEVEL_LIST_STYLE,       listBevelStyle);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_LIST_COLUMNS,           listColumns);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_LIST_VISIBLE_ROWS,      listRows);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_LIST_TEXT_COLOR,        listTextColor);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_LIST_TEXT_COLOR_FOCUS,  listTextColorFocus);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_LIST_FOCUS_IMAGE,       listFocusImage);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_LIST_SCROLLBAR_IMAGE,   listScrollbarImage);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_LIST_SCROLLIND_IMAGE,   listScrollIndImage);
        pSkin->getAttrValue(XML_SCREEN_SERVER, 
                            XML_ATT_LIST_DROP_SHADOW,       listDropShadow);

        rectListBox.setX(listX);
        rectListBox.setY(listY);

        if (listHeight > 0)
        {
            rectListBox.setHeight(listHeight);
        }

        if (listWidth > 0)
        {
            rectListBox.setWidth(listWidth);
        }

        nBevelListBox = listBevelWidth;

        if (listBevelStyle == XML_BEVEL_RAISED)
        {
            styleBevel = MPainter::bsRaised;
        }
        else
            if (listBevelStyle == XML_BEVEL_SUNKEN)
        {
            styleBevel = MPainter::bsSunken;
        }
        else
            if (listBevelStyle == XML_BEVEL_UPDOWN)
        {
            styleBevel = MPainter::bsUpDown;
        }

        if (listRows > 0)
        {
            nRows = listRows;
        }

        //set text color if necessary
        if (!listTextColor.isEmpty())
        {
            nListTextColor = pSkin->getColorValue(listTextColor.s());
        }

        //set text color when focused if necessary
        if (!listTextColorFocus.isEmpty())
        {
            nListTextColorFocus = pSkin->getColorValue(listTextColorFocus.s());
        }

        //TODO: handle listColumns


        _diskFree = NULL;
    }
    else
    {
        l1 = new MLabel(this, MRect(52-38,25,350,40), "ServerList", "serverlabel");
        l1->setFont(MFont(app(), control()->cfg()->get(Config::SCREEN_TITLE_FONT)));

        new MLabel(this, MRect(27,180,100,20), "Name");
    }

    //required widgets - these widgets must always exist for this screen so 
    //we create them here.

    MLabel *bevel = new MLabel(this, rectListBox);
    bevel->setBevel(nBevelListBox, styleBevel);
    listbox = new _LibraryListBox(bevel, cfg(), MRect(2,2,bevel->width()-4,bevel->height()-4), 15, "server_listbox");
    listbox->setTextColor(nListTextColor);
    listbox->setTextColorFocus(nListTextColorFocus);
    listbox->scrollBar()->setButtons(false);
    listbox->scrollBar()->setIndicator(true);
    listbox->setTotalVisible(nRows);
    listbox->captureKeys(true, true);
    listbox->setIndent(10);

    listbox->setFocusImage(pSkin->getImage(listFocusImage.s()));
    listbox->scrollBar()->setImage(pSkin->getImage(listScrollbarImage.s()));
    listbox->scrollBar()->setIndicatorImage(pSkin->getImage(listScrollIndImage.s()));
    listbox->setDropShadow(listDropShadow);

    listbox->addColumn(500);    // name

    listbox->addHandler(this);
    bevel->raise();

}


void ExtServerScreen::addToList(MediaServer *server, int index)
{

    char buf[BIG_BUFSIZE];
    BKNI_Snprintf(buf, BIG_BUFSIZE, "%s", (const char *)server->friendlyName);

    listbox->insert(index, buf);
}

void ExtServerScreen::show()
{
#ifndef B_HAS_DRM_ND
    l1->setText("Serverlist");
#endif
    MMessageBox mbox(fb(), 250, 200);
    mbox.set("Searching for servers. Please wait.", 0);
    mbox.show();
    mbox.drawNow();

    UpnpFinish();
    serverCount = 0;
    itemCount = 0;
    frsttime = 0;
    if (playpump)
    {
        control()->checkinPlayEngine(playEngine);
        bplaypump_stop(playpump);
    }

    decode = control()->getDecode();
    if (decode)
        bdecode_stop(decode);

    searchAddServer();
    if (serverCount > 0 )
    {
        int i;
        listbox->clear();
        for (i=0; i<serverCount; i++)
        {
            addToList(_serverList[i].list, i);
        }
    }
    GenericScreen::show();
}

bool ExtServerScreen::focus()
{
    // don't set focus if it could be thrown back at us
    return listbox->total()?listbox->focus():_tvbevel.focus();
}

void ExtServerScreen::searchAddServer()
{
    int res;

    res = UpnpInit( 0, 0 );
    if ( res != UPNP_E_SUCCESS )
    {
        BDBG_ERR(("%s", UpnpGetErrorMessage( res )) );
        return;
    }

    Cookie cookie;
    cookie.servScreen = this;
    _serverList[serverCount].cookie = &cookie;

    res = UpnpRegisterClient( callback, &cookie, &cookie.clientHandle );
    if ( res != UPNP_E_SUCCESS )
    {
        BDBG_ERR(("%s", UpnpGetErrorMessage( res )) );
    }

    res = UpnpSearchAsync( cookie.clientHandle, 5, MEDIA_SERVER_DEVICE, &cookie );
    if ( res != UPNP_E_SUCCESS )
    {
        BDBG_ERR(("%s", UpnpGetErrorMessage( res )));
    }
    sleep(5);
}

int callback( Upnp_EventType eventType, void* event, void* pCookie )
{
    Cookie* cookie = ( Cookie* )pCookie;

    switch ( eventType )
    {
    
    case UPNP_DISCOVERY_SEARCH_RESULT:
        {
            struct Upnp_Discovery* discovery = ( struct Upnp_Discovery* )event;

            IXML_Document *descriptionDoc = 0;

            int res;
            res = UpnpDownloadXmlDoc( discovery->Location, &descriptionDoc );
            if ( res != UPNP_E_SUCCESS )
            {
                fprintf(stderr,"%s:%d: Could not download device description!", __FILE__, __LINE__ );
                return res;
            }

            IXML_NodeList* deviceList = ixmlDocument_getElementsByTagName( descriptionDoc, (char*)"device" );
            if ( deviceList )
            {
                unsigned int i;

                for (  i = 0; i < ixmlNodeList_length( deviceList ); i++ )
                {
                    IXML_Element* deviceElement = ( IXML_Element* )ixmlNodeList_item( deviceList, i );

                    const char* deviceType = cookie->servScreen->xml_GetChildElementValue( deviceElement, "deviceType" );
                    if ( strcmp( MEDIA_SERVER_DEVICE, deviceType ) != 0 ) continue;
                    BDBG_MSG(("Device Type : %s", deviceType));

                    const char* UDN = cookie->servScreen->xml_GetChildElementValue( deviceElement, "UDN" );
                    if ( cookie->servScreen->getServer( UDN ) != 0 )
                        continue;
                    BDBG_MSG(("Device element : %s", UDN));

                    const char* friendlyName = cookie->servScreen->xml_GetChildElementValue( deviceElement, "friendlyName" );
                    BDBG_MSG(("Friendly Name : %s", friendlyName));

                    MediaServer *server = (MediaServer *)malloc(sizeof(MediaServer));
                    server->cookie = cookie;
                    server->UDN = strdup(UDN);
                    server->friendlyName = strdup(friendlyName);
                    server->contents = 0;

                    if (!cookie->servScreen->addServer(server, descriptionDoc, discovery->Location, discovery->Expires, cookie))
                    {
                        free(server);
                        continue;
                    }

                }
            }
        }
        break;

    default:
        //printf( "%s:%d: DEBUG: UNHANDLED EVENT ( TYPE=%d )", __FILE__, __LINE__, eventType );
        break;
    }
    return UPNP_E_SUCCESS;
}

bool ExtServerScreen::addServer( MediaServer* s, IXML_Document* doc, const char* location, int expire, Cookie* cookie )
{
    if ( getServer( s->UDN ) != 0 ) return false;

    if (serverCount == MAX_DEVICES)
    {
        printf("MAX DEVICES exceed\n");
        return false;
    }


    _serverList[serverCount].list = (MediaServer *)malloc(sizeof(MediaServer));
    _serverList[serverCount].list->UDN = (char *)malloc(strlen(s->UDN));
    _serverList[serverCount].list->UDN = strdup(s->UDN);
    _serverList[serverCount].list->friendlyName = (char *)malloc(strlen(s->friendlyName));
    _serverList[serverCount].list->friendlyName = strdup(s->friendlyName);
    _serverList[serverCount].list->cookie = s->cookie;
    _serverList[serverCount].list->contents = s->contents;
    _serverList[serverCount].location = (char *)malloc(strlen(location));
    _serverList[serverCount].location = strdup(location);
    _serverList[serverCount].expire = expire;
    _serverList[serverCount].descriptionDoc = doc;

    _serverList[serverCount].cookie = (Cookie *)malloc(sizeof(Cookie));
    _serverList[serverCount].cookie->clientHandle = cookie->clientHandle;
    serverCount++;

    return true;
}

MediaServer* ExtServerScreen::getServer( const char* UDN )
{
    MediaServer *result = 0;

    int i;
    for ( i = 0; i < serverCount; i++ )
    {
        if ( strcmp( UDN, _serverList[i].list->UDN ) == 0 )
        {
            result = _serverList[i].list;
            break;
        }
    }

    return result;
}


const char* ExtServerScreen::xml_GetChildElementValue( IXML_Element* parent, const char* tagName )
{
    if ( !parent ) return 0;
    if ( !tagName ) return 0;
    char* s = strdup( tagName );
    IXML_NodeList* nodeList = ixmlElement_getElementsByTagName( parent, s );
    free( s );
    if ( !nodeList ) return 0;

    IXML_Node* element = ixmlNodeList_item( nodeList, 0 );
    ixmlNodeList_free( nodeList );
    if ( !element ) return 0;

    IXML_Node* textNode = ixmlNode_getFirstChild( element );
    if ( !textNode ) return 0;

    return ixmlNode_getNodeValue( textNode );
}


void ExtServerScreen::onClick(MButton *self)
{
    if (control()->getSkin()->isValid(XML_SCREEN_SERVER))
    {
        control()->getSkin()->processAction(self);
        //library screen has no screen specific actions in onClick() so return...
        return;
    }

    GenericScreen::onClick(self);
}

void ExtServerScreen::onItemClicked(MListBox *self)
{
#ifndef B_HAS_DRM_ND
    {
        l1->setText("Ext Playlist");
    }
#endif

    IXML_Document *doc;
    if (frsttime==0)
    {
        MMessageBox mbox(fb(), 250, 200);
        mbox.set("Generating Playlist from the server. Please wait ...", 0);
        mbox.show();
        mbox.drawNow();
        int i = listbox->currentIndex();
        currentServerIndex = listbox->currentIndex();
        doc = _serverList[i].descriptionDoc;

        ParseDeviceDescription(_serverList[i].descriptionDoc, _serverList[i].location, _serverList[i].expire, _serverList[i].cookie);
        if (listbox->total() != 0)
        {
            listbox->clear();
            //      int total = listbox->total();
            //      for (int t=0; t<=total; t++) {
            //      printf("\n Inside clear list %d %d\n",t , total);
            //          listbox->remove(t);
            //      }
        }
        int j;
        for (j=0; j<itemCount; j++)
        {
            char buf[BIG_BUFSIZE];
            BKNI_Snprintf(buf, BIG_BUFSIZE, "%s", (const char *)ItemList[j].title);

            listbox->insert(j, buf);
        }
    }
    else
    {
        bdrmnd_gen_config cfg;
        cfg.server_name = _serverList[currentServerIndex].list->friendlyName;
        int i = listbox->currentIndex();

        int retCode = UpnpUnRegisterClient(_serverList[serverCount - 1].cookie->clientHandle);

        if ( retCode != UPNP_E_SUCCESS )
        {
            BDBG_ERR(("%s:%d: Could not UnREGISTER CLIENT %s %d!", __FILE__, __LINE__, UpnpGetErrorMessage(retCode), _serverList[serverCount - 1].cookie->clientHandle ));
        }

        bstream_mpeg_init(&mpeg);
        mpeg.mpeg_type = bstream_mpeg_type_asf;// = vid->mpeg;
        mpeg.video[0].pid = 0x2;
        mpeg.video[0].format = bvideo_codec_vc1;
        mpeg.audio[0].pid = 0x1;
        mpeg.audio[0].format = baudio_format_wma_std;
        playEngine = control()->checkoutPlayEngine();
        playpump = playEngine->playpump();

        BDBG_ASSERT((playEngine));
        BDBG_ASSERT((playpump));

        const char* drm_nd_link = NULL;
        const char* drm_nd_desc_doc = NULL;

        unsigned char* key_nd_out = (unsigned char *)malloc(BDRMND_KEY_SIZE);
        unsigned char* session_id_out = (unsigned char *)malloc(BDRMND_SESSION_ID_SIZE);

        bdrmnd_set_config (&cfg);
        drm_nd_link = (const char *)ItemList[i].resource;//b_extra_arg_link.link;
        drm_nd_desc_doc = "carnat.xml";//b_extra_arg_link.desc_doc;
        /* set up stream input */
        nd_stream.fn_lic_callback = bdrmnd_stage_callback;
        nd_stream.lic_callback_ctx = this;
        nd_stream.nd_link = drm_nd_link;
        nd_stream.desc_doc_name = drm_nd_desc_doc;

        /* set up key/session output */
        nd_session.key_nd_out = &key_nd_out;
        nd_session.session_id_out = &session_id_out;

        BDBG_ASSERT((playEngine));

        bplaypump_params playpump_params;
        bplaypump_params_init(&playpump_params, playpump);
        stream = bplaypump_start(playpump, &mpeg, &playpump_params);

        BDBG_ASSERT((stream));

        decode = control()->getDecode();
        window = control()->getWindow();

        int rc = bdecode_start(decode, stream, window); /* start decode of the stream from the stream using specified decode engine displaying on the specified  window */
        if (rc!=b_ok)
        {
            BDBG_ERR(("Can't start decode"));
            return ;
        }

        control()->showScreen(Control::eTV);

        /* set up network device pipe */
        nd_pipe.fd = 0;
        nd_pipe.playpump = playpump;

        bdrmnd_start_stream(&nd_stream, &nd_session);

        memcpy(mpeg.encryption.key, key_nd_out, BDRMND_KEY_SIZE);       /* copy key into decryptor instance */
        memcpy(g_decode_nd_key, key_nd_out, BDRMND_KEY_SIZE);

        bdrmnd_send_transfer_request(drm_nd_link, session_id_out, 
                                     bdrmnd_http_transfer_callback, 
                                     &nd_pipe);
    }

    frsttime=1;
}

int bdrmnd_http_transfer_callback (unsigned char* data, unsigned long len, void* context)
{
    unsigned char* buffer=NULL;
    size_t buffer_size = 0, fed=0;
    bdrmnd_transfer_pipe* nd_pipe = (bdrmnd_transfer_pipe*)context;;
    bplaypump_status status;
    playpump =  nd_pipe->playpump;

    if (nd_pipe == NULL)
    {
        BDBG_WRN(("context null"));
        return -1;
    }

    if (nd_pipe->flags == 0xFF)
    {
        return -1;
    }

    //BDBG_MSG(("got %d data to feed playpump %p",len,nd_pipe->playpump));
    while (len>0)
    {
        while (bplaypump_get_buffer(nd_pipe->playpump, (void**)&buffer, &buffer_size)==0)
        {
            if (buffer_size==0)
            {
                usleep(100000);
                continue;
            }
            break;
        }
        if (len <= buffer_size)
        {
            memcpy(buffer, data, len);
            fed=len;
            len=0;
        }
        else
        {
            memcpy(buffer, data, buffer_size);
            data +=buffer_size;
            len -=buffer_size;
            fed=buffer_size;

        }

        if (bplaypump_read_complete(nd_pipe->playpump,0,fed))
        {
            BDBG_ERR(("read complete failed len=%d",len));
            break;
        }
        //BDBG_MSG(("fed %d to plyaback,remaining %d bytes",fed,len));
    }
    return 0;
}

int bdrmnd_stage_callback (bdrmnd_license_stage stage, bdrmnd_license_status status, void *context)
{
    int ret = 0;
    ExtServerScreen *screen = (ExtServerScreen *)context;
    MMessageBox mbox(screen->fb(), 150, 100);
    switch (stage)
    {
    case bdrmnd_lic_stage_scanning:

        mbox.set("Scanning ...", 0);
        BDBG_MSG(("Scanning ..."));
        mbox.show();
        mbox.drawNow();
        break;

    case bdrmnd_lic_stage_authorization:
        mbox.set("Authorizing ...", 0);
        BDBG_MSG(("Authorizing ..."));
        mbox.show();
        mbox.drawNow();
        break;

    case bdrmnd_lic_stage_registration:
        mbox.set("Registering ...", 0);
        BDBG_MSG(("Registering ..."));
        mbox.show();
        mbox.drawNow();
        break;

    case bdrmnd_lic_stage_proximity:
        mbox.set("Proximity Check...", 0);
        BDBG_MSG(("Proximity Check..."));
        mbox.show();
        mbox.drawNow();
        break;

    case bdrmnd_lic_stage_validation:
        mbox.set("Validating ...", 0);
        mbox.show();
        mbox.drawNow();
        BDBG_MSG(("Validating ..."));
        break;

    case bdrmnd_lic_stage_licensing:
        mbox.set("License acquired ...", 0);
        mbox.show();
        mbox.drawNow();
        BDBG_MSG(("License acquired ..."));
        break;
    }
    BDBG_MSG(("#### DRMND STAGE CALLBACK : stage=%d status=%d",stage, status));

    return(stage);
}


void ExtServerScreen::ParseDeviceDescription( IXML_Document* doc, const char* location, int expires, Cookie* cookie )
{
    if (!doc)
    {
        BDBG_ERR(("No Document"));
        return;
    }
    if (!location)
    {
        BDBG_ERR(("No Location"));
        return;
    }
    const char* baseURL = location;

    // Try to extract baseURL

    IXML_NodeList* urlList = ixmlDocument_getElementsByTagName( doc, (char*)"baseURL" );
    if ( urlList )
    {
        IXML_Node *urlNode = ixmlNodeList_item( urlList, 0 );
        if ( urlNode )
        {
            IXML_Node* textNode = ixmlNode_getFirstChild( urlNode );
            if ( textNode ) baseURL = ixmlNode_getNodeValue( textNode );
        }

        ixmlNodeList_free( urlList );
    }

    // Get devices

    IXML_NodeList* deviceList = ixmlDocument_getElementsByTagName( doc, (char*)"device" );
    if ( deviceList )
    {
        unsigned int i;

        for (  i = 0; i < ixmlNodeList_length( deviceList ); i++ )
        {
            IXML_Element* deviceElement = ( IXML_Element* )ixmlNodeList_item( deviceList, i );

            const char* deviceType = xml_GetChildElementValue( deviceElement, "deviceType" );
            if ( strcmp( MEDIA_SERVER_DEVICE, deviceType ) != 0 ) continue;

            const char* UDN = xml_GetChildElementValue( deviceElement, "UDN" );

            const char* friendlyName = xml_GetChildElementValue( deviceElement, "friendlyName" );

            MediaServer *server = (MediaServer *)malloc(sizeof(MediaServer));
            server->cookie = cookie;
            server->UDN = strdup(UDN);
            server->friendlyName = strdup(friendlyName);
            server->contents = 0;

            // Check for ContentDirectory service...

            IXML_NodeList* serviceList = ixmlElement_getElementsByTagName( deviceElement, (char*)"service" );
            if ( serviceList )
            {
                unsigned int j;
                for ( j = 0; j < ixmlNodeList_length( serviceList ); j++ )
                {
                    IXML_Element* serviceElement = ( IXML_Element* )ixmlNodeList_item( serviceList, j );

                    const char* serviceType = xml_GetChildElementValue( serviceElement, "serviceType" );
                    if ( !serviceType ) continue;
                    if ( strcmp( CONTENT_DIRECTORY_SERVICE, serviceType ) != 0 ) continue;

                    const char* eventSubURL = xml_GetChildElementValue( serviceElement, "eventSubURL" );
                    if ( !eventSubURL ) continue;

                    const char* controlURL = xml_GetChildElementValue( serviceElement, "controlURL" );
                    if ( !controlURL ) continue;

                    // Try to subscribe to ContentDirectory service

                    char* url = ( char* )malloc( strlen( baseURL ) + strlen( eventSubURL ) + 1 );
                    if ( url )
                    {
                        char* s1 = strdup( baseURL );
                        char* s2 = strdup( eventSubURL );
                        if ( UpnpResolveURL( s1, s2, url ) == UPNP_E_SUCCESS )
                        {
                            server->contentDirectoryEventURL = url;

                            int timeOut = 1810;
                            Upnp_SID sid;

                            int res = UpnpSubscribe( cookie->clientHandle, url, &timeOut, sid );

                            if ( res == UPNP_E_SUCCESS )
                            {
                                server->subscriptionTimeOut = timeOut;
                                memcpy( server->subscriptionID, sid, sizeof( Upnp_SID ) );
                            }
                            else
                            {
                                BDBG_ERR(("Subscriptoin FAIL %s %d", UpnpGetErrorMessage( res ), cookie->clientHandle));
                            }
                        }
                        free( s1 );
                        free( s2 );
                        free( url );
                    }

                    // Try to browse content directory...
                    url = ( char* )malloc( strlen( baseURL ) + strlen( controlURL ) + 1 );
                    memset(url, '\0', strlen(url));
                    if ( url )
                    {
                        char* s1 = strdup( baseURL );
                        char* s2 = strdup( controlURL );
                        if ( UpnpResolveURL( s1, s2, url ) == UPNP_E_SUCCESS )
                        {
                            server->contentDirectoryControlURL = url;

                            FetchContents(server);
                        }

                        free( s1 );
                        free( s2 );
                        free( url );
                    }
                }
                ixmlNodeList_free( serviceList );
            }
        }

        ixmlNodeList_free( deviceList );
    }
}




IXML_Document* ExtServerScreen::browseAction( const char* pObjectID, const char* pBrowseFlag, const char* pFilter,
                                              const char* pStartingIndex, const char* pRequestedCount, const char* pSortCriteria, MediaServer *server )
{
    IXML_Document* action = 0;
    IXML_Document* response = 0;
    const char* url = server->contentDirectoryControlURL;
    if ( !url || strcmp( url, "" ) == 0 )
    {
        BDBG_WRN(("No subscription url set!" ));
        return 0;
    }

#if 0
    char* ObjectID = strdup( pObjectID );
    char* BrowseFlag = strdup( pBrowseFlag );
    char* Filter = strdup( pFilter );
    char* StartingIndex = strdup( pStartingIndex );
    char* RequestedCount = strdup( pRequestedCount );
    char* SortCriteria = strdup( pSortCriteria );
#endif
    char* serviceType = strdup( CONTENT_DIRECTORY_SERVICE );

    int res;
    res = UpnpAddToAction( &action, (char*)"Browse", serviceType, (char*)"ObjectID", (char *)pObjectID );
    if ( res != UPNP_E_SUCCESS )
    {
        BDBG_ERR(("%s:%d: ERROR: %s", __FILE__, __LINE__, UpnpGetErrorMessage( res ) ));
        goto browseActionCleanup;
    }

    res = UpnpAddToAction( &action, (char*)"Browse", serviceType, (char*)"BrowseFlag", (char *)pBrowseFlag );
    if ( res != UPNP_E_SUCCESS )
    {
        BDBG_ERR(("%s:%d: ERROR: %s", __FILE__, __LINE__, UpnpGetErrorMessage( res ) ));
        goto browseActionCleanup;
    }
    res = UpnpAddToAction( &action, (char*)"Browse", serviceType, (char*)"Filter",(char *)pFilter );
    if ( res != UPNP_E_SUCCESS )
    {
        BDBG_ERR(("%s:%d: ERROR: %s", __FILE__, __LINE__, UpnpGetErrorMessage( res )) );
        goto browseActionCleanup;
    }

    res = UpnpAddToAction( &action, (char*)"Browse", serviceType, (char*)"StartingIndex", (char *)pStartingIndex );
    if ( res != UPNP_E_SUCCESS )
    {
        /* msg_Dbg( _cookie->serviceDiscovery, "%s:%d: ERROR: %s", __FILE__, __LINE__, UpnpGetErrorMessage( res ) ); */ 
        goto browseActionCleanup;
    }

    res = UpnpAddToAction( &action, (char*)"Browse", serviceType, (char*)"RequestedCount", (char *)pRequestedCount );
    if ( res != UPNP_E_SUCCESS )
    { /* msg_Dbg( _cookie->serviceDiscovery, "%s:%d: ERROR: %s", __FILE__, __LINE__, UpnpGetErrorMessage( res ) ); */ 
        goto browseActionCleanup;
    }

    res = UpnpAddToAction( &action, (char*)"Browse", serviceType, (char*)"SortCriteria", (char *)pSortCriteria );
    if ( res != UPNP_E_SUCCESS )
    { /* msg_Dbg( _cookie->serviceDiscovery, "%s:%d: ERROR: %s", __FILE__, __LINE__, UpnpGetErrorMessage( res ) ); */ 
        goto browseActionCleanup;
    }
    res = UpnpSendAction( server->cookie->clientHandle,
                          url,
                          CONTENT_DIRECTORY_SERVICE,
                          0,
                          action,
                          &response );
    if ( res != UPNP_E_SUCCESS )
    {
        BDBG_ERR(("%s:%d: ERROR: %s", __FILE__, __LINE__, UpnpGetErrorMessage( res )));
        ixmlDocument_free( response );
        response = 0;
    }

    browseActionCleanup:

//    free( ObjectID );
//    free( BrowseFlag );
//    free( Filter );
//    free( StartingIndex );
//    free( RequestedCount );
//    free( SortCriteria );

//    free( serviceType );

    ixmlDocument_free( action );
    return response;
}

bool ExtServerScreen::FetchContents(MediaServer *server)
{
    Container *root = (Container *)malloc(sizeof(Container));
    root->parent = 0;
    root->objectID = (char*)"0";
    root->title = server->friendlyName;
    root->itemCount = 0;
    root->containerCount = 0;

    _FetchContents(root, server);

    server->contents = root;

    //set playlist
    //build playlist
}

bool ExtServerScreen::_FetchContents(Container *parent, MediaServer *server)
{
    if (!parent)
    {
        BDBG_WRN(("%s:%d: parent==NULL", __FILE__, __LINE__ ));
        return false;
    }
    IXML_Document* response = browseAction( parent->objectID, "BrowseDirectChildren", "*", "0", "0", "", server );
    if ( !response )
    {
        BDBG_ERR(("ERROR!" ));
        return false;
    }

    IXML_Document* result = ParseBrowseResult( response );
#if 0
    b_print_xml((IXML_Node*)result);
//    ixmlDocument_free( response );
#endif

    if ( !result )
    {
        BDBG_ERR(("%s:%d: ERROR!\n", __FILE__, __LINE__ ));
        return false;
    }

    IXML_NodeList* containerNodeList = ixmlDocument_getElementsByTagName( result, (char*)"container" );
    if ( containerNodeList )
    {
        unsigned int i;
        for ( i = 0; i < ixmlNodeList_length( containerNodeList ); i++ )
        {
            IXML_Element* containerElement = ( IXML_Element* )ixmlNodeList_item( containerNodeList, i );

            const char* objectID = ixmlElement_getAttribute( containerElement, (char*)"id" );
            if ( !objectID ) continue;

            const char* childCountStr = ixmlElement_getAttribute( containerElement, (char*)"childCount" );
            if ( !childCountStr ) continue;
            int childCount = atoi( childCountStr );

            const char* title = xml_GetChildElementValue( containerElement, "dc:title" );
            if ( !title ) continue;

            const char* resource = xml_GetChildElementValue( containerElement, "res" );


            if ( resource && childCount < 1 )
            {
                Item *item = (Item *)malloc(sizeof(Item));
                item->parent = parent;

                item->objectID = (char *)malloc(strlen(objectID));
                item->objectID = strdup(objectID);
                item->title = (char *)malloc(strlen(title));
                item->title = strdup(title);
                item->resource = (char *)malloc(strlen(resource));
                item->resource = strdup(resource);

                if (strcmp( item->parent->title, "All Music") == 0)
                {
                    addItem(parent, item);
                }

                if (strcmp( item->parent->title, "All Video") == 0)
                {
                    addItem(parent, item);
                }
            }
            else
            {
                Container *container = (Container *)malloc(sizeof(Container));
                container->parent = parent;
                container->objectID = (char *)malloc(strlen(objectID));
                container->objectID = strdup(objectID);
                container->title = (char *)malloc(strlen(title));
                container->title = strdup(title);
                container->itemCount = 0;
                container->containerCount = 0;
                addContainer(parent, container);

                if ( childCount > 0 ) _FetchContents( container, server );
            }
        }

        ixmlNodeList_free( containerNodeList );
    }

    IXML_NodeList* itemNodeList = ixmlDocument_getElementsByTagName( result, (char*)"item" );
    IXML_NodeList* resNodeList =  ixmlDocument_getElementsByTagName( result, (char*)"res" );
    if ( itemNodeList && resNodeList )
    {
        unsigned int i;
        for (  i = 0; i < ixmlNodeList_length( resNodeList ); i++ )
        {
            IXML_Element* tempElement = ( IXML_Element* )ixmlNodeList_item( resNodeList, i );
            const char* prot = ixmlElement_getAttribute( tempElement, (char*)"protection");
            if (!prot)
                continue;
            else if (strcmp(prot, "Microsoft WM DRM") != 0)
                continue;

            IXML_Element* itemElement = ( IXML_Element* )ixmlNodeList_item( itemNodeList, i );

            const char* objectID = ixmlElement_getAttribute( itemElement, (char*)"id" );
            if ( !objectID ) continue;

            const char* title = xml_GetChildElementValue( itemElement, "dc:title" );
            if ( !title ) continue;

            const char* resource = xml_GetChildElementValue( itemElement, "res" );
            if ( !resource ) continue;

            Item *item = (Item *)malloc(sizeof(Item));
            item->parent = parent;
            item->objectID = (char *)malloc(strlen(objectID));
            item->objectID = strdup(objectID);
            item->title = (char *)malloc(strlen(title));
            item->title = strdup(title);
            item->resource = (char *)malloc(strlen(resource));
            item->resource = strdup(resource);

            if (strcmp( item->parent->title, "All Music") == 0)
            {
                BDBG_MSG(("Adding Item : %s Parent Title: %s", item->title, item->parent->title));
                addItem(parent, item);
            }
            else if (strcmp( item->parent->title, "All Video") == 0)
            {
                BDBG_MSG(("Adding Item : %s\nParent Title: %s", item->title, item->parent->title));
                addItem(parent, item);
            }
        }
        ixmlNodeList_free( itemNodeList );
    }

    ixmlDocument_free( result );

    return true;
}

IXML_Document* ExtServerScreen::ParseBrowseResult( IXML_Document* doc )
{
    if ( !doc ) return 0;

    IXML_NodeList* resultList = ixmlDocument_getElementsByTagName( doc, (char*)"Result" );
    if ( !resultList ) return 0;

    IXML_Node* resultNode = ixmlNodeList_item( resultList, 0 );

    ixmlNodeList_free( resultList );

    if ( !resultNode ) return 0;

    IXML_Node* textNode = ixmlNode_getFirstChild( resultNode );
    if ( !textNode ) return 0;

    const char* resultString = ixmlNode_getNodeValue( textNode );
    char* resultXML = (char*)resultString;//convert_xml_special_chars( resultString );

    IXML_Document* browseDoc = ixmlParseBuffer( resultXML );

    free( resultXML );

    return browseDoc;
}



bool ExtServerScreen::addItem(Container *parent, Item *item)
{
    if (parent->itemCount >= MAX_ITEM)
        return false;
    parent->item[parent->itemCount] = *item;
    parent->itemCount++;

    ItemList[itemCount] = *item;
    itemCount++;
}

bool ExtServerScreen::addContainer(Container *parent, Container *container)
{
    if (parent->containerCount >= MAX_ITEM)
        return false;

    parent->container = container;
    parent->containerCount++;
}



char *ExtServerScreen::convert_xml_special_chars( const char *psz_content )
{
    char *psz_temp = (char *)malloc( 6 * strlen( psz_content ) + 1 );
    const char *p_from = psz_content;
    char *p_to   = psz_temp;

    while ( *p_from )
    {
        if ( *p_from == '<' )
        {
            strcpy( p_to, "&lt;" );
            p_to += 4;
        }
        else if ( *p_from == '>' )
        {
            strcpy( p_to, "&gt;" );
            p_to += 4;
        }
        else if ( *p_from == '&' )
        {
            strcpy( p_to, "&amp;" );
            p_to += 5;
        }
        else if ( *p_from == '\"' )
        {
            strcpy( p_to, "&quot;" );
            p_to += 6;
        }
        else if ( *p_from == '\'' )
        {
            strcpy( p_to, "&#039;" );
            p_to += 6;
        }
        else
        {
            *p_to = *p_from;
            p_to++;
        }
        p_from++;
    }
    *p_to = '\0';

    return psz_temp;
}

void b_print_xml(IXML_Node *xml)
{
    DOMString s = ixmlNodetoString(xml);
    puts(s);
    ixmlFreeDOMString(s);
}

