/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_tty.cpp $
 * $brcm_Revision: 14 $
 * $brcm_Date: 7/30/10 1:11p $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_tty.cpp $
 * 
 * 14   7/30/10 1:11p erickson
 * SW7408-91: add '#' -tty option for scanning channels, remove BCHP_CHIP
 * list
 *
 * 13   2/8/07 5:32p erickson
 * PR17108: added 'q' and TAB support to -tty
 *
 * 12   8/24/06 2:50p jgarrett
 * PR 23890: Adding DUAL_OUTPUT support for 97400, fixing bug where a in
 * single UI would lose focus
 *
 * 11   7/31/06 1:59p erickson
 * PR21941: added info panel
 *
 * 10   12/1/05 4:28p erickson
 * PR17860: allow + to work for chanup
 *
 * 9   8/23/05 12:40p erickson
 * PR15512: added SPACE to tty help
 *
 * 8   8/23/05 12:37p erickson
 * PR16747: test for n before removing
 *
 * 7   8/23/05 12:36p erickson
 * PR16747: remove trailing n
 *
 * 6   8/22/05 9:25a erickson
 * PR16747: added ; key for entertext. this takes stdin out of
 * raw/unblocked mode temporarily
 *
 * 5   7/21/05 4:49p bandrews
 * PR14935: Added conditional compile around check using conditionally
 * compiled global
 *
 * 4   7/18/05 2:14p erickson
 * PR14935: first app to create InputTTY owns it. this enables -tty on -2
 * apps
 *
 * 3   7/6/05 4:39p erickson
 * PR16119: added --exec and -tty slow forward/reverse support
 *
 * 2   6/10/05 2:01p erickson
 * PR15512: added '?' help, removed output modification so that n is
 * normal
 *
 * 1   2/7/05 8:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/9   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 *
 * Irvine_BSEAVSW_Devel/8   7/23/04 3:02p erickson
 * PR11771: now that tty response time stinks (waiting on idle time), at
 * least loop to pick up multiple keys
 *
 * Irvine_BSEAVSW_Devel/7   7/15/04 5:28p erickson
 * PR11771: timer needs to keep being called
 *
 * Irvine_BSEAVSW_Devel/6   7/6/04 3:56p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/5   6/29/04 11:13a dlwin
 * PR 11728: Fixed the problem '-tty' not working on legacy platforms.
 *
 * Irvine_BSEAVSW_Devel/4   4/27/04 10:31a erickson
 * PR10813: Don't use stdin in poll...yet. The round robin hack in bwin.c
 * requires that all fd's be -1.
 *
 * Irvine_BSEAVSW_Devel/3   4/16/04 4:17p erickson
 * PR9883: added 'a' key to toggle between apps in DUAL_OUTPUT mode
 *
 * Irvine_BSEAVSW_Devel/2   4/16/04 11:27a erickson
 * PR9029: convert to BrutusCommand for device-independent command
 *
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#include "input_tty.h"
#include "input_devicecodes.h"
#include "inputengine.h"
#include "control.h"
#include <bstd.h>
BDBG_MODULE(brutus_input_tty);

#ifdef LINUX

/* DUAL_OUTPUT_TTY  */
static int currentAppIndex = -1;

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
static struct termios g_normal_termios;
static int b_save_termios(int fd)
{
    tcgetattr(fd, &g_normal_termios);
}
static int b_set_raw_mode(int fd, int enabled)
{
    struct termios buf = g_normal_termios;
    if (enabled) {
        buf.c_lflag &= ~(ECHO|ICANON|IEXTEN);
        buf.c_iflag &= ~(ICRNL|INPCK|ISTRIP|IXON);
        buf.c_cflag &= ~(CSIZE|PARENB);
        buf.c_cflag |= CS8;
        buf.c_cc[VMIN] = 1;
        buf.c_cc[VTIME] = 0;
        if (fcntl(fd, F_SETFL, O_NONBLOCK))
            return -1;
    }
    else {
        int flags = fcntl(fd, F_GETFL);
        if (flags == -1) return -1;
        if (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK))
            return -1;
    }

    if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
        return -1;
    return 0;
}

