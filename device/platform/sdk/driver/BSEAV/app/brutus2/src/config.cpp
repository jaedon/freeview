/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: config.cpp $
* $brcm_Revision: 10 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/config.cpp $
* 
* 10   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 9   10/7/12 2:56p tokushig
* SW7231-749: change video format enum to use nexus type
* 
* 8   9/15/12 1:04a tokushig
* SW7231-749: add graphics class
* 
* 7   9/10/12 1:59p tokushig
* SW7231-749: add simple audio/video decoders to resource manager.  for
* now we will add just 1 of each but this can be easily increased as
* necessary in the future.
* 
* 6   5/23/12 5:32p jrubio
* SW7346-644: start to move away from BRUTUS_NUM defines and use
* NEXUS_NUM
* 
* 5   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "config.h"

BDBG_MODULE(brutus_config);

#undef MIN
#define MIN(a,b)    ((a) < (b) ? (a) : (b))

eRet CModelConfig::addChip(CChipInfo info)
{
    eRet        ret       = eRet_Ok;
    CChipInfo * pChipInfo = NULL;

    pChipInfo = new CChipInfo(info);
    BDBG_ASSERT(pChipInfo);
    _chipList.add(pChipInfo);

    return ret;
}

eRet CModelConfig::removeChip(CChipInfo info)
{
    eRet                     ret                = eRet_Ok;
    CChipInfo              * pInfo              = NULL;
    MListItr <CChipInfo>     itr(&_chipList);

    /* find and remove chip info from chip list */
    for (pInfo = itr.first(); pInfo; pInfo = itr.next())
    {
        if (info == *pInfo)
        {
            pInfo = _chipList.remove();
            BDBG_ASSERT(pInfo);
            delete pInfo;
        }
    }

    return ret;
}

void CModelConfig::dump()
{
    MListItr <CChipInfo>     itr(&_chipList);
    CChipInfo              * pInfo              = NULL;

    BDBG_WRN((""));
    BDBG_WRN(("CHIP INFO"));
    for (pInfo = itr.first(); pInfo; pInfo = itr.next())
    {
        pInfo->dump();
    }

    _features.dump();
    _resources.dump();
}

/* MString-ify #define value */
#define MXSTR_DEF(def) MString(XSTR_DEF(def))
/* string-ify #define value */
#define XSTR_DEF(def) STR_DEF(def)
/* MString-ify #define name */
#define MSTR_DEF(def) MString(STR_DEF(def))
/* string-ify #define name */
#define STR_DEF(def) #def

eRet CModelConfig::initChipInfo()
{
    eRet      ret   = eRet_Ok;
    CChipInfo info;

#ifndef BCHP_VER
    #error BCHP_VER support must be added
#endif

    /* add main chip to list */
    info.setNumber(BCHP_CHIP);

    if (eRet_Ok != info.setRevision(MXSTR_DEF(BCHP_VER)))
    {
        ret = BRUTUS_ERROR("BCHP_VER is invalid", eRet_ExternalError);
    }
    else
    {
        addChip(info);
    }

    return ret;
}

NEXUS_VideoFormat CModelConfig::findMaxHdFormat(NEXUS_PlatformSettings platformSettings)
{
    NEXUS_VideoFormat maxHdFormat = NEXUS_VideoFormat_eUnknown;

    /* check platformSettings.displayModuleSettings to see if display supports HD and what max format of HD.
    test the old api first, then test all of the per-window heap settings. the first hit will set the max format. */
    if (platformSettings.displayModuleSettings.fullHdBuffers.count) {
        maxHdFormat = platformSettings.displayModuleSettings.fullHdBuffers.format;
    }
    else if (platformSettings.displayModuleSettings.hdBuffers.count) {
        maxHdFormat = platformSettings.displayModuleSettings.hdBuffers.format;
    }
    else {
        unsigned i;
        for (i=0;i<NEXUS_MAX_HEAPS;i++) {
            if (platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.count) {
                maxHdFormat = platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.format;
                break;
            }
            else if (platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.count) {
                maxHdFormat = platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.format;
            }
        }
    }

    return maxHdFormat;
}

