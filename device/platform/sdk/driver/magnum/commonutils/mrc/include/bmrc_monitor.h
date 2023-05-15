/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmrc_monitor.h $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 5/21/12 4:32p $
 *
 * Module Description:
 *
 * Implementation of the Realtime Memory Monitor for 7038
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mrc/7038/bmrc_monitor.h $
 * 
 * Hydra_Software_Devel/12   5/21/12 4:32p albertl
 * SW7422-168: Fixed comments for custom and block tags.
 * 
 * Hydra_Software_Devel/11   1/13/11 5:20p albertl
 * SW7422-168: Default number of checkers set to -1 in
 * BMRC_Monitor_Settings, handled as maximum available.
 * 
 * Hydra_Software_Devel/10   1/12/11 3:09p albertl
 * SW7422-168: Added comments for BMRC_Monitor_Settings.
 * 
 * Hydra_Software_Devel/9   1/12/11 2:50p albertl
 * SW7422-168: Changed BMRC_Monitor_Open to take settings structure.
 * Removed BMRC_Monitor_SetSettings().  Added ability to configure how
 * many checkers BMRC_Monitor uses to support more than one monitor on
 * same MEMC.
 * 
 * Hydra_Software_Devel/8   12/6/10 6:16p albertl
 * SW7405-3892: Added ability to configure MRC violation behavior.
 * 
 * Hydra_Software_Devel/7   6/30/10 6:11p albertl
 * SW7405-4047: Added BMRC_Monitor_JailByFilename.
 * 
 * Hydra_Software_Devel/6   3/2/10 1:25p erickson
 * SW7405-3892: add BMRC_Monitor_PrintClients for debug, fix
 * BMRC_Monitor_RemoveCustomTag name, add sample code for blocking
 * unwanted kernel writes
 *
 * Hydra_Software_Devel/5   4/3/06 4:25p albertl
 * PR20489:  Added BMRC_Monitor_JailDefault function.
 *
 * Hydra_Software_Devel/4   3/1/06 5:16p albertl
 * PR18701:  Updated to match include latest RMM updates.
 *
 * Hydra_Software_Devel/3   8/4/05 6:50p albertl
 * PR13641:  Fixed incorrect PR numbers in chagelog.
 *
 * Hydra_Software_Devel/1   8/1/05 10:14p albertl
 * PR13641:  Initial Revision, based on RMM module.
 *
 ***************************************************************************/
#ifndef BMRC_MONITOR_H_
#define BMRC_MONITOR_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "bint.h"
#include "bchp.h"
#include "bmem.h"
#include "bmrc.h"

