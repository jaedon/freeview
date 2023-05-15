/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: model.h $
* $brcm_Revision: 11 $
* $brcm_Date: 9/21/12 8:42p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/model.h $
* 
* 11   9/21/12 8:42p tokushig
* SW7231-749: add eMode_Record to mode
* 
* 10   9/21/12 8:28p tokushig
* SW7231-749: move mode from control to model
* 
* 9   9/15/12 1:08a tokushig
* SW7231-749: add graphics to model
* 
* 8   9/10/12 6:05p tokushig
* SW7231-749: add remove methods
* 
* 7   9/10/12 2:03p tokushig
* SW7231-749: add simple audio/video decoders to model
* 
* 7   9/10/12 2:02p tokushig
* SW7231-749: add simple audio/video decoders to model
* 
* 6   8/29/12 10:45a jrubio
* SW7346-1004: add record functions
* 
* 5   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 4   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 3   4/19/12 7:52p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef MODEL_H__
#define MODEL_H__

#include "mvc.h"

class CModelChannelMgr;
class CDisplay;
class CGraphics;
class CStc;
class CMixer;
class CVideoDecode;
class CSimpleVideoDecode;
class CAudioDecode;
class CSimpleAudioDecode;
class CIrRemote;
class CPlayback;
class CRecord;
class CPlaybackList;

#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
    eMode_Live,
    eMode_Scan,
    eMode_Playback,
    eMode_Record,
    eMode_Invalid,
    eMode_Max
} eMode;

class CModel : public CMvcModel
{
public:
    CModel(void);

    void setModelChannelMgr(CModelChannelMgr * pModelChannelMgr) { _pModelChannelMgr = pModelChannelMgr; };
    CModelChannelMgr * getModelChannelMgr(void) { return _pModelChannelMgr; };

    void addDisplay(CDisplay * pDisplay) { _pDisplay = pDisplay; };
    void removeDisplay(CDisplay * pDisplay) { _pDisplay = NULL; };
    CDisplay * getDisplay(void) { return _pDisplay; };

    void addGraphics(CGraphics * pGraphics) { _pGraphics = pGraphics; };
    void removeGraphics(CGraphics * pGraphics) { _pGraphics = NULL; };
    CGraphics * getGraphics(void) { return _pGraphics; };

    void addStc(CStc * pStc) { _pStc = pStc; };
    void removeStc(CStc * pStc) { _pStc = NULL; };
    CStc * getStc(void) { return _pStc; };

    void addMixer(CMixer * pMixer) { _pMixer = pMixer; };
    void removeMixer(CMixer * pMixer) { _pMixer = NULL; };
    CMixer * getMixer(void) { return _pMixer; };

    void addVideoDecode(CVideoDecode * pVideoDecode) { _pVideoDecode = pVideoDecode; };
    void removeVideoDecode(CVideoDecode * pVideoDecode) { _pVideoDecode = NULL; };
    CVideoDecode * getVideoDecode(void) { return _pVideoDecode; };

    void addSimpleVideoDecode(CSimpleVideoDecode * pSimpleVideoDecode) { _pSimpleVideoDecode = pSimpleVideoDecode; };
    void removeSimpleVideoDecode(CSimpleVideoDecode * pSimpleVideoDecode) { _pSimpleVideoDecode = NULL; };
    CSimpleVideoDecode * getSimpleVideoDecode(void) { return _pSimpleVideoDecode; };

    void addAudioDecode(CAudioDecode * pAudioDecode) { _pAudioDecode = pAudioDecode; };
    void removeAudioDecode(CAudioDecode * pAudioDecode) { _pAudioDecode = NULL; };
    CAudioDecode * getAudioDecode(void) { return _pAudioDecode; };

    void addSimpleAudioDecode(CSimpleAudioDecode * pSimpleAudioDecode) { _pSimpleAudioDecode = pSimpleAudioDecode; };
    void removeSimpleAudioDecode(CSimpleAudioDecode * pSimpleAudioDecode) { _pSimpleAudioDecode = NULL; };
    CSimpleAudioDecode * getSimpleAudioDecode(void) { return _pSimpleAudioDecode; };

    void addIrRemote(CIrRemote * pIrRemote) { _pIrRemote = pIrRemote; };
    void removeIrRemote(CIrRemote * pIrRemote) { _pIrRemote = NULL; };
    CIrRemote * getIrRemote(void) { return _pIrRemote; };

    void addPlayback(CPlayback * pPlayback) { _pPlayback = pPlayback; };
    void removePlayback(CPlayback * pPlayback) { _pPlayback = NULL; };
    CPlayback * getPlayback(void) { return _pPlayback; };

    void addRecord(CRecord * pRecord) { _pRecord = pRecord; };
    void removeRecord(CRecord * pRecord) { _pRecord = NULL; };
    CRecord * getRecord(void) { return _pRecord; };

    void setPlaybackList(CPlaybackList * pPlaybackList) { _pPlaybackList = pPlaybackList; };
    CPlaybackList * getPlaybackList(void) { return _pPlaybackList; };

    eMode getMode(void) { return _mode; };
    void setMode(eMode mode) { _mode = mode; };

    void setId(void * id) { _id = id; };
    void * getId(void) { return _id; };

protected:
    CModelChannelMgr      * _pModelChannelMgr;
    CDisplay              * _pDisplay;
    CGraphics             * _pGraphics;
    CStc                  * _pStc;
    CMixer                * _pMixer;
    CVideoDecode          * _pVideoDecode;
    CSimpleVideoDecode    * _pSimpleVideoDecode;
    CAudioDecode          * _pAudioDecode;
    CSimpleAudioDecode    * _pSimpleAudioDecode;
    CIrRemote             * _pIrRemote;
    CPlayback             * _pPlayback;
    CRecord               * _pRecord;
    CPlaybackList         * _pPlaybackList;
    eMode                   _mode;
    void                  * _id;
};


#ifdef __cplusplus
}
#endif

#endif /* MODEL_H__ */
