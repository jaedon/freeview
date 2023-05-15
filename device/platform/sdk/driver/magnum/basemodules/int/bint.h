/***************************************************************************
 *	   Copyright (c) 2003-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint.h $
 * $brcm_Revision: Hydra_Software_Devel/31 $
 * $brcm_Date: 12/20/12 10:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/bint.h $
 * 
 * Hydra_Software_Devel/31   12/20/12 10:44p jgarrett
 * SW7425-4345: Merge to main
 * 
 * Hydra_Software_Devel/SW7425-4345/1   12/18/12 4:35p jgarrett
 * SW7425-4345: Adding BINT_ApplyL2State_isr
 * 
 * Hydra_Software_Devel/30   3/30/09 3:15p erickson
 * PR35457: update comments, especially regarding BINT_Id
 *
 * Hydra_Software_Devel/29   5/25/06 4:13p albertl
 * PR21392:  BINT_Stats functions now split off into bint_stats.h to solve
 * BTMR circular dependency.
 *
 * Hydra_Software_Devel/28   5/24/06 6:56p albertl
 * PR21392:  Changed BINT stats tracking to use timers from TMR module.
 * Added BINT_Stats_Enable and BINT_Stats_Disable.
 *
 * Hydra_Software_Devel/27   2/15/06 5:30p vsilyaev
 * PR 19693: Added support for acquiring interrupt rate
 *
 * Hydra_Software_Devel/26   9/12/05 7:18p darnstein
 * PR16885: include bstd.h.
 *
 * Hydra_Software_Devel/25   4/13/05 5:16p albertl
 * PR10596:  Moved bDefaultBins to BINT_Stats_CallbackStats.
 *
 * Hydra_Software_Devel/24   4/11/05 5:29p albertl
 * PR10596:  Added BINT_Stats_DestroyBins and BINT_Stats_Reset.
 * BINT_STATS_ENABLE now tied to BDBG_DEBUG_BUILD.
 *
 * Hydra_Software_Devel/23   4/7/05 4:15p albertl
 * PR10596:  Changed BINT_STATS_TRACK to BINT_STATS_ENABLE.
 *
 * Hydra_Software_Devel/22   4/6/05 2:26p albertl
 * PR10596:  Removed unnecessary #ifdef BINT_STATS_TRACK cases for better
 * readability.
 *
 * Hydra_Software_Devel/21   4/5/05 7:13p albertl
 * PR10596:  Added new statistics tracking functionality.
 *
 * Hydra_Software_Devel/20   4/16/04 5:08p marcusk
 * PR10666: Merged from B0 branch to keep API compatible.
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   4/16/04 4:55p marcusk
 * PR10666: Do not clear interrupts when enabling and added
 * BINT_ClearCallback() routines.
 *
 * Hydra_Software_Devel/19   12/29/03 3:58p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/18   12/1/03 4:52p jasonh
 * Added documentation for interrupt clearing.
 *
 * Hydra_Software_Devel/17   9/16/03 10:30a marcusk
 * Updated to comply with DocJet requirements. Fixes for PR8055.
 *
 * Hydra_Software_Devel/16   8/13/03 1:28p marcusk
 * Added naming convention note.
 *
 * Hydra_Software_Devel/15   6/18/03 3:25p dlwin
 * Updated comments.
 *
 * Hydra_Software_Devel/14   6/11/03 3:07p marcusk
 * Updated verbage to include only standarized L2 interrupt registers.
 *
 * Hydra_Software_Devel/13   4/2/03 10:39a marcusk
 * Updated to support flag to specify if the interrupt can be triggered by
 * the CPU.
 *
 * Hydra_Software_Devel/11   3/31/03 1:11p marcusk
 * Clarified some comments.
 *
 * Hydra_Software_Devel/10   3/21/03 6:30p marcusk
 * Minor updates.
 *
 * Hydra_Software_Devel/9   3/19/03 4:57p marcusk
 * Added int value parameter that is returned when an interrupt triggers.
 *
 * Hydra_Software_Devel/8   3/19/03 4:03p marcusk
 * Removed some spaces from module overview.
 *
 * Hydra_Software_Devel/7   3/19/03 4:02p marcusk
 * Used comments instead of spaces in module overview (twiki test).
 *
 * Hydra_Software_Devel/6   3/19/03 3:41p marcusk
 * Slightly modified comments.
 *
 * Hydra_Software_Devel/5   3/19/03 11:20a marcusk
 * Added module overview.
 *
 * Hydra_Software_Devel/4   3/17/03 9:06a marcusk
 * Updated with const strings and removed un-needed typedef.
 *
 * Hydra_Software_Devel/3   3/13/03 3:22p marcusk
 * Added a couple new function and updated comments.
 *
 * Hydra_Software_Devel/2   3/13/03 2:45p marcusk
 * Updated comments.
 *
 * Hydra_Software_Devel/1   3/11/03 5:42p marcusk
 * Initial version.
 *
 ***************************************************************************/

