/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: resource.h $
* $brcm_Revision: 5 $
* $brcm_Date: 9/15/12 1:04a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/resource.h $
* 
* 5   9/15/12 1:04a tokushig
* SW7231-749: add graphics class
* 
* 4   9/10/12 4:20p tokushig
* SW7231-749: add simple audio/video decoders to resource manager.  for
* now we will add just 1 of each but this can be easily increased as
* necessary in the future.
* 
* 3   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef RESOURCE_H__
#define RESOURCE_H__

#include "brutus.h"
#include "brutus_cfg.h"
#include "model.h"

#ifdef __cplusplus
extern "C" {
#endif

/* changes made to this enum may require changes to isXXXXX() methods in the CResource class. */
typedef enum eBoardResource
{
    eBoardResource_display,
    eBoardResource_graphics,
    eBoardResource_decodeVideo,
    eBoardResource_simpleDecodeVideo,
    eBoardResource_decodeAudio,
    eBoardResource_simpleDecodeAudio,
    eBoardResource_stcChannel,
    eBoardResource_pcmPlayback,
    eBoardResource_pcmCapture,
    eBoardResource_decodeStill,
    eBoardResource_decodeEs,
    eBoardResource_decodeMosaic,
    eBoardResource_streamer,
#if NEXUS_HAS_FRONTEND
    eBoardResource_frontendQam,
    eBoardResource_frontendVsb,
    eBoardResource_frontendSds,
    eBoardResource_frontendIp,
    eBoardResource_frontendOfdm,
#endif
    eBoardResource_linein,
    eBoardResource_record,
    eBoardResource_recordPes,
    eBoardResource_recordTsdma,
    eBoardResource_playback,
    eBoardResource_encode,
    eBoardResource_inputBand,
    eBoardResource_parserBand,
    eBoardResource_mixer,

    eBoardResource_irRemote,

    eBoardResource_outputSpdif,
    eBoardResource_outputAudioDac,
    eBoardResource_outputComponent,
    eBoardResource_outputSVideo,
    eBoardResource_outputComposite,
    eBoardResource_outputHdmi,

    eBoardResource_count /* invalid */
} eBoardResource;


class CResource : public CMvcModel
{
public:
    CResource(const char * name, const uint16_t number, const eBoardResource type, CConfiguration * pCfg);
    ~CResource(void);


    const char * getName(void) { return _name.s(); };
    uint16_t getNumber(void) { return _number; };
    eBoardResource getType(void) {return _type; };
    void setType(eBoardResource type) { _type = type; };
    bool isOutput(void);
    bool isVideoOutput(void);
    bool isAudioOutput(void);
    bool isFrontend(void);
    void setCheckedOut(bool checkedOut) { _checkedOut = checkedOut; };
    bool isCheckedOut() { return _checkedOut; };
    void setReserved(void * id) { _reserveId = id; };
    bool isReserved() { return (NULL != _reserveId); };
    bool isReservedMatch(void * id) { return (_reserveId == id); };
    bool validReservation(void * id) { return ((_reserveId == id) || (_reserveId == NULL)); };

    virtual eRet initialize();
    virtual void dump(void);

protected:
    MString          _name;
    uint16_t         _number;
    eBoardResource   _type;
    CConfiguration * _pCfg;
    bool             _checkedOut;
    void           * _reserveId;
};


#ifdef __cplusplus
}
#endif

#endif /* #ifndef RESOURCE_H__ */