/*=Module Overview: ********************************************************
BMRC_Monitor provides a way to monitor which hardware clients access certain blocks of memory.
BMRC_Monitor uses the Address Range Checker (ARC) hardware which is a feature of the memory controller (MEMC).
It does this via the BMRC module.

This module easily integrates with magnum's memory manager (BMEM), which automatically configures
clients based on BMEM_Alloc allocations, using the debug tag filename to associate a memory block
with a certain set of MEMC clients.

Because there are limited ARC's on each chip, BMRC_Monitor performs a combine operation on all BMRC_Monitor
regions and clients. The end result is that multiple blocks and hardware clients may be combined into a
single region. This limits the accuracy of the check. Not every bad memory access can be caught.

To print the final BMRC_Monitor configuration, enable DBG's BDBG_eMsg Level for the BMRC_Monitor module and
call BMRC_Monitor_JailUpdate.  For Settop API, set msg_modules=mem_monitor.

Example:
This shows the default configuration for BMRC_Monitor using the automatic configuration
from BMEM. Error checking code is ommited for the sake of simplicity.

void main(void)
{
	BMRC_Monitor_Handle hMonitor;
	BMEM_Handle heap;
	BREG_Handle reg;
	BINT_Handle isr;
	BCHP_Handle chp;
	BMRC_Handle mrc;
	BERR_Code rc;
	static BMEM_MonitorInterface interface;

	.... Initialize MemoryManager, RegisterInterface, InterruptInterface, ChipInterface, and MemoryRangeChecker ...

	rc = BMRC_Monitor_Open(&hMonitor, reg, isr, chp, mrc, 0, 256 * 1024 * 1024); ... Control all 256MBytes window ...
	rc = BMRC_Monitor_GetMemoryInterface(hMonitor, &interface);
	rc = BMEM_InstallMonitor(heap, &interface);
}

This shows the use of a jail for debugging a specific client:
	BMRC_Monitor_Handle hMonitor;
	BMEM_Handle heap;
	BREG_Handle reg;
	BINT_Handle isr;
	BCHP_Handle chp;
	BERR_Code rc;
	static BMEM_MonitorInterface interface;

	.... Initialize MemoryManager, RegisterInterface, InterruptInterface and ChipInterface ...

	rc = BMRC_Monitor_Open(&hMonitor, reg, isr, chp, mrc, 0, 256 * 1024 * 1024); ... Control all 256MBytes window ...
	rc = BMRC_Monitor_GetMemoryInterface(hMonitor, &interface);
	rc = BMEM_InstallMonitor(heap, &interface);
	... Performing AVD debug, so optimize range checking for AVD ...
	BMRC_Monitor_JailAdd(hMonitor, BRMM_CLIENT_AVD_BLK_0);
	BMRC_Monitor_JailAdd(hMonitor, BRMM_CLIENT_AVD_ILA_0);
	BMRC_Monitor_JailAdd(hMonitor, BRMM_CLIENT_AVD_OLA_0);
	BMRC_Monitor_JailAdd(hMonitor, BRMM_CLIENT_AVD_CAB_0);
	BMRC_Monitor_JailAdd(hMonitor, BRMM_CLIENT_AVD_SYM_0);
	BMRC_Monitor_JailUpdate(hMonitor);
}


See Also:
	BMEM_InstallMonitor
	BRMM_Open
****************************************************************************/

/*
 * Summary this type is used to specify memory client
 */
typedef unsigned BMRC_Monitor_MemoryClientId;

/*
 * Summary this type is used to specify instance of the memory monitor
 */
typedef struct BMRC_P_MonitorContext *BMRC_Monitor_Handle;

typedef struct BMRC_Monitor_Settings
{
	BMRC_AccessType eKernelBlockMode;    /* blocking mode when violations occur for kernel memory */
	BMRC_AccessType eBlockMode;          /* blocking mode when violations occur for non-kernel memory */
	uint32_t        ulNumCheckersToUse;  /* number of hardware checkers used by this instance of mrc monitor */
} BMRC_Monitor_Settings;

/***************************************************************************
Summary:
	Opens a realtime memory monitor

Description:
	You may create only one instance of RMM per system.

Returns:
	BERR_SUCCESS - Memory monitor was opened.

See Also:
	BMRC_Close
****************************************************************************/
BERR_Code BMRC_Monitor_Open(
		BMRC_Monitor_Handle *phMonitor,   /* [out] this argument is used to return instance (handle) of memory monitor */
		BREG_Handle hReg,                 /* RegisterInterface handle */
		BINT_Handle iIsr,                 /* InterruptInterface handle */
		BCHP_Handle hChp,                 /* ChipInterface handle */
		BMRC_Handle hMrc,                 /* Memory Range Checker Handle */
		uint32_t ulMemLow,                /* lowest address to control by the memory monitor, usually 0 */
		uint32_t ulMemHigh,               /* highest address to control by the memory monitor, usualy 256*2^20 - 1 */
		BMRC_Monitor_Settings *pSettings  /* monitor configuration settings.  use NULL for default settings. */
		);

/***************************************************************************
Summary:
	Closes memory monitor

Description:
	This function is used to release resources allocated by
	BMRC_Monitor_Open.

Returns:
	N/A

See Also:
	BMRC_Open
****************************************************************************/
void BMRC_Monitor_Close(
		BMRC_Monitor_Handle hMonitor /* Instance of the memory monitor */
		);

/***************************************************************************
Summary:
	Gets default settings for a MRC Monitor

Description:
	This function gets the default configuration settings for a MRC
	Monitor by filling in pDefSettings.  It will fill the usMaxNumCheckers
	with the value of -1 which indicates the monitor will use the maximum
	available number of hardware checkers.  It can be overridden to specify
	a number less than that.

Returns:
	N/A

See Also:
	BMRC_Monitor_Open
****************************************************************************/
BERR_Code
BMRC_Monitor_GetDefaultSettings(
		BMRC_Monitor_Settings *pDefSettings
		);

