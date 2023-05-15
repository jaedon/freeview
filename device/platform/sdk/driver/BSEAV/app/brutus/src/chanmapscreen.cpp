/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: chanmapscreen.cpp $
 * $brcm_Revision: 5 $
 * $brcm_Date: 2/1/08 5:00p $
 *
 * Module Description: Channel map screen
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/chanmapscreen.cpp $
 * 
 * 5   2/1/08 5:00p katrep
 * PR39180: Added support for ofdm/DVB-T THD tuner
 * 
 * 4   9/4/07 11:10a yrajakar
 * PR34578: Adding support for RTP FEC
 * 
 * 3   4/27/07 3:16p jrubio
 * PR30010: added rtp_no_rtcp case
 * 
 * 2   4/24/07 1:13p jrubio
 * PR30010: fixed compiler errors
 * 
 * 1   4/24/07 12:47p jrubio
 * PR30010: adding GUI file for STREAMER server list
 * 
 **************************************************************************/
#include "control.h"
#include "mlabel.h"
#include "channelmap.h"
#include "chanmapscreen.h"

#define MAP() control()->channelMap()
#define BEVEL_WIDTH 5
#define MEGA_F 1000000.0


const char *btuner_ip_str(bsettop_ip_protocol proto)
{
    switch (proto) {
    case bsettop_ip_protocol_udp: return "UDP";
    case bsettop_ip_protocol_rtp_fec: return "FEC";
    case bsettop_ip_protocol_rtp: return "RTP";
    case bsettop_ip_protocol_rtp_no_rtcp: return "RTP";
    case bsettop_ip_protocol_http: return "HTTP";
    default: break;
    }

    return NULL;
}


ChanMapScreen::ChanMapScreen(Control *acontrol, MRect rectTVWin) :
    GenericScreen(acontrol, rectTVWin) //,SMALLTV )
{

    MRect                rectListBox(17, 200, 600, 200);
    MString              listFocusImage;
    MString              listScrollbarImage;
    MString              listScrollIndImage;
    uint32_t             listDropShadow       = 0;
    uint32_t             nBevelListBox        = 2;
    uint32_t             nRows                = 4;
    MPainter::BevelStyle styleBevel           = MPainter::bsRaised;
    int32_t              nListTextColor       = -1;
    int32_t              nListTextColorFocus  = -1;


    Skin * pSkin = control()->getSkin();

    if (pSkin->isValid(XML_SCREEN_CHAN_MAP))
    {
        pSkin->createWidgets(XML_SCREEN_CHAN_MAP, this);
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

        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_GEOM_LIST_X,            listX);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_GEOM_LIST_Y,            listY);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_GEOM_LIST_H,            listHeight);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_GEOM_LIST_W,            listWidth);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_BEVEL_LIST_WIDTH,       listBevelWidth);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_BEVEL_LIST_STYLE,       listBevelStyle);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_LIST_COLUMNS,           listColumns);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_LIST_VISIBLE_ROWS,      listRows);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_LIST_TEXT_COLOR,        listTextColor);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_LIST_TEXT_COLOR_FOCUS,  listTextColorFocus);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_LIST_FOCUS_IMAGE,       listFocusImage);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_LIST_SCROLLBAR_IMAGE,   listScrollbarImage);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
                            XML_ATT_LIST_SCROLLIND_IMAGE,   listScrollIndImage);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK, 
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
    }

    MLabel *bevel = new MLabel(this, rectListBox);
    bevel->setBevel(nBevelListBox, styleBevel);
    _list = new MColumnListBox(bevel,
        MRect(BEVEL_WIDTH,BEVEL_WIDTH,bevel->width()-2*BEVEL_WIDTH,bevel->height()-2*BEVEL_WIDTH));

    _list->scrollBar()->setButtons(false);
    _list->scrollBar()->setIndicator(true);
    _list->setTotalVisible(7);
    _list->captureKeys(true, true);

    _list->setFocusImage(pSkin->getImage(listFocusImage.s()));
    _list->scrollBar()->setImage(pSkin->getImage(listScrollbarImage.s()));
    _list->scrollBar()->setIndicatorImage(pSkin->getImage(listScrollIndImage.s()));
    _list->setDropShadow(listDropShadow);

    _list->setIndent(10);
    _list->addColumn(30);   // number
    _list->addColumn(100);  // mode
    _list->addColumn(140);  // freq
    _list->addColumn(80);  // symbolrate
    _list->addColumn(200);   // annex or sdsmode

    _modified = false;
    
    _list->addHandler(this);
    bevel->raise();
    refresh();

}


