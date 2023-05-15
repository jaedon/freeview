/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: graphics.h $
* $brcm_Revision: 3 $
* $brcm_Date: 10/7/12 3:35p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/graphics.h $
* 
* 3   10/7/12 3:35p tokushig
* SW7231-749: add support for auto format change
* 
* 2   9/17/12 4:02p tokushig
* SW7231-749: use simple compositor for graphics
* 
* 1   9/15/12 1:11a tokushig
* SW7231-749: add graphics class
* 
* 
*********************************************************************************/

#ifndef GRAPHICS_H__
#define GRAPHICS_H__

#include "output.h"
#include "bwidgets.h"

#define UINT32_C(c)	c ## U
#include "nexus_graphics2d.h"
#include "nexus_core_utils.h"
#include "nexus_surface_compositor.h"

#ifdef __cplusplus
extern "C" {
#endif

class CGraphics : public CResource
{
public:
    CGraphics(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CGraphics(void);

    virtual eRet open(void);
    virtual void close(void);
    virtual void dump();

    bwin_engine_t getWinEngine(void) { return _bwinEngine; };
    NEXUS_Graphics2DHandle getBlitter(void) { return _blitter; };
    NEXUS_SurfaceClientHandle getDesktopClient(void) { return _desktopClient; };
    NEXUS_SurfaceHandle getSurface(void) { return _surface; };
    bwin_t getWinFramebuffer(void) { return _winFramebuffer; };
    bwin_font_t getFont(uint8_t size = 12);

    CDisplay * getDisplay(uint8_t num = 0) { return (0 == num) ? _pDisplayPrimary : _pDisplaySecondary; };
    void setDisplays(CDisplay * pDisplayPrimary, CDisplay * pDisplaySecondary) { 
        _pDisplayPrimary = pDisplayPrimary; _pDisplaySecondary = pDisplaySecondary; };
    void setActive(bool bActive);
    bool isActive(void);
    eRet setFramebufferSize(uint16_t width, uint16_t height);

    eRet initGraphics(uint16_t width, uint16_t height, uint16_t backWidth, uint16_t backHeight);
    void uninitGraphics(void);
    void graphicsCheckpoint(void);

    void sync(bwin_framebuffer_t framebuffer);

protected:
    NEXUS_SurfaceClientHandle        _desktopClient;
    NEXUS_SurfaceCompositorHandle    _surfaceCompositor;

    CDisplay                       * _pDisplayPrimary;
    CDisplay                       * _pDisplaySecondary;

    uint16_t                         _graphicsWidth;
    uint16_t                         _graphicsHeight;
    NEXUS_Graphics2DHandle           _blitter;
    B_EventHandle                    _checkpointEvent;
    B_EventHandle                    _inactiveEvent;
    B_EventHandle                    _displayedEvent;
    NEXUS_SurfaceHandle              _surface; 
    MList <NEXUS_SurfaceHandle>      _backSurfaceList; 
    MListItr <NEXUS_SurfaceHandle>   _itrBackSurface;

    bwin_engine_t                    _bwinEngine;
    bwin_font_t                      _font10;
    bwin_font_t                      _font12;
    bwin_font_t                      _font14;
    bwin_font_t                      _font17;
    bwin_framebuffer_t               _framebuffer;
    bwin_t                           _winFramebuffer;
};


#ifdef __cplusplus
}
#endif

#endif /* GRAPHICS_H__ */