eRet CModelConfig::initFeatures(NEXUS_PlatformSettings platformSettings)
{
    eRet              ret         = eRet_Ok;
    CBoardFeatures    features;
    NEXUS_VideoFormat maxHdFormat = NEXUS_VideoFormat_eUnknown;

    /* set default features */
    _features._pvrEncryption                                           = true;
    _features._videoFormatIsSupported[NEXUS_VideoFormat_eNtsc]         = true;
    _features._videoFormatIsSupported[NEXUS_VideoFormat_ePal]          = true;

    /* set optional features */
#if     B_HAS_SDS || B_HAS_AST                  
    _features._sds           = true; 
#endif
#if     B_HAS_VSB /* DTT || B_HAS_3255*/ || B_HAS_QAM    
    _features._qam           = true; 
#endif
#if     B_HAS_VSB /* DTT || B_HAS_3255*/                 
    _features._vsb           = true; 
#endif
#if     B_HAS_OFDM                              
    _features._ofdm          = true; 
#endif
#if     B_HAS_ANALOG || B_HAS_EXTERNAL_ANALOG   
    _features._analog        = true; 
#endif
#ifndef B_XVD_NO_HD_SUPPORT                     
    _features._videoHd       = true; 
#endif
#ifndef B_DISPLAY_NO_HD_SUPPORT                 
    _features._displayHd     = true; 
#endif
#if     B_HAS_DCR                               
    _features._dnrDcr        = true; 
#endif
#if     B_HAS_BNR                               
    _features._dnrBnr        = true; 
#endif
#if     B_HAS_MNR                               
    _features._dnrMnr        = true; 
#endif
#if     B_HAS_ANR                               
    _features._anr           = true; 
#endif
#if     B_HAS_MAD                               
    _features._deinterlacer  = true; 
#endif
#if     B_HAS_BOX_DETECT                        
    _features._boxDetect     = true; 
#endif
#if     B_HAS_SHARPNESS                         
    _features._sharpness     = true; 
#endif
#if     B_HAS_CAB                               
    _features._cab           = true; 
#endif
#if     B_HAS_LAB                               
    _features._lab           = true; 
#endif
#if     B_MAX_MOSAIC_DECODES > 0                
    _features._mosaic        = true; 
#endif
#if     B_HAS_SECAM                             
    _features._videoFormatIsSupported[bvideo_format_secam] = true; 
#endif
#if     B_HAS_SRS_TRUE_VOLUME
    _features._srsVolume     = true;
#endif
#if     B_HAS_DOLBY_VOLUME
    _features._dolbyVolume   = true;
#endif

    maxHdFormat = findMaxHdFormat(platformSettings);

    /* set display formats based on max_hd_format */
    if (maxHdFormat != NEXUS_VideoFormat_eUnknown) 
    {
        _features._videoFormatIsSupported[NEXUS_VideoFormat_e480p]      = true;
        _features._videoFormatIsSupported[NEXUS_VideoFormat_e576p]      = true;
        _features._videoFormatIsSupported[NEXUS_VideoFormat_e1080i]     = true;
        _features._videoFormatIsSupported[NEXUS_VideoFormat_e1080i50hz] = true;
        _features._videoFormatIsSupported[NEXUS_VideoFormat_e720p]      = true;
        _features._videoFormatIsSupported[NEXUS_VideoFormat_e720p50hz]  = true;
        BDBG_MSG(("HD formats supported"));

        if (maxHdFormat == NEXUS_VideoFormat_e1080p) 
        {
            _features._videoFormatIsSupported[NEXUS_VideoFormat_e1080p]      = true;
            _features._videoFormatIsSupported[NEXUS_VideoFormat_e1080p50hz] = true;
            BDBG_MSG(("1080p50/60 HD formats supported"));
        }

        if (maxHdFormat == NEXUS_VideoFormat_e1080p || 
            maxHdFormat == NEXUS_VideoFormat_e1080p30hz) 
        {
            _features._videoFormatIsSupported[NEXUS_VideoFormat_e1080p24hz] = true;
            _features._videoFormatIsSupported[NEXUS_VideoFormat_e1080p25hz] = true;
            _features._videoFormatIsSupported[NEXUS_VideoFormat_e1080p30hz] = true;
            BDBG_MSG(("1080p24/25/30 HD formats supported"));
        }

        /* DTT do this in subclass for specific chip */
#if 0
        /* need to limit HD display to 480p on 97466 in 32 bit UMA mode
           we may have HD buffers allocated to 480p display */
#if BCM_BOARD==97466 || BCM_BOARD==97468
        if (max_hd_format == NEXUS_VideoFormat_e1080i) {
            BDBG_MSG(("Limiting HD display to 480p on 97466 / 97468"));
            bconfig_.features.video_format_is_supported[bvideo_format_480p] = true;
            bconfig_.features.video_format_is_supported[bvideo_format_576p] = true;
            bconfig_.features.video_format_is_supported[bvideo_format_1080i] = false;
            bconfig_.features.video_format_is_supported[bvideo_format_1080i_50hz] = false;
            bconfig_.features.video_format_is_supported[bvideo_format_720p] = false;
            bconfig_.features.video_format_is_supported[bvideo_format_720p_50hz] = false;
            bconfig_.features.video_format_is_supported[bvideo_format_1080p] = false;
            bconfig_.features.video_format_is_supported[bvideo_format_1080p_25hz]=false;
            bconfig_.features.video_format_is_supported[bvideo_format_1080p_30hz] = false;
        }
#endif
#endif
    }

    return ret;
}


