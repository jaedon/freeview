/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: adminscreen.cpp $
 * $brcm_Revision: 27 $
 * $brcm_Date: 9/10/12 11:45a $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/adminscreen.cpp $
 * 
 * 27   9/10/12 11:45a mward
 * SW7125-905:  Move 7125-specific feature branch.
 * 
 * 26   1/18/12 4:11p jtna
 * SW7425-2213: add deep sleep standby to brutus
 * 
 * 25   7/11/11 3:34p mward
 * SW7125-968:  Set QAM parameters from first entry in channel map when
 * going into active standby (as done in Phase 6.0 reference software).
 * 
 * 24   4/6/11 2:30p gmohile
 * SW7420-1792 : Add power standby support
 * 
 * 23   7/2/09 5:47p erickson
 * PR56373: move standby logic to Control where it can be called from
 * multiple spots in the UI
 *
 * 22   6/26/09 2:08p erickson
 * PR56373: rework brutus passive standby to not exit the app
 *
 * 21   8/18/08 9:45a ahulse
 * PR44902: Add StandbyActive mode to brutus shutdown
 *
 * 20   7/31/08 1:10p ahulse
 * PR44901: Quit button renamed to StandbyPassive if POWER_ENABLED=y
 * enviro variable set
 *
 * 19   7/31/08 10:07a ahulse
 * PR44909: Change Halt button name to Shutdown
 *
 * 18   3/3/08 3:46p erickson
 * PR40168: added VIDEO_TOOL brutus.cfg optoin
 *
 * 17   3/3/08 11:28a erickson
 * PR40168: add BBS VideoTool
 *
 * 16   2/27/08 6:29p mphillip
 * PR39626: Add Quit Brutus button to admin screen
 *
 * 15   3/14/07 10:15a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 * 14   1/31/07 11:31a erickson
 * PR19687: add Standby button to brutus. call settop api functions.
 * enable POWER button to enter Standby.
 *
 * 13   11/30/06 6:41p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 12   9/26/06 11:27a erickson
 * PR24374: convert to DUAL_CMP_SUPPORT, remove bad check
 *
 * 10   12/19/05 6:08p rjlewis
 * PR18712: disabled button.
 *
 * 9   12/19/05 5:27p rjlewis
 * PR17612: Added support for displaying IP address under vxworks.
 *
 * 8   8/24/05 4:20p arbisman
 * PR16678: Add support for WinCE
 *
 * 7   8/12/05 10:42a jjordan
 * PR16668: add IP STB config screen to IP STB GUI
 *
 * 6   6/14/05 10:03a erickson
 * PR15831: restored channel scan to admin page with different
 * functionality for magnum vs. legacy
 *
 * 3   3/29/05 10:33p erickson
 * PR14593: renamed Back button
 *
 * 2   2/23/05 2:44p erickson
 * PR14180: use bsettop_os.h for portability
 *
 * 1   2/7/05 7:51p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 ****************************************************************************/
#include "adminscreen.h"
#include "cfgsettings.h"
#include "channelmgr.h"
#include "control.h"
#include "brutus_power.h"

#ifdef _WIN32_WCE
    #include <Winsock2.h>
#else
    #include <time.h>
    #include <unistd.h>
    #include <errno.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h> //inet_ntop
#endif

#include "mmessagebox.h"
#include "bsettop_os.h"

// these must follow the previous #include's
#ifdef LINUX
#include <linux/sockios.h>
#include <linux/if.h>
#endif

#ifdef AUDIO_SUPPORT
#include "audioscreen.h"
#define AUDIO() ((AudioScreen*)control()->screen(Control::eAudio))
#endif
#include "runscriptscreen.h"
#include "brutus_video_tool.h"

#include "bstd.h"
BDBG_MODULE(brutus_adminscreen);

/**
* Setting the date/time on the set-top is problematic.
* First, there are timezone issues which I haven't dealt with.
* Second, MApplication::tickCount() depends on system time.
* It really shouldn't, but this requires some redesign.
**/
//#define ALLOW_SET_TIME

