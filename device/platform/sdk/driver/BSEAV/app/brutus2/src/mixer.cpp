/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: mixer.cpp $
* $brcm_Revision: 4 $
* $brcm_Date: 9/21/12 4:24p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/mixer.cpp $
* 
* 4   9/21/12 4:24p tokushig
* SW7231-749: use decode get connector method
* 
* 3   9/10/12 2:00p tokushig
* SW7231-749: update mixer for audio decoder support
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "mixer.h"
#include "nexus_audio_mixer.h"
#include "audio_decode.h"

BDBG_MODULE(brutus_mixer);


CMixer::CMixer(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_mixer, pCfg),
    _mixer(NULL)
{
}

CMixer::~CMixer()
{
}

eRet CMixer::initialize()
{
    eRet ret = eRet_Ok;
    return ret;
}

eRet CMixer::open()
{
    eRet                     ret        = eRet_Ok;
    NEXUS_AudioMixerSettings settings;

    NEXUS_AudioMixer_GetDefaultSettings(&settings);
    _mixer = NEXUS_AudioMixer_Open(&settings);
    CHECK_PTR_GOTO("Error opening audio mixer", _mixer, ret, eRet_ExternalError, error);

error:
    return ret;
}

void CMixer::close()
{
    if (NULL != _mixer)
    {
        NEXUS_AudioMixer_Close(_mixer);
    }
}


eRet CMixer::connect(CAudioDecode * pDecode)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    BDBG_ASSERT(NULL != pDecode);

    nerror = NEXUS_AudioMixer_AddInput(getMixer(),
                                       pDecode->getConnector(NEXUS_AudioDecoderConnectorType_eStereo));

    CHECK_NEXUS_ERROR_GOTO("error adding input to mixer", ret, nerror, error);

error:
    return ret;
}

NEXUS_AudioInput CMixer::getConnector()
{
    BDBG_ASSERT(_mixer);
    return NEXUS_AudioMixer_GetConnector(_mixer);
}

eRet CMixer::disconnect()
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    nerror = NEXUS_AudioMixer_RemoveAllInputs(getMixer());
    CHECK_NEXUS_ERROR_GOTO("error removing all inputs from mixer", ret, nerror, error);

error:
    return ret;
}
