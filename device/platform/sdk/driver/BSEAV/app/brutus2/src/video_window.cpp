/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: video_window.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 10/7/12 3:12p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/video_window.cpp $
* 
* 3   10/7/12 3:12p tokushig
* SW7231-749: add support for display panel settings
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "video_window.h"
#include "display.h"
#include "nexus_video_decoder.h"
#include "nexus_video_adj.h"

BDBG_MODULE(brutus_video_window);

CVideoWindow::CVideoWindow(const uint16_t number, CDisplay * pDisplay, CConfiguration * pCfg) :
    _number(number),
    _videoWindow(NULL),
    _pDisplay(pDisplay),
    _pCfg(pCfg),
    _checkedOut(false),

    _position(0, 0, GET_INT(pCfg, VIDEO_WINDOW_WIDTH), GET_INT(pCfg, VIDEO_WINDOW_HEIGHT)),
    _zOrder(number),
    _deinterlacer(0 == number && 0 == pDisplay->getNumber()), /* only for 1st window on 1st display */
    _playbackDeinterlacer(false),
    _sharpness(0) /* DTT what should this default be? */
{
}

CVideoWindow::~CVideoWindow()
{
    close();
}

eRet CVideoWindow::open()
{
    eRet ret = eRet_Ok;
    BDBG_ASSERT(NULL != _pDisplay);

    _videoWindow = NEXUS_VideoWindow_Open(_pDisplay->getDisplay(), _number);
    CHECK_PTR_GOTO("Nexus video window open failed", _videoWindow, ret, eRet_ExternalError, error);

error:
    return ret;
}

void CVideoWindow::close()
{
    if (NULL != _videoWindow)
    {
        NEXUS_VideoWindow_RemoveAllInputs(_videoWindow);
        _videoDecodeList.clear();
        NEXUS_VideoWindow_Close(_videoWindow);
        _videoWindow = NULL;
    }
}

eRet CVideoWindow::addInput(CVideoDecode * pVideoDecode)
{
    eRet        ret    = eRet_InvalidParameter;  /* assume error */
    NEXUS_Error nerror = NEXUS_SUCCESS;

    BDBG_ASSERT(pVideoDecode);
    BDBG_ASSERT(pVideoDecode->getDecoder());

    nerror = NEXUS_VideoWindow_AddInput(_videoWindow, NEXUS_VideoDecoder_GetConnector(pVideoDecode->getDecoder()));
    CHECK_NEXUS_ERROR_GOTO("Adding decode to video window failed", ret, nerror, error);
    _videoDecodeList.add(pVideoDecode);
    ret = eRet_Ok;

error:
    return ret;
}

eRet CVideoWindow::setDeinterlacer(bool bDeinterlacer)
{
    NEXUS_VideoWindowMadSettings settings;
    eRet                         ret        = eRet_Ok;
    NEXUS_Error                  nerror     = NEXUS_SUCCESS;

    NEXUS_VideoWindow_GetMadSettings(getWindow(), &settings);
    settings.deinterlace = bDeinterlacer;
    nerror = NEXUS_VideoWindow_SetMadSettings(getWindow(), &settings);
    CHECK_NEXUS_ERROR_GOTO("unable to set deinterlacer", ret, nerror, error);

error:
    return ret;
}

eRet CVideoWindow::setBoxDetect(bool bBoxDetect)
{
    NEXUS_VideoWindowSettings settings;
    eRet                      ret        = eRet_Ok;
    NEXUS_Error               nerror     = NEXUS_SUCCESS;

    NEXUS_VideoWindow_GetSettings(getWindow(), &settings);
    settings.letterBoxDetect  = bBoxDetect;
    settings.letterBoxAutoCut = true;
    nerror = NEXUS_VideoWindow_SetSettings(getWindow(), &settings);
    CHECK_NEXUS_ERROR_GOTO("unable to set box detect", ret, nerror, error);

error:
    return ret;
}
