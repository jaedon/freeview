/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: mixer.h $
* $brcm_Revision: 3 $
* $brcm_Date: 9/10/12 2:00p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/mixer.h $
* 
* 3   9/10/12 2:00p tokushig
* SW7231-749: update mixer for audio decoder support
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef MIXER_H__
#define MIXER_H__

#include "resource.h"
#include "nexus_types.h"
#include "nexus_audio_mixer.h"

#ifdef __cplusplus
extern "C" {
#endif


class CMixer : public CResource
{
public:
    CMixer(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CMixer(void);
    
    virtual eRet initialize();
    NEXUS_AudioMixerHandle getMixer() { return _mixer; };
    eRet open(void);
    void close(void);
    eRet connect(CAudioDecode * pDecode);
    eRet disconnect(void);
    NEXUS_AudioInput getConnector();

protected:
    NEXUS_AudioMixerHandle          _mixer;
};


#ifdef __cplusplus
}
#endif

#endif /* MIXER_H__ */
