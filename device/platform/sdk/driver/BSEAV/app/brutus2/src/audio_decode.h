/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: audio_decode.h $
* $brcm_Revision: 9 $
* $brcm_Date: 10/8/12 8:56p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/audio_decode.h $
* 
* 9   10/8/12 8:56p tokushig
* SW7231-749: add audio lua support
* 
* 8   9/29/12 9:20p tokushig
* SW7231-749: connect audio settings window panel to simple audio
* decoder.  pid, spdif, hdmi, downmix, dualmono, dolby drc, and dialog
* norm are implemented.
* 
* 7   9/26/12 4:06p tokushig
* SW7231-749: add ability to set spdif/hdmi output type
* 
* 6   9/21/12 4:11p tokushig
* SW7231-749: fix close/stop in audio decode
* 
* 5   9/10/12 1:56p tokushig
* SW7231-749: add simple audio decoder class.  it is subclassed from
* regular audio decoder class.  it manages it's own resources similar to
* how nexus simple audio decoder does.
* 
* 4   5/25/12 3:12p tokushig
* SW7231-749: remove platform specific file and move to common
* platform.h.  also change appropriate brutus defines to nexus defines
* when available.
* 
* 3   4/15/12 9:41p tokushig
* SW7231-749: added audio source changed callback support
* 
* 2   2/28/12 5:02p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef AUDIO_DECODE_H__
#define AUDIO_DECODE_H__

#include "brutus.h"
#include "brutus_cfg.h"
#include "brutus_os.h"
#include "pid.h"
#include "stc.h"
#include "bwidgets.h"

#include "nexus_simple_audio_decoder_server.h"
#include "nexus_audio_decoder_types.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_capture.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum eSpdifInput
{
    eSpdifInput_Pcm,
    eSpdifInput_Compressed,
    eSpdifInput_EncodeDts,
    eSpdifInput_EncodeAc3,
    eSpdifInput_None,
    eSpdifInput_Max
} eSpdifInput;

typedef enum eHdmiAudioInput
{
    eHdmiAudioInput_Pcm,
    eHdmiAudioInput_Compressed,
    eHdmiAudioInput_Multichannel,
    eHdmiAudioInput_EncodeDts,
    eHdmiAudioInput_EncodeAc3,
    eHdmiAudioInput_None,
    eHdmiAudioInput_Max
} eHdmiAudioInput;

typedef enum eAudioDownmix
{
    eAudioDownmix_None,
    eAudioDownmix_Left,
    eAudioDownmix_Right,
    eAudioDownmix_Monomix,
    eAudioDownmix_Matrix,   /* aac only */
    eAudioDownmix_Arib,     /* aac only */
    eAudioDownmix_LtRt,     /* aac only */
    eAudioDownmix_Max
} eAudioDownmix;

typedef enum eAudioDualMono
{
    eAudioDualMono_Left,
    eAudioDualMono_Right,
    eAudioDualMono_Stereo,
    eAudioDualMono_Monomix,
    eAudioDualMono_Max
} eAudioDualMono;


typedef enum eDolbyDRC
{
    eDolbyDRC_None,
    eDolbyDRC_Light,
    eDolbyDRC_Medium,
    eDolbyDRC_Heavy,
    eDolbyDRC_Max
} eDolbyDRC;


