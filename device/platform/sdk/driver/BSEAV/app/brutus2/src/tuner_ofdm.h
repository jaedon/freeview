/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner_ofdm.h $
* $brcm_Revision: 5 $
* $brcm_Date: 11/13/12 10:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner_ofdm.h $
* 
* 5   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 4   11/12/12 3:41p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 3   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 2   8/27/12 10:07a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 1   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 
*********************************************************************************/

#ifndef TUNER_OFDM_H__
#define TUNER_OFDM_H__

#include "brutus_cfg.h"
#include "brutus_os.h"
#include "resource.h"
#include "band.h"
#include "tuner.h"
#include "tspsimgr2.h"

#ifdef __cplusplus
extern "C" {
#endif

class CTunerOfdmScanData : public CTunerScanData
{
public:
    CTunerOfdmScanData(bool appendToChannelList = false, uint16_t majorStartNum = 1) :
        CTunerScanData(eBoardResource_frontendOfdm, appendToChannelList, majorStartNum),
        _bandwidth(6000000),
        _mode(-1) {};

    /* assignment operator overload */
    CTunerOfdmScanData & operator=(const CTunerOfdmScanData & rhs) {
        /* call base class '=' operator method */
        CTunerScanData::operator=(rhs);

        _bandwidth = rhs._bandwidth;
        _mode      = rhs._mode;

        return *this;
    };

    void dump(void);

public:
    int _bandwidth; /* in Hz */
    int _mode;
};


class CTunerOfdm : public CTuner
{
public:
    CTunerOfdm(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CTunerOfdm(void);
    
    eRet tune(NEXUS_FrontendOfdmSettings settings);
    void unTune(void);
    virtual void saveScanData(CTunerScanData * pScanData);
    virtual void doScan(void);
    NEXUS_FrontendLockStatus isLocked(void);
    NEXUS_FrontendOfdmMode getDefaultMode(void);
    eRet getChannelInfo(CParserBand * pBand, CHANNEL_INFO_T * pChanInfo, bool bScanning);

protected:
    CTunerOfdmScanData _scanData;
};

#ifdef __cplusplus
}
#endif

#endif /* TUNER_OFDM_H__ */
