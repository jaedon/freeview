/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: video_window.h $
* $brcm_Revision: 5 $
* $brcm_Date: 10/7/12 3:12p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/video_window.h $
* 
* 5   10/7/12 3:12p tokushig
* SW7231-749: add support for display panel settings
* 
* 4   9/10/12 4:18p tokushig
* SW7231-749: add method to get nexus video window
* 
* 3   4/9/12 7:29p tokushig
* SW7405-5581: added video source changed callback support
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef VIDEO_WINDOW_H__
#define  VIDEO_WINDOW_H__

#include "video_decode.h"
#include "nexus_video_window.h"

#include "mgeom.h"

class CDisplay;

#ifdef __cplusplus
extern "C" {
#endif

class CVideoWindow 
{
public:
    CVideoWindow(const uint16_t number, CDisplay * pDisplay, CConfiguration * pCfg);
    ~CVideoWindow(void);

    eRet open(void);
    void close(void);

    eRet addInput(CVideoDecode * pVideoDecode);
    uint16_t getNumber(void) { return _number; };
    NEXUS_VideoWindowHandle getWindow(void) { return _videoWindow; };
    eRet setDeinterlacer(bool bDeinterlacer);
    eRet setBoxDetect(bool bBoxDetect);

    bool isCheckedOut() { return _checkedOut; };
    void setCheckedOut(bool checkedOut) { _checkedOut = checkedOut; };

protected:
    uint16_t                  _number;
    NEXUS_VideoWindowHandle   _videoWindow;
    CDisplay                * _pDisplay;
    CConfiguration          * _pCfg;
    bool                      _checkedOut;
    MList <CVideoDecode>      _videoDecodeList;

    /* DTT these can probably be replaced with NEXUS_VideoWindowSettings variable?! */
    MRect                     _position;
    uint16_t                  _zOrder;
    bool                      _deinterlacer;
    bool                      _playbackDeinterlacer;
    uint16_t                  _sharpness;
};


#ifdef __cplusplus
}
#endif

#endif /* VIDEO_WINDOW_H__ */
