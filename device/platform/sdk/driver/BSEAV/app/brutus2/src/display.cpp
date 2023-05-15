/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: display.cpp $
* $brcm_Revision: 13 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/display.cpp $
* 
* 13   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 12   10/7/12 3:00p tokushig
* SW7231-749: add format change feature and display panel support
* 
* 11   9/15/12 1:05a tokushig
* SW7231-749: remove graphics from display class
* 
* 10   9/10/12 1:47p tokushig
* SW7231-749: add check in for video window
* 
* 9   6/13/12 11:14a tokushig
* SW7425-3238: fixed problem with framebuffer back buffer creation on
* 7425.
* 
* 8   5/25/12 3:12p tokushig
* SW7231-749: remove platform specific file and move to common
* platform.h.  also change appropriate brutus defines to nexus defines
* when available.
* 
* 7   5/16/12 11:37a jrubio
* SW7346-644: revert back to verdana fonts
* 
* 6   5/15/12 4:50p jrubio
* SW7346-644: add bmedia to nexus codec functions
* 
* 5   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 4   4/9/12 7:30p tokushig
* SW7405-5581: set up hd graphics.  using 960x540 for easy scaling for
* both 720p and 1080i/p
* 
* 3   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "brutus.h"
#include "display.h"

BDBG_MODULE(brutus_display);


CDisplay::CDisplay(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_display, pCfg),
    _display(NULL),
    _bAutoFormat(false)
{
    NEXUS_PlatformConfiguration platformConfig;
    uint16_t                    maxVideoWindows = 0;

    BDBG_ASSERT(NEXUS_NUM_DISPLAYS > number);

    /* create list of video windows associated with this display */
    NEXUS_Platform_GetConfiguration(&platformConfig);
    maxVideoWindows = platformConfig.numWindowsPerDisplay;

    for (uint16_t i = 0; maxVideoWindows > i; i++)
    {
        _videoWindowList.add(new CVideoWindow(i, this, _pCfg));
    }
}

CDisplay::~CDisplay()
{
    close();
}

void CDisplay::dump()
{
    BDBG_WRN(("<%d>%s:%d (video windows:%d)",
              _type, 
              _name.s(), 
              _number,
              _videoWindowList.total()));
}

eRet CDisplay::open()
{
    NEXUS_DisplaySettings settings;
    eRet                  ret       = eRet_Ok;
    NEXUS_Error           nerror    = NEXUS_SUCCESS;


    NEXUS_Display_GetDefaultSettings(&settings);
    settings.displayType = NEXUS_DisplayType_eAuto;
    settings.format      = NEXUS_VideoFormat_e1080i;

    _display = NEXUS_Display_Open(_number, &settings);
    CHECK_PTR_GOTO("nexus display open failed!", _display, ret, eRet_ExternalError, error);

    {
        NEXUS_VideoFormatInfo  videoFormatInfo;
        NEXUS_GraphicsSettings graphicsSettings;

        NEXUS_VideoFormat_GetInfo(settings.format, &videoFormatInfo);

        NEXUS_Display_GetGraphicsSettings(getDisplay(), &graphicsSettings);
        graphicsSettings.enabled     = true;
        graphicsSettings.clip.width  = videoFormatInfo.width;
        graphicsSettings.clip.height = videoFormatInfo.height;
        nerror = NEXUS_Display_SetGraphicsSettings(getDisplay(), &graphicsSettings);
        CHECK_NEXUS_ERROR_GOTO("Error setting graphics settings", ret, nerror, error);
    }

    return ret;

error:
    if (_display)
    {
        NEXUS_Display_Close(_display);
        _display = NULL;
    }

    return ret;
}

void CDisplay::close()
{
    CVideoWindow            * pVideoWindow                    = NULL;
    MListItr <CVideoWindow>   itrWindows(&_videoWindowList);
    
    NEXUS_Display_RemoveAllOutputs(_display);
    _outputList.clear();

    /* close any open video windows */
    for (pVideoWindow = itrWindows.first(); pVideoWindow; pVideoWindow = itrWindows.next())
    {
        if (true == pVideoWindow->isCheckedOut())
        {
            pVideoWindow->setCheckedOut(false);
        }

        pVideoWindow->close();
    }

    if (NULL != _display)
    {
        NEXUS_Display_Close(_display);
    }
}

static void setEventCallback(void *context, int param)
{
    BSTD_UNUSED(param);
    B_Event_Set((B_EventHandle)context);
}

