/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: still_decode.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 5/25/12 3:12p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/still_decode.cpp $
* 
* 3   5/25/12 3:12p tokushig
* SW7231-749: remove platform specific file and move to common
* platform.h.  also change appropriate brutus defines to nexus defines
* when available.
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "still_decode.h"

BDBG_MODULE(brutus_still_decode);


static void stillEventCallback(void * context, int param)
{
    CStillDecode * pStillDecode = (CStillDecode *)context;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != pStillDecode);

    /* bdecode_stop(decode); */
    /* dtt - send notification still decode event */
}

void NexusStillEventCallback(void * context, int param)
{
    CStillDecode * pStillDecode = (CStillDecode *)context;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != pStillDecode);

    if (pStillDecode->getStillEventHandle())
    {
        BDBG_MSG(("Still decode event"));
        B_SchedulerCallback_Fire(pStillDecode->getStillEventHandle());
    }
}

CStillDecode::CStillDecode(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_decodeStill, pCfg),
    _decoder(NULL),
    _pVideoDecode(NULL)
{
    eRet ret = eRet_Ok;
    BDBG_ASSERT(NEXUS_NUM_STILL_DECODES > _number);

    /* create still event callback */
    _stillEventCallbackHandle = B_SchedulerCallback_Create(gScheduler, this, NULL);
    CHECK_PTR_GOTO("Scheduler callback create failed!", _stillEventCallbackHandle, ret, eRet_NotAvailable, error);

    if (B_ERROR_SUCCESS != B_SchedulerCallback_SetFunction(_stillEventCallbackHandle, 
                                                           stillEventCallback, 
                                                           this, 
                                                           0))
    {
        ret = eRet_NotAvailable;
        CHECK_ERROR_GOTO("Scheduler callback set function failed!", ret, error);
    }

error:
    BDBG_ASSERT(eRet_Ok == ret);
}

CStillDecode::~CStillDecode()
{
    if (_stillEventCallbackHandle)
    {
        B_SchedulerCallback_Destroy(_stillEventCallbackHandle);
        _stillEventCallbackHandle = NULL;
    }
}

eRet CStillDecode::open()
{
    eRet ret = eRet_Ok;

    BDBG_ASSERT(NULL != _pVideoDecode);

    if ((false == _pVideoDecode->isOpened()) || (_number != _pVideoDecode->getNumber()))
    {
        ret = eRet_InvalidParameter;
        CHECK_ERROR_GOTO("The corresponding number video decoder must be opened before a still decoder.", ret, error);
    }

    if (true == isOpened())
    {
        ret = eRet_NotAvailable;
        CHECK_ERROR_GOTO("Still decoder is already opened.", ret, error);
    }

    _decoder = NEXUS_StillDecoder_Open(_pVideoDecode->getDecoder(), 0, NULL);
    CHECK_PTR_GOTO("Nexus still decoder open failed!", _decoder, ret, eRet_NotAvailable, error);

    /* DTT - todo still decode callback creation */

error:
    return ret;
}

void CStillDecode::close()
{
    if (_decoder)
    {
        NEXUS_StillDecoder_Stop(_decoder);
        NEXUS_StillDecoder_Close(_decoder);
        _decoder = NULL;
    }
}