static char *makeDateTimeLabel() {
    static char buf[64];
#ifndef _WIN32_WCE
    time_t ti = time(NULL);
    struct tm *t = localtime(&ti);
    sprintf(buf, "%d/%d/%d %d:%02d %s",
        t->tm_mon+1,
        t->tm_mday,
        t->tm_year+1900,
        (t->tm_hour==0)?12:t->tm_hour%12,
        t->tm_min,
        (t->tm_hour>12)?"pm":"am");
#else
    SYSTEMTIME sysTime;
    GetSystemTime(&sysTime);
    sprintf(buf, "%d/%d/%d %d:%02d %s",
    sysTime.wMonth,
    sysTime.wDay,
    sysTime.wYear,
    (sysTime.wHour==0)?12:sysTime.wHour%12,
    sysTime.wMinute,
    (sysTime.wHour>12)?"pm":"am"
    );
#endif /* _WIN32_WCE */
    return buf;
}

// this comes from main.cpp which is built every time
extern const char *buildDate();

AdminScreen::AdminScreen(Control *acontrol) :
    GenericScreen(acontrol, NOTV), _timer(acontrol->fb()->app())
{
    int x = 60-38;
    int y = 25;

    new MLabel(this, MRect(x,y,700,30), MString("Admin Screen                        Built on ") + buildDate());

    y += 30;
    _upTime = new MLabel(this, MRect(x,y,350,40));
    setUpTime();

    y += 30;
    _dateTimeLabel = new MLabel(this, MRect(x,y,175,30), "System Date/Time");
    y += 30;
    _dateTime = new MLineEdit(this, MRect(x,y,200,40));
    _dateTime->setEnabled(false);
#ifdef ALLOW_SET_TIME
    y += 45;
    _changeDateTime = new MPushButton(this, MRect(x, y, 100, 40), "Set");
    _changeDateTime->addHandler(this);
#endif
    _dateTime->setText(makeDateTimeLabel());
    y += 50;
    _stickAdmin = new MCheckBox(this, MRect(x, y, 200, 30), "Admin Mode");
    _stickAdmin->addHandler(this);

#define ADMIN_BTNWIDTH 170
#define ADMIN_BTNHEIGHT 40

    //////////////////////////////////////////
    // first column
    x += 230;
    y = 55;
    _runScript = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Run Script");
    _runScript->addHandler(this);
    /* We disable this until brutus_app.cpp enables it. */
    _runScript->setEnabled(false);

    y += ADMIN_BTNHEIGHT+10;
    _scanChannels = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Scan Channels");
    _scanChannels->addHandler(this);
    y += ADMIN_BTNHEIGHT+10;
    _editChannels = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Edit Channels");
    _editChannels->addHandler(this);
#if 0
#ifdef DUAL_CMP_SUPPORT
    if (!cfg()->getBool(Config::SECOND_DISPLAY)) // fixed so 2nd display doesn't show this button
    {
        y += ADMIN_BTNHEIGHT+10;
        _toggleDualOutputMode = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT),
            control()->pipEnabled()?"DUAL OUTPUT":"PIP MODE");
        _toggleDualOutputMode->addHandler(this);
    }
#endif
#endif

    y += ADMIN_BTNHEIGHT+10;
    _activeStandby = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Active Standby");
    _activeStandby->addHandler(this);

    y += ADMIN_BTNHEIGHT+10;
    _passiveStandby = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Passive Standby");
    _passiveStandby->addHandler(this);

    y += ADMIN_BTNHEIGHT+10;
    _deepSleep = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Deep Sleep");
    _deepSleep->addHandler(this);

    y += ADMIN_BTNHEIGHT+10;
    _quit = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Quit Brutus");
    _quit->addHandler(this);

    //////////////////////////////////////////
    // second column

    x += ADMIN_BTNWIDTH + 20;
    y = 55;

    // by default, the buttons are not there
    _deleteAll = NULL;
    _refreshRecs = NULL;
    _rereadMP3s = NULL;

    _halt = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Halt");
    _halt->addHandler(this);
    y += ADMIN_BTNHEIGHT+10;

    // allocate buttons based on compile time and run time options
#ifdef PVR_SUPPORT
    if (cfg()->getBool(Config::PLAYBACK_ENABLED)) {
        _deleteAll = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Delete Streams");
        _deleteAll->addHandler(this);
        y += ADMIN_BTNHEIGHT+10;
        _refreshRecs = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Refresh Streams");
        _refreshRecs->addHandler(this);
        y += ADMIN_BTNHEIGHT+10;
    }
#endif
#ifdef AUDIO_SUPPORT
    if (cfg()->getBool(Config::MP3_ENABLED)) {
        _rereadMP3s = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Refresh MP3's");
        _rereadMP3s->addHandler(this);
        y += ADMIN_BTNHEIGHT+10;
    }