/***************************************************************************
Summary:
	Gets current settings for a MRC Monitor

Description:
	This function gets the current configuration settings for a MRC
	Monitor by filling in pDefSettings.

Returns:
	N/A

See Also:
	BMRC_Monitor_Open
****************************************************************************/
BERR_Code
BMRC_Monitor_GetSettings(
		BMRC_Monitor_Handle hMonitor, /* Instance of the memory monitor */
		BMRC_Monitor_Settings *pSettings
		);

/***************************************************************************
Summary:
	Returns BMEM_MonitorInterface which is passed into BMEM_InstallMonitor
	for automatic configuration.

Description:
	If your system uses multiple instances of BMEM, you can pass the
	BMEM_MonitorInterface into each one.

	BMEM_MonitorInterface is passed to BMEM by reference and so it is the
	application's responsibility to insure that the instance of
	BMEM_MonitorInterface remains intact all the time while the interface
	can be referenced. For example, do not use an instance of
	BMEM_MonitorInterface which is allocated as a function's local variable.


Returns:
	BERR_SUCCESS - Memory monitor was opened.

****************************************************************************/
BERR_Code
BMRC_Monitor_GetMemoryInterface(
		BMRC_Monitor_Handle hMonitor, /* Instance of the memory monitor */
		BMEM_MonitorInterface *pInterface /* [out] memory interface */
		);

/***************************************************************************
Summary:
	Resets jail to default configuration.

Description:
	This function is used to reset the "jail" to its default configuration.
	It will reset all changes to the jail made after the module was
	initialized and replace the jail client list with the standard default
	hardware client list.  User needs to call function BMRC_Monitor_JailUpdate
	before changes take effect.

Returns:
	BERR_SUCCESS - Memory monitor was opened.

See also:
	BMRC_Monitor_JailUpdate
	BMRC_Monitor_JailAdd
	BMRC_Monitor_JailRemove

****************************************************************************/
BERR_Code
BMRC_Monitor_JailDefault(
		BMRC_Monitor_Handle hMonitor /* Instance of the memory monitor */
		);

/***************************************************************************
Summary:
	Adds memory client associated with a filename into the jail.

Description:
	This function is used to add memory clients that are associated with a filename
	to the "jail".  These client would only be able to access memory blocks allocated by
	the file. User needs to call function BMRC_Monitor_JailUpdate before changes take effect.

Note:
	Current implementation uses the same rules for all clients which are "jailed".

Returns:
	BERR_SUCCESS - Memory monitor was opened.

See also:
	BMRC_Monitor_JailAdd
	BMRC_Monitor_JailUpdate
	BMRC_Monitor_JailRemove

****************************************************************************/
BERR_Code BMRC_Monitor_JailByFilename(
		BMRC_Monitor_Handle hMonitor, /* Instance of the memory monitor */
		const char *filename /* filename  */
		);

/***************************************************************************
Summary:
	Adds memory client into the jail.

Description:
	This function is used to add memory client to the "jail", such client would be able
	to access memory blocks which are allocated only to the given client. User needs
	to call function BMRC_Monitor_JailUpdate before changes take effect.

Note:
	Current implementation uses the same rules for all clients which are "jailed".

Returns:
	BERR_SUCCESS - Memory monitor was opened.

See also:
	BMRC_Monitor_JailUpdate
	BMRC_Monitor_JailRemove

****************************************************************************/
BERR_Code BMRC_Monitor_JailAdd(
		BMRC_Monitor_Handle hMonitor, /* Instance of the memory monitor */
		BMRC_Monitor_MemoryClientId client /* id of the memory client */
		);

/***************************************************************************
Summary:
	Removes memory client from the jail.

Description:
	This function is used to remove memory client to the "jail", any specific blocks would
	be removed from the speocified client.
	User needs to call function BMRC_Monitor_JailUpdate before changes take effect.

Returns:
	BERR_SUCCESS - Memory monitor was opened.

See also:
	BMRC_Monitor_JailUpdate
	BMRC_Monitor_JailAdd
****************************************************************************/
BERR_Code BMRC_Monitor_JailRemove(
		BMRC_Monitor_Handle hMonitor, /* Instance of the memory monitor */
		BMRC_Monitor_MemoryClientId client /* id of the memory client */
		);