CVideoWindow * CDisplay::checkoutVideoWindow()
{
    CVideoWindow            * pVideoWindow           = NULL;
    CVideoWindow            * pCheckedOutVideoWindow = NULL;
    MListItr <CVideoWindow>   itr(&_videoWindowList);

    for (pVideoWindow = itr.first(); pVideoWindow; pVideoWindow = itr.next())
    {
        if (false == pVideoWindow->isCheckedOut())
        {
            pCheckedOutVideoWindow = pVideoWindow;
            pCheckedOutVideoWindow->setCheckedOut(true);
            break;
        }
    }

    return pCheckedOutVideoWindow;
}

eRet CDisplay::checkinVideoWindow(CVideoWindow * pWindow)
{
    if (pWindow->isCheckedOut())
    {
        pWindow->setCheckedOut(false);
    }
}

eRet CDisplay::addOutput(CVideoOutput * pOutput)
{
    eRet ret = eRet_InvalidParameter; /* assume error */

    if (pOutput->isVideoOutput())
    {
        NEXUS_Error nerror = NEXUS_SUCCESS;

        nerror = NEXUS_Display_AddOutput(_display, pOutput->getConnector());
        CHECK_NEXUS_ERROR_GOTO("failure to add output", ret, nerror, error);
        _outputList.add(pOutput);
        ret = eRet_Ok;
    }

error:
    return ret;
}

eRet CDisplay::removeOutput(CVideoOutput * pOutput)
{
    eRet ret = eRet_InvalidParameter; /* assume error */

    if (pOutput->isVideoOutput())
    {
        NEXUS_Error nerror = NEXUS_SUCCESS;

        nerror = NEXUS_Display_RemoveOutput(_display, pOutput->getConnector());
        CHECK_NEXUS_ERROR_GOTO("failure to remove output", ret, nerror, error);
        _outputList.remove(pOutput);
        ret = eRet_Ok;
    }

error:
    return ret;
}

eRet CDisplay::setFramebuffer(NEXUS_SurfaceHandle surface)
{
    NEXUS_Error nerror = NEXUS_SUCCESS;
    eRet        ret    = eRet_Ok;

    BDBG_ASSERT(NULL != surface);

    nerror = NEXUS_Display_SetGraphicsFramebuffer(getDisplay(), surface);
    CHECK_NEXUS_ERROR_GOTO("Unable to set graphics framebuffer.", ret, nerror, error);

error:
    return ret;
}

MRect CDisplay::getGraphicsGeometry()
{
    NEXUS_GraphicsSettings graphicsSettings;
    MRect                  rect;

    NEXUS_Display_GetGraphicsSettings(getDisplay(), &graphicsSettings);
    rect.set(graphicsSettings.clip.x,
             graphicsSettings.clip.y,
             graphicsSettings.clip.width,
             graphicsSettings.clip.height);

    return rect;
}

eRet CDisplay::updateGraphicsGeometry()
{
    NEXUS_DisplaySettings  settings;
    NEXUS_VideoFormatInfo  videoFormatInfo;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Error            nerror               = NEXUS_SUCCESS;
    eRet                   ret                  = eRet_Ok;

    NEXUS_Display_GetSettings(getDisplay(), &settings);
    NEXUS_VideoFormat_GetInfo(settings.format, &videoFormatInfo);

    NEXUS_Display_GetGraphicsSettings(getDisplay(), &graphicsSettings);
    graphicsSettings.enabled     = true;
    graphicsSettings.clip.width  = videoFormatInfo.width;
    graphicsSettings.clip.height = videoFormatInfo.height;
    nerror = NEXUS_Display_SetGraphicsSettings(getDisplay(), &graphicsSettings);
    CHECK_NEXUS_ERROR_GOTO("Error setting display graphics settings", ret, nerror, error);
error:
    return ret;
}

eRet CDisplay::setColorSpace(NEXUS_ColorSpace colorSpace)
{
    eRet                      ret               = eRet_Ok;
    CVideoOutput            * pOutput           = NULL;
    MListItr <CVideoOutput>   itr(&_outputList);

    for (pOutput = itr.first(); pOutput; pOutput = itr.next())
    {
        ret = pOutput->setColorSpace(colorSpace);
        CHECK_ERROR_GOTO("unable to set color space", ret, error);
    }

    notifyObservers(eNotify_ColorSpaceChanged, &colorSpace);

error:
    return ret;
}