class CAudioDecode : public CResource
{
public:
    CAudioDecode(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CAudioDecode(void);

    virtual eRet   open(bwidget_engine_t widgetEngine, CStc * pStc);
    virtual CStc * close(void);
    virtual eRet   start(CPid * pPid);
    virtual CPid * stop(void);
    virtual CPid * getPid(void) { return _pPid; };
    virtual void setPid(CPid * pPid) { _pPid = pPid; };
    virtual CStc * getStc(void) { return _pStc; };
    virtual bool   isStarted() { return _started; };
    virtual bool   isOpened() { return _decoder ? true : false; };

    virtual NEXUS_AudioDecoderHandle getDecoder(void) { return _decoder; };

    virtual eRet getStatus(NEXUS_AudioDecoderStatus * pStatus);
    NEXUS_AudioInput getConnector(NEXUS_AudioConnectorType type);

    bwidget_engine_t getWidgetEngine(void) { return _widgetEngine; };
    bool isSourceChanged() { return _sourceChanged; };
    void setSourceChanged(bool sourceChanged) { _sourceChanged = sourceChanged; };


protected:
    NEXUS_AudioDecoderHandle    _decoder;
    B_SchedulerCallbackHandle   _sourceChangedCallbackHandle;
    CStc                      * _pStc;
    CPid                      * _pPid;
    bwidget_engine_t            _widgetEngine;
    bool                        _started;
    bool                        _sourceChanged;

#if B_HAS_EXTERNAL_ANALOG
    NEXUS_I2sInputHandle _i2sInput;
#endif
#if B_HAS_DTS_ENCODE
    NEXUS_DtsEncodeHandle _dtsEncoder;
#endif
#if B_HAS_AC3_ENCODE
    NEXUS_Ac3EncodeHandle _ac3Encoder;
#endif
#if B_HAS_SRS_TRUE_VOLUME
    NEXUS_TruVolumeHandle _trueVolume;
#endif
#if B_HAS_DOLBY_VOLUME
    NEXUS_DolbyVolumeHandle _dolbyVolume;
#endif
#if NEXUS_NUM_AUDIO_CAPTURES
    NEXUS_AudioCaptureHandle _audioCapture;
#endif
};


class CBoardResources;
class COutputSpdif;
class COutputHdmi;


class CSimpleAudioDecode : public CAudioDecode
{
public:
    CSimpleAudioDecode(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CSimpleAudioDecode(void);
        
    virtual eRet open(bwidget_engine_t widgetEngine, CStc * pStc);
    virtual CStc * close(void);
    virtual eRet start(CPid * pPid);
    virtual CPid * stop(void);
    virtual void setPid(CPid * pPid);
    virtual bool isOpened() { return _simpleDecoder ? true : false; };

    virtual eRet setDownmix(eAudioDownmix downmix);
    virtual eAudioDownmix getDownmix(void) { return _downmix; };
    virtual eRet setDualMono(eAudioDualMono dualMono);
    virtual eAudioDualMono getDualMono(void) { return _dualMono; };
    virtual eRet setDolbyDRC(eDolbyDRC dolbyDRC);
    virtual eDolbyDRC getDolbyDRC(void) { return _dolbyDRC; };
    virtual eRet setDolbyDialogNorm(bool dolbyDialogNorm);
    virtual bool getDolbyDialogNorm(void) { return _dolbyDialogNorm; };


    void setResources(void * id, CBoardResources * pResources);
    void setOutputHdmi(COutputHdmi * pHdmi) { _pHdmi = pHdmi; };
    eRet getStatus(NEXUS_AudioDecoderStatus * pStatus);

    virtual NEXUS_AudioDecoderHandle getDecoder(void) { BDBG_ASSERT(0); };
    virtual NEXUS_SimpleAudioDecoderHandle getSimpleDecoder(void) { return _simpleDecoder; };
    NEXUS_AudioInput getConnector(uint8_t num, NEXUS_AudioConnectorType type);

    void getSettings(NEXUS_SimpleAudioDecoderServerSettings * pSettings);
    eRet setSettings(NEXUS_SimpleAudioDecoderServerSettings * pSettings);

    eRet setSpdifInputDecoderType(eSpdifInput spdifInput,
                                  NEXUS_SimpleAudioDecoderServerSettings * pSettings = NULL);
    eSpdifInput getSpdifInputDecoderType(void) { return _spdifInput; };

    eRet setHdmiInputDecoderType(eHdmiAudioInput hdmiInput,
                                 NEXUS_SimpleAudioDecoderServerSettings * pSettings = NULL);
    eHdmiAudioInput getHdmiInputDecoderType(void) { return _hdmiInput; };

protected:
    NEXUS_SimpleAudioDecoderHandle   _simpleDecoder;
    CBoardResources                * _pBoardResources;
    CAudioDecode                   * _pDecoders[2];
    COutputSpdif                   * _pSpdif;            /* DTT TODO: should be a list? */
    COutputHdmi                    * _pHdmi;             /* DTT TODO: should be a list? */
    uint16_t                         _numSpdif;
    uint16_t                         _numHdmi;
    void                           * _resourceId;
    eSpdifInput                      _spdifInput;
    eHdmiAudioInput                  _hdmiInput;

    eAudioDownmix                    _downmix;
    eAudioDualMono                   _dualMono;
    eDolbyDRC                        _dolbyDRC;
    bool                             _dolbyDialogNorm;
};

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_DECODE_H__ */
