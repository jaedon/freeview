/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner_sat.h $
* $brcm_Revision: 8 $
* $brcm_Date: 11/13/12 10:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner_sat.h $
* 
* 8   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 7   11/12/12 3:41p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 6   8/27/12 10:07a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 5   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 4   4/19/12 7:35p tokushig
* SW7231-749: fix problem with scan thread where notifications were not
* synchronized with the bwidget main loop.
* 
* 3   4/11/12 1:19p jrubio
* SW7346-644: add Peak Scan functionality
* 
* 2   3/27/12 10:31a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 1   3/14/12 12:48p jrubio
* SW7346-644: add tuner sat
*
* 
*********************************************************************************/

#ifndef TUNER_SAT_H__
#define TUNER_SAT_H__

#include "brutus_cfg.h"
#include "brutus_os.h"
#include "resource.h"
#include "tuner.h"
#include "channel_sat.h"

#ifdef __cplusplus
extern "C" {
#endif



class CTunerSatScanData : public CTunerScanData
{
public:
    CTunerSatScanData(bool appendToChannelList = false, uint16_t majorStartNum = 1) :
        CTunerScanData(eBoardResource_frontendSds, appendToChannelList, majorStartNum),
        _mode(-1){};

    /* assignment operator overload */
    CTunerSatScanData & operator=(const CTunerSatScanData & rhs) {
        /* call base class '=' operator method */
        CTunerScanData::operator=(rhs);

        _mode      = rhs._mode;

        return *this;
    };

    void dump(void);

public:
    /* optional parameters (-1 if unspecified) */
    int _mode;
};


class CTunerSat : public CTuner
{
public:
    CTunerSat(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CTunerSat (void);
    
    eRet tune(satSettings settings);
    void unTune(void);
    virtual void saveScanData(CTunerScanData * pScanData);
    virtual void doScan(void);
    virtual CModelChannel * createChannel(void);
    virtual void destroyChannel(CModelChannel * pChannel);
    NEXUS_FrontendLockStatus isLocked(void);

    NEXUS_FrontendSatellitePeakscanResult _scanThread_psResult;
    BKNI_EventHandle     _scanThread_psCallbackEvent;

protected:
    CTunerSatScanData _scanData;
};

#ifdef __cplusplus
}
#endif

#endif
