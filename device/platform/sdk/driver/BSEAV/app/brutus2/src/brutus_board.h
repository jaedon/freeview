/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_board.h $
* $brcm_Revision: 12 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus_board.h $
* 
* 12   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 11   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 10   10/7/12 2:48p tokushig
* SW7231-749: change video format enum to use nexus type
* 
* 9   9/18/12 5:39p jrubio
* SW7346-1004: correctly add record resources
* 
* 8   9/15/12 1:04a tokushig
* SW7231-749: add graphics class
* 
* 7   9/10/12 1:58p tokushig
* SW7231-749: add simple audio/video decoders to resource manager.  for
* now we will add just 1 of each but this can be easily increased as
* necessary in the future.
* 
* 6   4/19/12 7:52p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 5   4/11/12 1:15p jrubio
* SW7346-644: add CPlayback Class, remove CModelSource Class and Replace
* with CPidMgr, add CPidMgr to base Channel Class
* 
* 4   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef BRUTUS_BOARD_H__
#define BRUTUS_BOARD_H__

#include "mstring.h"
#include "mlist.h"
#include "brutus.h"
#include "resource.h"
#include "display.h"
#include "graphics.h"
#if NEXUS_HAS_FRONTEND
#include "tuner.h"
#endif
#include "band.h"
#include "mixer.h"
#include "remote.h"
#include "playback.h"
#include "record.h"

#include "nexus_video_types.h"