eRet CDisplay::setMpaaDecimation(bool bMpaaDecimation)
{
    eRet                      ret               = eRet_Ok;
    CVideoOutput            * pOutput           = NULL;
    MListItr <CVideoOutput>   itr(&_outputList);

    for (pOutput = itr.first(); pOutput; pOutput = itr.next())
    {
        ret = pOutput->setMpaaDecimation(bMpaaDecimation);
        CHECK_ERROR_GOTO("unable to set mpaa decimation", ret, error);
    }

    notifyObservers(eNotify_MpaaDecimationChanged, &bMpaaDecimation);

error:
    return ret;
}

eRet CDisplay::setDeinterlacer(bool bDeinterlacer)
{
    eRet                      ret                     = eRet_Ok;
    CVideoWindow            * pVideoWindow            = NULL;
    MListItr <CVideoWindow>   itr(&_videoWindowList);

    /* only set deinterlacing on primary video window */
    pVideoWindow = itr.first();
    
    ret = pVideoWindow->setDeinterlacer(bDeinterlacer);
    CHECK_ERROR_GOTO("unable to set Deinterlacer", ret, error);

    notifyObservers(eNotify_DeinterlacerChanged, &bDeinterlacer);

error:
    return ret;
}

eRet CDisplay::setBoxDetect(bool bBoxDetect)
{
    eRet                      ret                     = eRet_InvalidState;
    CVideoWindow            * pVideoWindow            = NULL;
    MListItr <CVideoWindow>   itr(&_videoWindowList);

    for (pVideoWindow = itr.first(); pVideoWindow; pVideoWindow = itr.next())
    {
        if (NULL != pVideoWindow->getWindow())
        {
            ret = pVideoWindow->setBoxDetect(bBoxDetect);
            CHECK_ERROR_GOTO("unable to set box detect", ret, error);
        }
    }

    CHECK_ERROR_GOTO("unable to set box detect", ret, error);
    notifyObservers(eNotify_BoxDetectChanged, &bBoxDetect);

error:
    return ret;
}

eRet CDisplay::setAspectRatio(NEXUS_DisplayAspectRatio aspectRatio)
{
    eRet                      ret           = eRet_Ok;
    NEXUS_Error               nerror        = NEXUS_SUCCESS;
    NEXUS_DisplaySettings     settings;
    NEXUS_VideoFormatInfo     formatInfo;

    NEXUS_Display_GetSettings(getDisplay(), &settings);

/*
    we could check for HD and default to 16x9, but for now we'll just
    do whatever the caller requests.

    NEXUS_VideoFormat_GetInfo(settings.format, &formatInfo);

    if ((formatInfo.height > 576) || (false == formatInfo.interlaced))
    {
        if ((settings.format != NEXUS_VideoFormat_e480p) ||
            (settings.format != NEXUS_VideoFormat_e576p))
        {
        }
    }
*/

    settings.aspectRatio = aspectRatio;
    nerror = NEXUS_Display_SetSettings(getDisplay(), &settings);
    CHECK_NEXUS_ERROR_GOTO("unable to set aspect ratio", ret, nerror, error);

    notifyObservers(eNotify_AspectRatioChanged, &aspectRatio);

error:
    return ret;
}

eRet CDisplay::setFormat(NEXUS_VideoFormat format, CGraphics * pGraphics)
{
    eRet                      ret           = eRet_Ok;
    NEXUS_Error               nerror        = NEXUS_SUCCESS;
    NEXUS_DisplaySettings     settings;
    NEXUS_VideoFormatInfo     formatInfo;

    pGraphics->setActive(false);

    /* set display format */
    NEXUS_Display_GetSettings(getDisplay(), &settings);
    settings.format = format;
    nerror = NEXUS_Display_SetSettings(getDisplay(), &settings);
    CHECK_NEXUS_ERROR_GOTO("unable to set video format", ret, nerror, error);

    /* update framebuffer size */
    {
        MRect rect;

        rect = getGraphicsGeometry();
        ret = pGraphics->setFramebufferSize(rect.width(), rect.height());
        CHECK_ERROR_GOTO("unable to set frame buffer size", ret, error);
    }

    pGraphics->setActive(true);

    notifyObservers(eNotify_VideoFormatChanged, &format);
error:
    return ret;
}

NEXUS_VideoFormat CDisplay::getFormat()
{
    NEXUS_DisplaySettings     settings;

    BDBG_ASSERT(NULL != getDisplay());

    NEXUS_Display_GetSettings(getDisplay(), &settings);
    return settings.format;
}