TTYInput::TTYInput(int appIndex, MApplication *app, InputEngine *ie) :
    InputDevice(app, ie, -1), _timer(app)
{
    b_save_termios(0);

    _appIndex = appIndex;

    /* The first app to create TTYInput owns it */
    if (_inputEngine->control()->cfg()->getBool(Config::DUAL_OUTPUT)) {
        if (currentAppIndex == -1) {
            currentAppIndex = _appIndex;
        }
    }

    /* Set stdin into raw, nonblocking mode and add it to the event queue.
    This doesn't use the init_io function because it doesn't use settop api.
    Make sure to keep the index allocation straight. */
    if (b_set_raw_mode(0, 1)) {
        BDBG_ERR(("Unable to start -tty mode because of system error"));
    }
    else {
        _timer.addHandler(this);
        _timer.start(0, MTimer::eForever);
    }
}

TTYInput::~TTYInput()
{
    _timer.stop();
    b_set_raw_mode(0, 0);
}

void TTYInput::onTimeout(MTimer *timer)
{
    /* Do a non-blocking read at idle time. Efficiency doesn't matter. */
    process(0);
}

void TTYInput::printUsage()
{
    BKNI_Printf(
"-tty keymappings\n"
"--------------------------\n"
"w    goto TV Screen (Exit)\n"
"e    goto Playback Screen\n"
"t    goto Home screen\n"
"r    goto Record Screen\n"
"hjkl navigate left,down,up,right (some contexts)\n"
"u    toggle info panel\n"
"SPACE select\n"
"TAB  tab to next control\n"
"`    reverse tab to previous control\n"
"z    stop PVR\n"
"x    pause PVR\n"
"c    play PVR\n"
"b n  fast forward & rewind, frame adv when paused\n"
", .  slow forward & rewind\n"
"+ -  channel up,down\n"
"p    toggle pip\n"
"o    swap main and PIP\n"
"i    toggle focus between main and PIP\n"
"] [  volume up,down\n"
"\\    mute audio\n"
"a    switch between application instances in DUAL_OUTPUT modes.\n"
";    enter text into a text box\n"
"#    scan channels\n"
"?    this help screen\n"
"q    quit brutus\n"
    );
}

void TTYInput::process(uint32_t data)
{
    char ch;

    if (currentAppIndex != -1 && _appIndex != currentAppIndex)
        return;

    /* See if anything is there. */
    while (1) {
        if (read(0, &ch, 1) != 1)
            return;
        if (ch > 32)
            BDBG_MSG(("tty %c", ch));
        else
            BDBG_MSG(("tty 0x%02x", ch));

        if (ch == '+') ch = '='; /* handle shift = just like = */

        if (ch == 'a') {
            if (currentAppIndex != -1) {
                if (++currentAppIndex == 2) {
                    currentAppIndex = 0;
                }
            }
        }
        else if (ch == '?') {
            printUsage();
            continue;
        }
        else if (ch == 0x09 || ch == '`') {
            app()->sendKeyDown(app()->focusedWidget(), ch, 0, 0);
            continue;
        }
        else if (ch == 'q') {
            app()->terminate(); // this kills all microwidgets message loops
            return;
        }
        else if (ch == '#') {
            _inputEngine->control()->scan();
            continue;
        }
        else if (ch == ';') {
            char buf[256];
            b_set_raw_mode(0, 0);

            MWidget *widget = app()->focusedWidget();
            if (widget && !strcmp(widget->className(), "MLineEdit")) {
                printf("Enter text: ");fflush(stdout);
                fgets(buf, 256, stdin);
                int len = strlen(buf);
                if (len && buf[len-1] == '\n')
                    buf[len-1] = 0;
                widget->setText(buf);
            }

            b_set_raw_mode(0, 1);
            continue;
        }

        BrutusCommand cmd;
        if (!translateCmd(dev_tty, ch, &cmd))
            _inputEngine->processCmd(cmd);
    }
}

#endif
