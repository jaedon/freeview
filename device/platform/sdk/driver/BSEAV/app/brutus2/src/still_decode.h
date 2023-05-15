/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: still_decode.h $
* $brcm_Revision: 2 $
* $brcm_Date: 2/28/12 5:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/still_decode.h $
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef STILL_DECODE_H__
#define STILL_DECODE_H__

#include "brutus_os.h"
#include "nexus_still_decoder.h"
#include "resource.h"
#include "video_decode.h"

#ifdef __cplusplus
extern "C" {
#endif


class CStillDecode : public CResource
{
public:
    CStillDecode(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CStillDecode(void);

    virtual eRet open(void);
    virtual void close(void);

    bool isOpened(void) { return _decoder ? true : false; };
    B_SchedulerCallbackHandle getStillEventHandle(void) { return _stillEventCallbackHandle; };
    eRet setVideoDecode(CVideoDecode * pVideoDecode) { _pVideoDecode = pVideoDecode; };

protected:
    NEXUS_StillDecoderHandle    _decoder;
    CVideoDecode              * _pVideoDecode;
    B_SchedulerCallbackHandle   _stillEventCallbackHandle;
};


#ifdef __cplusplus
}
#endif

#endif /* STILL_DECODE_H__ */
