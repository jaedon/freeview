/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_mem.h $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 3/9/12 4:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_mem.h $
 * 
 * Hydra_Software_Devel/15   3/9/12 4:48p erickson
 * SW7231-705: add BDBG_OBJECT_ASSERT
 * 
 * Hydra_Software_Devel/14   8/24/10 8:25a erickson
 * SW3548-815: allow BREG_Read/Write to be used inside a
 * BREG_AtomicUpdate32 callback without warning. this avoids the need for
 * the app to create a second method of accessing registers.
 *
 * Hydra_Software_Devel/13   5/3/10 10:41a erickson
 * SW3548-815: document BREG_AtomicUpdate32 and
 * BREG_SetAtomicUpdate32Callback more
 *
 * Hydra_Software_Devel/12   9/24/08 7:32p vsilyaev
 * PR 46950: Use unified declaration of BREG handle for debug and release
 * builds
 *
 * Hydra_Software_Devel/11   9/23/08 3:43p erickson
 * PR46950: added BREG_SetAtomicUpdate32Callback
 *
 * Hydra_Software_Devel/10   6/5/08 5:24p vsilyaev
 * PR43119: PR 43119: Added API for atomic register access
 *
 * Hydra_Software_Devel/PR43119/1   5/29/08 12:14p vsilyaev
 * PR 43119: Added API for atomic register access
 *
 * Hydra_Software_Devel/9   7/21/06 11:27a vsilyaev
 * PR 22695: Changes to make BREG_MEM compatible between debug and release
 * builds
 *
 * Hydra_Software_Devel/8   12/22/04 4:06p marcusk
 * PR13660: Added _isr flavors for each function.
 *
 * Hydra_Software_Devel/7   8/10/04 10:40a vsilyaev
 * PR 12172: Unified type of the BREG_Handle, it removes space for
 * different compiler behaviour in debug and release builds
 *
 * Hydra_Software_Devel/6   9/15/03 6:08p marcusk
 * Updated to use void * as register base address.
 *
 * Hydra_Software_Devel/5   5/30/03 12:03p marcusk
 * Changed CreateRegHandle/DestroyRegHandle to Open/Close to be more
 * consistant.
 *
 * Hydra_Software_Devel/4   3/26/03 2:08p marcusk
 * Added comments.
 *
 * Hydra_Software_Devel/3   3/7/03 9:24a marcusk
 * Minor cleanup.
 *
 * Hydra_Software_Devel/2   3/5/03 4:19p marcusk
 * Fixed minor issues (got it to compile).
 *
 * Hydra_Software_Devel/1   3/5/03 3:34p marcusk
 * Initial version.
 *
 ***************************************************************************/

/*= Module Overview ********************************************************
This module supplies the function required to access memory mapped registers.
Before any registers can be accessed an appropriate register handle should
be created (normally done at system init time).  This register handle is
then given to any modules that require access to the corresponding memory
mapped registers.
***************************************************************************/

#ifndef BREG_MEM_H
#define BREG_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
Summary:
This is an opaque handle that is used for memory mapped register functions.
*/
typedef struct BREG_Impl *BREG_Handle;




/*
Summary:
This function creates a register handle for memory mapped registers.

Description:
This function must be called before any memory mapped registers can be
accessed.  This normally is done by platform specific code during init
time.  The resulting register handle is then passed to any modules that
must access the corresponding registers.
*/
void BREG_Open(
                          BREG_Handle *pRegHandle, /* [out] Returns a register handle */
                          void *Address, /* Base address of the memory mapped register range */
                          size_t MaxRegOffset /* Maximum offset for this memory mapped register range */
                          );

/*
Summary:
This function destroys a previous created register handle.

Description:
This function frees any resources associate with a memory mapped register
handle.  After this function is called the register handle can no longer
be used for subsequent register accesses.
*/
void BREG_Close(
                           BREG_Handle RegHandle /* Register handle created by BREG_CreateRegHandle() */
                           );


#if BDBG_DEBUG_BUILD != 1

#include "breg_mem_opt.h"   /* Contains optimized versions of these routines */

#else

/*
Summary:
This function writes 32 bits to a register.

Description:
Although this fuction will never return an error it will assert if the
RegHandle is invalid or during a debug build if the reg offset is larger
than the MaxRegOffset specified in the BREG_CreateRegHandle function.
*/
void BREG_Write32(
                  BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                  uint32_t reg, /* Register offset to write */
                  uint32_t data /* Data value to write to register */
                  );

/*
Summary:
This function writes 16 bits to a register.

Description:
Although this fuction will never return an error it will assert if the
RegHandle is invalid or during a debug build if the reg offset is larger
than the MaxRegOffset specified in the BREG_CreateRegHandle function.
*/
void BREG_Write16(
                  BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                  uint32_t reg, /* Register offset to write */
                  uint16_t data /* Data value to write to register */
                  );

/*
Summary:
This function writes 8 bits to a register.

Description:
Although this fuction will never return an error it will assert if the
RegHandle is invalid or during a debug build if the reg offset is larger
than the MaxRegOffset specified in the BREG_CreateRegHandle function.
*/
void BREG_Write8(
                  BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                  uint32_t reg, /* Register offset to write */
                 uint8_t data /* Data value to write to register */
                 );

/*
Summary:
This function reads 32 bits from a register.

Description:
Although this fuction cannot return an error it will assert if the
RegHandle is invalid or during a debug build if the reg offset is larger
than the MaxRegOffset specified in the BREG_CreateRegHandle function.
*/
uint32_t BREG_Read32(
                 BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                 uint32_t reg /* Register offset to read */
                 );

