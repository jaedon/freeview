/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_streamer.cpp $
* $brcm_Revision: 12 $
* $brcm_Date: 11/29/12 6:07p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_streamer.cpp $
* 
* 12   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 11   10/29/12 5:40p jrubio
* SW7346-1071: readd mapInput Band function
* 
* 10   10/19/12 5:43p tokushig
* SW7231-749: ensure tuners are properly checked in/out during tune/scan.
* 
* 9   10/18/12 4:11p jrubio
* SW7346-1071: create streamer mapInputband
* 
* 8   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 7   4/11/12 1:16p jrubio
* SW7346-644: add CPlayback Class, remove CModelSource Class and Replace
* with CPidMgr, add CPidMgr to base Channel Class
* 
* 6   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 5   3/9/12 3:22p jrubio
* SW7346-644: add XML Print
* 
* 4   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "channel_streamer.h"
#include "band.h"

#include "nexus_parser_band.h"

BDBG_MODULE(channel_streamer);


CModelChannelStreamer::CModelChannelStreamer() :
    CModelChannel(eBoardResource_streamer)
{
}

CModelChannelStreamer::CModelChannelStreamer(const CModelChannelStreamer & chStreamer) :
    CModelChannel(chStreamer)
{
    /* copy constructor will not copy any unique objects like tuner */
}

CModelChannelStreamer::~CModelChannelStreamer() 
{
}

CModelChannel * CModelChannelStreamer::createCopy(CModelChannel * pChannel) 
{
    CModelChannelStreamer * pChNew = NULL;

    pChNew = new CModelChannelStreamer();
    return pChNew;
}

eRet CModelChannelStreamer::readXML(MXmlElement * xmlElemChannel)
{
    eRet ret = eRet_Ok;

    CModelChannel::readXML(xmlElemChannel);

    setTransportType(NEXUS_TransportType_eTs);
    _pidMgr.readPidXML(xmlElemChannel);

    return ret;
}

void CModelChannelStreamer::writeXML(MXmlElement * xmlElemChannel)
{
    BDBG_MSG((" Add streamer channel"));
    
    CModelChannel::writeXML(xmlElemChannel);

    /* Create Child PIDS */
    xmlElemChannel->addAttr(XML_ATT_TYPE,"streamer");
    
    _pidMgr.writePidXML(xmlElemChannel);
}


eRet CModelChannelStreamer::initialize(PROGRAM_INFO_T * pProgramInfo)
{
    eRet ret = eRet_Ok;

    BDBG_ASSERT(NULL != pProgramInfo);

    setProgramNum(pProgramInfo->program_number);
    _pidMgr.clearPids();
    ret = _pidMgr.createPids(pProgramInfo);
    CHECK_ERROR_GOTO("unable to create pids from program info", ret, error);

error:
    return ret;
}

eRet CModelChannelStreamer::mapInputBand(CInputBand * pInputBand)
{
    eRet                       ret        = eRet_Ok;
    NEXUS_Error                nerror     = NEXUS_SUCCESS;
    NEXUS_ParserBandSettings   settings;
    NEXUS_ParserBand           band;

    BDBG_ASSERT(NULL != _pParserBand);
    BDBG_ASSERT(NULL != pInputBand);

    band = _pParserBand->getBand();

    NEXUS_ParserBand_GetSettings(band, &settings);
    settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    settings.sourceTypeSettings.inputBand = pInputBand->getBand();
    BDBG_ASSERT((NEXUS_TransportType_eTs     == _transportType) ||
                (NEXUS_TransportType_eDssEs  == _transportType) ||
                (NEXUS_TransportType_eDssPes == _transportType));
    settings.transportType = _transportType;
    nerror = NEXUS_ParserBand_SetSettings(band, &settings);
    CHECK_NEXUS_ERROR_GOTO("error setting parser band settings", ret, nerror, error);

error:
    return ret;
}


eRet CModelChannelStreamer::tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock)
{
    eRet              ret             = eRet_Ok;
    NEXUS_Error       nerror          = NEXUS_SUCCESS;
    NEXUS_InputBand   inputBandIndex  = 0;
    CInputBand      * pInputBand      = NULL;
    CParserBand     * pParserBand     = NULL;
    CBoardResources * pBoardResources = NULL;

    BSTD_UNUSED(bWaitForLock);
    BDBG_ASSERT(NULL != pModelConfig);
    pBoardResources = pModelConfig->getBoardResources();

    nerror = NEXUS_Platform_GetStreamerInputBand(0, &inputBandIndex);
    CHECK_NEXUS_ERROR_GOTO("unable to get streamer input band", ret, nerror, errorInputBand);

    pInputBand = (CInputBand *)pBoardResources->checkoutResource(id, eBoardResource_inputBand, inputBandIndex);
    CHECK_PTR_GOTO("unable to checkout inputband", pInputBand, ret, eRet_NotAvailable, errorInputBand);
    setInputBand(pInputBand);

    pParserBand = (CParserBand *)pBoardResources->checkoutResource(id, eBoardResource_parserBand);
    CHECK_PTR_GOTO("unable to checkout parser band", pParserBand, ret, eRet_NotAvailable, errorParserBand);
    setParserBand(pParserBand);

    ret = mapInputBand(pInputBand);
    CHECK_ERROR_GOTO("error mapping input band to parser band", ret, errorMapping);

    BDBG_MSG(("CH NUMBER: %d.%d", getMajor(), getMinor()));
    _tuned = true;

    goto done;

errorMapping:
    pBoardResources->checkinResource(pParserBand);

errorParserBand:
    pBoardResources->checkinResource(pInputBand);

errorInputBand:
done:
    return ret;
}


eRet CModelChannelStreamer::unTune(CModelConfig * pModelConfig, bool bCheckInTuner)
{
    eRet              ret             = eRet_Ok;
    CBoardResources * pBoardResources = NULL;

    BDBG_ASSERT(NULL != pModelConfig);
    pBoardResources = pModelConfig->getBoardResources();
    
    if ((NULL != _pParserBand) && (_pParserBand->isCheckedOut()))
    {
        pBoardResources->checkinResource(_pParserBand);
        setParserBand(NULL);
    }

    if ((NULL != _pInputBand) && (_pInputBand->isCheckedOut()))
    {
        pBoardResources->checkinResource(_pInputBand);
        setInputBand(NULL);
    }

    _tuned = false;

    return ret;
}
