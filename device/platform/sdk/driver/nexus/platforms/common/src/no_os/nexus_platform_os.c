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
* $brcm_Workfile: nexus_platform_os.c $
* $brcm_Revision: 13 $
* $brcm_Date: 9/29/12 8:16p $
*
* API Description:
*   API name: Platform linuxuser
*    linuxuser OS routines
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/no_os/nexus_platform_os.c $
* 
* 13   9/29/12 8:16p agin
* SWNOOS-575:  Make consistent with linux, fix warnings.
* 
* 12   9/29/12 3:40p agin
* SWNOOS-575:  Exand memory test to include cache areas of MEM0 (non
*  KSEG0/1) and MEM1.
* 
* 11   9/19/12 5:35p agin
* SWNOOS-573:  Leveled to \linuxuser\nexus_platform_os.c (main\70).
* 
* 10   9/13/12 4:45p agin
* SWNOOS-570:  97408 no-os cache work-around.
* 
* 9   8/28/12 1:59p agin
* SWNOOS-559:  Make consistent with ucos_ii.
* 
* 8   6/13/12 2:47p agin
* SWNOOS-543:  Support FAT over USB mass storage.
* 
* 7   5/17/12 9:01a agin
* SWNOOS-536:  Relevel 97408 no-os to 20110325 Phase 2.0.
* 
* 6   4/20/12 9:32p agin
* SWNOOS-530:  Add no-os support for 97435.
* 
* 5   3/20/12 12:24a agin
* SWNOOS-523:  Support 97429 no-os.
* 
* 4   3/18/12 6:45p agin
* SWNOOS-523:  Support 97429 no-os.
* 
* 3   11/9/11 8:00p agin
* SWNOOS-497:  Unstub NEXUS_Platform_P_DisconnectInterrupt.
* 
* 2   11/9/11 1:47p agin
* SWNOOS-497:  Relevel 97231 to prerel for B0.
* 
* 1   7/22/11 5:08p agin
* SWNOOS-473:  Relevel 97358 no-os to 20110701 Phase 2.0.
* 
* 24   7/18/11 1:38p agin
* SWNOOS-473:  Relevel 97358 no-os to 20110701 Phase 2.0.
* 
* 23   7/7/11 2:43p agin
* SWNOOS-469:  Relevel 97125 no-os to 20110222 Phase 6.0.
* 
* 22   6/21/11 11:06a agin
* SWNOOS-461:  Fixed virtual address for 97420.
* 
* 21   6/7/11 11:31a agin
* SWNOOS-462:  Relevel 97346 no-os to 20110513 Phase 2.0.
* 
* 20   5/21/11 11:42a agin
* SWNOOS-461:  Relevel 97420 no-os to 20110318 Phase 6.0.
* 
* 19   5/19/11 12:38a agin
* SWNOOS-458:  Fix compiler warnings.
* 
* 18   5/12/11 8:36p agin
* SWNOOS-457:  Relevel 97550 no-os to Phase 7.0.
* 
* 17   4/21/11 12:47p agin
* SWNOOS-446:  Support 97425 no-os.
* 
* 16   3/15/11 9:31p agin
* SWNOOS-447:  Support 97231 no-os.
* 
* SWNOOS-445/1   3/2/11 4:58p agin
* SWNOOS-445:  Support MOCA.
* 
* 9   9/27/10 1:02a agin
* SWNOOS-425:  Add NEXUS_Platform_P_AtomicUpdateCallback_isr.
* 
* 8   7/5/10 7:33a agin
* SWNOOS-410:  Support 7408.
* 
* 7   11/16/09 8:04p agin
* SWNOOS-366: Add NEXUS_Platform_P_GetHostMemory.
* 
* 6   9/7/09 9:12a agin
* SWNOOS-369: Don't need to include some files, fixed warnings.
* 
* 5   8/14/09 11:47a agin
* PR57719: Unresolved external reference when using latest code.
* 
* 4   3/3/09 5:07p agin
* PR52583: Add support for 7420.
* 
* 3   2/16/09 9:46p agin
* PR52128:  Added NEXUS_Platform_P_Magnum_Init() and
*  EXUS_Platform_P_Magnum_Uninit().
* 
* 2   12/29/08 12:55p agin
* PR50741: Check in changes for 20081222 BCM97325 non-os release.
* 
* 1   6/13/08 9:20a agin
* PR43579: Check in changes for 20080530 BCM97325 non-os release.
* 
***************************************************************************/

