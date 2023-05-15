/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: adminscreen.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 1/18/12 4:12p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/adminscreen.h $
 * 
 * 10   1/18/12 4:12p jtna
 * SW7425-2213: add deep sleep standby to brutus
 * 
 * 9   6/26/09 2:08p erickson
 * PR56373: rework brutus passive standby to not exit the app
 *
 * 8   3/3/08 11:28a erickson
 * PR40168: add BBS VideoTool
 *
 * 7   2/27/08 6:29p mphillip
 * PR39626: Add Quit Brutus button to admin screen
 *
 * 6   1/31/07 11:31a erickson
 * PR19687: add Standby button to brutus. call settop api functions.
 * enable POWER button to enter Standby.
 *
 * 5   11/30/06 6:41p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 4   8/12/05 10:42a jjordan
 * PR16668: add IP STB config screen to IP STB GUI
 *
 * 3   6/14/05 10:03a erickson
 * PR15831: restored channel scan to admin page with different
 * functionality for magnum vs. legacy
 *
 * 1   2/7/05 7:52p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 ***************************************************************************/
#ifndef ADMINSCREEN_H__
#define ADMINSCREEN_H__

#include "genericscreen.h"
#include "mbutton.h"
#include "mradiobuttongroup.h"
#include "mlineedit.h"
#include "mtimer.h"
#include "mlabel.h"

class AdminScreen : public GenericScreen, MTimerEventHandler {
public:
    AdminScreen(Control *control);

    void enableRunScript();

    // events
    void onClick(MButton *self);
    void onTimeout(MTimer *self);

    // overrides
    void show();

protected:
    MPushButton *_back;

    MPushButton *_changeDateTime,
        *_scanChannels,
        *_runScript, *_editChannels,
        *_ipStbConfig,
        *_showIPs;

    MPushButton *_rereadMP3s, *_deleteAll, *_refreshRecs;
    MPushButton *_toggleDualOutputMode;
    MPushButton *_halt;
    MPushButton *_quit;
    MPushButton *_activeStandby;
    MPushButton *_passiveStandby;
    MPushButton *_deepSleep;
    MPushButton *_videoTool;
    MLabel *_dateTimeLabel, *_upTime;
    MLineEdit *_dateTime;
    MTimer _timer;
    MCheckBox *_stickAdmin;

    void setUpTime();
    int showIpAddresses();
};

#endif
