/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner_qam.h $
* $brcm_Revision: 12 $
* $brcm_Date: 11/13/12 10:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner_qam.h $
* 
* 12   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 11   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 10   8/27/12 10:07a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 9   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 8   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 7   4/19/12 7:35p tokushig
* SW7231-749: fix problem with scan thread where notifications were not
* synchronized with the bwidget main loop.
* 
* 6   4/5/12 11:40a tokushig
* SW7405-5581: optimization for 7405 qam scan
* 
* 5   3/30/12 9:37p tokushig
* SW7231-749: modify fast qam scan and test on 97231
* 
* 4   3/27/12 10:30a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 3   3/9/12 2:58p tokushig
* SW7405-5581: added fast scan capability for qam scan - not supported on
* 7405 but scan will still work.  further testing required on 7231.
* 
* 2   3/1/12 3:56p tokushig
* SW7405-5581: moved qam scan into separate thread to enable screen
* updates while scan is running.  added on screen scan progress to test.
* NOTE: this does not yet have protection against conflicts so trying to
* channel up/down while scan is running will result in a crash
* 
* 1   2/27/12 4:17p tokushig
* SW7405-5581: break out qam tuner from tuner files
* 
* 
*********************************************************************************/

#ifndef TUNER_QAM_H__
#define TUNER_QAM_H__

#include "brutus_cfg.h"
#include "brutus_os.h"
#include "resource.h"
#include "band.h"
#include "tuner.h"
#include "tspsimgr2.h"

#ifdef __cplusplus
extern "C" {
#endif

class CTunerQamScanData : public CTunerScanData
{
public:
    CTunerQamScanData(bool appendToChannelList = false, uint16_t majorStartNum = 1) :
        CTunerScanData(eBoardResource_frontendQam, appendToChannelList, majorStartNum),
        _bandwidth(6000000),
        _mode(-1),
        _annex(-1) {};

    /* assignment operator overload */
    CTunerQamScanData & operator=(const CTunerQamScanData & rhs) {
        /* call base class '=' operator method */
        CTunerScanData::operator=(rhs);

        _bandwidth = rhs._bandwidth;
        _mode      = rhs._mode;
        _annex     = rhs._annex;

        return *this;
    };

    void dump(void);

public:
    int _bandwidth; /* in Hz */

    /* optional parameters (-1 if unspecified) */
    int _mode;
    int _annex;
};


class CTunerQam : public CTuner
{
public:
    CTunerQam(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CTunerQam(void);
    
    eRet tune(NEXUS_FrontendQamSettings settings);
    void unTune(void);
    virtual void saveScanData(CTunerScanData * pScanData);
    virtual void doScan(void);

    NEXUS_FrontendLockStatus isLocked(void);
    NEXUS_FrontendQamMode getDefaultMode(void);

protected:
    CTunerQamScanData _scanData;

};

#ifdef __cplusplus
}
#endif

#endif