#endif
    _showIPs = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Show IP's");
    _showIPs->addHandler(this);
    y += ADMIN_BTNHEIGHT+10;

    _videoTool = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Video Tool");
    _videoTool->addHandler(this);
    y += ADMIN_BTNHEIGHT+10;

    _ipStbConfig = new MPushButton(this, MRect(x,y,ADMIN_BTNWIDTH,ADMIN_BTNHEIGHT), "Configure IP STB");
    _ipStbConfig->addHandler(this);

    x = 60-38;
    _back = new MPushButton(this, MRect(x,375,120,ADMIN_BTNHEIGHT), "Home");
    _back->addHandler(this);

    _timer.addHandler(this);
    _timer.start(15 * 1000, MTimer::eForever);
}

void AdminScreen::show() {
    // This isn't in the constructor because main.cpp sets it
    // afterwards.
    _stickAdmin->setChecked(control()->cfg()->getBool(Config::ADMIN));

    GenericScreen::show();
}

void AdminScreen::onTimeout(MTimer *self) {
    if (!_dateTime->hasFocus())
        _dateTime->setText(makeDateTimeLabel());
    setUpTime();
}

void AdminScreen::setUpTime() {
    static int init = 0;
    static b_time_t tv1;
    b_time_t tv2;
    int diff, hr, min;
    char buf[50];

    if (!init) {
        b_time_get(&tv1);
        init = 1;
    }
    b_time_get(&tv2);
    diff = b_time_diff(&tv2, &tv1);

    hr = diff / 1000 / 60 / 60;
    min = (diff / 1000 % (60*60)) / 60;

    sprintf(buf, "Up time %d:%02d (h:m)", hr, min);
    _upTime->setText(buf);
}

void AdminScreen::onClick(MButton *button) {
    if (button == _back)
        control()->showScreen(Control::eHome);
#ifdef PVR_SUPPORT
    else if (button == _deleteAll) {
        if (MMessageBox::show(fb(), "All PVR will be stopped and all streams will be permanently deleted. Are you sure?",
            MMessageBox::bYes|MMessageBox::bNo,
            MMessageBox::bNo) == MMessageBox::bYes)
        {
            int deleted, remaining;
            char buf[200];

            /* Need a dialog because it can take a while */
            MMessageBox *msg = new MMessageBox(fb(), 300, 100);
            msg->set("Deleting...", 0);
            msg->show();
            msg->drawNow();

            control()->stopAllPvr();
            control()->videos()->refreshFromDisk();
            deleted = control()->videos()->total();
            control()->videos()->deleteAll();
            /* if we can't delete from disk, delete() will fail. */
            remaining = control()->videos()->total();
            deleted -= remaining;
            control()->setChannel(control()->channel());

            delete msg;

            int len = sprintf(buf, "%d stream%s deleted.", deleted, deleted==1?"":"s");
            if (remaining) {
                sprintf(&buf[len], " %d stream%s could not be deleted.",
                    remaining, remaining==1?"":"s");
            }
            MMessageBox::info(fb(), buf);
        }
    }
    else if (button == _refreshRecs) {
        control()->stopAllPvr();
        control()->videos()->refreshFromDisk();
        MMessageBox::info(fb(), "Streams refreshed");
    }
#endif
#ifdef ALLOW_SET_TIME
    else if (button == _changeDateTime) {
        int mon, day, year, hour, min;
        char buf[256];
        if (sscanf(_dateTime->text(), "%d/%d/%d %d:%d %s",
            &mon, &day, &year, &hour, &min, buf) == 6)
        {
            struct tm t;
            memset(&t, 0, sizeof(t));
            t.tm_mon = mon-1;
            t.tm_mday = day;
            t.tm_year = year-1900;
            if (strstr(buf,"PM") || strstr(buf,"pm")) {
                hour += 12;
            }
            if (hour == 12)
                hour = 0;
            else
                hour--;
            printf("setting hour %d\n", hour);
            t.tm_hour = hour;
            t.tm_min = min;

            time_t ti = mktime(&t);
            if (stime(&ti))
                MMessageBox::info(fb(), "You don't have permission to change the date/time.");
            else {
                MMessageBox::info(fb(), "Date and time have been set.");
                _dateTime->setText(makeDateTimeLabel());
            }
        }
        else {
            MMessageBox::info(fb(), "Invalid date/time");
        }
    }
#endif
    else if (button == _scanChannels) {
        control()->scan();
    }
#ifdef AUDIO_SUPPORT
    else if (button == _rereadMP3s) {
        AUDIO()->readDisk();
        MMessageBox::info(fb(), "MP3's refreshed");
    }
#endif
#ifdef SCRIPT_SUPPORT
    else if (button == _runScript) {
        control()->showScreen(Control::eRunScript);
    }
#endif
    else if (button == _editChannels) {
        control()->showScreen(Control::eEditChannels);
    }
    else if (button == _ipStbConfig) {
        control()->showScreen(Control::eIpStbConfig);
    }
    else if (button == _stickAdmin) {
        control()->cfg()->set(Config::ADMIN, _stickAdmin->checked()?1:0);
    }
    else if (button == _showIPs) {
        showIpAddresses();
    }
    else if (button == _videoTool) {
        launchVideoTool();
    }
    else if (button == _activeStandby) {
        control()->standby(brutus_standby_state_eActiveStandby);
    }
    else if (button == _passiveStandby) {
        control()->standby(brutus_standby_state_ePassiveStandby);
    }
    else if (button == _deepSleep) {
        control()->standby(brutus_standby_state_eDeepSleep);
    }
    else if (button == _quit) {
        control()->shutdown(brutus_shutdown_state_eQuit);
    }
    else if (button == _halt) {
        control()->shutdown(brutus_shutdown_state_eHalt);
    }
    else {
        GenericScreen::onClick(button);
    }
}

