/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: stc.h $
* $brcm_Revision: 3 $
* $brcm_Date: 4/15/12 9:53p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/stc.h $
* 
* 3   4/15/12 9:53p tokushig
* SW7231-749: added decoder info menu, split notification handling in
* screen_test, and modified main menu button sizes
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef STC_H__
#define STC_H__

#include "nexus_types.h"
#include "nexus_stc_channel.h"

#include "brutus_cfg.h"
#include "resource.h"
#include "pid.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum eStcType 
{
    eStcType_ParserBand,
    eStcType_TtsPacing,
    eStcType_PcrPacing,
    eStcType_IpLivePlayback,
    eStcType_PvrPlayback
} eStcType;

class CStc : public CResource
{
public:
    CStc(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CStc(void);

    eRet open(void);
    void close(void);
    eRet configure(CPid * pPcrPid);

    CPid * getPid(void) { return _pPcrPid; };


    void doit(CPid * pPcrPid);




    eStcType getStcType() { return _stcType; };
    void setStcType(eStcType stcType) { _stcType = stcType; };
    NEXUS_StcChannelHandle getStcChannel() { return _stcChannel; };

protected:
    NEXUS_StcChannelHandle   _stcChannel;
    NEXUS_Timebase           _timebase;
    eStcType                 _stcType;
    CPid                   * _pPcrPid;
};

#ifdef __cplusplus
}
#endif

#endif /* STC_H__ */