#ifdef __cplusplus
extern "C" {
#endif

class CChipFamily
{
public:
    CChipFamily(void);
    ~CChipFamily(void);
    bool operator==(const CChipFamily &other);
protected:
    MString  _name;
    MString  _revision;
    char     _major;
    uint16_t _minor;
};


class CChipInfo
{
public:
    CChipInfo(void);
    ~CChipInfo(void);

    eRet setNumber(uint16_t number);
    uint16_t getNumber(void) { return _number; };
    eRet setRevision(const char * revision);
    const char * getRevision(void) { return _revision.s(); };
    char getMajor() { return _major; };
    uint16_t getMinor() { return _minor; };

    eRet addFamily(CChipFamily family);
    eRet removeFamily(CChipFamily family);

    void dump();

    bool operator==(const CChipInfo &other);

protected:
    uint16_t                _number;
    MString                 _revision;
    char                    _major;
    uint16_t                _minor;
    MList <CChipFamily> _familyList;
};


class CBoardFeatures
{
public:
    CBoardFeatures(void);
    ~CBoardFeatures(void);

    void clear();
    void dump();

public:
    bool _sds;            /* system might have SDS */
    bool _qam;            /* system might have QAM */
    bool _vsb;            /* system might have VSB */
    bool _ofdm;           /* system might have DVB-T OFDM */
    bool _analog;         /* system might have analog audio/video source */
    bool _videoHd;        /* system could decode HD video */
    bool _displayHd;      /* system could display HD video */
    bool _dnrDcr;         /* system might have DCR support (digital contour removal) */
    bool _dnrBnr;         /* system might have BNR support (block noise removal) */
    bool _dnrMnr;         /* system might have MNR support (mosquito noise removal)*/
    bool _anr;            /* system might have ANR support (analog noise reduction)*/
    bool _deinterlacer;   /* system might have deinterlacing support */
    bool _boxDetect;      /* system might have letterbox detection support */
    bool _pvrEncryption;  /* system might have PVR encryption */
    bool _sharpness;      /* System might have sharpness control */
    bool _cab;            /* System might have color adjustments [Green stretch, Blue Stretch, Auto Flesh Tome] */
    bool _lab;            /* System might have luma adjustments [Dynamic Contrast] */
    bool _mosaic;         /* System might support mosaic mode */
    bool _dolbyVolume;    /* System might have Dolby volume post processing */
    bool _srsVolume;      /* System might have SRS volume IQ post processing */

    bool _videoFormatIsSupported[NEXUS_VideoFormat_eMax]; /* Array of supported video formats (index w/bvideo_format values) */
};


#define ANY_INDEX 0xFFFF

class CBoardResources
{
public:
    CBoardResources();
    ~CBoardResources();

    eRet add(eBoardResource   resource,
             const uint16_t   numResources, 
             const char     * name,
             CConfiguration * pCfg);
    eRet addFrontend(const uint16_t numTuner, CConfiguration * pCfg);

    CResource * checkoutResource(void * id, eBoardResource resource, uint16_t index = ANY_INDEX);
    eRet checkinResource(CResource * pResource);
    CResource * reserveResource(void * id, eBoardResource resource, uint16_t index = ANY_INDEX);
    CResource * findCheckedoutResource(void * id, eBoardResource resource, uint16_t index = ANY_INDEX);
    bool findResource(void * id, eBoardResource resource, uint16_t index = ANY_INDEX);

    eRet registerObserver(void           * id, 
                          eBoardResource   resource, 
                          CObserver      * pObserver,
                          uint16_t         index        = ANY_INDEX, 
                          eNotification    notification = eNotify_All);
    eRet unregisterObserver(void           * id, 
                            eBoardResource   resource, 
                            CObserver      * pObserver,
                            uint16_t         index        = ANY_INDEX, 
                            eNotification    notification = eNotify_All);

    void dump();
    void dumpList(MList <CResource> *pList);

protected:
    MList <CDisplay>            _displayList;           /* list of displays */
    MList <CGraphics>           _graphicsList;          /* list of graphics */
    MList <CVideoDecode>        _decodeVideoList;       /* list of video decodes */
    MList <CSimpleVideoDecode>  _simpleDecodeVideoList; /* list of simple video decodes */
    MList <CAudioDecode>        _decodeAudioList;       /* list of audio decodes */
    MList <CSimpleAudioDecode>  _simpleDecodeAudioList; /* list of simple audio decodes */
    MList <CStc>                _stcChannelList;        /* list of stc channels */
    MList <CResource>           _pcmPlaybackList;       /* list of PCM playbacks */
    MList <CResource>           _pcmCaptureList;        /* list of PCM capture channels */
    MList <CResource>           _decodeStillList;       /* list of video still picture decodes */
    MList <CResource>           _decodeEsList;          /* list of ES only decoders (used or audio ES decode)*/
    MList <CResource>           _decodeMosaicList;      /* list of mosaic mode decoders */
    MList <CResource>           _streamerList;          /* list of streamer inputs */
#if NEXUS_HAS_FRONTEND
    MList <CTuner>              _tunerList;             /* list of frontends */
#endif
    MList <CResource>           _lineinList;            /* list of line-in */
    MList <CRecord>             _recordList;            /* list of records */
    MList <CResource>           _recordPesList;         /* list of PES records */
    MList <CResource>           _recordTsdmaList;       /* list of TSDMA records */
    MList <CPlayback>           _playbackList;          /* list of playbacks */
    MList <CResource>           _encodeList;            /* list of MPEG encoders */
    MList <CInputBand>          _inputBandList;         /* list of transport input bands */
    MList <CParserBand>         _parserBandList;        /* list of transport parser bands */
    MList <CMixer>              _mixerList;             /* list of audio mixers */

    MList <COutputSpdif>        _outputSpdifList;       /* list of SPDIF outputs */
    MList <COutputAudioDac>     _outputAudioDacList;    /* list of audio dac outputs */
    MList <COutputComponent>    _outputComponentList;   /* list of Component outputs */
    MList <COutputSVideo>       _outputSVideoList;      /* list of SVideo outputs */
    MList <COutputComposite>    _outputCompositeList;   /* list of composite outputs */
    MList <COutputHdmi>         _outputHdmiList;        /* list of Hdmi outputs */

    MList <CIrRemote>           _remoteList;            /* list of input remotes */

    MList <CResource> * _mapResourceList[eBoardResource_count];     /* resource enum to resource list mapping */
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_BOARD_H__ */
