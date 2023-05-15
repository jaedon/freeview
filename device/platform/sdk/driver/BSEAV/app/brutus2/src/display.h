/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: display.h $
* $brcm_Revision: 7 $
* $brcm_Date: 10/8/12 6:47p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/display.h $
* 
* 7   10/8/12 6:47p tokushig
* SW7231-749: add lua commands for display settings
* 
* 6   10/7/12 3:00p tokushig
* SW7231-749: add format change feature and display panel support
* 
* 5   9/15/12 1:05a tokushig
* SW7231-749: remove graphics from display class
* 
* 4   9/10/12 1:47p tokushig
* SW7231-749: add check in for video window
* 
* 3   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef DISPLAY_H__
#define DISPLAY_H__

#include "video_window.h"
#include "output.h"
#include "graphics.h"
#include "bwidgets.h"

#define UINT32_C(c)	c ## U
#include "nexus_display.h"
#include "nexus_graphics2d.h"
#include "nexus_core_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

class CDisplay : public CResource
{
public:
    CDisplay(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CDisplay(void);

    virtual eRet open(void);
    virtual void close(void);
    virtual void dump();

    CVideoWindow * checkoutVideoWindow(void);
    eRet checkinVideoWindow(CVideoWindow * pWindow);
    eRet addOutput(CVideoOutput * pOutput);
    eRet removeOutput(CVideoOutput * pOutput);
    MList <CVideoOutput> * getOutputList(void) { return &_outputList; };
    NEXUS_DisplayHandle getDisplay(void) { return _display; };
    eRet setFramebuffer(NEXUS_SurfaceHandle surface);
    MRect getGraphicsGeometry(void);
    eRet updateGraphicsGeometry();
    eRet setColorSpace(NEXUS_ColorSpace colorSpace);
    eRet setMpaaDecimation(bool bMpaaDecimation);
    eRet setDeinterlacer(bool bDeinterlacer);
    eRet setBoxDetect(bool bBoxDetect);
    eRet setAspectRatio(NEXUS_DisplayAspectRatio aspectRatio);
    eRet setFormat(NEXUS_VideoFormat format, CGraphics * pGraphics);
    NEXUS_VideoFormat getFormat(void);
    void setAutoFormat(bool bAutoFormat) { _bAutoFormat = bAutoFormat; notifyObservers(eNotify_AutoVideoFormatChanged, &bAutoFormat); };
    bool isAutoFormat(void) { return _bAutoFormat; };

protected:
    NEXUS_DisplayHandle             _display;
    MAutoList <CVideoWindow>        _videoWindowList;
    MList <CVideoOutput>            _outputList;
    bool                            _bAutoFormat;
};


#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H__ */
