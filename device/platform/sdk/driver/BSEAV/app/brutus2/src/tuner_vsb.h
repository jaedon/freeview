/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner_vsb.h $
* $brcm_Revision: 9 $
* $brcm_Date: 11/13/12 10:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner_vsb.h $
* 
* 9   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 8   11/12/12 3:41p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 7   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 6   8/27/12 10:07a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 5   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 4   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 3   4/19/12 7:35p tokushig
* SW7231-749: fix problem with scan thread where notifications were not
* synchronized with the bwidget main loop.
* 
* 2   4/6/12 10:04a tokushig
* SW7405-5581: advanced scan changes for VSB tuner.
* 
* 1   3/27/12 10:31a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 
*********************************************************************************/

#ifndef TUNER_VSB_H__
#define TUNER_VSB_H__

#include "brutus_cfg.h"
#include "brutus_os.h"
#include "resource.h"
#include "band.h"
#include "tuner.h"
#include "tspsimgr2.h"

#ifdef __cplusplus
extern "C" {
#endif

class CTunerVsbScanData : public CTunerScanData
{
public:
    CTunerVsbScanData(bool appendToChannelList = false, uint16_t majorStartNum = 1) :
        CTunerScanData(eBoardResource_frontendVsb, appendToChannelList, majorStartNum),
        _bandwidth(6000000),
        _mode(-1) {};

    /* assignment operator overload */
    CTunerVsbScanData & operator=(const CTunerVsbScanData & rhs) {
        /* call base class '=' operator method */
        CTunerScanData::operator=(rhs);

        _bandwidth = rhs._bandwidth;
        _mode      = rhs._mode;

        return *this;
    };

    void dump(void);

public:
    int _bandwidth; /* in Hz */

    /* optional parameters (-1 if unspecified) */
    int _mode;
};


class CTunerVsb : public CTuner
{
public:
    CTunerVsb(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CTunerVsb(void);
    
    eRet tune(NEXUS_FrontendVsbSettings settings);
    void unTune(void);
    virtual void saveScanData(CTunerScanData * pScanData);
    virtual void doScan(void);
    NEXUS_FrontendLockStatus isLocked(void);

protected:
    CTunerVsbScanData _scanData;
};

#ifdef __cplusplus
}
#endif

#endif /* TUNER_VSB_H__ */