#ifdef __vxworks
#include <hostLib.h>
#endif

int AdminScreen::showIpAddresses()
{
    char buf[500];
#ifdef __vxworks
    //hostTblInit(); // isn't this done in the BSP?
    if (ERROR == gethostname(buf, sizeof(buf))) return -1;
    uint32_t ip = (uint32_t)hostGetByName(buf);
    //printf("getHostName = %s, ip = 0x%x\n", buf, ip);
    sprintf(buf, "%d.%d.%d.%d", ip>>24, (ip>>16)&0xff, (ip>>8)&0xff, ip&0xff);

#elif defined(LINUX)
    int skfd;
    struct ifreq *ifr;
    struct ifconf ifc;
    struct sockaddr_in *sin;
    int n;
    int len = 0;

    skfd = socket(AF_INET, SOCK_STREAM, 0);
    if (skfd < 0)
        return -1;
    ifc.ifc_len =  sizeof(struct ifreq) * 20;
    ifc.ifc_buf = (char*)malloc(ifc.ifc_len);
    if (ioctl(skfd, SIOCGIFCONF, &ifc)) {
        printf("SIOCGIFCONF error %d\n", errno);
        close(skfd);
        return -1;
    }

    ifr=ifc.ifc_req;
    for (n=0; n < ifc.ifc_len; n+=sizeof(struct ifreq)) {
        //ifr.ifr_addr.sa_family = AF_INET;
        if (ioctl(skfd, SIOCGIFDSTADDR, ifr) < 0) {
            printf("SIOCGIFDSTADDR error %d\n", errno);
            close(skfd);
            return -1;
        }

        sin = (struct sockaddr_in *)&ifr->ifr_dstaddr;
        len += sprintf(&buf[len], "%s: %s\n", ifr->ifr_name, inet_ntoa(sin->sin_addr));
        ifr++;
    }

#elif defined(_WIN32_WCE)
#if 0
    char hostName[64];
    hostent *hostEntry;
    int len=0, wsaErr;
    WORD    wsaVer = MAKEWORD(2,2);
    WSADATA wsaData;

    wsaErr = WSAStartup(wsaVer, &wsaData);

    if ( !gethostname(hostName, 64))
    {
        hostEntry = gethostbyname(hostName);
        if(hostEntry != NULL)
        {
            sprintf(&buf[len], "%s : %s\n", hostEntry->h_name, hostEntry->h_addr);
            printf("%s IP addr:%d len %x\r\n", hostEntry->h_name, hostEntry->h_length, hostEntry->h_addr);
        }
        else
        {
            sprintf(&buf[len], "%s error: %d", hostName, WSAGetLastError());
            printf("%s error: %d\r\n", hostName, WSAGetLastError());
        }
    }
    if(wsaErr == 0)
        WSACleanup();
#endif

#else
    sprintf(buf, "Not supported!");
#endif

    MMessageBox mbox(fb(), 250, 200);
    mbox.set(buf, MMessageBox::bOK);
    mbox.setWrapMode(MLabel::NewLineWrap);
    mbox.process();
    return 0;
}

void AdminScreen::enableRunScript()
{
    _runScript->setEnabled(true);
}

