/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_board.cpp $
* $brcm_Revision: 18 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus_board.cpp $
* 
* 18   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 17   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 16   10/7/12 2:48p tokushig
* SW7231-749: change video format enum to use nexus type
* 
* 15   9/18/12 5:39p jrubio
* SW7346-1004: correctly add record resources
* 
* 14   9/15/12 1:04a tokushig
* SW7231-749: add graphics class
* 
* 13   9/10/12 1:58p tokushig
* SW7231-749: add simple audio/video decoders to resource manager.  for
* now we will add just 1 of each but this can be easily increased as
* necessary in the future.
* 
* 12   7/24/12 3:19p tokushig
* SW7231-749: fix ofdm tuner discovery and notifications setup
* 
* 11   4/19/12 7:52p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 10   4/11/12 1:14p jrubio
* SW7346-644: add playback file, remove CModelSource Class and Replace
* with CPidMgr
* 
* 9   4/6/12 9:58a tokushig
* SW7231-749: fix error handling when resource is unavailable.
* 
* 8   3/27/12 10:30a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 7   3/14/12 12:39p jrubio
* SW7346-644: add tuner sat
* 
* 6   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "brutus_board.h"
#if NEXUS_HAS_FRONTEND
#include "tuner_qam.h"
#include "tuner_vsb.h"
#include "tuner_sat.h"
#include "tuner_ofdm.h"
#endif /* NEXUS_HAS_FRONTEND */

BDBG_MODULE(brutus_board);


CChipFamily::CChipFamily()
{
}

CChipFamily::~CChipFamily()
{
}

bool CChipFamily::operator==(const CChipFamily &other)
{
    return ((_name == other._name) && (_revision == other._revision));
}



CChipInfo::CChipInfo() :
    _number(0),
    _revision(""),
    _major(0),
    _minor(0)
{
    _familyList.clear();
}

CChipInfo::~CChipInfo()
{
}

eRet CChipInfo::setNumber(uint16_t number)
{
    eRet ret = eRet_Ok;
    _number = number;
    return ret;
}

eRet CChipInfo::setRevision(const char * rev)
{
    eRet    ret      = eRet_Ok;
    MString revision = rev;

    if (12 != revision.length())
    {
        ret = eRet_InvalidParameter;
    }
    else
    {
        _revision = revision;
        _major    = 'A' + strtoul(revision.mid(3, 4), NULL, 10);   /* parse letter from revision */
        _minor    = strtoul(revision.right(1), NULL, 10);          /* parse number from revision */
    }

    return ret;
}

eRet CChipInfo::addFamily(CChipFamily family)
{
    CChipFamily * pChipFamily = NULL;
    eRet          ret         = eRet_Ok;

    pChipFamily = new CChipFamily(family);
    BDBG_ASSERT(pChipFamily);
    _familyList.add(pChipFamily);
    return ret;
}

eRet CChipInfo::removeFamily(CChipFamily family)
{
    eRet                     ret                = eRet_Ok;
    CChipFamily            * pFamily            = NULL;
    MListItr <CChipFamily>   itr(&_familyList);

    /* find and remove family from family list */
    for (pFamily = itr.first(); pFamily; pFamily = itr.next())
    {
        if (family == *pFamily)
        {
            pFamily = _familyList.remove();
            BDBG_ASSERT(pFamily);
            delete pFamily;
        }
    }

    return ret;
}

void CChipInfo::dump()
{
    BDBG_WRN(("chip num:%d chip revisio:1n:%c%d %s", 
              getNumber(), 
              getMajor(),
              getMinor(),
              getRevision()));
}

bool CChipInfo::operator==(const CChipInfo &other)
{
    return ((_number == other._number) && (_revision == other._revision));
}


CBoardFeatures::CBoardFeatures()
{
    clear();
}

CBoardFeatures::~CBoardFeatures()
{
}

void CBoardFeatures::clear()
{
    _sds            = false;
    _qam            = false;
    _vsb            = false;
    _ofdm           = false;
    _analog         = false;
    _videoHd        = false;
    _displayHd      = false;
    _dnrDcr         = false;
    _dnrBnr         = false;
    _dnrMnr         = false;
    _anr            = false;
    _deinterlacer   = false;
    _boxDetect      = false;
    _pvrEncryption  = false;
    _sharpness      = false;
    _cab            = false;
    _lab            = false;
    _mosaic         = false;
    _dolbyVolume    = false;
    _srsVolume      = false;

    memset(_videoFormatIsSupported, false, NEXUS_VideoFormat_eMax);
}