/*= Module Overview *********************************************************

The InterruptInterface is used to provide an abstraction that can be
used by software modules to provide the following functionality for
various L2 interrupts found in the system:

	1 Assign ISR callback routines to specific interrupts.
		* Multiple callbacks can be installed for a single interrupt.
	2 Enable and disable callbacks.
		* The InterruptInterface will only manage L2 interrupt registers and
	      will not touch bits that are not enabled through the exposed interface.
		  This allows platform specific code to "share" L1 interrupt bits between
		  platform specific code and the interrupt interface.
		* The implementation of the InterruptInterface will automatically
	      mask the L2 hardware interrupt when all callbacks using that specific
		  interrupt bit have been disabled.  Likewise it will un-mask an L2
		  interrupt when one or more callbacks are enabled for a specific interrupt.
	3 Trigger an interrupt (software controlled and if hardware supports it).
	4 Uses interrupt names to specify specific interrupt (calling modules do
      not need to know the layout of each interrupt bit, only the standard
	  name of the interrupt).
		* Strings are used to identify interrupts which allows code to select
	      the proper interrupt when multiple instances of an interrupt exist.
		  This is easily be accomplished through [[KernelInterface][BKNI_Snprintf]]
		  that allows a number value to be inserted into a string.
	5 To support various L2 implementations that may appear on a system
	  (ie, Bcm7028 and Bcm3250 chip in one system), the board/chip-specific code
	  must be provide functions to set/clear interrupts,
	  set/clear interrupt masks,  and process L2 interrupt.

*Interrupt Naming Convention*

The following naming convention should be used when defining interrupt
names:

BXXX_INTERRUPT_NAME

Where BXXX is the PortingInterface module that owns the interrupt.

***************************************************************************/

#ifndef BINT_H
#define BINT_H

#include "bstd.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Summary:
The BINT_Id typedef specifies the ID an interrupt.

Description:
Each BINT_Id value corresponds to one bit in an L2 interrupt register.
The value of BINT_Id contains both the L2 register address and the shift value for the interrupt bit in that L2 in a packed format.
Use the BCHP_INT_ID_CREATE macro (see bchp.h) to build a BINT_Id value.
*/
typedef uint32_t BINT_Id;

/*
Summary:
This structure defines an opaque handle that is used for InterruptInterface function calls.
*/
typedef struct BINT_P_Context *BINT_Handle;

/*
Summary:
This structure defines an opaque handle that is used for InterruptInterface function calls.
*/
typedef struct BINT_P_Callback *BINT_CallbackHandle;

/*
Summary:
This defines the function prototype that is used for InterruptInterface callbacks.
*/
typedef void (*BINT_CallbackFunc)( void * pParm1, int parm2 );

/*
Summary:
This function creates a callback.

Description:
After this function is used to create a callback, the BINT_EnableCallback()
function must be used before the callback will actually be enabled.  When
the specified interrupt triggers, the func specified will be called with the
pParm passed into this function.

The first time an callback is created for a specific interrupt, the interrupt
status will be cleared.
*/
BERR_Code BINT_CreateCallback(
							  BINT_CallbackHandle *pCbHandle, /* [out] Returns handle */
							  BINT_Handle handle, /* [in] InterruptInterface handle */
							  BINT_Id intId, /* [in] Id of the interrupt to associate with callback function */
							  BINT_CallbackFunc func, /* [in] Callback function that should be called when the specified interrupt triggers */
							  void * pParm1, /* [in] Parameter that is returned to callback function when interrupt triggers */
							  int parm2 /* [in] Parameter that is returned to callback function when interrupt triggers */
							  );

/*
Summary:
This function destroys a callback and frees any resources associated
with the callback.

Description:
If the specified callback is enabled when this function is called, the
callback may be called at any time until this function returns.
*/
BERR_Code BINT_DestroyCallback(
							   BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
							   );

/*
Summary:
This function enables a callback.

Description:
The callback may be called any number of times before this function returns!
*/
BERR_Code BINT_EnableCallback(
							  BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
							  );

/*
Summary:
This function enables a callback in ISR context.

Description:
The callback may be called any number of times before this function returns!
*/
BERR_Code BINT_EnableCallback_isr(
								  BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
								  );