#include <stdio.h>
#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "nexus_core_utils.h"
#if (BCHP_CHIP != 7408)        
    #include "nexus_base.h"
#endif
#include "bkni.h"
#include "int1.h"
#include "memmap.h"

BDBG_MODULE(nexus_platform_os);

typedef struct IsrTableEntry
{
    NEXUS_Core_InterruptFunction pFunction;
    void *pFuncParam;
    int iFuncParam;
} IsrTableEntry;

static NEXUS_Error NEXUS_Platform_P_ConnectInterrupt_isr(
    unsigned irqNum,
    NEXUS_Core_InterruptFunction pIsrFunc,
    void *pFuncParam,
    int iFuncParam
    );

static void NEXUS_Platform_P_DisconnectInterrupt_isr(
    unsigned irqNum
    );


NEXUS_Error NEXUS_Platform_P_InitOSMem(void)
{
    return 0;
}

void NEXUS_Platform_P_UninitOSMem(void)
{
    return;
}

NEXUS_Error NEXUS_Platform_P_InitOS(void)
{
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Platform_P_UninitOS(void)
{
    return BERR_SUCCESS;
}


NEXUS_Error NEXUS_Platform_P_GetHostMemory(NEXUS_PlatformMemory *pMemory)
{
    /* The following information is obtained from /sys/devices/platform/brcmstb/bmem.* */
    pMemory->osRegion[0].base = 0x04000000;
    pMemory->osRegion[0].length = 192*1024*1024;
   
    #if (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7435)
        pMemory->osRegion[1].base = 0x20000000;
        pMemory->osRegion[1].length = 192*1024*1024;        
        pMemory->osRegion[2].base = 0x90000000;
        pMemory->osRegion[2].length = 1024*1024*1024;
    #elif (BCHP_CHIP==7231) || (BCHP_CHIP==7344) || (BCHP_CHIP==7346)
        pMemory->osRegion[1].base = 0x20000000;
        pMemory->osRegion[1].length = 512*1024*1024;
    #elif (BCHP_CHIP==7429) || (BCHP_CHIP==7408)
        pMemory->osRegion[1].base = 0x20000000;
        pMemory->osRegion[1].length = 768*1024*1024;
    #endif
    
    {
        int i;
        for (i=0; i<NEXUS_MAX_HEAPS; i++)
            BDBG_MSG(("pMemory->osRegion[%d].base=0x%x, pMemory->osRegion[%d].length=%d MB", i, pMemory->osRegion[i].base, i, pMemory->osRegion[i].length/(1024*1024)));
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Hook to connect a L1 interrupt to the OS
See Also:
    NEXUS_Platform_P_DisconnectInterrupt
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_ConnectInterrupt(
    unsigned irqNum,
    NEXUS_Core_InterruptFunction pIsrFunc,
    void *pFuncParam,
    int iFuncParam
    )
{
    NEXUS_Error errCode;

    BKNI_EnterCriticalSection();
    errCode = NEXUS_Platform_P_ConnectInterrupt_isr(irqNum, pIsrFunc, pFuncParam, iFuncParam);
    BKNI_LeaveCriticalSection();

    return errCode;
}

static NEXUS_Error NEXUS_Platform_P_ConnectInterrupt_isr(
    unsigned irqNum,
    NEXUS_Core_InterruptFunction pIsrFunc,
    void *pFuncParam,
    int iFuncParam
    )
{
    CPUINT1_ConnectIsr(irqNum, pIsrFunc, pFuncParam, iFuncParam);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Hook to disconnect a L1 interrupt from the OS
See Also:
    NEXUS_Platform_P_ConnectInterrupt
 ***************************************************************************/
void NEXUS_Platform_P_DisconnectInterrupt(
    unsigned irqNum
    )
{
    BKNI_EnterCriticalSection();
    NEXUS_Platform_P_DisconnectInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
}

static void NEXUS_Platform_P_DisconnectInterrupt_isr(
    unsigned irqNum
    )
{
    NEXUS_Platform_P_DisableInterrupt_isr(irqNum);
    (void)NEXUS_Platform_P_ConnectInterrupt_isr(irqNum, NULL, NULL, 0);
}

/***************************************************************************
Summary:
    Hook to enable an L1 interrupt
See Also:
    NEXUS_Platform_P_EnableInterrupt
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_EnableInterrupt(
    unsigned irqNum
    )
{
    NEXUS_Error errCode;

    BKNI_EnterCriticalSection();
    errCode = NEXUS_Platform_P_EnableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();

    return errCode;
}

/***************************************************************************
Summary:
    Hook to enable an L1 interrupt
See Also:
    NEXUS_Platform_P_EnableInterrupt_isr
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_EnableInterrupt_isr(
    unsigned irqNum
    )
{
    CPUINT1_Enable(irqNum);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Hook to disable an L1 interrupt
See Also:
    NEXUS_Platform_P_DisableInterrupt
 ***************************************************************************/
void NEXUS_Platform_P_DisableInterrupt(
    unsigned irqNum
    )
{
    BKNI_EnterCriticalSection();
    NEXUS_Platform_P_DisableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
}

/***************************************************************************
Summary:
    Hook to disable an L1 interrupt
See Also:
    NEXUS_Platform_P_DisableInterrupt_isr
 ***************************************************************************/
void NEXUS_Platform_P_DisableInterrupt_isr(
    unsigned irqNum
    )
{
    CPUINT1_Disable(irqNum);
}

/***************************************************************************
Summary:
    Map physical memory into virtual space
Returns:
    Valid address on success, NULL for failure.
See Also:
    NEXUS_Platform_P_UnmapMemory
 ***************************************************************************/
void *NEXUS_Platform_P_MapMemory(
    unsigned long offset,
    unsigned long length,
    bool cached)
{
    unsigned long addr = 0UL;

    #if (BCHP_CHIP == 7420)

        switch (offset & 0xf0000000)
        {
            case 0x00000000:	/* memc0 */
            case 0x10000000:	/* chips regs */
                addr =  (void *)(offset | 0xA0000000ul);
                break;
                break;
            case DRAM_1_256MB_PHYS_ADDR_START:	/* memc1 */
			    offset &= ~0xf0000000;
                addr = (void *)(offset | DRAM_1_256MB_VIRT_ADDR_START);
                break;
            default:
                printf("NEXUS_Platform_P_MapMemory:  unknown offset:  0x%08x\n", offset);
        }
    
    #elif (BCHP_CHIP == 7231) || (BCHP_CHIP == 7346) || (BCHP_CHIP==7408) || (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7429) || (BCHP_CHIP==7435)

        switch (offset & 0xf0000000)
        {
            case 0x00000000:	/* memc0 */
                #ifndef CACHE_WORKAROUND
                    if (cached)
                        addr = offset + 0x80000000;
                    else
                        addr = offset + 0xA0000000;
                    break;
                #endif
            case 0x10000000:	/* chips regs */
                addr = offset + 0xA0000000;
                break;
            case 0x20000000:	/* memc0 */
            case 0x30000000:	/* memc0 */
            case 0x40000000:	/* memc0 */
                if (cached)
                    addr = offset + 0xa0000000;
                else
                    addr = offset - 0x10000000;
                break;
            #if (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP==7435)
            case 0x90000000:	/* memc1 */
            case 0xa0000000:	/* memc1 */
            case 0xb0000000:	/* memc1 */
            case 0xc0000000:	/* memc1 */
                if (cached)
                    addr = offset + 0x40000000;
                else
                    addr = offset - 0x50000000;
                break;
            #endif
            default:
                BDBG_ERR(("NEXUS_Platform_P_MapMemory:  unknown offset:  0x%08lx", offset));
        }
    
    #else

        if (0) /* (cached) */
            return (void *)(offset | 0x80000000ul);
        else
            return (void *)(offset | 0xA0000000ul);

    #endif

    BDBG_MSG(("mmap offset=%x=>%x size=%d MB 0x%08x fd=%d", offset, addr, length/(1024*1024), length, -1));

    return (void *)addr;
}

/***************************************************************************
Summary:
    Unmap a virtual address
See Also:
    NEXUS_Platform_P_MapMemory
 ***************************************************************************/
void NEXUS_Platform_P_UnmapMemory(
    void *pMem,
    unsigned long length
    )
{
    BSTD_UNUSED(pMem);
    BSTD_UNUSED(length);
    return;
}

/***************************************************************************
Summary:
    Reset any pending L1 interrupts
 ***************************************************************************/
void NEXUS_Platform_P_ResetInterrupts(void)
{
/*    printf("NEXUS_Platform_P_ResetInterrupts stubbed ...\n");*/
}

void NEXUS_Platform_P_AtomicUpdateCallback_isr(void *callbackContext, uint32_t reg, uint32_t mask, uint32_t value)
{
    uint32_t temp;
    BSTD_UNUSED(callbackContext);
    temp = BREG_Read32(g_pCoreHandles->reg, reg);
    temp &= ~mask;
    temp |= value;
    BREG_Write32(g_pCoreHandles->reg, reg, temp);
}

static bool g_NEXUS_magnum_init = false;
NEXUS_Error
NEXUS_Platform_P_Magnum_Init(void)
{
    BERR_Code rc;

    if(!g_NEXUS_magnum_init) {
        rc = BKNI_Init();
        if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
        rc = BDBG_Init();
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); BKNI_Uninit();return rc;}
#if (BCHP_CHIP != 7408)        
        rc = NEXUS_Base_Core_Init();
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); BDBG_Uninit();BKNI_Uninit();return rc;}
#endif        
        g_NEXUS_magnum_init = true;
    }
    return BERR_SUCCESS;
}

