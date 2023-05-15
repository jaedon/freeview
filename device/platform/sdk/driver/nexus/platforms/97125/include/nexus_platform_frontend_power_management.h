/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_platform_frontend_power_management.h $
* $brcm_Revision: 16 $
* $brcm_Date: 5/12/11 12:31p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97125/include/nexus_platform_frontend_power_management.h $
* 
* 16   5/12/11 12:31p mward
* SW7125-905:  Adding option to not re-open frontends when switching
*  control.
* 
* 15   11/30/10 9:39a mward
* SW7125-737:  Added NEXUS_Platform_FrontendType().  App should use it to
*  get frontend type, and must not attempt to access the userParams to
*  read the frontend type.
* 
* 14   11/22/10 5:39p mward
* SW7125-737:  Adding NEXUS_Platform_ReadFrontendControlSharedMemory()
* 
* SW7125-729/SW7125-737/1   11/12/10 10:44a mward
* SW7125-737:  Adding NEXUS_Platform_ReadFrontendControlSharedMemory()
* 
* 13   11/22/10 12:37p mward
* SW7125-729:  NEXUS_Platform_PowerUpLNA() and
*  NEXUS_Platform_PowerDownLNA()
* 
* SW7125-729/1   11/10/10 5:34p mward
* SW7125-729:  NEXUS_Platform_PowerUpLNA() and
*  NEXUS_Platform_PowerDownLNA()
* 
* 12   10/22/10 12:31p ayxiang
* SW7125-675: change and add API to provide frontend handle to be
*  controlled by user
* 
* 11   10/1/10 5:36p mward
* SW7125-497: Corrected declaration format for kernel/proxy mode compile.
* 
* 10   9/28/10 3:27p ayxiang
* SW7125-497: correct the name for the function writing to shared memory
* 
* 9   9/28/10 3:04p ayxiang
* SW7125-497: remove 3112 host control function which will be included
*  into frontend host control function; modify writed dhared memory
*  function declaration
* 
* 8   8/26/10 6:15p mward
* SW7125-497: Host control for internal tuner/DS.
* 
* 7   8/10/10 4:32p ayxiang
* SW7125-547: make the comment more clear
* 
* 6   7/13/10 5:08p ayxiang
* SW7125-497: change name for wakeup/shutdown eCM
* 
* 5   7/1/10 6:49p mward
* SW7125-497: Corrected declaration formats for kernel/proxy mode
*  compile.
* 
* 4   6/29/10 7:29p ayxiang
* SW7125-497: make CM wakeup/shutdown public
* 
* 3   6/29/10 4:26p mward
* SW7125-474:  Move static private function declarations to
*  nexus_platform_frontend_power_management.c.  Format declarations for
*  proxy build.
* 
* 2   6/28/10 8:12p ayxiang
* SW7125-497: Add functions to enable/disable host 3112 control
* 
* 1   6/28/10 6:43p ayxiang
* SW7125-497: frontend power management file
* 
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_FRONTEND_POWER_MANAGEMENT_H__
#define NEXUS_PLATFORM_FRONTEND_POWER_MANAGEMENT_H__

#include "nexus_platform.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct NEXUS_PlatformFrontendStandby_Settings 
{
   int NEXUS_PlatformFrontendStandbyVersion;
} NEXUS_PlatformFrontendStandby_Settings ;


/***************************************************************************
Summary:
Platform frontend power management mode used in NEXUS_PlatformStandbySettings

Description:

***************************************************************************/
typedef enum NEXUS_PlatformFrontendStandbyMode
{
    NEXUS_PlatformFrontendStandbyMode_eFullPower,	/*normal power on mode*/
    NEXUS_PlatformFrontendStandbyMode_ePassive, /* IrInput, UhfInput, HdmiOutput (CEC), Gpio and Keypad are available to be configured as wakeup devices. MIPS will be put to sleep by the OS. Lowest power. */
    NEXUS_PlatformFrontendStandbyMode_eActive,  /* frontend and transport modules are running. All wakeup devices for ePassive are available. */
    NEXUS_PlatformFrontendStandbyMode_eMax
} NEXUS_PlatformFrontendStandbyMode;

/***************************************************************************
Summary:
State used for NEXUS_PlatformFrontendPowerManagement
***************************************************************************/
typedef struct NEXUS_PlatformFrontendStandbyState
{
    NEXUS_PlatformFrontendStandbyMode mode;

} NEXUS_PlatformFrontendStandbyState;