/*
Summary:
This function reads 16 bits from a register.

Description:
Although this fuction cannot return an error it will assert if the
RegHandle is invalid or during a debug build if the reg offset is larger
than the MaxRegOffset specified in the BREG_CreateRegHandle function.
*/
uint16_t BREG_Read16(
                 BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                 uint32_t reg /* Register offset to read */
                 );

/*
Summary:
This function reads 8 bits from a register.

Description:
Although this fuction cannot return an error it will assert if the
RegHandle is invalid or during a debug build if the reg offset is larger
than the MaxRegOffset specified in the BREG_CreateRegHandle function.
*/
uint8_t BREG_Read8(
                 BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                 uint32_t reg /* Register offset to read */
                 );

#endif /* BDBG_DEBUG_BUILD != 1 */

/*
Summary:
This function atomically updates a 32 bit register value.

Description:
This function atomically update 32 bit value stored in the hardware register.
It's equivalent to the following code:

   BREG_Write32(regHandle, reg, (BREG_Read32(regHandle, reg) & (~mask)) | value);

with the added guarantee that the read/modify/write will be performed atomically across all magnum modules.

See BREG_SetAtomicUpdate32Callback for extending the synchronization domain outside of magnum and into the OS as well.
*/
void BREG_AtomicUpdate32(
                  BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                  uint32_t reg, /* Register offset to write */
                  uint32_t mask,
                  uint32_t value
                  );

/*
Summary:
This function atomically updates a 32 bit register value. Used for _isr context.

Description:
See BREG_AtomicUpdate32.
*/
void BREG_AtomicUpdate32_isr(
                  BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                  uint32_t reg, /* Register offset to write */
                  uint32_t mask,
                  uint32_t value
                  );

/*
Summary:
Callback for BREG_SetAtomicUpdate32Callback

Description:
Operation should be implemented as:

    uint32_t value;
    lock();
    value = *CONVERT_TO_ADDR(reg);
    value &= ~mask;
    value |= value;
    *CONVERT_TO_ADDR(reg) = value;
    unlock();
*/
typedef void (*BREG_AtomicUpdate32Callback)(
    void *callbackContext, /* callbackContext passed into BREG_SetAtomicUpdate32Callback */
    uint32_t reg,   /* Register offset to write */
    uint32_t mask,
    uint32_t value
    );

/*
Summary:
Set optional atomic update callback

Description:
On some systems, there are registers which are shared between magnum and the OS.
In these cases, a platform-specific atomic update must be implemented outside of Magnum by means of this isr callback.

The required implementation varies per OS. For example:
 - In linux kernel mode, the atomic update callback will acquire a spinlock.
 - In linux user mode, the atomic update callback will call an ioctl to the user mode driver which will then acquire a spinlock.
 - In vxworks, the atomic update callback could acquire a mutex shared by the OS.

This callback is not required if there are no registers shared between magnum and the OS.
Also, if the kernel can call BKNI_EnterCriticalSection for shared registers, this callback is not required.
*/
void BREG_SetAtomicUpdate32Callback(
                  BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                  BREG_AtomicUpdate32Callback callback_isr, /* If NULL, default behavior will be restored.
                                                               This callback is called in isr context. It must be very fast. */
                  void *callbackContext                     /* User context passed to callback */
                  );

/*
Summary:
This function atomically compares and updates 32 bit register value.

Description:
This function atomically reads 32 bit value from  the hardware register, compares with value
given by user, and if match newValue stored in the hardware register. It's equivalent to the following
code:

    uint32_t val = BREG_Read32(regHandle, reg);
    if(val==oldValue) BREG_Write32(regHandle, reg, newValue)
    return val;

with an added guarantee that the read and write are performed atomically.
*/
uint32_t BREG_CompareAndSwap32(
                  BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                  uint32_t reg, /* Register offset to write */
                  uint32_t oldValue, /* value to compare  */
                  uint32_t newValue /* value to write  */
                  );

/*
Summary:
This function atomically compares and updates 32 bit register value. Used for _isr context.
*/
uint32_t BREG_CompareAndSwap32_isr(
                  BREG_Handle RegHandle, /* Register handle created by BREG_CreateRegHandle() */
                  uint32_t reg, /* Register offset to write */
                  uint32_t oldValue, /* value to compare  */
                  uint32_t newValue /* value to write  */
                  );

/* ISR() flavors go here so they are picked up for both debug and release builds */

/*
Summary:
This function writes 16 bits to a register.  Used for _isr context.
*/
#define BREG_Write16_isr BREG_Write16

/*
Summary:
This function writes 8 bits to a register.  Used for _isr context.
*/
#define BREG_Write8_isr BREG_Write8

/*
Summary:
This function writes 32 bits to a register.  Used for _isr context.
*/
#define BREG_Write32_isr BREG_Write32

/*
Summary:
This function reads 8 bits from a register.  Used for _isr context.
*/
#define BREG_Read8_isr BREG_Read8

/*
Summary:
This function reads 16 bits from a register.  Used for _isr context.
*/
#define BREG_Read16_isr BREG_Read16

/*
Summary:
This function reads 32 bits from a register.  Used for _isr context.
*/
#define BREG_Read32_isr BREG_Read32

/* Internal representation of the BREG handle */
BDBG_OBJECT_ID_DECLARE(BREG);
typedef struct BREG_Impl
{
    BDBG_OBJECT(BREG)
    uint32_t BaseAddr; /* BaseAddr shall be the first member to keep it run-time compatible with the release builds */
    size_t MaxRegOffset;

    struct {
        BREG_AtomicUpdate32Callback callback_isr;
        void *callbackContext;
    } atomicUpdate32;
    bool inAtomicUpdateCallback;
} BREG_Impl;

#ifdef __cplusplus
}
#endif

#endif
/* End of File */