void 
NEXUS_Platform_P_Magnum_Uninit(void)
{
    if(g_NEXUS_magnum_init) {
        BDBG_Uninit();
        BKNI_Uninit();
        g_NEXUS_magnum_init = false;
    }
    return;
}

#if BCHP_CHIP == 7408
/***************************************************************************
Summary:
Read reserved memory
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadReserved(
    uint32_t physicalAddress
    )
{
	uint32_t value;
	value = *((volatile uint32_t *)(0xa0000000 | physicalAddress));
	return value; 
}

/***************************************************************************
Summary:
Write reserved memory
***************************************************************************/
void NEXUS_Platform_P_WriteReserved(
    uint32_t physicalAddress,
    uint32_t value
    )
{
	*((volatile uint32_t *)(0xa0000000 | physicalAddress)) = value;
}

/***************************************************************************
Summary:
Read core register
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadCoreReg(
    uint32_t offset
    )
{
	uint32_t physicalAddress, value;
	uint32_t cbr = CpuCbrGet();
	physicalAddress = cbr & ~0x3ffff; /* mask off lower 18 bits */
	physicalAddress |= offset;
	value = *((volatile uint32_t *)(0xa0000000 | physicalAddress));
	return value;
}

/***************************************************************************
Summary:
Write core register
***************************************************************************/
void NEXUS_Platform_P_WriteCoreReg(
    uint32_t offset,
    uint32_t value
    )
{
	uint32_t physicalAddress;
	uint32_t cbr = CpuCbrGet();
	physicalAddress = CpuCbrGet() & ~0x3ffff; /* mask off lower 18 bits */
	physicalAddress |= offset;
	*((volatile uint32_t *)(0xa0000000 | physicalAddress)) = value;
}

/***************************************************************************
Summary:
Read CMT Control Register
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadCmtControl(void)
{
    /*printf("NEXUS_Platform_P_ReadCmtControl stubbed\n");*/
    return 0;
}

/***************************************************************************
Summary:
Write CMT Control Register
***************************************************************************/
void NEXUS_Platform_P_WriteCmtControl(
    uint32_t value
    )
{
    /*printf("NEXUS_Platform_P_WriteCmtControl stubbed\n");*/
}
#endif

void NEXUS_Platform_P_StopCallbacks(void *interfaceHandle)
{
    NEXUS_Base_P_StopCallbacks(interfaceHandle);
    return;
}

void NEXUS_Platform_P_StartCallbacks(void *interfaceHandle)
{
    NEXUS_Base_P_StartCallbacks(interfaceHandle);
    return;
}