/***************************************************************************
Summary:
Set frontend standby settings

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_SetFrontendStandbySetting(
	const NEXUS_PlatformFrontendStandby_Settings *pSettings
	);

/***************************************************************************
Summary:
Get frontend standby settings

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_GetFrontendStandbySetting(
	NEXUS_PlatformFrontendStandby_Settings *pSettings /* [out] */
	);

/***************************************************************************
Summary:
Get frontend standby default settings

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_GetDefaultFrontendStandbySetting(
	NEXUS_PlatformFrontendStandby_Settings *pSettings /* [out] */
	);

/***************************************************************************
Summary:
Get frontend standby mode

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_GetFrontendStandbyMode(
	NEXUS_PlatformFrontendStandbyMode *pStandbyMode /* [out] */
	);

/***************************************************************************
Summary:
Wakeup eCM

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_WakeupCM(void);

/***************************************************************************
Summary:
Shutdown eCM

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_ShutdownCM(void);

/***************************************************************************
Summary:
Read frontend control information from shared memory

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_ReadFrontendControlSharedMemory(
    uint32_t *pHostControlledFrontend /* [out] */
    );

/***************************************************************************
Summary:
Write frontend control information to shared memory

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_WriteFrontendControlSharedMemory(
    uint32_t hostControlledFrontend
    );

/***************************************************************************
Summary:
Set frontend standby mode

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_SetFrontendStandbyMode(
	NEXUS_PlatformFrontendStandbyMode standbyMode
	);


/***************************************************************************
Summary:
Enable host frontend control   

Description:
On the 97125 and 97019 Reference Platforms, the bus BSC_M3 used
for control of external frontends and LNA is controlled by either
host or eCM.  Therefore, either host or eCM controls all frontends.
This functions switches control from eCM to host.

First, the platform's open handles for eCM controlled frontends are all closed.

Then control of BSC_M3 is switched. Finally, if openAllFrontends is true,
the platform's default set of frontends is opened in the default order
as done by NEXUS_Platform_InitFrontend(), (calling NEXUS_Platform_Init(pSettings->openFrontend=true)).

After calling this function, the app must get the new frontend handles using
NEXUS_Platform_OpenFrontend(), NEXUS_Platform_Frontend_GetFrontendInstanceControlled(),
or by calling NEXUS_Platform_GetConfiguration() to get an updated
platform configuration containing the new frontend handles.
If openAllFrontends is false, the app should use NEXUS_Platform_OpenFrontend()
to open the host-controlled frontend(s) in the desired order.

***************************************************************************/
NEXUS_Error NEXUS_Platform_Frontend_EnableHostControl(
	bool openAllFrontends
	);

/***************************************************************************
Summary:
Disable host frontend control

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_Frontend_DisableHostControl(
	bool openAllFrontends
	);

/***************************************************************************
Summary:
Get frontend to be controlled after Standby/ON status switched

Description:
On the 97125 and 97019 Reference Platforms, the bus BSC_M3 used
for control of external frontends and LNA is controlled by either
host or eCM.  Therefore, either host or eCM controls all frontends.
This functions switches control from host to eCM.

First, the platform's open handles for host controlled frontends are all closed.

Then control of BSC_M3 is switched. Finally, if openAllFrontends is true,
the platform's default set of frontends is opened in the default order
as done by NEXUS_Platform_InitFrontend(), (calling NEXUS_Platform_Init(pSettings->openFrontend=true)).

After calling this function, the app must get the new frontend handles using
NEXUS_Platform_OpenFrontend(), NEXUS_Platform_Frontend_GetFrontendInstanceControlled(),
or by calling NEXUS_Platform_GetConfiguration() to get an updated
platform configuration containing the new frontend handles.
If openAllFrontends is false, the app should use NEXUS_Platform_OpenFrontend()
to open the eCM-controlled frontend(s) in the desired order.

***************************************************************************/
NEXUS_Error NEXUS_Platform_Frontend_GetFrontendInstanceControlled(
	NEXUS_FrontendHandle * pControlledFrontend, /* [out] */
	int index
	);

/***************************************************************************
Summary:
Power up the LNA

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_PowerUpLNA(void);

/***************************************************************************
Summary:
Power down the LNA

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_PowerDownLNA(void);

/***************************************************************************
Summary:
Get the frontend type (chip number).

Description:

***************************************************************************/
NEXUS_Error NEXUS_Platform_FrontendType(
	NEXUS_FrontendHandle handle,
    uint32_t *pFrontendType /* [out] */
	);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef NEXUS_PLATFORM_FRONTEND_POWER_MANAGEMENT_H__ */