/*
Summary:
This function disables a callback.

Description:
The callback may be called at any time until the function returns.
*/
BERR_Code BINT_DisableCallback(
							   BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
							   );


/*
Summary:
This function disables a callback in ISR context.

Description:
The callback may be called at any time until the function returns.
*/
BERR_Code BINT_DisableCallback_isr(
								   BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
								   );

/*
Summary:
This functions clears a pending interrupt specified by the callback.

Description:
The interrupt will only be cleared if there are no callbacks currently
enabled for the interrupt associated with the specified callback.
*/
BERR_Code BINT_ClearCallback(
							  BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
							  );

/*
Summary:
This functions clears a pending interrupt specified by the callback.

Description:
The interrupt will only be cleared if there are no callbacks currently
enabled for the interrupt associated with the specified callback.
*/
BERR_Code BINT_ClearCallback_isr(
							  BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
							  );

/*
Summary:
This function triggers a hardware interrupt.

Description:
The interrupt triggers the hardware specified by the BINT_CallbackHandle.
The callback handle is used directly trigger the interrupt.

Interrupts must be flagged as CPU trigger-able before this function will allow the interrupt
to be triggered (this flag exists in the BINT_P_intDef structure).  This allows specific
platforms and environments to prevent software from triggering interrupts they should not be.
*/
BERR_Code BINT_TriggerInterruptByHandle(
								BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
								);

/*
Summary:
This function triggers a hardware interrupt.

Description:
The interrupt triggers the hardware specified by the BINT_CallbackHandle.
The callback handle is used directly trigger the interrupt.

Interrupts must be flagged as CPU trigger-able before this function will allow the interrupt
to be triggered (this flag exists in the BINT_P_intDef structure).  This allows specific
platforms and environments to prevent software from triggering interrupts they should not be.
*/
BERR_Code BINT_TriggerInterruptByHandle_isr(
									BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
									);

/*
Summary:
Gets the first registered callback.

Description:
Gets the first callback handle stored in the INT module. used with BINT_GetCallbackNext
to traverse through all installed callbacks.
*/
BINT_CallbackHandle BINT_GetCallbackFirst(
										  BINT_Handle intHandle /* [in] Interrupt handle */
										  );

/*
Summary:
Gets the next registered callback.

Description:
Returns the next callback handle after cbHandle. Order of callbacks returned is in
order of callback creation.
*/
BINT_CallbackHandle BINT_GetCallbackNext(
										 BINT_CallbackHandle cbHandle /* [in] Callback handle returned by BINT_CreateCallback() */
										 );
/*
Summary:
Gets the interrupt ID for this callback.

Description:
Returns the interrupt ID for the specific callback in pIntId. Used with the above
traversal functions to identify a callback hooked to a specific interrupt.
*/
BERR_Code BINT_GetInterruptId(
							  BINT_CallbackHandle cbHandle, /* [in] Callback handle returned by BINT_CreateCallback() */
							  BINT_Id *pIntId /* [out] Pointer to storage for the interrupt ID */
							  );

/*
Summary:
Gets callback's active status.

Description:
Gets the status of a callback and returns whether it is active or not in pbEnabled.
*/
BERR_Code BINT_GetCallbackStatus(
								 BINT_CallbackHandle cbHandle,  /* [in] Callback handle returned by BINT_CreateCallback() */
								 bool *pbEnabled /* [out] Pointer to storage for the callback's active status */
								 );

/*
Summary:
This function Re-applies the mask status of an L2 interrupt register

Description:
If for some reason an L2 register is modified outside BINT (e.g. block 
reset), this will restore the expected mask status to the L2 register 
provided. 
*/
void BINT_ApplyL2State_isr( 
                           BINT_Handle  intHandle,
                           uint32_t     L2Reg
                           );
    	
/*
Summary:
This function outputs statistics of the interrupt interface.

Description:
Prints out statistics, using message level debug output and then resets accumulated information.
*/
void BINT_DumpInfo(BINT_Handle intHandle);
#if BDBG_DEBUG_BUILD
/* {secret} */
BERR_Code BINT_P_CreateCallback_Tag(
							  BINT_CallbackHandle *pCbHandle,
							  BINT_Handle handle,
							  BINT_Id intId,
							  BINT_CallbackFunc func,
							  void * pParm1,
							  int parm2,
							  const char *callbackName
							  );

#define BINT_CreateCallback(cbh, h, id, f, p1, p2) BINT_P_CreateCallback_Tag((cbh), (h), (id), (f), (p1), (p2), #f)
#endif

#ifdef __cplusplus
}
#endif

#endif
/* End of File */