/***************************************************************************
Summary:
	Applies previous JailAdd and JailRemove calls.

Description:
	This function is used to update memory guards which are used to isolate
	clients which are set into the "jail" mode.

Returns:
	BERR_SUCCESS - Memory monitor was opened.

See also:
	BMRC_Monitor_JailRemove
	BMRC_Monitor_JailAdd
****************************************************************************/
BERR_Code BMRC_Monitor_JailUpdate(
		BMRC_Monitor_Handle hMonitor /* Instance of the memory monitor */
		);


/***************************************************************************
Summary:
	Allow a client to access a memory block.

Description:
	This works in conjuction with the automatic BMEM configuration.
	You can change memory clients at runtime instead of modifying
	bmrc_monitor_clients.c's default list of clients.

	You should use block that was already allocated by the BMEM module,
	and any number of clients per block.  The actual configuration of memory
	blocks that will be monitored is based on the capabilities of the
	ARC hardware.

Returns:
	BERR_SUCCESS - Memory monitor was opened.

****************************************************************************/
BERR_Code BMRC_Monitor_BlockTag(
		BMRC_Monitor_Handle hMonitor, /* Instance of the memory monitor */
		uint32_t ulAddr, /* offset (address) of the allocated block */
		size_t size,  /* size (in bytes) of the allocated block */
		BMRC_Monitor_MemoryClientId client /* id of the memory client */
		);

/***************************************************************************
Summary:
	Instead of using the automatic BMEM configuration, manually specify
	that a client can access a memory block.

Description:
	This function is used to add a custom memory block instead of using
	the automatic BMEM configuration.  If you use this, the automatic BMEM
	configuration is not used. You must configure everything yourself.

	You can add any number of memory blocks, and any number of clients
	per block.  The actual configuration of memory blocks that will be
	monitored is based on the capabilities of the ARC hardware.

Note:
	o Custom memory blocks can not overlap
	o There is a limited amount of the custom memory block that ould be added

Returns:
	BERR_SUCCESS - Memory monitor was opened.

See also:
	BMRC_Monitor_RemoveCustomTag

****************************************************************************/
BERR_Code BMRC_Monitor_AddCustomTag(
		BMRC_Monitor_Handle hMonitor, /* Instance of the memory monitor */
		uint32_t ulAddr, /* offset (address) of the memory block */
		size_t size,  /* size (in bytes) of the memory block */
		BMRC_Monitor_MemoryClientId client /* id of the memory client */
		);

/***************************************************************************
Summary:
	Removes custom block from the memory monitor map

Description:
	This function is used to remove custom memory block.
	Once last custom memory block was removed, memory monitor would use automatic BMEM configuration.

You must call BMRC_Monitor_JailUpdate before this change takes effect.

Returns:
	BERR_SUCCESS - Memory monitor was opened.

See also:
	BMRC_Monitor_AddCustomTag

****************************************************************************/
BERR_Code BMRC_Monitor_RemoveCustomTag(
		BMRC_Monitor_Handle hMonitor, /* Instance of the memory monitor */
		uint32_t ulAddr, /* offset (address) of the memory block */
		size_t size,  /* size (in bytes) of the memory block */
		BMRC_Monitor_MemoryClientId client /* id of the memory client */
		);

/***************************************************************************
Summary:
Print the names of the clients set in ARC registers.

Description:
This does a reverse lookup of the ARC client registers. It allows the user
to see the names of the clients that are currently being monitored.

The client registers are ARC_x_READ_RIGHTS_x and ARC_x_WRITE_RIGHTS_x.
****************************************************************************/
BERR_Code BMRC_Monitor_PrintClients(
	BMRC_Monitor_Handle hMonitor,
	uint32_t clients0,
	uint32_t clients1,
	uint32_t clients2,
	uint32_t clients3
	);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BMRC_MONITOR_H_ */

/* End of File */