#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#endif
#include "nexus_transport_capabilities.h"

eRet CModelConfig::initResources(NEXUS_PlatformConfiguration platformConfig) 
{
    eRet        ret       = eRet_Ok;

    _resources.add(eBoardResource_display,              NEXUS_NUM_DISPLAYS,                 "display",                &_cfg);
    _resources.add(eBoardResource_graphics,             BRUTUS_NUM_GRAPHICS,                "graphics",               &_cfg);
    _resources.add(eBoardResource_decodeVideo,          NEXUS_NUM_VIDEO_DECODERS,           "videoDecode",            &_cfg);
    _resources.add(eBoardResource_simpleDecodeVideo,    BRUTUS_NUM_SIMPLE_VIDEO_DECODES,    "simpleVideoDecode",      &_cfg);
    _resources.add(eBoardResource_decodeAudio,          NEXUS_NUM_AUDIO_DECODERS,           "audioDecode",            &_cfg);
    _resources.add(eBoardResource_simpleDecodeAudio,    BRUTUS_NUM_SIMPLE_AUDIO_DECODES,    "simpleAudioDecode",      &_cfg);
    _resources.add(eBoardResource_stcChannel,           NEXUS_NUM_STC_CHANNELS,             "stcChannel",             &_cfg);
    _resources.add(eBoardResource_pcmPlayback,          BRUTUS_NUM_PCM_PLAYBACKS,           "pcmPlayback",            &_cfg);
    _resources.add(eBoardResource_pcmCapture,           BRUTUS_NUM_PCM_CAPTURES,            "pcmCapture",             &_cfg);
    _resources.add(eBoardResource_decodeStill,          NEXUS_NUM_STILL_DECODES,            "decodeStill",            &_cfg);
    _resources.add(eBoardResource_decodeEs,             BRUTUS_NUM_ES_DECODES,              "decodeEs",               &_cfg);
    _resources.add(eBoardResource_streamer,             BRUTUS_NUM_STREAMERS,               "streamer",               &_cfg);
    _resources.add(eBoardResource_linein,               BRUTUS_NUM_LINEIN,                  "linein",                 &_cfg);
    _resources.add(eBoardResource_decodeMosaic,         NEXUS_NUM_MOSAIC_DECODES,           "mosaic",                 &_cfg);
    _resources.add(eBoardResource_mixer,                NEXUS_NUM_AUDIO_MIXERS,             "mixer",                  &_cfg);
    _resources.add(eBoardResource_irRemote,             BRUTUS_NUM_IR_REMOTES,              "irRemote",               &_cfg);

#if NEXUS_HAS_FRONTEND
    /* query nexus for frontend capabilities and add to tuner lists */
    {
        NEXUS_PlatformConfiguration platformConfig;
        NEXUS_Platform_GetConfiguration(&platformConfig);

        for (int i = 0; i < NEXUS_MAX_FRONTENDS; i++)
        {
            if (NULL != platformConfig.frontend[i])
            {
                _resources.addFrontend(i, &_cfg);
            }
        }
    }
#endif /* NEXUS_HAS_FRONTEND */

    /* query nexus for transport capabilities and add to resource lists */
    {
        uint16_t numRecPumps = 0;
        NEXUS_TransportCapabilities capabilities;

        NEXUS_GetTransportCapabilities(&capabilities);
        numRecPumps = MIN(capabilities.numRecpumps, 4);  /* limit max number of recpumps to 4 */

        _resources.add(eBoardResource_record,          numRecPumps,                   "record",           &_cfg);
        _resources.add(eBoardResource_playback,        capabilities.numPlaypumps,     "playback",         &_cfg);
        _resources.add(eBoardResource_inputBand,       capabilities.numInputBands,    "inputBand",        &_cfg);
        _resources.add(eBoardResource_parserBand,      capabilities.numParserBands,   "parserBand",       &_cfg);
        /* DTT todo recordTsdma?  recordPes? */
    }

    /* add output resources */
    _resources.add(eBoardResource_outputSpdif,     NEXUS_NUM_SPDIF_OUTPUTS,      "spdifOutput",      &_cfg);
    _resources.add(eBoardResource_outputAudioDac,  NEXUS_NUM_AUDIO_DACS,         "audioDacOutput",   &_cfg);
    _resources.add(eBoardResource_outputComponent, NEXUS_NUM_COMPONENT_OUTPUTS,  "componentOutput",  &_cfg);
    _resources.add(eBoardResource_outputSVideo,    NEXUS_NUM_SVIDEO_OUTPUTS,     "SVideoOutput",     &_cfg);
    _resources.add(eBoardResource_outputComposite, NEXUS_NUM_COMPOSITE_OUTPUTS,  "compositeOutput",  &_cfg);
    _resources.add(eBoardResource_outputHdmi,      NEXUS_NUM_HDMI_OUTPUTS,       "hdmiOutput",       &_cfg);

    /*_resources.dump();*/

    return ret;
}


CModelConfig::CModelConfig() :
    _revision(0)
{
    NEXUS_VideoFormat           maxHdFormat      = NEXUS_VideoFormat_eUnknown;

    _name = "BCM_BOARD_STR";
    _chipList.clear();
    _features.clear();
}

CModelConfig::~CModelConfig()
{
}

eRet CModelConfig::initialize()
{
    NEXUS_PlatformSettings      platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    eRet                        ret              = eRet_Ok;

    NEXUS_Platform_GetSettings(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    ret = initChipInfo();
    CHECK_ERROR_GOTO("error initializing chip info", ret, error);

    ret = initFeatures(platformSettings);
    CHECK_ERROR_GOTO("error initializing board features", ret, error);

    ret = initResources(platformConfig);
    CHECK_ERROR_GOTO("error initializing board resources", ret, error);

    /* update configuration settings from file */
    _cfg.read(GET_STR(&_cfg, CONFIG_FILENAME));

error:
    return ret;
}

