/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel.h $
* $brcm_Revision: 12 $
* $brcm_Date: 11/29/12 6:07p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel.h $
* 
* 12   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 11   11/15/12 11:04a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 10   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 9   10/11/12 9:36p jrubio
* SW7346-1004: add multiple records
* 
* 8   9/21/12 8:49p tokushig
* SW7231-749: fix merge error
* 
* 7   9/21/12 8:19p tokushig
* SW7231-749: add findPid method
* 
* 6   9/18/12 5:42p jrubio
* SW7346-1004: add isRecording bool
* 
* 5   9/17/12 10:25a jrubio
* SW7346-1004: channel needs record attributes
* 
* 4   8/27/12 9:54a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 3   4/11/12 1:16p jrubio
* SW7346-644: add CPlayback Class, remove CModelSource Class and Replace
* with CPidMgr, add CPidMgr to base Channel Class
* 
* 2   3/19/12 4:25p tokushig
* SW7405-5581: Add support for filtering channel list during scan.
* Ignore encrypted channels, channels with invalid pcr pids, and audio
* only channels.
* 
* 1   3/13/12 4:00p tokushig
* SW7405-5581: more CModelChannel base class out of channelmgr file and
* into channel file
* 
* 
*********************************************************************************/

#ifndef CHANNEL_H__
#define CHANNEL_H__

#include "brutus_os.h"
#include "config.h"
#include "mvc.h"
#include "pidmgr.h"
#include "mxmlelement.h"
#include "tuner.h"

class CInputBand;
class CParserBand;
class CRecord;

#ifdef __cplusplus
extern "C" {
#endif

class CModelChannel : public CMvcModel
{
public:
    CModelChannel(eBoardResource type);
    CModelChannel(const CModelChannel & ch); /* copy constructor */
    virtual CModelChannel * createCopy(CModelChannel * pChannel) = 0;

    virtual eRet initialize(PROGRAM_INFO_T *pProgramInfo) = 0;

    virtual eRet tune(void * id, CModelConfig * pResourceLibrary, bool bWaitForLock) = 0;
    virtual eRet unTune(CModelConfig * pResourceLibrary, bool bCheckInTuner = true) = 0;
    virtual eRet readXML(MXmlElement * xmlElemChannel);
    virtual void writeXML(MXmlElement * xmlElemChannel);

    virtual void dump();

    /*
    virtual eRet initVideoDecode(void);
    virtual eRet initAudioDecode(void);
    virtual eRet startVideoDecode(void);
    virtual eRet starmaketAudioDecode(void);
    */
    eBoardResource getType(void) { return _type; };
    void setMajor(uint16_t major) { _major = major; };
    uint16_t getMajor(void) { return _major; };
    void setMinor(uint16_t minor) { _minor = minor; };
    uint16_t getMinor(void) { return _minor; };
    void setProgramNum(uint16_t programNum) { _programNum = programNum; };
    uint16_t getprogramNum(void) { return _programNum; };
    void setInputBand(CInputBand * pInputBand) { _pInputBand = pInputBand; };
    CInputBand * getInputBand() { return _pInputBand; };
    bool isTuned(void) { return _tuned; };
    void setTransportType(NEXUS_TransportType type) { _transportType = type; }; //move
    void setParserBand(CParserBand * pParserBand) { _pParserBand = pParserBand; }; //move
    CParserBand * getParserBand(void) { return _pParserBand; }; //move
#if NEXUS_HAS_FRONTEND
    CTuner * getTuner(void) { return _pTuner; };
    void setTuner(CTuner * pTuner) { _pTuner = pTuner; };
#endif
    eRet mapInputBand(CInputBand * pInputBand); // used my channels
    CPidMgr * getPidMgr(void) { return &_pidMgr; };
    CPid * getPid(uint16_t index, ePidType type){return _pidMgr.getPid(index,type); }; // Get Pid
    CPid * findPid(uint16_t pidNum, ePidType type) { return _pidMgr.findPid(pidNum, type); };
    bool isEncrypted(void) {return _pidMgr.isEncrypted(); }; //Encrypted
    void setRecord(CRecord * pRecord){_pRecord = pRecord; };
    CRecord * getRecord(void){return _pRecord; };
    bool isRecording(void);

    virtual bool operator==(CModelChannel &other);
    virtual eRet getChannelInfo(CHANNEL_INFO_T * pChanInfo, bool bScanning);
    virtual int addPsiPrograms(CTunerScanCallback addChannelCallback, void * context);

    ~CModelChannel(void);
protected:
    eBoardResource        _type;
    uint16_t              _major;
    uint16_t              _minor;
    uint16_t              _programNum;
    CInputBand          * _pInputBand;
    bool                  _tuned;
    CParserBand         * _pParserBand;
    NEXUS_TransportType   _transportType;
    CPidMgr               _pidMgr;
#if NEXUS_HAS_FRONTEND
    CTuner              * _pTuner;
#endif
    CRecord             * _pRecord;

};


#ifdef __cplusplus
}
#endif

#endif /* CHANNEL_H__ */
