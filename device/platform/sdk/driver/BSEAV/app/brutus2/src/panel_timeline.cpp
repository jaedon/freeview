/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_timeline.cpp $
* $brcm_Revision: 11 $
* $brcm_Date: 10/19/12 10:37a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_timeline.cpp $
* 
* 11   10/19/12 10:37a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 10   10/11/12 11:38p jrubio
* SW7346-1004: fix playback timeline
* 
* 9   10/11/12 11:22p tokushig
* SW7231-749: fix transparency in timeline
* 
* 8   10/11/12 7:51p jrubio
* SW7346-1004: remove timeline after stop
* 
* 7   10/11/12 5:04p jrubio
* SW7346-1004: reset timeline when switching playbacks
* 
* 6   10/11/12 4:09p jrubio
* SW7346-1004: fix timeline issue
* 
* 5   10/11/12 2:36p jrubio
* SW7346-1004: Change Playback Notifiation to PlaybackStateChanged
* 
* 4   10/10/12 10:01p jrubio
* SW7346-1004: adjust timeline
* 
* 3   10/8/12 4:16p jrubio
* SW7346-1004: correct the Size
* 
* 2   10/8/12 9:04a jrubio
* SW7346-1004: decrease size
* 
* 1   10/3/12 5:29p jrubio
* SW7346-1004: add panel_timeline
* 
* 
* 
*********************************************************************************/
#include "avtypes.h"
#include "nexus_video_decoder.h"
#include "panel_timeline.h"

#define B_TIME_PERIOD_FORMAT_LONG 0
#define B_TIME_PERIOD_FORMAT_COMPACT 1

BDBG_MODULE(panel_timeline);

/* as long as pos, firstpos and lastpos are scaled the same way, you can apply
any scale factor */
#define SCALE_MPEGPOS(POS) ((POS) >> 5)