int ChanMapScreen::refresh()
{
    _list->beginUpdates();
    _list->clear();
    _modified = false;

    for (int i=0;i<MAP()->total();i++) {
        ChannelMap::ChannelMapEntry entry;
        char buf[256];

    if(MAP()->getEntry(i, &entry) == false){
        return -1;
    }

        /* printf (("CMAP: type=%d\n",entry.getType())); */
        switch (entry.getType()) {
        case btuner_streamer_stream:
            snprintf(buf, sizeof(buf), "%d\tSTREAMER\t%s\t%d (TS%d_IN)",
                    i,
                    bstream_mpeg_type_str(entry.streamer.mpeg_type),
                    entry.streamer.index,
                    entry.streamer.index+1
                    );
                break;
        case btuner_vsb_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%0.2f\t%f",
                i,
                btuner_vsb_str(entry.vsb.mode),
                entry.freq,
                entry.vsb.symbol_rate / MEGA_F
                );
            break;
        case btuner_ofdm_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%0.2f\t%d",
                i,
                btuner_ofdm_str(),
                entry.freq,
                entry.ofdm.bandwidth
                );
            break;
        case btuner_sds_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%0.2f\t%0.2f",
                i,
                btuner_sds_str(entry.sds.params.mode, entry.sds.mpeg_type),
                entry.freq,
                entry.sds.params.symbol_rate / MEGA_F
                );
            break;
        case btuner_qam_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%0.2f\t%f\t%s",
                i,
                btuner_qam_str(entry.qam.mode),
                entry.freq,
                entry.qam.symbol_rate / MEGA_F,
                btuner_qam_annex_str(entry.qam.annex));
            break;
        case btuner_analog_stream:
            if (entry.analog.linein >= 0)
                snprintf(buf, sizeof(buf), "%d\t%s\t\t(LINEINPUT\t%d)\t%s",
                    i,
                    b_lineinput_str[entry.analog.linein],
                    entry.analog.linein,
                    btuner_analog_format_str(entry.analog.params.video_format));
            else
                snprintf(buf, sizeof(buf), "%d\t%s\t%0.2f\t\t%s",
                    i,
                    btuner_analog_str(&entry),
                    entry.freq,
                    btuner_analog_format_str(entry.analog.params.video_format));
            break;
        case btuner_ip_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%s\t%d\t%s",
            i,
            btuner_ip_str(entry.ip.params.open_params.protocol),
            entry.ip.params.open_params.ip_addr,
            entry.ip.params.open_params.port,
            (entry.ip.params.open_params.protocol == bsettop_ip_protocol_http) ? entry.ip.params.open_params.url :"" ); 
            /* printf(("ChannelInfo: %s\n",buf)); */
            break;

        default: return -1; /* compiler warning */
        }
        buf[sizeof(buf)-1]='\0';
        _list->add(buf);
    }
    _list->setCurrentIndex(0);
    _list->endUpdates();
    return 0;
}


void ChanMapScreen::onClick(MButton *self) {

    if (control()->getSkin()->isValid(XML_SCREEN_CHAN_MAP))
    {
        //handle generic skin actions not specific to this screen
        control()->getSkin()->processAction(self);
    }
    else
        GenericScreen::onClick(self);
}

void ChanMapScreen::onItemClicked(MListBox *self)
{
    control()->setChannel(_list->currentIndex());
    control()->showScreen(Control::eTV);
}

bool ChanMapScreen::focus() {
    // don't set focus if it could be thrown back at us
    return _list->total()?_list->setFocus(control()->channel()):_tvbevel.focus();
}
