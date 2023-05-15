/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_streamer.h $
* $brcm_Revision: 11 $
* $brcm_Date: 11/29/12 6:07p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_streamer.h $
* 
* 11   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 10   10/29/12 5:40p jrubio
* SW7346-1071: readd mapInput Band function
* 
* 9   10/19/12 5:43p tokushig
* SW7231-749: ensure tuners are properly checked in/out during tune/scan.
* 
* 8   10/18/12 4:11p jrubio
* SW7346-1071: create streamer mapInputband
* 
* 7   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 6   3/13/12 4:00p tokushig
* SW7405-5581: more CModelChannel base class out of channelmgr file and
* into channel file
* 
* 5   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 4   3/9/12 3:22p jrubio
* SW7346-644: add XML Print
* 
* 3   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef CHANNEL_STREAMER_H__
#define CHANNEL_STREAMER_H__

#include "config.h"
#include "channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
    CModelChannelStreamer encapsulates all the functionality specific to a
    Streamer channel.  This includes tune/untune and serializing to/from disk.
*/
class CModelChannelStreamer : public CModelChannel
{
public:
    CModelChannelStreamer(void);
    CModelChannelStreamer(const CModelChannelStreamer & chStreamer);
    ~CModelChannelStreamer(void);
    eRet initialize(PROGRAM_INFO_T * pProgramInfo);
    CModelChannel * createCopy(CModelChannel * pChannel);

    virtual eRet tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock);
    virtual eRet mapInputBand(CInputBand * pInputBand);
    virtual eRet unTune(CModelConfig * pModelConfig, bool bCheckInTuner = true);
    virtual eRet readXML(MXmlElement * xmlElemChannel);
    virtual void writeXML(MXmlElement * xmlElemChannel);

protected:
};


#ifdef __cplusplus
}
#endif

#endif /* CHANNEL_STREAMER_H__ */
