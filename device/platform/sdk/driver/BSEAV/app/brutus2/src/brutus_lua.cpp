/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_lua.cpp $
* $brcm_Revision: 39 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus_lua.cpp $
* 
* 39   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 38   10/19/12 1:02p jrubio
* SW7346-1071: allow no arguments in playbackStop() function
* 
* 37   10/12/12 1:00p jrubio
* SW7346-1004: added custom LUA ONLY trickmodes
* 
* 36   10/10/12 8:32p tokushig
* SW7231-749: use case insensitive strcmp in lua setDebugLevel
* 
* 35   10/9/12 10:29p tokushig
* SW7231-749: update brutus lua extension api list to match twiki
* documentation
* 
* 34   10/9/12 2:08p tokushig
* SW7231-749: add lua command to set the debug level for a given debug
* module
* 
* 33   10/9/12 1:19p tokushig
* SW7231-749: add runScript lua command to allow script running after
* brutus2 has started
* 
* 32   10/8/12 8:56p tokushig
* SW7231-749: add audio lua support
* 
* 31   10/8/12 6:47p tokushig
* SW7231-749: add lua commands for display settings
* 
* 30   9/25/12 12:43p jrubio
* SW7346-1004: allow recordStart to have no arguments
* 
* 29   9/21/12 8:34p tokushig
* SW7231-749: change scan notification to scanStart
* 
* 29   9/21/12 4:12p tokushig
* SW7231-749: change scan notification to scan start
* 
* 28   9/18/12 5:41p jrubio
* SW7346-1004:record command take in two arguments currently
* 
* 27   9/17/12 10:24a jrubio
* SW7346-1004: add record lua
* 
* 26   7/24/12 2:28p tokushig
* SW7231-749: add lua support for ofdm scan
* 
* 25   4/20/12 2:20p jrubio
* SW7346-644: do not set Default path here for Videos, control will use
* GET_STR
* 
* 24   4/19/12 7:46p tokushig
* SW7231-749: set pointers to null after freeing
* 
* 24   4/19/12 7:46p tokushig
* SW7231-749: set pointers to null after freeing
* 
* 23   4/18/12 5:13p jrubio
* SW7346-644: modify playback start
* 
* 22   4/18/12 12:41a jrubio
* SW7346-644: add refresh from disk function
* 
* 21   4/15/12 12:11a jrubio
* SW7346-644: add playback stop command
* 
* 20   4/14/12 10:18p jrubio
* SW7346-644: add default path and set playback index to self if not
* passed in
* 
* 19   4/12/12 12:07a jrubio
* SW7346-644: do not set the Path to \/data\/videos if 3rd param is
* playbackStart is not present
* 
* 18   4/11/12 1:19p jrubio
* SW7346-644: add Playback Lua command
* 
* 17   4/9/12 7:26p tokushig
* SW7405-5581: callback name change
* 
* 16   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 15   3/27/12 3:30p tokushig
* SW7405-5581: synchronize lua commands
* 
* 14   3/27/12 10:30a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 13   3/14/12 12:39p jrubio
* SW7346-644: add tuner sat
* 
* 12   3/12/12 8:51p tokushig
* SW7405-5581: add brutus.channelListDump() lua command
* 
* 11   3/9/12 3:46p tokushig
* SW7405-5581: integrate lua channel list save command with channelmgr
* code.
* 
* 10   3/9/12 2:52p tokushig
* SW7405-5581: partially added channel list save to lua
* 
* 9   3/2/12 4:54p tokushig
* SW7405-5581: brutus.channelListLoad() lua command working.
* 
* 8   2/29/12 1:36p tokushig
* SW7405-5581: added partial scanSat
* 
* 7   2/27/12 4:17p tokushig
* SW7405-5581: break out qam tuner from tuner files
* 
* 
*********************************************************************************/

#include "brutus.h"
#include "brutus_os.h"
#include "brutus_lua.h"
#include "string.h"

#if NEXUS_HAS_FRONTEND
#include "tuner_qam.h"
#include "tuner_vsb.h"
#include "tuner_sat.h"
#include "tuner_ofdm.h"
#endif

#include "channelmgr.h"
#include "playback.h"
#include "record.h"

#ifdef LINENOISE_SUPPORT
extern "C" {
#include "linenoise.h"
}

#include "mlist.h"
#include "mstring.h"
#include "ctype.h"
#endif

BDBG_MODULE(brutus_lua);

/* name of the brutus lua extension library.  brutus specific commands in lua will have the "brutus." prefix. */
#define LUA_BRUTUSLIBNAME	"brutus"

/* the address of this variable is used as a unique key to store data in the lua registry */
static const char LUAKEY_CLUA = 'k';

static CLua * getCLua(lua_State * pLua)
{
    CLua      * pThis     = NULL;

    BDBG_ASSERT(NULL != pLua);

    lua_pushlightuserdata(pLua, (void *)&LUAKEY_CLUA);
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    pThis = (CLua *)lua_tointeger(pLua, -1);

    return pThis;
}