CPanelTimeline::CPanelTimeline(bwidget_engine_t   engine, 
                             CWidgetBase      * pParentWidget, 
                             MRect              geometry,
                             bwin_font_t        font,
                             bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _PlaybackMeter(NULL),
    _PlaybackTime(NULL),
    _PlaybackSymbol(NULL),
    _PlaybackName(NULL),
    _graphicsWidth(0),
    _graphicsHeight(0),
    _timeSet(false),
    _timerBuffers(engine, this, 333)
{
}



CPanelTimeline::~CPanelTimeline()
{
    uninitialize();
}

eRet CPanelTimeline::initialize(CModel * pModel, CModelConfig * pModelConfig)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bwin_font_t           font10;
    int                   marginBkgnd     = 1; 
    int                   panelWidth      = 650;
    int                   panelHeight     = 32;
    int                   meterHeight     = 15 - getBevel();
    int                   meterWidth      = panelWidth-getBevel();
    int                   labelHeight     = 15;
    int                   labelWidth      = 75;
    int                   labelY          = 0; /* Where the labels sit right on top of the PlaybackMeter */
    MRect                 rectPanel;
    MRect                 rectTimeline;
    MRect                 rectTime;

    BDBG_ASSERT(NULL != pModel);
    BDBG_ASSERT(NULL != pModelConfig);

    setModel(pModel);
    setModelConfig(pModelConfig);

    pGraphics = _pModel->getGraphics();
    BDBG_ASSERT(NULL != pGraphics);

    font10 = pGraphics->getFont(10);
    BDBG_ASSERT(NULL != font10);

    _graphicsWidth  = GET_INT(_pCfg, GRAPHICS_SURFACE_WIDTH);
    _graphicsHeight = GET_INT(_pCfg, GRAPHICS_SURFACE_HEIGHT);

    /* set the size of the panel itself */
    rectPanel.set((_graphicsWidth / 2) - (panelWidth / 2), _graphicsHeight - panelHeight-25, panelWidth, panelHeight);
    rectPanel.setX(rectPanel.x() - marginBkgnd * 2);
    rectPanel.setY(rectPanel.y() - marginBkgnd * 2);
    rectPanel.grow(marginBkgnd * 2);
    setGeometry(rectPanel);
    setBackgroundColor(0xFFE8E8E8); 
    

    /* offset for now to see the background*/
    rectTimeline.set(getBevel(), rectPanel.height()-getBevel()- meterHeight, meterWidth ,meterHeight);
    _PlaybackMeter = new CWidgetProgress(getEngine(), this, rectTimeline, font10);
    CHECK_PTR_GOTO("unable to allocate menu widget", _PlaybackMeter, ret, eRet_OutOfMemory, error);
    _PlaybackMeter->setLevel(0);
    _PlaybackMeter->setBevel(1);
    _PlaybackMeter->showProgress(true);

    labelY = rectTimeline.y()- labelHeight;

    rectTime.set(getBevel(),labelY,labelWidth+10,labelHeight);
    _PlaybackSymbol = new CWidgetLabel(getEngine(),this,rectTime,font10);
    _PlaybackSymbol->setText("Play", bwidget_justify_horiz_center, bwidget_justify_vert_middle);
    _PlaybackSymbol->setBevel(1);

    rectTime.set(rectTimeline.width()+ getBevel()-labelWidth,labelY,labelWidth,labelHeight);
    _PlaybackTime = new CWidgetLabel(getEngine(),this,rectTime,font10);
    _PlaybackTime->setText("0:00", bwidget_justify_horiz_center, bwidget_justify_vert_middle);
    _PlaybackTime->setBevel(1);

    /* center the Name of the File */
    labelY = rectTimeline.y()-labelHeight;
    rectTime.set(rectTimeline.width()/3,labelY ,labelWidth*3,labelHeight);
    _PlaybackName = new CWidgetLabel(getEngine(),this,rectTime,font10);
    _PlaybackName->setText("Unknown File", bwidget_justify_horiz_center, bwidget_justify_vert_middle);
    _PlaybackName->setBackgroundColor(getBackgroundColor());
    _PlaybackName->setBevel(0);
    
    
    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelTimeline::uninitialize()
{    
    DEL(_PlaybackName);
    DEL(_PlaybackTime);
    DEL(_PlaybackSymbol);
    DEL(_PlaybackMeter);
}

bool CPanelTimeline::isPlaybackActive()
{
    bool        bActive   = false;
    CPlayback * pPlayback = _pModel->getPlayback();

    if (NULL != pPlayback)
    {
        bActive = pPlayback->isActive();
    }

    return bActive;
}

MString CPanelTimeline::printTime(int format)
{
    CPlayback                * pPlayback        = _pModel->getPlayback();
    NEXUS_PlaybackStatus       nPlaybackStatus;
    NEXUS_Error                nerror           =  NEXUS_SUCCESS;
    char buf[64];
    int len = 0;
    int seconds = 0;
    int hours = 0;
    int minutes = 0;
    
    if (NULL != pPlayback)
    {
        nerror = NEXUS_Playback_GetStatus(pPlayback->getPlayback(), &nPlaybackStatus);
        CHECK_NEXUS_ERROR_ASSERT("getting playback status failed!", nerror);
    } else
       return MString();

    if (nPlaybackStatus.last == 0)
    {
       return  MString();
    }

    seconds = nPlaybackStatus.last / 1000;
    hours = seconds / 3600;
    minutes = (seconds % 3600) / 60;
    seconds = seconds % 60;

    switch (format) {
    case B_TIME_PERIOD_FORMAT_LONG:
    	{
    	MString s;
    	// this prevents lots of reallocs
    	s.allocate(100);
    	if (hours) {
    		s = MString(hours) + " hour";
    		if (hours != 1)
    			s += "s";
    	}
    	if (minutes) {
    		if (!s.isNull())
    			s += ", ";
    		s += MString(minutes) + " minute";
    		if (minutes != 1)
    			s += "s";
    	}
    	if (seconds && !hours && minutes < 5) {
    		if (!s.isNull())
    			s += ", ";
    		s += MString(seconds) + " second";
    		if (seconds != 1)
    			s += "s";
    	}
    	if (s.isNull())
    		s = "0";
    	strcpy(buf, s);
    	len = strlen(buf);
    	}
    	break;
    case B_TIME_PERIOD_FORMAT_COMPACT:
    	{
    	if (hours)
    		len = BKNI_Snprintf(buf, 64,"%d:%02d:%02d", hours, minutes, seconds);
    	else
    		len = BKNI_Snprintf(buf, 64,"%d:%02d", minutes, seconds);
    	}
    	break;
    }

    BDBG_MSG(("TIME in files is %s, %d minutes %d seconds",buf,minutes,seconds));


    return MString(buf);
}


void CPanelTimeline::onClick(bwidget_t widget)
{
    eRet            ret           = eRet_NotSupported;  /* assume we will not consume click */
    eNotification   notification  = eNotify_Invalid;
    CWidgetBase   * pWidget       = NULL;

    /* find the widget object that corresponds to the given bwidget_t */
    {
        blabel_settings   labelSettings;
        blabel_get(widget, &labelSettings);
        pWidget = (CWidgetBase *)labelSettings.widget.data;
    }
}


void CPanelTimeline::show(bool bShow)
{
    /* avoid CPanel's "restore last focus" feature since
       there is no focusable widgets in this panel */
    CWidgetLabel::show(bShow);

    /* Playback Started get Name */
    CPlayback                * pPlayback        = _pModel->getPlayback();

    if (NULL != pPlayback && pPlayback->getVideo()!= NULL)
    {
       if (!(pPlayback->getVideoName()).isEmpty())
       {
          _PlaybackName->setText((pPlayback->getVideoName()).s(),bwidget_justify_horiz_center, bwidget_justify_vert_middle);
       }
    }

    if (true == bShow)
    {
        _timerBuffers.start(GET_INT(_pCfg, UI_BUFFERS_UPDATE_TIMEOUT));
        
    }
    else
    {
        _timerBuffers.stop();
    }
}

void CPanelTimeline::processNotification(CNotification & notification)
{
    switch(notification.getId())
    {
    case eNotify_Timeout:
    {
        CTimer * pTimer = (CTimer *)notification.getData();
        if (&_timerBuffers == pTimer)
        {
           updateBuffersData();
           if (!_timeSet && !printTime(1).isEmpty())
           {           
              _PlaybackTime->setText(printTime(1).s(), bwidget_justify_horiz_center, bwidget_justify_vert_middle);  
              _timeSet = true;
           }
           _timerBuffers.start();
        }
    }
    break;

    case eNotify_PlaybackStateChanged:
        {
            CPlayback * pPlayback = (CPlayback *)notification.getData();
            MString state = pPlayback->getTrickModeState();
            
            show(true);
            _PlaybackSymbol->setText(state,bwidget_justify_horiz_center, bwidget_justify_vert_middle);

            if (state.strncmp("Stop",4) >= 0)
            {
                _PlaybackMeter->setLevel(0);
                _timeSet = false;
                _PlaybackTime->setText("NA",bwidget_justify_horiz_center, bwidget_justify_vert_middle);
                _PlaybackSymbol->setText("Reset",bwidget_justify_horiz_center, bwidget_justify_vert_middle);
                show(false);
            }
        }
        break;

    default:
        break;
    }

error:
    return;
}

eRet CPanelTimeline::updateBuffersData()
{
    eRet                       ret              = eRet_Ok;
    CSimpleVideoDecode       * pVideoDecode     = _pModel->getSimpleVideoDecode();
    CPlayback                * pPlayback        = _pModel->getPlayback();
    NEXUS_VideoDecoderStatus   nVideoStatus;
    NEXUS_PlaybackStatus       nPlaybackStatus;
    NEXUS_Error                nerror           = NEXUS_SUCCESS;
    uint16_t                   level            = 0;
    uint32_t                   current          = 0; 
    uint32_t                   firstpos         = 0; 
    uint32_t                   lastpos          = 0; 
    
    ret = pVideoDecode->getStatus(&nVideoStatus);
    CHECK_ERROR_GOTO("unable to retrieve video decode status", ret, error);
    
    level = 0;
    if (NULL != pPlayback)
    {
        nerror = NEXUS_Playback_GetStatus(pPlayback->getPlayback(), &nPlaybackStatus);
        CHECK_NEXUS_ERROR_ASSERT("getting playback status failed!", nerror);

        /**
        This code used to be more complicated. It can and should remain simple now.
         If the pos needs to be adjusted for good UI, please do that in the UI code.
         If timestamp is unavailable, please add timestamp approximation
         code.
         **/
        if (!pPlayback->hasIndex())
        {
           /* no index, so use mpeg file data */
           current = SCALE_MPEGPOS(nPlaybackStatus.position);
           firstpos = 0;
           lastpos = SCALE_MPEGPOS(nPlaybackStatus.last);
       }
       else
       {
           current = SCALE_MPEGPOS(nPlaybackStatus.position);
           firstpos = SCALE_MPEGPOS(nPlaybackStatus.first);
           lastpos = SCALE_MPEGPOS(nPlaybackStatus.last);
       }
         BDBG_MSG(("pos %d in %d...%d (from %s)", current, firstpos, lastpos, pPlayback->hasIndex()?"index":"data"));

         if (lastpos == 0 )
         {
            BDBG_WRN(("no data"));
            goto error; 
         }

         BDBG_MSG((" nPlaybackStatus.position get %lu", nPlaybackStatus.position));
         BDBG_MSG(("nPlaybackStatus.last %lu", nPlaybackStatus.last));
         level =  (current* 65535)/lastpos;
         BDBG_MSG(("level %hu" , level));

    }
    _PlaybackMeter->setLevel(level);

    

error:
    return ret;
}


