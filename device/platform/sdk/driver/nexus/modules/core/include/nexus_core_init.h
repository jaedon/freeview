/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_core_init.h $
* $brcm_Revision: 16 $
* $brcm_Date: 10/16/12 10:15a $
*
* API Description:
*   API name: Base
*   It also initializes the magnum base modules. And provides system level
*   interrupt routing
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/include/nexus_core_init.h $
* 
* 16   10/16/12 10:15a erickson
* SW7425-1792: standardize NEXUS_CoreModule_Init to return module handle
* 
* 15   3/28/12 11:09a erickson
* SW7435-65: implement NEXUS_PlatformSettings.heap[].guardBanding
* 
* 14   3/14/12 3:12p erickson
* SW7425-2607: use NEXUS_MAX_MEMC
* 
* 13   3/14/12 11:04a erickson
* SW7425-2607: use NEXUS_MAX_MEMC and allow NEXUS_NUM_MEMC to be
*  customized
* 
* 12   2/24/12 5:15p erickson
* SW7425-1969: public API cannot #include priv header file
* 
* 11   3/4/11 11:26a erickson
* SW7400-3006: add locked setting. rework so that NEXUS_Heap_Create
*  populates g_NexusCore.publicHandles.
*
* 10   12/20/10 2:11p erickson
* SW7422-2: redefine NEXUS_MemoryType to be bitmask for different memory
*  mapping requirements per heap, remove deprecated api's
*
* 9   10/7/10 11:19a vsilyaev
* SW7405-4924: Communicate size of each DDRAM controller
*
* 8   9/23/10 2:13p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 7   7/14/10 10:30a erickson
* SW7405-4621: all chips now share core/CHIP/include. make
*  nexus_core_features.h chip-generic.
*
* 6   12/2/09 2:16p erickson
* SW7405-3514: rename param to avoid compiler issue
*
* 5   3/9/09 4:59p erickson
* PR50748: coding convention changes, comments
*
* 4   3/9/09 2:32p erickson
* PR50748: remove nexus multi-heap management
*
* 3   8/21/08 3:47p jgarrett
* PR 45527: Adding image support
*
* 2   6/30/08 1:44p erickson
* PR44165: allow no NEXUS_CORE_NUM_SECURE_HEAPS define
*
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/6   11/30/07 11:12a erickson
* PR35457: api update
*
* Nexus_Devel/5   9/21/07 6:26p jgarrett
* PR 35002: Adding interrupt param for BINT_Isr
*
* Nexus_Devel/4   9/21/07 10:50a erickson
* PR35002: compilation errors
*
* Nexus_Devel/3   9/19/07 6:33p jgarrett
* PR 35002: Moving register interface init to platform
*
* Nexus_Devel/2   9/18/07 5:42p jgarrett
* PR 35002: Adding nexus_core_features.h
*
* Nexus_Devel/1   9/13/07 2:35p vsilyaev
* PR 34419: Added core_module interface
*
* Nexus_Devel/4   9/10/07 5:34p vsilyaev
* PR 34419: Added array of memory heaps
*
* Nexus_Devel/3   9/5/07 2:47p vsilyaev
* PR 34419: Added interrupt interface
*
* Nexus_Devel/2   8/31/07 6:02p vsilyaev
* PR 34419: Added configuration structure for NEXUS_Base
*
* Nexus_Devel/1   8/28/07 12:46p vsilyaev
* PR 34254: Split system/board functionality
*
***************************************************************************/
#ifndef NEXUS_CORE_MODULE_H
#define NEXUS_CORE_MODULE_H

#include "nexus_types.h"
#include "nexus_memory.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************
Summary:
Structure used by Platform to instruct Core how to create magnum heaps and their accompanying Nexus heap handles
****************************************************************************/
typedef struct NEXUS_Core_MemoryRegion
{
    unsigned memcIndex;     /* memory controller index */
    void *pvAddr;           /* virtual uncached address of memory region */
    uint32_t offset;        /* physical device offset (address) of memory region */
    size_t length;          /* length in bytes of the memory region */
    void *pvAddrCached;     /* virtual cached address of memory region, optional */
    unsigned memoryType;
    unsigned alignment;     /* required alignment (in bytes) of allocations in this region */
    bool locked;            /* nexus does not allow new allocations from a locked heap. */
    bool guardBanding;      /* if true, use guard bands if possible. if false, force no guard banding for higher performance. */
} NEXUS_Core_MemoryRegion;