static int brutusLua_ChannelUp(lua_State * pLua)
{
    CLua      * pThis     = getCLua(pLua);
    CLuaEvent * pLuaEvent = NULL; 
    eRet        err       = eRet_Ok;

    BDBG_ASSERT(pThis);

    pLuaEvent = new CLuaEvent(eNotify_ChUp, eNotify_CurrentChannel, 5000);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

static int brutusLua_ChannelDown(lua_State * pLua)
{
    CLua      * pThis     = getCLua(pLua);
    CLuaEvent * pLuaEvent = NULL; 
    eRet        err       = eRet_Ok;

    BDBG_ASSERT(pThis);

    pLuaEvent = new CLuaEvent(eNotify_ChDown, eNotify_CurrentChannel, 5000);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

#if NEXUS_HAS_FRONTEND
/* brutus.scanQam(
        --- required ---
        start frequency,   frequency in Hz at which to begin scanning
        end frequency  ,   frequency in Hz at which to end scanning
        bandwidth      ,   step in Hz to increment
        --- optional ---
        append = false,    if true, add found channels to channel list.  otherwise replace channel list.
        mode   = 4,
        annex  = 1)
*/
static int brutusLua_ScanQam(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if ((3 > numArgTotal) || (6 < numArgTotal))
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [startFreq][endFreq][bandwidth]<append><mode><annex>");
    }

    /* get arguments */
    uint32_t startFreq = luaL_checknumber(pLua, argNum++);
    uint32_t endFreq   = luaL_checknumber(pLua, argNum++);
    int      bandwidth = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (startFreq > endFreq)
    {
        luaL_error(pLua, "start frequency must be less than end frequency");
    }
    if (bandwidth < 1000000)
    {
        /* dtt todo: upper bound? */
        luaL_error(pLua, "bandwidth must be > 1000000");
    }

    CTunerQamScanData * pQamScanData = new CTunerQamScanData();

    /* add required arguments to scan data */

    /* create list of freq to scan */
    for (uint32_t freq = startFreq; freq <= endFreq; freq += bandwidth)
    {
        pQamScanData->_freqList.add(new uint32_t(freq));
    }
    pQamScanData->_bandwidth = bandwidth;
    
    /* add optional arguments to scan data */
    if (4 <= numArgTotal)
    {
        pQamScanData->_appendToChannelList = lua_toboolean(pLua, argNum++);
    }
    if (5 <= numArgTotal)
    {
        pQamScanData->_mode = luaL_checknumber(pLua, argNum++);
    }
    if (6 <= numArgTotal)
    {
        pQamScanData->_annex = luaL_checknumber(pLua, argNum++);
    }

    /* test print out requested frequencies */
    pQamScanData->dump();

    /* create lua event and give it scan data */
    CLuaDataEvent <CTunerQamScanData> * pLuaEvent = 
        new CLuaDataEvent <CTunerQamScanData>(eNotify_ScanStart, pQamScanData, eNotify_ScanStopped, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.scanVsb(
        --- required ---
        start frequency,   frequency in Hz at which to begin scanning
        end frequency  ,   frequency in Hz at which to end scanning
        bandwidth      ,   step in Hz to increment
        --- optional ---
        append = false,    if true, add found channels to channel list.  otherwise replace channel list.
        mode   = 0)
*/
static int brutusLua_ScanVsb(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if ((3 > numArgTotal) || (5 < numArgTotal))
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [startFreq][endFreq][bandwidth]<append><mode>");
    }

    /* get arguments */
    uint32_t startFreq = luaL_checknumber(pLua, argNum++);
    uint32_t endFreq   = luaL_checknumber(pLua, argNum++);
    int      bandwidth = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (startFreq > endFreq)
    {
        luaL_error(pLua, "start frequency must be less than end frequency");
    }
    if (bandwidth < 1000000)
    {
        /* dtt todo: upper bound? */
        luaL_error(pLua, "bandwidth must be > 1000000");
    }

    CTunerVsbScanData * pVsbScanData = new CTunerVsbScanData();

    /* add required arguments to scan data */

    /* create list of freq to scan */
    for (uint32_t freq = startFreq; freq <= endFreq; freq += bandwidth)
    {
        pVsbScanData->_freqList.add(new uint32_t(freq));
    }
    pVsbScanData->_bandwidth = bandwidth;
    
    /* add optional arguments to scan data */
    if (4 <= numArgTotal)
    {
        pVsbScanData->_appendToChannelList = lua_toboolean(pLua, argNum++);
    }
    if (5 <= numArgTotal)
    {
        pVsbScanData->_mode = luaL_checknumber(pLua, argNum++);
    }

    /* test print out requested frequencies */
    pVsbScanData->dump();

    /* create lua event and give it scan data */
    CLuaDataEvent <CTunerVsbScanData> * pLuaEvent = 
        new CLuaDataEvent <CTunerVsbScanData>(eNotify_ScanStart, pVsbScanData, eNotify_ScanStopped, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}


/* brutus.scanSat(
        --- required ---
        start frequency,   frequency in MHz at which to begin scanning
        end frequency  ,   frequency in MHz at which to end scanning
        bandwidth      ,   step in MHz to increment
        --- optional ---
        append = false)    if true, add found channels to channel list.  otherwise replace channel list.
*/
static int brutusLua_ScanSat(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if ((3 > numArgTotal) || (4 < numArgTotal))
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [startFreq][endFreq][bandwidth]<append>");
    }

    /* get arguments */
    uint32_t startFreq = luaL_checknumber(pLua, argNum++);
    uint32_t endFreq   = luaL_checknumber(pLua, argNum++);
    int      bandwidth = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (startFreq > endFreq)
    {
        luaL_error(pLua, "start frequency must be less than end frequency");
    }
#if 0
    if (bandwidth < 1000000)
    {
        /* dtt todo: upper bound? */
        luaL_error(pLua, "bandwidth must be > 1000000");
    }
#endif 
    CTunerSatScanData * pSatScanData = new CTunerSatScanData();

    /* add required arguments to scan data */

    /* create list of freq to scan */
    for (uint32_t freq = startFreq; freq <= endFreq; freq += bandwidth)
    {
        pSatScanData->_freqList.add(new uint32_t(freq));
    }
   // pSatScanData->_bandwidth = bandwidth;
    
    /* add optional arguments to scan data */
    if (4 <= numArgTotal)
    {
        pSatScanData->_appendToChannelList = lua_toboolean(pLua, argNum++);
    }

    /* test print out requested frequencies */
    pSatScanData->dump();

    /* create lua event and give it scan data */
    CLuaDataEvent <CTunerSatScanData> * pLuaEvent = 
        new CLuaDataEvent <CTunerSatScanData>(eNotify_ScanStart, pSatScanData, eNotify_ScanStopped, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.scanOfdm(
        --- required ---
        start frequency,   frequency in Hz at which to begin scanning
        end frequency  ,   frequency in Hz at which to end scanning
        bandwidth      ,   step in Hz to increment
        --- optional ---
        append = false,    if true, add found channels to channel list.  otherwise replace channel list.
        mode   = 2)
*/
static int brutusLua_ScanOfdm(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if ((3 > numArgTotal) || (5 < numArgTotal))
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [startFreq][endFreq][bandwidth]<append><mode><annex>");
    }

    /* get arguments */
    uint32_t startFreq = luaL_checknumber(pLua, argNum++);
    uint32_t endFreq   = luaL_checknumber(pLua, argNum++);
    int      bandwidth = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (startFreq > endFreq)
    {
        luaL_error(pLua, "start frequency must be less than end frequency");
    }
    if (bandwidth < 1000000)
    {
        /* dtt todo: upper bound? */
        luaL_error(pLua, "bandwidth must be > 1000000");
    }

    CTunerOfdmScanData * pOfdmScanData = new CTunerOfdmScanData();

    /* add required arguments to scan data */

    /* create list of freq to scan */
    for (uint32_t freq = startFreq; freq <= endFreq; freq += bandwidth)
    {
        pOfdmScanData->_freqList.add(new uint32_t(freq));
    }
    pOfdmScanData->_bandwidth = bandwidth;
    
    /* add optional arguments to scan data */
    if (4 <= numArgTotal)
    {
        pOfdmScanData->_appendToChannelList = lua_toboolean(pLua, argNum++);
    }
    if (5 <= numArgTotal)
    {
        pOfdmScanData->_mode = luaL_checknumber(pLua, argNum++);
    }

    /* test print out requested frequencies */
    pOfdmScanData->dump();

    /* create lua event and give it scan data */
    CLuaDataEvent <CTunerOfdmScanData> * pLuaEvent = 
        new CLuaDataEvent <CTunerOfdmScanData>(eNotify_ScanStart, pOfdmScanData, eNotify_ScanStopped, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}
#endif /* NEXUS_HAS_FRONTEND */

/* brutus.setColorSpace(
        --- required ---
        color space type,   NEXUS color space type to switch to
        --- optional ---
        none
*/
static int brutusLua_SetColorSpace(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [colorSpace]");
    }

    /* get arguments */
    uint32_t formatType = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (NEXUS_ColorSpace_eMax <= formatType)
    {
        luaL_error(pLua, "Invalid color space type");
    }

    NEXUS_ColorSpace * pColorSpace = new NEXUS_ColorSpace;

    /* add required arguments to video format data */

    *pColorSpace = (NEXUS_ColorSpace)formatType;

    /* create lua event and give it scan data */
    CLuaDataEvent <NEXUS_ColorSpace> * pLuaEvent = 
        new CLuaDataEvent <NEXUS_ColorSpace>(eNotify_SetColorSpace, pColorSpace, eNotify_ColorSpaceChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setMpaaDecimation(
        --- required ---
        mpaa decimation state,   if true, turn on mpaa decimation
        --- optional ---
        none
*/
static int brutusLua_SetMpaaDecimation(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [bool]");
    }

    /* get arguments */
    bool mpaaDecimation = lua_toboolean(pLua, argNum++);
    bool * pMpaaDecimation = new bool;

    /* add required arguments to mpaa decimation data */
    *pMpaaDecimation = mpaaDecimation;

    /* create lua event and give it scan data */
    CLuaDataEvent <bool> * pLuaEvent = 
        new CLuaDataEvent <bool>(eNotify_SetMpaaDecimation, pMpaaDecimation, eNotify_MpaaDecimationChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setDeinterlacer(
        --- required ---
        deinterlacer state,   if true, turn on MAD deinterlacer 
        --- optional ---
        none
*/
static int brutusLua_SetDeinterlacer(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [bool]");
    }

    /* get arguments */
    bool deinterlacer = lua_toboolean(pLua, argNum++);
    bool * pDeinterlacer = new bool;

    /* add required arguments to deinterlacer data */
    *pDeinterlacer = deinterlacer;

    /* create lua event and give it scan data */
    CLuaDataEvent <bool> * pLuaEvent = 
        new CLuaDataEvent <bool>(eNotify_SetDeinterlacer, pDeinterlacer, eNotify_DeinterlacerChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setBoxDetect(
        --- required ---
        BoxDetect state,   if true, turn on BoxDetect 
        --- optional ---
        none
*/
static int brutusLua_SetBoxDetect(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [bool]");
    }

    /* get arguments */
    bool boxDetect = lua_toboolean(pLua, argNum++);
    bool * pBoxDetect = new bool;

    /* add required arguments to BoxDetect data */
    *pBoxDetect = boxDetect;

    /* create lua event and give it scan data */
    CLuaDataEvent <bool> * pLuaEvent = 
        new CLuaDataEvent <bool>(eNotify_SetBoxDetect, pBoxDetect, eNotify_BoxDetectChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setAspectRatio(
        --- required ---
        aspect ratio type,   NEXUS aspect ratio type to switch to
        --- optional ---
        none
*/
static int brutusLua_SetAspectRatio(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [aspectRatio]");
    }

    /* get arguments */
    uint32_t formatType = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (NEXUS_AspectRatio_eMax <= formatType)
    {
        luaL_error(pLua, "Invalid aspect ratio type");
    }

    NEXUS_AspectRatio * pAspectRatio = new NEXUS_AspectRatio;

    /* add required arguments to video format data */

    *pAspectRatio = (NEXUS_AspectRatio)formatType;

    /* create lua event and give it scan data */
    CLuaDataEvent <NEXUS_AspectRatio> * pLuaEvent = 
        new CLuaDataEvent <NEXUS_AspectRatio>(eNotify_SetAspectRatio, pAspectRatio, eNotify_AspectRatioChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setVideoFormat(
        --- required ---
        video format type,   NEXUS video format type to switch to
        --- optional ---
        none
*/
static int brutusLua_SetVideoFormat(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [formatType]");
    }

    /* get arguments */
    uint32_t formatType = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (NEXUS_VideoFormat_eMax <= formatType)
    {
        luaL_error(pLua, "Invalid video format type");
    }

    NEXUS_VideoFormat * pVideoFormat = new NEXUS_VideoFormat;

    /* add required arguments to video format data */

    *pVideoFormat = (NEXUS_VideoFormat)formatType;

    /* create lua event and give it scan data */
    CLuaDataEvent <NEXUS_VideoFormat> * pLuaEvent = 
        new CLuaDataEvent <NEXUS_VideoFormat>(eNotify_SetVideoFormat, pVideoFormat, eNotify_VideoFormatChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setAutoVideoFormat(
        --- required ---
        auto format state,   if true, turn on auto format
        --- optional ---
        none
*/
static int brutusLua_SetAutoVideoFormat(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [bool]");
    }

    /* get arguments */
    bool autoFormat = lua_toboolean(pLua, argNum++);
    bool * pAutoVideoFormat = new bool;

    /* add required arguments to auto format data */
    *pAutoVideoFormat = autoFormat;

    /* create lua event and give it scan data */
    CLuaDataEvent <bool> * pLuaEvent = 
        new CLuaDataEvent <bool>(eNotify_SetAutoVideoFormat, pAutoVideoFormat, eNotify_AutoVideoFormatChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.channelListLoad(
        --- optional ---
        fileName,          filename containing list
        listName,          name of channel list in fileName
        append = false)    if true, add new channel list to current channel list.  otherwise replace current channel list
*/
static int brutusLua_ChannelListLoad(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (3 < numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [fileName][listName]]<append>");
    }

    /* add required arguments to scan data */
    CChannelMgrLoadSaveData * pLoadSaveData =  new CChannelMgrLoadSaveData();

    /* add optional arguments to scan data */
    if (1 <= numArgTotal)
    {
        const char * strFileName = luaL_checkstring(pLua, argNum++);
        if (NULL != strFileName)
        {
            pLoadSaveData->_strFileName = strFileName;
        }
    }
    if (2 <= numArgTotal)
    {
        const char * strListName = luaL_checkstring(pLua, argNum++);
        if (NULL != strListName)
        {
            pLoadSaveData->_strListName = strListName;
        }
    }
    if (3 <= numArgTotal)
    {
        pLoadSaveData->_append = lua_toboolean(pLua, argNum++);
    }

    /* create lua event and give it scan data */
    CLuaDataEvent <CChannelMgrLoadSaveData> * pLuaEvent = 
        new CLuaDataEvent <CChannelMgrLoadSaveData>(eNotify_ChannelListLoad, pLoadSaveData, eNotify_ChannelListChanged);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}


/* brutus.channelListSave(
        --- optional ---
        fileName,          filename to save list to 
        listName,          name of channel list 
        append = false)    if true, add add new list to end of file.  otherwise, overwrite file
*/
static int brutusLua_ChannelListSave(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (3 < numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [fileName][listName]]<append>");
    }

    /* add required arguments to scan data */
    CChannelMgrLoadSaveData * pLoadSaveData =  new CChannelMgrLoadSaveData();

    /* add optional arguments to scan data */
    if (1 <= numArgTotal)
    {
        const char * strFileName = luaL_checkstring(pLua, argNum++);
        if (NULL != strFileName)
        {
            pLoadSaveData->_strFileName = strFileName;
        }
    }
    if (2 <= numArgTotal)
    {
        const char * strListName = luaL_checkstring(pLua, argNum++);
        if (NULL != strListName)
        {
            pLoadSaveData->_strListName = strListName;
        }
    }
    if (3 <= numArgTotal)
    {
        pLoadSaveData->_append = lua_toboolean(pLua, argNum++);
    }

    /* create lua event and give it scan data */
    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    CLuaDataEvent <CChannelMgrLoadSaveData> * pLuaEvent = 
        new CLuaDataEvent <CChannelMgrLoadSaveData>(eNotify_ChannelListSave, pLoadSaveData);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.channelListDump()
*/
static int brutusLua_ChannelListDump(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;

    BDBG_ASSERT(pThis);

    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    CLuaEvent * pLuaEvent =  new CLuaEvent(eNotify_ChannelListDump);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.playbackTrickMode(
        --- optional ---
        fileName,          filename to save list to 
        indexName,          name of channel list 
        path = NULL)    if set, use the path give to find file otherwise use /data/videos
*/
static int brutusLua_PlaybackTrickMode(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;
    const char * trickModeBuf = NULL;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (2 < numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [fileName]<Path>");
    }

    /* add required arguments to scan data */
    CPlaybackTrickModeData * pPlaybackTrickModeData =  new CPlaybackTrickModeData();
    /* add optional arguments to scan data */
    if (1 <= numArgTotal)
    {
        trickModeBuf = luaL_checkstring(pLua, argNum++);
        if (NULL != trickModeBuf)
        {
            pPlaybackTrickModeData->_trickModeBuf = trickModeBuf;
        }
    }
     
    if (numArgTotal <1 || !strcmp(trickModeBuf, "help"))
    {
        BDBG_WRN(("\nCommands:\n"
                "  play - resume normal playback\n"
                "  pause - pause playback\n"
                "  i - decode only I frames\n"
                "  ip - decode only I & P frames\n"
                "  all - decode all frames\n"));
        BDBG_WRN(("\n  fa - frame advance\n"
                "  fr - frame reverse\n"
                "  rate(rate) - set trick rate (floating point number, 1.0 is normal play)\n"
                "  host(type[,modifier,slowrate]) - set host trick mode\n"
                "    type=i,ip,all; modifier=1,-1 for forward,reverse(i only); slowrate=decoder repeat rate (1=1x,2=2x)\n"
                "  seek(pos) - jump to position (in milliseconds)\n" ));
        if (numArgTotal<1)
        {
            luaL_error(pLua,"Please specify a command ");
        }
    }


    /* create lua event and give it playback info */
    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    CLuaDataEvent <CPlaybackTrickModeData> * pLuaEvent = 
        new CLuaDataEvent <CPlaybackTrickModeData>(eNotify_PlaybackTrickMode, pPlaybackTrickModeData);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);
error:
    return 0;
}

/* brutus.playbackStart(
        --- optional ---
        fileName,          filename to save list to 
        indexName,          name of channel list 
        path = NULL)    if set, use the path give to find file otherwise use /data/videos
*/
static int brutusLua_PlaybackStart(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (3 < numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [fileName][indexName]]<Path>");
    }

    /* add required arguments to scan data */
    CPlaybackData * pPlaybackData =  new CPlaybackData();

    /* add optional arguments to scan data */
    if (1 <= numArgTotal)
    {
        const char * strFileName = luaL_checkstring(pLua, argNum++);
        if (NULL != strFileName)
        {
            pPlaybackData->_strFileName = strFileName;
        }
    }else
        luaL_error(pLua, "need to define at least filename Arguments: [fileName][indexName]]<Path>");

    if (2 <= numArgTotal)
    {
        const char * strIndexName = luaL_checkstring(pLua, argNum++);
        if (NULL != strIndexName)
        {
            pPlaybackData->_strIndexName = strIndexName;
        }
    }else
        pPlaybackData->_strIndexName = pPlaybackData->_strFileName;

    if (3 <= numArgTotal)
    {
        const char * strPath = luaL_checkstring(pLua, argNum++);
        if (NULL != strPath)
        {
            pPlaybackData->_strPath = strPath;
        }
    }

    /* create lua event and give it playback info */
    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    CLuaDataEvent <CPlaybackData> * pLuaEvent = 
        new CLuaDataEvent <CPlaybackData>(eNotify_PlaybackStart, pPlaybackData);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.playbackStop(
        --- optional ---
        fileName)           file Name to stop playing  
*/
static int brutusLua_PlaybackStop(lua_State * pLua)
{
  CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* add required arguments to scan data */
    CPlaybackData * pPlaybackData =  new CPlaybackData();

    /* add optional arguments to scan data */
    if (1 <= numArgTotal)
    {
        const char * strFileName = luaL_checkstring(pLua, argNum++);
        if (NULL != strFileName)
        {
            pPlaybackData->_strFileName = strFileName;
        }
    }else
        luaL_error(pLua, "need to define at least filename Arguments: [fileName]");


    /* create lua event and give it playback info */
    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    CLuaDataEvent <CPlaybackData> * pLuaEvent = 
        new CLuaDataEvent <CPlaybackData>(eNotify_PlaybackStop, pPlaybackData);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.recordStart(
        --- optional ---
        fileName,          filename to save list to 
        path = NULL)    if set, use the path give to find file otherwise use /data/videos
*/
static int brutusLua_RecordStart(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (3 < numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [fileName][indexName]]<Path>");
    }

    /* add required arguments to scan data */
    CRecordData * pRecordData =  new CRecordData();

    /* add optional arguments to scan data */
    if (1 <= numArgTotal)
    {
        const char * strFileName = luaL_checkstring(pLua, argNum++);
        if (NULL != strFileName)
        {
            pRecordData->_strFileName = strFileName;
        }
    }
    
    if (2 <= numArgTotal)
    {
        const char * strPath = luaL_checkstring(pLua, argNum++);
        if (NULL != strPath)
        {
            pRecordData->_strPath = strPath;
        }
    }

    /* create lua event and give it record info */
    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    CLuaDataEvent <CRecordData> * pLuaEvent = 
        new CLuaDataEvent <CRecordData>(eNotify_RecordStart, pRecordData);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.recordStop(
        --- optional ---
        fileName)           file Name to stop playing  
*/
static int brutusLua_RecordStop(lua_State * pLua)
{
  CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (2 < numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [fileName]");
    }

    /* add required arguments to scan data */
    CRecordData * pRecordData =  new CRecordData();

    /* add optional arguments to scan data */
    if (1 <= numArgTotal)
    {
        const char * strFileName = luaL_checkstring(pLua, argNum++);
        if (NULL != strFileName)
        {
            pRecordData->_strFileName = strFileName;
        }
    }

    /* create lua event and give it playback info */
    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    CLuaDataEvent <CRecordData> * pLuaEvent = 
        new CLuaDataEvent <CRecordData>(eNotify_RecordStop, pRecordData);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}


static int brutusLua_RefreshPlaybackList(lua_State * pLua)
{
    CLua      * pThis     = getCLua(pLua);
    CLuaEvent * pLuaEvent = NULL; 
    eRet        err       = eRet_Ok;

    BDBG_ASSERT(pThis);

    pLuaEvent = new CLuaEvent(eNotify_RefreshPlaybackList,eNotify_Invalid, 5000);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setAudioProgram(
        --- required ---
        audio program pid,   audio program pid to use
        --- optional ---
        none
*/
static int brutusLua_SetAudioProgram(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [formatType]");
    }

    /* get arguments */
    uint32_t pid = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (0x1FFF == pid)
    {
        luaL_error(pLua, "Invalid program pid number");
    }

    uint16_t * pPid = new uint16_t;

    /* add required arguments to video format data */

    *pPid = (uint16_t)pid;

    /* create lua event and give it scan data */
    CLuaDataEvent <uint16_t> * pLuaEvent = 
        new CLuaDataEvent <uint16_t>(eNotify_SetAudioProgram, pPid, eNotify_AudioSourceChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setSpdifInput(
        --- required ---
        spdif input type,   spdif input type to use
        --- optional ---
        none
*/
static int brutusLua_SetSpdifInput(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [spdifInputType]");
    }

    /* get arguments */
    uint32_t spdifInput = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (eSpdifInput_Max <= spdifInput)
    {
        luaL_error(pLua, "Invalid spdif input type");
    }

    eSpdifInput * pSpdifInput = new eSpdifInput;

    /* add required arguments to spdif input type data */

    *pSpdifInput = (eSpdifInput)spdifInput;

    /* create lua event and give it scan data */
    CLuaDataEvent <eSpdifInput> * pLuaEvent = 
        new CLuaDataEvent <eSpdifInput>(eNotify_SetSpdifInput, pSpdifInput, eNotify_AudioSourceChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setHdmiAudioInput(
        --- required ---
        Hdmi input type,   Hdmi input type to use
        --- optional ---
        none
*/
static int brutusLua_SetHdmiAudioInput(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [hdmiInputType]");
    }

    /* get arguments */
    uint32_t hdmiInput = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (eHdmiAudioInput_Max <= hdmiInput)
    {
        luaL_error(pLua, "Invalid hdmi input type");
    }

    eHdmiAudioInput * pHdmiInput = new eHdmiAudioInput;

    /* add required arguments to Hdmi input type data */

    *pHdmiInput = (eHdmiAudioInput)hdmiInput;

    /* create lua event and give it scan data */
    CLuaDataEvent <eHdmiAudioInput> * pLuaEvent = 
        new CLuaDataEvent <eHdmiAudioInput>(eNotify_SetHdmiAudioInput, pHdmiInput, eNotify_AudioSourceChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setDownmix(
        --- required ---
        downmix type,   downmix type to use
        --- optional ---
        none
*/
static int brutusLua_SetDownmix(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [hdmiInputType]");
    }

    /* get arguments */
    uint32_t downmix = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (eAudioDownmix_Max <= downmix)
    {
        luaL_error(pLua, "Invalid downmix type");
    }

    eAudioDownmix * pDownmix = new eAudioDownmix;

    /* add required arguments to downmix input type data */

    *pDownmix = (eAudioDownmix)downmix;

    /* create lua event and give it scan data */
    CLuaDataEvent <eAudioDownmix> * pLuaEvent = 
        new CLuaDataEvent <eAudioDownmix>(eNotify_SetAudioDownmix, pDownmix, eNotify_AudioSourceChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setDualMono(
        --- required ---
        DualMono type,   DualMono type to use
        --- optional ---
        none
*/
static int brutusLua_SetDualMono(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [hdmiInputType]");
    }

    /* get arguments */
    uint32_t dualMono = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (eAudioDualMono_Max <= dualMono)
    {
        luaL_error(pLua, "Invalid dualMono type");
    }

    eAudioDualMono * pDualMono = new eAudioDualMono;

    /* add required arguments to DualMono input type data */

    *pDualMono = (eAudioDualMono)dualMono;

    /* create lua event and give it scan data */
    CLuaDataEvent <eAudioDualMono> * pLuaEvent = 
        new CLuaDataEvent <eAudioDualMono>(eNotify_SetAudioDualMono, pDualMono, eNotify_AudioSourceChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}

/* brutus.setDolbyDRC(
        --- required ---
        DolbyDRC type,   DolbyDRC type to use
        --- optional ---
        none
*/
static int brutusLua_SetDolbyDRC(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [hdmiInputType]");
    }

    /* get arguments */
    uint32_t dolbyDRC = luaL_checknumber(pLua, argNum++);

    /* error checking for arguments */
    if (eDolbyDRC_Max <= dolbyDRC)
    {
        luaL_error(pLua, "Invalid dolbyDRC type");
    }

    eDolbyDRC * pDolbyDRC = new eDolbyDRC;

    /* add required arguments to DolbyDRC input type data */

    *pDolbyDRC = (eDolbyDRC)dolbyDRC;

    /* create lua event and give it scan data */
    CLuaDataEvent <eDolbyDRC> * pLuaEvent = 
        new CLuaDataEvent <eDolbyDRC>(eNotify_SetDolbyDRC, pDolbyDRC, eNotify_AudioSourceChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}


/* brutus.setDolbyDialogNorm(
        --- required ---
        DialogNorm state,   if true, turn on dialog normalization
        --- optional ---
        none
*/
static int brutusLua_SetDolbyDialogNorm(lua_State * pLua)
{
    CLua      * pThis       = getCLua(pLua);
    eRet        err         = eRet_Ok;
    uint8_t     argNum      = 1;
    uint8_t     numArgTotal = lua_gettop(pLua) - 1;

    BDBG_ASSERT(pThis);

    /* check number of lua arguments on stack */
    if (1 != numArgTotal)
    {
        /* wrong number of arguments */
        luaL_error(pLua, "wrong number of arguments: [bool]");
    }

    /* get arguments */
    bool dialogNorm = lua_toboolean(pLua, argNum++);
    bool * pDialogNorm = new bool;

    /* add required arguments to DialogNorm data */
    *pDialogNorm = dialogNorm;

    /* create lua event and give it scan data */
    CLuaDataEvent <bool> * pLuaEvent = 
        new CLuaDataEvent <bool>(eNotify_SetDolbyDialogNorm, pDialogNorm, eNotify_AudioSourceChanged, B_WAIT_FOREVER);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);

    /* save lua event to queue - this event will be serviced when we get the bwin io callback:
       bwinLuaCallback() */
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}


static int brutusLua_SetDebugLevel(lua_State * pLua)
{
    CLua       * pThis       = getCLua(pLua);
    const char * strModule   = luaL_checkstring(pLua, 1);
    const char * strLevel    = luaL_checkstring(pLua, 2);
    if ((NULL != strModule) && (NULL != strLevel))
    {
        BDBG_Level level = (BDBG_Level)0;
        if (!strcasecmp(strLevel, "trace"))
        {
            level = BDBG_eTrace;
        }
        else
        if (!strcasecmp(strLevel, "msg"))
        {
            level = BDBG_eMsg;
        }
        else
        if (!strcasecmp(strLevel, "wrn"))
        {
            level = BDBG_eWrn;
        }
        else
        if (!strcasecmp(strLevel, "err"))
        {
            level = BDBG_eErr;
        }
        else
        if (!strcasecmp(strLevel, "log"))
        {
            level = BDBG_eLog;
        }
        else
        {
            level = BDBG_eWrn;
        }

        BDBG_WRN(("Set debug level:%d for module:%s", level, strModule));
        BDBG_SetModuleLevel(strModule, level);
    }
    else
    {
        luaL_error(pLua, "wrong number of arguments: [module][debugLevel]");
    }

    return 0;
}

static int brutusLua_RunScript(lua_State * pLua)
{
    CLua       * pThis       = getCLua(pLua);
    const char * strFileName = luaL_checkstring(pLua, 1);
    if (NULL != strFileName)
    {
        int lerror = 0;

        BDBG_WRN(("Running script: %s", strFileName));
        lerror = luaL_loadfile(pThis->getLuaState(), strFileName);
        if (lerror)
        {
            BDBG_WRN(("LUA error: %s", lua_tostring(pThis->getLuaState(), -1)));
            lua_pop(pThis->getLuaState(), 1);
        }
        else
        {
            lerror = lua_pcall(pThis->getLuaState(), 0, 0, 0);
            if (lerror)
            {
                BDBG_WRN(("LUA error: %s", lua_tostring(pThis->getLuaState(), -1)));
                lua_pop(pThis->getLuaState(), 1);
            }
        }
    }

    return 0;
}

static int brutusLua_Sleep(lua_State * pLua)
{
    int         usecs     = 0;

    usecs = lua_tointeger(pLua, -1);
    BKNI_Sleep(usecs);

    return 0;
}

static int brutusLua_Exit(lua_State * pLua)
{
    CLua      * pThis     = getCLua(pLua);
    CLuaEvent * pLuaEvent = NULL;
    eRet        err       = eRet_Ok;

    /* save lua event and necessary data */
    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    pLuaEvent = new CLuaEvent(eNotify_Exit);
    CHECK_PTR_GOTO("Unable to malloc CLuaEvent", pLuaEvent, err, eRet_OutOfMemory, error);
    pThis->addEvent(pLuaEvent);

    /* trigger bwin io event here */
    pThis->trigger(pLuaEvent);

error:
    return 0;
}


/* list of brutus lua extension APIs */
static const struct luaL_Reg brutusLua[] = {
    {"channelUp",           brutusLua_ChannelUp},           /* channel up */
    {"channelDown",         brutusLua_ChannelDown},         /* channel down */
#if NEXUS_HAS_FRONTEND
    {"scanQam",             brutusLua_ScanQam},             /* scan for qam channels */
    {"scanVsb",             brutusLua_ScanVsb},             /* scan for vsb channels */
    {"scanSat",             brutusLua_ScanSat},             /* scan for sat channels */
    {"scanOfdm",            brutusLua_ScanOfdm},            /* scan for ofdm channels */
#endif
    {"setVideoFormat",      brutusLua_SetVideoFormat},      /* set the video format */
    {"setColorSpace",       brutusLua_SetColorSpace},       /* set the color space */
    {"setMpaaDecimation",   brutusLua_SetMpaaDecimation},   /* set mpaa decimation */
    {"setDeinterlacer",     brutusLua_SetDeinterlacer},     /* set MAD deinterlacer */
    {"setBoxDetect",        brutusLua_SetBoxDetect},        /* set box detect */
    {"setAspectRatio",      brutusLua_SetAspectRatio},      /* set aspect ratio */
    {"setAutoVideoFormat",  brutusLua_SetAutoVideoFormat},  /* set auto video format */
    {"channelListLoad",     brutusLua_ChannelListLoad},     /* load channel list */
    {"channelListSave",     brutusLua_ChannelListSave},     /* save current channel list */
    {"channelListDump",     brutusLua_ChannelListDump},     /* dump current channel list */
    {"playbackStart",       brutusLua_PlaybackStart},       /* Start Playback of a file */
    {"playbackStop",        brutusLua_PlaybackStop},        /* Stop Playback of a file */
    {"playbackTrickMode",   brutusLua_PlaybackTrickMode},   /* Do a Custom trick Mode */
    {"refreshPlaybackList", brutusLua_RefreshPlaybackList}, /* refresh list of available videos */
    {"recordStart",         brutusLua_RecordStart},         /* Start Record of a file */
    {"recordStop",          brutusLua_RecordStop},          /* Stop Record of a file */
    {"setAudioProgram",     brutusLua_SetAudioProgram},     /* set the audio program pid */
    {"setSpdifInput",       brutusLua_SetSpdifInput},       /* set the spdif audio input type */
    {"setHdmiAudioInput",   brutusLua_SetHdmiAudioInput},   /* set the hdmi audio input type */
    {"setDownmix",          brutusLua_SetDownmix},          /* set the audio downmix */
    {"setDualMono",         brutusLua_SetDualMono},         /* set the audio DualMono */
    {"setDolbyDRC",         brutusLua_SetDolbyDRC},         /* set the Dolby DRC compression */
    {"setDolbyDialogNorm",  brutusLua_SetDolbyDialogNorm},  /* set the Dolby dialog normalization */
    {"setDebugLevel",       brutusLua_SetDebugLevel},       /* set debug level for given module */
    {"runScript",           brutusLua_RunScript},           /* Run given lua script */    
    {"sleep",               brutusLua_Sleep},               /* sleep */
    {"exit",                brutusLua_Exit},                /* exit application */
    {NULL,                  NULL}                           /* sentinel */
};

static int luaopen_brutus(lua_State * pLua)
{
    luaL_register(pLua, LUA_BRUTUSLIBNAME, brutusLua);
    return 1; 
}

#ifdef LINENOISE_SUPPORT
int autoCompleteDictionaryCompare(MString * str1, MString * str2)
{
    return (strcmp(str1->s(), str2->s()));
}

static MAutoList <MString> autoCompleteDictionary;
#endif


CLua::CLua() :
    _pLua(NULL),
    _threadRun(false),
    _busyLuaEvent(eNotify_Invalid),
    _pollInterval(200),
    _threadShell(NULL),
    _shellStarted(false),
    _pModelConfig(NULL),
    _pCfg(NULL)
{
    eRet err = eRet_Ok;

    _eventMutex = B_Mutex_Create(NULL);
    CHECK_PTR_GOTO("unable to create mutex", _eventMutex, err, eRet_ExternalError, error);

    _busyEvent = B_Event_Create(NULL);
    CHECK_PTR_GOTO("unable to create event", _busyEvent, err, eRet_ExternalError, error);

    return;
error:
    BDBG_ASSERT(false);
}

CLua::~CLua()
{
    uninitialize();

    if (NULL != _busyEvent)
    {
        B_Event_Destroy(_busyEvent);
        _busyEvent = NULL;
    }

    if (NULL != _eventMutex)
    {
        B_Mutex_Destroy(_eventMutex);
        _eventMutex = NULL;
    }
}

eRet CLua::initialize(CModelConfig * pModelConfig)
{
    eRet ret = eRet_Ok;
    BDBG_ASSERT(NULL != pModelConfig);

    _pModelConfig    = pModelConfig;
    _pCfg            = pModelConfig->getCfg();

#ifdef LINENOISE_SUPPORT
    autoCompleteDictionary.add(new MString("exit"));
    autoCompleteDictionary.add(new MString("io.write("));
    autoCompleteDictionary.add(new MString("io.read("));
    autoCompleteDictionary.add(new MString("help"));

    /* auto add brutus custom commands to auto complete list */
    for (int i = 0; brutusLua[i].name; i++)
    {
        MString * pStr = NULL;
        pStr = new MString("brutus.");
        *pStr += brutusLua[i].name;
        *pStr += "(";
        autoCompleteDictionary.add(pStr);
    }

    /* dtt todo - might better to manually sort by popularity rather than alphabetically.
       unless list gets huge, it probably won't matter at all as even a linear search is
       plenty fast when it comes to user input. */
    autoCompleteDictionary.sort(autoCompleteDictionaryCompare);
#endif

    _pLua = lua_open();
    CHECK_PTR_GOTO("lua open failed", _pLua, ret, eRet_ExternalError, error);

    /* open lua basic lib, table lib, i/o lib, string lib and math lib */
    luaL_openlibs(_pLua);
    /* open lua brutus lib */
    luaopen_brutus(_pLua);

    /* save "this" pointer to lua registry */
    lua_pushlightuserdata(_pLua, (void *)&LUAKEY_CLUA);
    lua_pushinteger(_pLua, (int)this);
    lua_settable(_pLua, LUA_REGISTRYINDEX);

error:
    return ret;
}

eRet CLua::uninitialize()
{
    eRet ret = eRet_Ok;

    autoCompleteDictionary.clear();

    if (NULL != _pLua)
    {
        lua_close(_pLua);
    }

    return ret;
}

void CLua::addEvent(CLuaEvent * pEvent)
{
    CScopedMutex scopedMutex(_eventMutex);

    _eventList.add(pEvent);
}

CLuaEvent * CLua::getEvent()
{
    CLuaEvent    * pLuaEvent                 = NULL;
    CScopedMutex   scopedMutex(_eventMutex);

    if (0 < _eventList.total())
    {
        pLuaEvent = (CLuaEvent *)_eventList.first();
    }

    return pLuaEvent;
}

CLuaEvent * CLua::removeEvent()
{
    CLuaEvent    * pLuaEvent                 = NULL;
    CScopedMutex   scopedMutex(_eventMutex);

    if (0 < _eventList.total())
    {
        pLuaEvent = (CLuaEvent *)_eventList.first();
        _eventList.remove();
    }

    return pLuaEvent;
}


#ifdef LINENOISE_SUPPORT
void luaShellCompletionCallback(const char *buf, linenoiseCompletions *lc) 
{
    MList <MString>     * pList                         = NULL;
    MString             * pString                       = NULL;
    MListItr <MString>    itr(&autoCompleteDictionary);

    if (!isalpha(buf[0]))
    {
        return;
    }

    /* search all autocomplete entries */
    for (pString = itr.first(); NULL != pString; pString = itr.next())
    {
        /* only add autocomplete choices that match current buffer */
        if (pString->left(strlen(buf)) == MString(buf))
        {
            /* only add autocomplete choices that aren't exactly the same as given buffer */
            if (pString->length() != MString(buf).length())
            {
                linenoiseAddCompletion(lc, (char *)pString->s());
            }
        }
    }
}
#endif

#include <termios.h>

static int tty_cbreak(int fd)
{
    struct termios ios;

    if (tcgetattr(fd, &ios) < 0)
    {
        return -1;
    }

    BDBG_MSG(("Inital termios settings - c_lflag:%x c_cc[VMIN]:%x c_cc[VTIME]:%x", 
              ios.c_lflag, ios.c_cc[VMIN], ios.c_cc[VTIME]));
    ios.c_lflag &= ~(ICANON | ECHO);
    ios.c_cc[VMIN] = 1;
    ios.c_cc[VTIME] = 0;
    BDBG_MSG(("Modified termios settings - c_lflag:%x c_cc[VMIN]:%x c_cc[VTIME]:%x", 
              ios.c_lflag, ios.c_cc[VMIN], ios.c_cc[VTIME]));

    return tcsetattr(fd, TCSAFLUSH, &ios);
}

static int tty_reset(int fd)
{
    struct termios ios;

    if (tcgetattr(fd, &ios) < 0)
    {
        return -1;
    }

    ios.c_lflag     |= (ICANON | ECHO);
    ios.c_cc[VMIN]   = 1;
    ios.c_cc[VTIME]  = 0;

    BDBG_WRN(("Restored termios settings - c_lflag:%x c_cc[VMIN]:%x c_cc[VTIME]:%x", 
              ios.c_lflag, ios.c_cc[VMIN], ios.c_cc[VTIME]));

    return tcsetattr(fd, TCSANOW, &ios);
}

int kbdhit(void)
{
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    return select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv) && FD_ISSET(STDIN_FILENO, &rfds);
}

void luaShell(void * pParam)
{
    CLua            * pLua        = (CLua *)pParam;
    int               lerror      = 0;
    char            * line        = NULL;
    char            * prompt      = NULL;
    CConfiguration  * pCfg        = NULL;
    char            * pScriptName = NULL;

    BDBG_ASSERT(NULL != pLua);

    pCfg = pLua->getCfg();
    BDBG_ASSERT(NULL != pCfg);

    /* if script name is set, then load and run it */
    pScriptName = (char *)GET_STR(pCfg, LUA_SCRIPT);
    if ((NULL != pScriptName) && (0 < strlen(pScriptName)))
    {
        BDBG_WRN(("Running script: %s", pScriptName));
        lerror = luaL_loadfile(pLua->getLuaState(), pScriptName);
        lerror = lua_pcall(pLua->getLuaState(), 0, 0, 0);
    }

#ifdef LINENOISE_SUPPORT
    prompt = (char *)GET_STR(pCfg, LUA_PROMPT);

    /* callback is used for autocompletion feature */
    linenoiseSetCompletionCallback(luaShellCompletionCallback);

    /* load past history from file */
    linenoiseHistoryLoad((char *)GET_STR(pCfg, LUA_HISTORY_FILENAME));

    tty_cbreak(STDIN_FILENO);
    while(true == pLua->_threadRun)
    {
        char lineLast[256];

        fprintf(stdout, "\r%s", prompt);
        fflush(stdout);
        BKNI_Sleep(pLua->_pollInterval);

        /* non-blocking check for key hit but do not read - linenoise will read the value after first
           changing the terminal settings */
        if (kbdhit())
        {
            /* use linenoise prompt to get user input.  linenoise has autocomplete (tab) as well as
               persistent history (up/down arrows) capabilites. */
            line = linenoise(prompt);

            if ('\0' != line[0])
            {
                BDBG_MSG(("echo: '%s'\n", line));

                if (0 != strncmp(lineLast, line, 255))
                {
                    /* add entered line to history (if different than the last command) and save */
                    linenoiseHistoryAdd(line);
                    linenoiseHistorySave((char *)GET_STR(pCfg, LUA_HISTORY_FILENAME)); 
                    strncpy(lineLast, line, 255);
                }

                /* chance to handle input command or pass it to lua */
                if (false == pLua->handleInput(line))
                {
                    /* pass entered line to lua */
                    lerror  = luaL_loadbuffer(pLua->getLuaState(), line, strlen(line), "line");
                    lerror |= lua_pcall(pLua->getLuaState(), 0, 0, 0);
                    if (lerror)
                    {
                        BDBG_WRN(("LUA error: %s", lua_tostring(pLua->getLuaState(), -1)));
                        lua_pop(pLua->getLuaState(), 1);
                    }
                }
            }

            /* linenoise allocates memory for the user generated line, we must free it here */
            free(line);
        }
    }
    tty_reset(STDIN_FILENO);
#endif

    pLua->setStartState(false);
}


/* bwin io callback that is executed when lua io is triggered */
void bwinLuaCallback(bwidget_engine_t engine, bwin_io_handle handle)
{
    eRet                 ret      = eRet_Ok;
    CLua               * pLua     = (CLua *)handle;
    CLuaEvent          * pEvent;

    BDBG_ASSERT(NULL != pLua);

    /* handle all queued lua events */
    while (NULL != (pEvent = pLua->removeEvent()))
    {
        BDBG_MSG(("Notify Observers for Lua event code: %#x", pEvent->getId()));
        ret = pLua->notifyObservers(pEvent->getId(), (void *)pEvent->getDataPtr());
        CHECK_ERROR_GOTO("error notifying observers", ret, error);

        /* delete lua event and any associated data */
        delete pEvent;
    }

error:
    return;
}

void CLua::trigger(CLuaEvent * pLuaEvent)
{
    BDBG_ASSERT(NULL != pLuaEvent);

    /* save copy of lua event in case we have to wait for a response notification */
    _busyLuaEvent = *pLuaEvent;

    bwidget_trigger_io(getWidgetEngine(), (bwin_io_handle)this);

    /* if command specified a valid wait notification, we will wait for it here */
    if (eNotify_Invalid != pLuaEvent->getWaitNotification())
    {
        B_Error berr = B_ERROR_SUCCESS;

        BDBG_MSG(("Waiting for lua command (%d) to complete...", pLuaEvent->getWaitNotification()));
        B_Event_Reset(_busyEvent);
        berr = B_Event_Wait(_busyEvent, pLuaEvent->getWaitTimeout());
        if (B_ERROR_SUCCESS != berr)
        {
            BDBG_ERR(("Lua wait timed out or returned error!"));
        }
    }
}

bool CLua::handleInput(char * pLine)
{
    bool ret    = false;
    int  maxLen = 0;

    BDBG_ASSERT(NULL != getWidgetEngine());

    if (NULL == pLine)
    {
        return ret;
    }

    if (0 == strncasecmp("exit", pLine, strlen("exit")))
    {
        brutusLua_Exit(getLuaState());

        /* mark as handled so it will not be forwarded on to lua */
        ret = true;
    }
    else
    if (0 == strncasecmp("help", pLine, strlen("help")))
    {
        printf("Help:\n");
        printf("\t<tab>\t\tAutocomplete\n");
        printf("\tbrutus.<tab>\tAutocomplete Brutus specific Lua functions\n");
        printf("\t<arrow Up/Down>\tHistory\n");
        printf("\t<help>\t\tShow Help\n");
        printf("\t<exit>\t\tQuit Application\n");

        /* mark as handled so it will not be forwarded on to lua */
        ret = true;
    }

error:
    return ret;
}

void CLua::run(bwidget_engine_t widgetEngine)
{
    B_ThreadSettings   settings;

    if (true == getStartState())
    {
        BDBG_WRN(("attempting to start lua when already started - ignored."));
        return;
    }

    _widgetEngine = widgetEngine;

    bwidget_add_io_handle(_widgetEngine, (bwin_io_handle)this, bwinLuaCallback);

    /* must set thread start state before starting thread or it will simply exit */
    _threadRun = true;

    B_Thread_GetDefaultSettings(&settings);
    _threadShell = B_Thread_Create("lua", luaShell, (void *)this, &settings);

    setStartState(true);
}

void CLua::stop()
{
    if (false == getStartState())
    {
        BDBG_WRN(("attempting to stop an already stopped Lua"));
        return;
    }

    bwidget_remove_io_handle(_widgetEngine, (bwin_io_handle)this);

    /* attempt to stop thread gracefully */
    _threadRun = false;
    BKNI_Sleep(_pollInterval + 1);

    if (NULL != _threadShell)
    {
        B_Thread_Destroy(_threadShell);
        _threadShell = NULL;
    }

    setStartState(false);
}

void CLua::processNotification(CNotification & notification)
{
    BDBG_MSG(("--- CLua Notification Received: %d ---", notification.getId()));
    BDBG_ASSERT(eNotify_Invalid != notification.getId());

    switch(notification.getId())
    {
#if NEXUS_HAS_FRONTEND
    case eNotify_ScanStarted:
        {
            CTunerScanNotificationData * pScanData = (CTunerScanNotificationData *)notification.getData();
            if (NULL != pScanData)
            {
                BDBG_MSG(("Tuner name:%s type:%lu num:%lu - Channel scan started",
                          pScanData->getTuner()->getName(),
                          pScanData->getTuner()->getType(),
                          pScanData->getTuner()->getNumber()));
            }
        }
        break;
    case eNotify_ScanStopped:
        {
            CTunerScanNotificationData * pScanData = (CTunerScanNotificationData *)notification.getData();
            if (NULL != pScanData)
            {
                BDBG_MSG(("Tuner name:%s type:%lu num:%lu - Channel scan stopped",
                          pScanData->getTuner()->getName(),
                          pScanData->getTuner()->getType(),
                          pScanData->getTuner()->getNumber()));
            }
            else
            {
                BDBG_WRN(("scan stopped for unknown/invalid tuner"));
            }
        }
        break;
    case eNotify_ScanProgress:
        {
            CTunerScanNotificationData * pScanData = (CTunerScanNotificationData *)notification.getData();
            if (NULL != pScanData)
            {
                BDBG_MSG(("Tuner name:%s type:%lu num:%lu - Channel scan %lu%% complete",
                          pScanData->getTuner()->getName(),
                          pScanData->getTuner()->getType(),
                          pScanData->getTuner()->getNumber(),
                          pScanData->getPercent()));
            }
        }
        break;
#endif /* NEXUS_HAS_FRONTEND */
    default:
        break;
    }

    /* check to see if the current notification is one that a pending lua command is waiting on */
    if (notification.getId() == _busyLuaEvent.getWaitNotification())
    {
        B_Event_Set(_busyEvent);
    }
}