void CBoardFeatures::dump()
{
    BDBG_WRN((""));
    BDBG_WRN(("BOARD FEATURES"));
    BDBG_WRN(("sds:                         %d", _sds));
    BDBG_WRN(("qam:                         %d", _qam));
    BDBG_WRN(("vsb:                         %d", _vsb));
    BDBG_WRN(("ofdm:                        %d", _ofdm));
    BDBG_WRN(("analog:                      %d", _analog));
    BDBG_WRN(("videoHd:                     %d", _videoHd));
    BDBG_WRN(("displayHd:                   %d", _displayHd));
    BDBG_WRN(("dnrDcr:                      %d", _dnrDcr));
    BDBG_WRN(("dnrBnr:                      %d", _dnrBnr));
    BDBG_WRN(("dnrMnr:                      %d", _dnrMnr));
    BDBG_WRN(("anr:                         %d", _anr));
    BDBG_WRN(("deinterlacer:                %d", _deinterlacer));
    BDBG_WRN(("boxDetect:                   %d", _boxDetect));
    BDBG_WRN(("pvrEncryption:               %d", _pvrEncryption));
    BDBG_WRN(("sharpness:                   %d", _sharpness));
    BDBG_WRN(("cab:                         %d", _cab));
    BDBG_WRN(("lab:                         %d", _lab));
    BDBG_WRN(("mosaic:                      %d", _mosaic));
    BDBG_WRN(("dolbyVolume:                 %d", _dolbyVolume));
    BDBG_WRN(("srsVolume:                   %d", _srsVolume));

    BDBG_WRN(("bvideo_format_ntsc:          %d", _videoFormatIsSupported[NEXUS_VideoFormat_eNtsc]));
    BDBG_WRN(("bvideo_format_ntsc_japan:    %d", _videoFormatIsSupported[NEXUS_VideoFormat_eNtscJapan]));
    BDBG_WRN(("bvideo_format_pal_m:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalM]));
    BDBG_WRN(("bvideo_format_pal_n:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalN]));
    BDBG_WRN(("bvideo_format_pal_nc:        %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalN]));
    BDBG_WRN(("bvideo_format_pal_b:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalB]));
    BDBG_WRN(("bvideo_format_pal_b1:        %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalB1]));
    BDBG_WRN(("bvideo_format_pal_d:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalD]));
    BDBG_WRN(("bvideo_format_pal_d1:        %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalD1]));
    BDBG_WRN(("bvideo_format_pal_g:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalG]));
    BDBG_WRN(("bvideo_format_pal:           %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePal]));
    BDBG_WRN(("bvideo_format_pal_h:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalH]));
    BDBG_WRN(("bvideo_format_pal_k:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalK]));
    BDBG_WRN(("bvideo_format_pal_i:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_ePalI]));
    BDBG_WRN(("bvideo_format_secam:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_eSecam]));
    BDBG_WRN(("bvideo_format_480p:          %d", _videoFormatIsSupported[NEXUS_VideoFormat_e480p]));
    BDBG_WRN(("bvideo_format_576p:          %d", _videoFormatIsSupported[NEXUS_VideoFormat_e576p]));
    BDBG_WRN(("bvideo_format_1080i:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_e1080i]));
    BDBG_WRN(("bvideo_format_1080i_50hz:    %d", _videoFormatIsSupported[NEXUS_VideoFormat_e1080i50hz]));
    BDBG_WRN(("bvideo_format_1080p:         %d", _videoFormatIsSupported[NEXUS_VideoFormat_e1080p]));
    BDBG_WRN(("bvideo_format_1080p_24hz:    %d", _videoFormatIsSupported[NEXUS_VideoFormat_e1080p24hz]));
    BDBG_WRN(("bvideo_format_1080p_25hz:    %d", _videoFormatIsSupported[NEXUS_VideoFormat_e1080p25hz]));
    BDBG_WRN(("bvideo_format_1080p_30hz:    %d", _videoFormatIsSupported[NEXUS_VideoFormat_e1080p30hz]));
    BDBG_WRN(("bvideo_format_1080p_50hz:    %d", _videoFormatIsSupported[NEXUS_VideoFormat_e1080p50hz]));
    BDBG_WRN(("bvideo_format_1250i_50hz:    %d", _videoFormatIsSupported[NEXUS_VideoFormat_e1250i50hz]));
    BDBG_WRN(("bvideo_format_720p:          %d", _videoFormatIsSupported[NEXUS_VideoFormat_e720p]));
    BDBG_WRN(("bvideo_format_720p_50hz:     %d", _videoFormatIsSupported[NEXUS_VideoFormat_e720p50hz]));
    BDBG_WRN(("bvideo_format_720p_24hz:     %d", _videoFormatIsSupported[NEXUS_VideoFormat_e720p24hz]));
}



CBoardResources::CBoardResources()
{
    _displayList.clear();
    _mapResourceList[eBoardResource_display]            = (MList <CResource> *)&_displayList;
    _graphicsList.clear();
    _mapResourceList[eBoardResource_graphics]           = (MList <CResource> *)&_graphicsList;
    _decodeVideoList.clear();
    _mapResourceList[eBoardResource_decodeVideo]        = (MList <CResource> *)&_decodeVideoList;
    _simpleDecodeVideoList.clear();
    _mapResourceList[eBoardResource_simpleDecodeVideo]  = (MList <CResource> *)&_simpleDecodeVideoList;
    _decodeAudioList.clear();
    _mapResourceList[eBoardResource_decodeAudio]        = (MList <CResource> *)&_decodeAudioList;
    _simpleDecodeAudioList.clear();
    _mapResourceList[eBoardResource_simpleDecodeAudio]  = (MList <CResource> *)&_simpleDecodeAudioList;
    _stcChannelList.clear();
    _mapResourceList[eBoardResource_stcChannel]         = (MList <CResource> *)&_stcChannelList;
    _pcmPlaybackList.clear();
    _mapResourceList[eBoardResource_pcmPlayback]        = (MList <CResource> *)&_pcmPlaybackList;
    _pcmCaptureList.clear();
    _mapResourceList[eBoardResource_pcmCapture]         = (MList <CResource> *)&_pcmCaptureList;
    _decodeStillList.clear();
    _mapResourceList[eBoardResource_decodeStill]        = (MList <CResource> *)&_decodeStillList;
    _decodeEsList.clear();
    _mapResourceList[eBoardResource_decodeEs]           = (MList <CResource> *)&_decodeEsList;
    _decodeMosaicList.clear();
    _mapResourceList[eBoardResource_decodeMosaic]       = (MList <CResource> *)&_decodeMosaicList;
    _streamerList.clear();
    _mapResourceList[eBoardResource_streamer]           = (MList <CResource> *)&_streamerList;
#if NEXUS_HAS_FRONTEND
    _tunerList.clear();
    _mapResourceList[eBoardResource_frontendQam]        = (MList <CResource> *)&_tunerList;
    _mapResourceList[eBoardResource_frontendVsb]        = (MList <CResource> *)&_tunerList;
    _mapResourceList[eBoardResource_frontendSds]        = (MList <CResource> *)&_tunerList;
    _mapResourceList[eBoardResource_frontendIp]         = (MList <CResource> *)&_tunerList;
    _mapResourceList[eBoardResource_frontendOfdm]       = (MList <CResource> *)&_tunerList;
#endif
    _lineinList.clear();
    _mapResourceList[eBoardResource_linein]             = (MList <CResource> *)&_lineinList;
    _recordList.clear();
    _mapResourceList[eBoardResource_record]             = (MList <CResource> *)&_recordList;
    _recordPesList.clear();
    _mapResourceList[eBoardResource_recordPes]          = (MList <CResource> *)&_recordPesList;
    _recordTsdmaList.clear();
    _mapResourceList[eBoardResource_recordTsdma]        = (MList <CResource> *)&_recordTsdmaList;
    _playbackList.clear();
    _mapResourceList[eBoardResource_playback]           = (MList <CResource> *)&_playbackList;
    _encodeList.clear();
    _mapResourceList[eBoardResource_encode]             = (MList <CResource> *)&_encodeList;
    _inputBandList.clear();
    _mapResourceList[eBoardResource_inputBand]          = (MList <CResource> *)&_inputBandList;
    _parserBandList.clear();
    _mapResourceList[eBoardResource_parserBand]         = (MList <CResource> *)&_parserBandList;
    _mixerList.clear();
    _mapResourceList[eBoardResource_mixer]              = (MList <CResource> *)&_mixerList;
    _outputSpdifList.clear();
    _mapResourceList[eBoardResource_outputSpdif]        = (MList <CResource> *)&_outputSpdifList;
    _outputAudioDacList.clear();
    _mapResourceList[eBoardResource_outputAudioDac]     = (MList <CResource> *)&_outputAudioDacList;
    _outputComponentList.clear();
    _mapResourceList[eBoardResource_outputComponent]    = (MList <CResource> *)&_outputComponentList;
    _outputSVideoList.clear();
    _mapResourceList[eBoardResource_outputSVideo]       = (MList <CResource> *)&_outputSVideoList;
    _outputCompositeList.clear();
    _mapResourceList[eBoardResource_outputComposite]    = (MList <CResource> *)&_outputCompositeList;
    _outputHdmiList.clear();
    _mapResourceList[eBoardResource_outputHdmi]         = (MList <CResource> *)&_outputHdmiList;
    _remoteList.clear();
    _mapResourceList[eBoardResource_irRemote]           = (MList <CResource> *)&_remoteList;

}

CBoardResources::~CBoardResources()
{
}

#if NEXUS_HAS_FRONTEND
/* frontend resources may be added multiple times for a given tuner.  for example, if a tuner can do both vsb and
   qam, it will be added once as a vsb frontend and once as a qam frontend.  the num used for each tuner will
   be identical, and checking out either tuner will automatically check out the other.  This is done because
   only a specific tuning object knows how to tune itself - so even if a tuner can be either qam or vsb, the
   object that represents it can only be one or the other. */
eRet CBoardResources::addFrontend(const uint16_t numTuner, CConfiguration * pCfg)
{
    eRet                          ret            = eRet_Ok;
    CTuner                      * pTuner         = NULL;
    NEXUS_PlatformConfiguration   platformConfig;
    NEXUS_FrontendCapabilities    capabilities;

#if NEXUS_HAS_FRONTEND

    NEXUS_Platform_GetConfiguration(&platformConfig);
    CHECK_PTR_GOTO("invalid tuner number parameter", platformConfig.frontend[numTuner], ret, eRet_InvalidParameter, error);
    NEXUS_Frontend_GetCapabilities(platformConfig.frontend[numTuner], &capabilities);

    BDBG_MSG(("adding Tuner #%d", numTuner));
    BDBG_MSG(("    analog:    %d", capabilities.analog));
    BDBG_MSG(("    ifd:       %d", capabilities.ifd));
    BDBG_MSG(("    qam:       %d", capabilities.qam));
    BDBG_MSG(("    vsb:       %d", capabilities.vsb));
    BDBG_MSG(("    satellite: %d", capabilities.satellite));
    BDBG_MSG(("    outOfBand: %d", capabilities.outOfBand));
    BDBG_MSG(("    upstream:  %d", capabilities.upstream));
    BDBG_MSG(("    ofdm:      %d", capabilities.ofdm));
    BDBG_MSG(("    scan:      %d", capabilities.scan));

    if (true == capabilities.analog)
    {
    }

    if (true == capabilities.qam)
    {
        pTuner = new CTunerQam("tunerQam", numTuner, pCfg);
        CHECK_PTR_GOTO("unable to allocate tuner", pTuner, ret, eRet_OutOfMemory, error);

        pTuner->initialize();
        _tunerList.add(pTuner);
    }

    if (true == capabilities.vsb)
    {
        pTuner = new CTunerVsb("tunerVsb", numTuner, pCfg);
        CHECK_PTR_GOTO("unable to allocate tuner", pTuner, ret, eRet_OutOfMemory, error);

        pTuner->initialize();
        _tunerList.add(pTuner);
    }

    if (true == capabilities.satellite)
    {
        pTuner = new CTunerSat("tunerSat", numTuner, pCfg);
        CHECK_PTR_GOTO("unable to allocate tuner", pTuner, ret, eRet_OutOfMemory, error);

        pTuner->initialize();
        _tunerList.add(pTuner);
    }

    if (true == capabilities.ofdm)
    {
        pTuner = new CTunerOfdm("tunerOfdm", numTuner, pCfg);
        CHECK_PTR_GOTO("unable to allocate tuner", pTuner, ret, eRet_OutOfMemory, error);

        pTuner->initialize();
        _tunerList.add(pTuner);
    }

#endif
error:
    return ret;
}
#endif /* NEXUS_HAS_FRONTEND */


/* add a number of resources to associated list */
eRet CBoardResources::add(eBoardResource   resource,
                          const uint16_t   numResources, 
                          const char     * name,
                          CConfiguration * pCfg)
{
    eRet                          ret               = eRet_Ok;
    CResource                   * pResource         = NULL;
    NEXUS_PlatformConfiguration   platformConfig;

    NEXUS_Platform_GetConfiguration(&platformConfig);

    for (uint16_t i = 0; i< numResources; i++)
    {
        switch(resource)
        {
        case eBoardResource_display:
            /* skip unsupported displays */
            if (platformConfig.supportedDisplay[i])
            {
                CDisplay * pDisplay = NULL;
                pDisplay = new CDisplay(name, i, pCfg);
                BDBG_ASSERT(pDisplay);
                pDisplay->initialize();
                _displayList.add(pDisplay);
            }
            break;

        case eBoardResource_graphics:
            /* skip unsupported graphics */
            if (platformConfig.supportedDisplay[i])
            {
                CGraphics * pGraphics = NULL;
                pGraphics = new CGraphics(name, i, pCfg);
                BDBG_ASSERT(pGraphics);
                pGraphics->initialize();
                _graphicsList.add(pGraphics);
            }
            break;

        case eBoardResource_decodeVideo:
            /* skip unsupported decoders */
            if (platformConfig.supportedDecoder[i])
            {
                CVideoDecode * pVideoDecode = NULL;
                pVideoDecode = new CVideoDecode(name, i, pCfg);
                BDBG_ASSERT(pVideoDecode);
                pVideoDecode->initialize();
                _decodeVideoList.add(pVideoDecode);
            }
            break;

        case eBoardResource_simpleDecodeVideo:
            /* skip unsupported decoders */
            {
                CSimpleVideoDecode * pSimpleVideoDecode = NULL;
                pSimpleVideoDecode = new CSimpleVideoDecode(name, i, pCfg);
                BDBG_ASSERT(pSimpleVideoDecode);
                pSimpleVideoDecode->initialize();
                _simpleDecodeVideoList.add(pSimpleVideoDecode);
            }
            break;

        case eBoardResource_decodeAudio:
            /* skip unsupported decoders */
            {
                CAudioDecode * pAudioDecode = NULL;
                pAudioDecode = new CAudioDecode(name, i, pCfg);
                BDBG_ASSERT(pAudioDecode);
                pAudioDecode->initialize();
                _decodeAudioList.add(pAudioDecode);
            }
            break;

        case eBoardResource_simpleDecodeAudio:
            /* skip unsupported decoders */
            {
                CSimpleAudioDecode * pSimpleAudioDecode = NULL;
                pSimpleAudioDecode = new CSimpleAudioDecode(name, i, pCfg);
                BDBG_ASSERT(pSimpleAudioDecode);
                pSimpleAudioDecode->initialize();
                _simpleDecodeAudioList.add(pSimpleAudioDecode);
            }
            break;

        case eBoardResource_stcChannel:
            {
                CStc * pStc = NULL;
                pStc = new CStc(name, i, pCfg);
                BDBG_ASSERT(pStc);
                pStc->initialize();
                _stcChannelList.add(pStc);
            }
            break;

        case eBoardResource_pcmPlayback:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _pcmPlaybackList.add(pResource);
            }
            break;

        case eBoardResource_pcmCapture:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _pcmCaptureList.add(pResource);
            }
            break;

        case eBoardResource_decodeStill:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _decodeStillList.add(pResource);
            }
            break;

        case eBoardResource_decodeEs:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _decodeEsList.add(pResource);
            }
            break;

        case eBoardResource_decodeMosaic:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _decodeMosaicList.add(pResource);
            }
            break;

        case eBoardResource_streamer:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _streamerList.add(pResource);
            }
            break;

#if NEXUS_HAS_FRONTEND
        case eBoardResource_frontendQam:
        case eBoardResource_frontendVsb:
        case eBoardResource_frontendSds:
        case eBoardResource_frontendIp:
        case eBoardResource_frontendOfdm:
            {
                BDBG_ERR(("use addFrontend() method for adding tuners."));
                BDBG_ASSERT(false);
            }
            break;
#endif
        case eBoardResource_linein:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _lineinList.add(pResource);
            }
            break;

        case eBoardResource_record:
            {
                CRecord * pRecord = NULL;
                pRecord = new CRecord(name, i, pCfg);
                BDBG_ASSERT(pRecord);
                pRecord->initialize();
                _recordList.add(pRecord);
            }
            break;

        case eBoardResource_recordPes:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _recordPesList.add(pResource);
            }
            break;

        case eBoardResource_recordTsdma:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _recordTsdmaList.add(pResource);
            }
            break;

        case eBoardResource_playback:
            {
                CPlayback * pPlayback = NULL;
                pPlayback = new CPlayback(name, i, pCfg);
                BDBG_ASSERT(pPlayback);
                pPlayback->initialize();
                _playbackList.add(pPlayback);
            }
            break;

        case eBoardResource_encode:
            {
                CResource * pResource = NULL;
                pResource = new CResource(name, i, resource, pCfg);
                BDBG_ASSERT(pResource);
                pResource->initialize();
                _encodeList.add(pResource);
            }
            break;

        case eBoardResource_inputBand:
            {
                CInputBand * pInputBand = NULL;
                pInputBand = new CInputBand(name, i, pCfg);
                BDBG_ASSERT(pInputBand);
                pInputBand->initialize();
                _inputBandList.add(pInputBand);
            }
            break;

        case eBoardResource_parserBand:
            {
                CParserBand * pParserBand = NULL;
                pParserBand = new CParserBand(name, i, pCfg);
                BDBG_ASSERT(pParserBand);
                pParserBand->initialize();
                _parserBandList.add(pParserBand);
            }
            break;

        case eBoardResource_mixer:
            {
                CMixer * pMixer = NULL;
                pMixer = new CMixer(name, i, pCfg);
                BDBG_ASSERT(pMixer);
                pMixer->initialize();
                _mixerList.add(pMixer);
            }
            break;

        case eBoardResource_outputSpdif:
            {
                COutputSpdif * pOutputSpdif = NULL;
                pOutputSpdif = new COutputSpdif(name, i, pCfg);
                BDBG_ASSERT(pOutputSpdif);
                pOutputSpdif->initialize();
                _outputSpdifList.add(pOutputSpdif);
            }
            break;

        case eBoardResource_outputAudioDac:
            {
                COutputAudioDac * pOutputAudioDac = NULL;
                pOutputAudioDac = new COutputAudioDac(name, i, pCfg);
                BDBG_ASSERT(pOutputAudioDac);
                pOutputAudioDac->initialize();
                _outputAudioDacList.add(pOutputAudioDac);
            }
            break;

        case eBoardResource_outputComponent:
            {
                COutputComponent * pOutputComponent = NULL;
                pOutputComponent = new COutputComponent(name, i, pCfg);
                BDBG_ASSERT(pOutputComponent);
                pOutputComponent->initialize();
                _outputComponentList.add(pOutputComponent);
            }
            break;

        case eBoardResource_outputSVideo:
            {
                COutputSVideo * pOutputSVideo = NULL;
                pOutputSVideo = new COutputSVideo(name, i, pCfg);
                BDBG_ASSERT(pOutputSVideo);
                pOutputSVideo->initialize();
                _outputSVideoList.add(pOutputSVideo);
            }
            break;

        case eBoardResource_outputComposite:
            {
                COutputComposite * pOutputComposite = NULL;
                pOutputComposite = new COutputComposite(name, i, pCfg);
                BDBG_ASSERT(pOutputComposite);
                pOutputComposite->initialize();
                _outputCompositeList.add(pOutputComposite);
            }
            break;


        case eBoardResource_outputHdmi:
            {
                COutputHdmi * pOutputHdmi = NULL;
                pOutputHdmi = new COutputHdmi(name, i, pCfg);
                BDBG_ASSERT(pOutputHdmi);
                pOutputHdmi->initialize();
                _outputHdmiList.add(pOutputHdmi);
            }
            break;

        case eBoardResource_irRemote:
            {
                CIrRemote * pIrRemote = NULL;
                pIrRemote = new CIrRemote(name, i, pCfg);
                BDBG_ASSERT(pIrRemote);
                pIrRemote->initialize();
                _remoteList.add(pIrRemote);
            }
            break;

        default:
            BDBG_WRN(("Warning: invalid resource add attempt - skipping resource:%d", resource));
            break;
        }
    }

    return ret;
}

/* find resource for a given id - if index == ANY_INDEX then find first available matching resource.
   this method only indicates the resource exists for the given id.  to reteive the resource you must
   reserve it (see reserveResource()) */
bool CBoardResources::findResource(void * id, eBoardResource resource, uint16_t index)
{
    CResource             * pResource           = NULL;
    MList <CResource> * pResourceList       = NULL;
    bool                    bFound              = false;

    BDBG_ASSERT(eBoardResource_count > resource);
    BDBG_ASSERT(0 <= resource);
    BDBG_ASSERT(NULL != id);

    pResourceList = _mapResourceList[resource];
    MListItr <CResource>    itr(pResourceList);

    if (ANY_INDEX == index)
    {
        /* no index specified, so search for first matching resource */
        for (pResource = itr.first(); pResource; pResource = itr.next())
        {
            if (pResource->getType() == resource)
            {
                /* found matching resource type */
                if (true == pResource->validReservation(id))
                {
                    /* not reserved for someone else */
                    bFound = true;
                    break;
                }
            }
        }
    }
    else
    {
        /* index is given so see if it exists for given id */
        pResource = itr.at(index);
        BDBG_ASSERT(NULL != pResource);
        if (pResource->getType() == resource)
        {
            /* resource type matches */
            if (true == pResource->validReservation(id))
            {
                /* found and not reserved */
                bFound = true;
            }
        }
    }

    return bFound;
}

/* reserve resource for a given id - if index == ANY_INDEX then reserve first available matching resource */
CResource * CBoardResources::reserveResource(void * id, eBoardResource resource, uint16_t index)
{
    CResource             * pResource           = NULL;
    CResource             * pReservedResource   = NULL;
    MList <CResource> * pResourceList       = NULL;

    BDBG_ASSERT(eBoardResource_count > resource);
    BDBG_ASSERT(0 <= resource);
    BDBG_ASSERT(NULL != id);

    pResourceList = _mapResourceList[resource];
    MListItr <CResource>    itr(pResourceList);

    if (ANY_INDEX == index)
    {
        /* no index specified, so search for first matching resource */
        for (pResource = itr.first(); pResource; pResource = itr.next())
        {
            if (pResource->getType() == resource)
            {
                /* found matching resource type */
                if ((false == pResource->isCheckedOut()) && (false == pResource->isReserved()))
                {
                    /* we can only reserve resources that are not already checked out or not already reserved*/
                    pReservedResource = pResource;
                    pReservedResource->setReserved(id);
                    break;
                }
            }
        }
    }
    else
    {
        /* index is given so try to reserve it */
        pResource = itr.at(index);
        BDBG_ASSERT(NULL != pResource);
        if (pResource->getType() == resource)
        {
            /* resource type matches */
            if ((false == pResource->isCheckedOut()) && (false == pResource->isReserved()))
            {
                /* we can only reserve resources that are not already checked out or not already reserved*/
                pReservedResource = pResource;
                pReservedResource->setReserved(id);
            }
        }
    }

#if NEXUS_HAS_FRONTEND
    /* special handling for frontends */
    if (pReservedResource->isFrontend())
    {
        /* we've reserved a frontend - now we must check out any other frontends with the same resource
           number since they correspond to the same physical frontend. */
        for (pResource = itr.first(); pResource; pResource = itr.next())
        {
            if (pResource->isFrontend())
            {
                /* found frontend. */
                if ((false == pResource->isCheckedOut()) && (false == pResource->isReserved()))
                {
                    /* the found resource is not checked out and is not reserved for someone else */
                    if (pReservedResource->getNumber() == pResource->getNumber())
                    {
                        /* tuner number matches reserved tuner number so this tuner also corresponds to the
                           same physical tuner - reserve it, too */
                         pResource->setReserved(id);
                    }
                }
            }
        }
    }
#endif

    return pReservedResource;
}

/* find given already checked out resource - if index == ANY_INDEX then checkout first available matching resource */
CResource * CBoardResources::findCheckedoutResource(void * id, eBoardResource resource, uint16_t index)
{
    CResource             * pResource           = NULL;
    CResource             * pCheckedOutResource = NULL;
    MList <CResource> * pResourceList       = NULL;

    BDBG_ASSERT(eBoardResource_count > resource);
    BDBG_ASSERT(0 <= resource);

    pResourceList = _mapResourceList[resource];
    MListItr <CResource>    itr(pResourceList);

    if (ANY_INDEX == index)
    {
        /* no index specified, so search for first matching resource */
        for (pResource = itr.first(); pResource; pResource = itr.next())
        {
            if (pResource->getType() == resource)
            {
                /* found matching resource type */
                if ((true == pResource->isCheckedOut()) && (true == pResource->validReservation(id)))
                {
                    /* found resource is not checked out and is not reserved for someone else
                       so check it out */
                    pCheckedOutResource = pResource;
                    break;
                }
            }
        }
    }
    else
    {
        /* index is given so try to check out */
        pResource = itr.at(index);
        BDBG_ASSERT(NULL != pResource);
        if (pResource->getType() == resource)
        {
            /* resource type matches*/
            if ((true == pResource->isCheckedOut()) && (true == pResource->validReservation(id)))
            {
                /* found resource is not checked out and is not reserved for someone else
                   so check it out */
                pCheckedOutResource = pResource;
            }
        }
    }

    if (NULL != pCheckedOutResource)
    {
        BDBG_ASSERT(resource == pCheckedOutResource->getType());
    }

    return pCheckedOutResource;
}

/* registers the given observer for the given resource type.  if index == ANY_INDEX, the observer will
   be registered for all resources matching both id and resource type. */
eRet CBoardResources::registerObserver(void           * id, 
                                       eBoardResource   resource, 
                                       CObserver      * pObserver, 
                                       uint16_t         resourceIndex, 
                                       eNotification    notification)
{
    eRet                    ret                 = eRet_NotAvailable; /* assume not found */
    CResource             * pResource           = NULL;
    MList <CResource> * pResourceList       = NULL;

    BDBG_ASSERT(eBoardResource_count > resource);
    BDBG_ASSERT(0 <= resource);
    BDBG_ASSERT(NULL != id);
    BDBG_ASSERT(NULL != pObserver);

    pResourceList = _mapResourceList[resource];
    MListItr <CResource>    itr(pResourceList);

    if (ANY_INDEX == resourceIndex)
    {
        /* no resourceIndex specified, so search for all matching resource */
        for (pResource = itr.first(); pResource; pResource = itr.next())
        {
            if ((pResource->getType() == resource) && (pResource->validReservation(id)))
            {
                /* found matching resource type and matching id */
                ret = pResource->registerObserver(pObserver, notification);
            }
        }
    }
    else
    {
        /* resourceIndex is given so try to reserve it */
        pResource = itr.at(resourceIndex);
        BDBG_ASSERT(NULL != pResource);
        if ((pResource->getType() == resource) && (pResource->validReservation(id)))
        {
            /* found matching resource type and matching id */
            ret = pResource->registerObserver(pObserver, notification);
        }
    }

    return ret;
}

/* unregisters the given observer for the given resource type.  if index == ANY_INDEX, the observer will
   be unregistered for all resources matching both id and resource type. */
eRet CBoardResources::unregisterObserver(void           * id, 
                                         eBoardResource   resource, 
                                         CObserver      * pObserver, 
                                         uint16_t         resourceIndex, 
                                         eNotification    notification)
{
    eRet                    ret                 = eRet_NotAvailable; /* assume not found */
    CResource             * pResource           = NULL;
    MList <CResource> * pResourceList       = NULL;

    BDBG_ASSERT(eBoardResource_count > resource);
    BDBG_ASSERT(0 <= resource);
    BDBG_ASSERT(NULL != id);
    BDBG_ASSERT(NULL != pObserver);

    pResourceList = _mapResourceList[resource];
    MListItr <CResource>    itr(pResourceList);

    if (ANY_INDEX == resourceIndex)
    {
        /* no resourceIndex specified, so search for all matching resource */
        for (pResource = itr.first(); pResource; pResource = itr.next())
        {
            if ((pResource->getType() == resource) && (pResource->validReservation(id)))
            {
                /* found matching resource type and matching id */
                ret = pResource->unregisterObserver(pObserver, notification);
            }
        }
    }
    else
    {
        /* resourceIndex is given so try to reserve it */
        pResource = itr.at(resourceIndex);
        BDBG_ASSERT(NULL != pResource);
        if ((pResource->getType() == resource) && (pResource->validReservation(id)))
        {
            /* found matching resource type and matching id */
            ret = pResource->unregisterObserver(pObserver, notification);
        }
    }

    return ret;
}

/* check out given resource - if index == ANY_INDEX then checkout first available matching resource */
CResource * CBoardResources::checkoutResource(void * id, eBoardResource resource, uint16_t index)
{
    eRet                    ret                 = eRet_Ok;
    CResource             * pResource           = NULL;
    CResource             * pCheckedOutResource = NULL;
    MList <CResource> * pResourceList       = NULL;

    BDBG_ASSERT(eBoardResource_count > resource);
    BDBG_ASSERT(0 <= resource);

    pResourceList = _mapResourceList[resource];
    MListItr <CResource>    itr(pResourceList);

    if (ANY_INDEX == index)
    {
        /* no index specified, so search for first matching resource */
        for (pResource = itr.first(); pResource; pResource = itr.next())
        {
            if ((pResource->getType() == resource))
            {
                /* found matching resource type OR we are checking out a frontend. */
                if ((false == pResource->isCheckedOut()) && (true == pResource->validReservation(id)))
                {
                    /* the found resource is not checked out and is not reserved for someone else
                       so check it out */
                    pCheckedOutResource = pResource;
                    pCheckedOutResource->setCheckedOut(true);

                    break;
                }
            }
        }
    }
    else
    {
        /* index is given so try to check out */
        pResource = itr.at(index);
        BDBG_ASSERT(NULL != pResource);
        if (pResource->getType() == resource)
        {
            /* resource type matches*/
            if ((false == pResource->isCheckedOut()) && (true == pResource->validReservation(id)))
            {
                /* found resource is not checked out and is not reserved for someone else
                   so check it out */
                pCheckedOutResource = pResource;
                pCheckedOutResource->setCheckedOut(true);
            }
        }
    }

    CHECK_PTR_GOTO("resource unavailable for checkout", pCheckedOutResource, ret, eRet_NotAvailable, error);
    BDBG_ASSERT(resource == pCheckedOutResource->getType());
    
#if NEXUS_HAS_FRONTEND
    /* special handling for frontends */
    if (pCheckedOutResource->isFrontend())
    {
        /* we've checked out a frontend - now we must check out any other frontends with the same resource
           number since they correspond to the same physical frontend. */
        for (pResource = itr.first(); pResource; pResource = itr.next())
        {
            if (pResource->isFrontend())
            {
                /* found frontend. */
                if ((false == pResource->isCheckedOut()) && (true == pResource->validReservation(id)))
                {
                    /* the found resource is not checked out and is not reserved for someone else */
                    if (pCheckedOutResource->getNumber() == pResource->getNumber())
                    {
                        /* frontend number matches found frontend number so this frontend also corresponds to the
                           same physical frontend - check it out, too */
                         pResource->setCheckedOut(true);
                    }
                }
            }
        }
    }
#endif

    goto done;
error:
    BDBG_ERR(("resource type: %d", resource));
done:
    return pCheckedOutResource;
}

eRet CBoardResources::checkinResource(CResource * pResource)
{
    eRet        ret                = eRet_Ok;
    CResource * pCheckedInResource = NULL;

    BDBG_ASSERT(NULL != pResource);
    BDBG_ASSERT(true == pResource->isCheckedOut());

    pResource->setCheckedOut(false);
    pCheckedInResource = pResource;

#if NEXUS_HAS_FRONTEND
    /* special handling for frontends */
    if (pCheckedInResource->isFrontend())
    {
        CResource             * pResourceFound = NULL;
        MList <CResource> * pResourceList  = NULL;

        pResourceList = _mapResourceList[pCheckedInResource->getType()];
        
        MListItr <CResource>    itr(pResourceList);

        /* we've checked in a frontend - now we must check in any other frontends with the same resource
           number since they correspond to the same physical frontend. */
        for (pResourceFound = itr.first(); pResourceFound; pResourceFound = itr.next())
        {
            if (pResourceFound->isFrontend())
            {
                /* found frontend. */
                if (true == pResourceFound->isCheckedOut())
                {
                    /* the found resource is checked out */
                    if (pCheckedInResource->getNumber() == pResourceFound->getNumber())
                    {
                        /* frontend number matches found frontend number so this frontend also corresponds to the
                           same physical frontend - check it in, too */
                         pResourceFound->setCheckedOut(false);
                    }
                }
            }
        }
    }
#endif

    return ret;
}

void CBoardResources::dumpList(MList <CResource> * pList)
{
    MListItr <CResource>    itr(pList);
    CResource             * pResource = NULL;

    for (pResource = itr.first(); pResource; pResource = itr.next())
    {
        pResource->dump();
    }
}

void CBoardResources::dump()
{
    BDBG_WRN((""));
    BDBG_WRN(("RESOURCES"));;
    BDBG_WRN(("displays:                    %d", _displayList.total()));
    dumpList((MList <CResource> *)&_displayList);
    BDBG_WRN(("decodes (video):             %d", _decodeVideoList.total()));
    dumpList((MList <CResource> *)&_decodeVideoList);
    BDBG_WRN(("decodes (audio):             %d", _decodeAudioList.total()));
    dumpList((MList <CResource> *)&_decodeAudioList);
    BDBG_WRN(("decodes (simple audio):      %d", _simpleDecodeAudioList.total()));
    dumpList((MList <CResource> *)&_simpleDecodeAudioList);
    BDBG_WRN(("stc channels:                %d", _stcChannelList.total()));
    dumpList((MList <CResource> *)&_stcChannelList);
    BDBG_WRN(("PCM playbacks:               %d", _pcmPlaybackList.total()));
    dumpList(&_pcmPlaybackList);
    BDBG_WRN(("PCM captures:                %d", _pcmCaptureList.total()));
    dumpList(&_pcmCaptureList);
    BDBG_WRN(("decodes (still picture):     %d", _decodeStillList.total()));
    dumpList(&_decodeStillList);
    BDBG_WRN(("decodes (ES):                %d", _decodeEsList.total()));
    dumpList(&_decodeEsList);
    BDBG_WRN(("decodes (mosaic):            %d", _decodeMosaicList.total()));
    dumpList(&_decodeMosaicList);
    BDBG_WRN(("streamers:                   %d", _streamerList.total()));
    dumpList(&_streamerList);
#if NEXUS_HAS_FRONTEND
    BDBG_WRN(("tuners:                      %d", _tunerList.total()));
    dumpList((MList <CResource> *)&_tunerList);
#endif
    BDBG_WRN(("line-ins:                    %d", _lineinList.total()));
    dumpList(&_lineinList);
    BDBG_WRN(("records:                     %d", _recordList.total()));
    dumpList((MList <CResource> *)&_recordList);
    BDBG_WRN(("PES records:                 %d", _recordPesList.total()));
    dumpList(&_recordPesList);
    BDBG_WRN(("TSDMA records:               %d", _recordTsdmaList.total()));
    dumpList(&_recordTsdmaList);
    BDBG_WRN(("playbacks:                   %d", _playbackList.total()));
    dumpList((MList <CResource> *)&_playbackList);
    BDBG_WRN(("MPEG encoders:               %d", _encodeList.total()));
    dumpList(&_encodeList);
    BDBG_WRN(("transport input bands:       %d", _inputBandList.total()));
    dumpList((MList <CResource> *)&_inputBandList);
    BDBG_WRN(("transport parser bands:      %d", _parserBandList.total()));
    dumpList((MList <CResource> *)&_parserBandList);
    BDBG_WRN(("SPDIF output:                %d", _outputSpdifList.total()));
    dumpList((MList <CResource> *)&_outputSpdifList);
    BDBG_WRN(("Audio DAC output:            %d", _outputAudioDacList.total()));
    dumpList((MList <CResource> *)&_outputAudioDacList);
    BDBG_WRN(("Component output:            %d", _outputComponentList.total()));
    dumpList((MList <CResource> *)&_outputComponentList);
    BDBG_WRN(("SVideo output:               %d", _outputSVideoList.total()));
    dumpList((MList <CResource> *)&_outputSVideoList);
    BDBG_WRN(("Composite output:            %d", _outputCompositeList.total()));
    dumpList((MList <CResource> *)&_outputCompositeList);
    BDBG_WRN(("Hdmi output:                 %d", _outputHdmiList.total()));
    dumpList((MList <CResource> *)&_outputHdmiList);
    BDBG_WRN(("Remote:                      %d", _remoteList.total()));
    dumpList((MList <CResource> *)&_remoteList);
}