/***************************************************************************
Summary:
Interface for handling OS interrupts
****************************************************************************/
typedef struct NEXUS_CoreInterruptInterface
{
    void (*pDisconnectInterrupt)(unsigned ulIrqNum);
    NEXUS_Error (*pConnectInterrupt)(unsigned irqNum, NEXUS_Core_InterruptFunction pIsrFunc, void *pFuncParam, int iFuncParam);
    NEXUS_Error (*pEnableInterrupt_isr)(unsigned irqNum);
    void  (*pDisableInterrupt_isr)(unsigned irqNum);
} NEXUS_CoreInterruptInterface;

/***************************************************************************
Summary:
Callback function prototype for NEXUS_CoreImageInterface
****************************************************************************/
typedef void*(*NEXUS_CoreImageInterface_Create)(
    const char *id
    );

/***************************************************************************
Summary:
Callback function prototype for NEXUS_CoreImageInterface
****************************************************************************/
typedef void (*NEXUS_CoreImageInterface_Destroy)(
    void *imageInterface
    );

/***************************************************************************
Summary:
Callback function prototype for NEXUS_CoreImageInterface
****************************************************************************/
typedef BERR_Code (*NEXUS_CoreImageInterface_Open)(
    void *context,  /* context of the image interface */
    void **image,  /* [out] pointer to the image context */
    unsigned image_id /* ID of the image */
    );

/***************************************************************************
Summary:
Callback function prototype for NEXUS_CoreImageInterface
****************************************************************************/
typedef void (*NEXUS_CoreImageInterface_Close)(
    void *image /* image context */
    );

/***************************************************************************
Summary:
Callback function prototype for NEXUS_CoreImageInterface
****************************************************************************/
typedef BERR_Code (*NEXUS_CoreImageInterface_Next)(
    void *image,  /* image context information, one returned by the 'open' call */
    unsigned chunk,  /* number of chunk, starts from 0, shall increment with each call */
    const void **data,  /*  [out] returns pointer to next piece of data, contents of this pointer is valid until succeding call to the 'next' function */
    uint16_t length /* number of bytes to read from the image,  length shall be less than 64K */
    );

/***************************************************************************
Summary:
Interface for Magnum's IMG interface used in NEXUS_Core_Settings.

Description:
The IMG interface is used for firmware loading in Magnum.
****************************************************************************/
typedef struct NEXUS_CoreImageInterface
{
    NEXUS_CoreImageInterface_Create create;   /* create image interface */
    NEXUS_CoreImageInterface_Destroy destroy; /* destroy the image interface  */
    NEXUS_CoreImageInterface_Open open;       /* open method */
    NEXUS_CoreImageInterface_Next next;       /* 'next' method */
    NEXUS_CoreImageInterface_Close close;     /* close method */
} NEXUS_CoreImageInterface;


/***************************************************************************
Summary:
Settings for the Core module
****************************************************************************/
typedef struct  NEXUS_Core_Memc_Information {
    uint32_t offset; /* offset e.g. physicall address of first memory location addressable by MEMC */
    size_t length; /* size of the memory region adressable by MEMC */
} NEXUS_Core_Memc_Information;

/***************************************************************************
Summary:
Settings for the Core module
****************************************************************************/
typedef struct NEXUS_Core_Settings
{
    BREG_Handle regHandle;
    NEXUS_Core_MemoryRegion heapRegion[NEXUS_MAX_HEAPS]; /* heaps to allocate. set heapRegion[].length == 0 to terminate. */
    NEXUS_CoreInterruptInterface interruptInterface;
    NEXUS_CoreImageInterface imgInterface; /* Image interface */
#define NEXUS_MAX_MEMC 3
    NEXUS_Core_Memc_Information memcRegion[NEXUS_MAX_MEMC];
} NEXUS_Core_Settings;

/***************************************************************************
Summary:
Get default settings for Core module
****************************************************************************/
void NEXUS_CoreModule_GetDefaultSettings(
    NEXUS_Core_Settings *pSettings /* [out] Default Settings */
    );

/***************************************************************************
Summary:
Init the Core module
****************************************************************************/
NEXUS_ModuleHandle NEXUS_CoreModule_Init(
    const NEXUS_Core_Settings *pSettings
    );

/***************************************************************************
Summary:
Uninit the Core module
****************************************************************************/
void NEXUS_CoreModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_CORE_MODULE_H */

