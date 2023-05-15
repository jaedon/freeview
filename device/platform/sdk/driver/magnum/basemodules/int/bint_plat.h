/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_plat.h $
 * $brcm_Revision: Hydra_Software_Devel/52 $
 * $brcm_Date: 10/15/12 4:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/bint_plat.h $
 * 
 * Hydra_Software_Devel/52   10/15/12 4:07p hongtaoz
 * SW7445-54: 7445 has 4 L1 intc registers;
 * 
 * Hydra_Software_Devel/51   10/5/12 11:12a mphillip
 * SW7425-3963: Merge interrupt changes to main
 * 
 * Hydra_Software_Devel/SW7425-3963/2   10/4/12 4:09p mphillip
 * SW7425-3963: Documentation edits
 * 
 * Hydra_Software_Devel/SW7425-3963/1   9/20/12 4:15p mphillip
 * SW7425-3963: Update interrupt handling with optimizations and support
 * for 'weak' interrupt masking
 * 
 * Hydra_Software_Devel/50   2/28/12 11:41a mward
 * SW7435-7:  add BINT_INTC_SIZE 4 for 7435, 3 for others
 * 
 * Hydra_Software_Devel/49   1/4/12 12:04p erickson
 * SW7425-2090: remove need to edit BCHP_CHIP list in bint_plat.h
 * 
 * Hydra_Software_Devel/48   12/16/11 7:29p bselva
 * SW7360-6: Added appframework support for 7360 platform
 * 
 * Hydra_Software_Devel/47   11/1/11 11:04a mward
 * SW7435-7:  Add 7435.
 * 
 * Hydra_Software_Devel/46   10/3/11 12:00p katrep
 * SW7429-1:7429 bringup
 * 
 * Hydra_Software_Devel/45   3/14/11 1:48p jhaberf
 * SW35330-13: replaced 935330 with 935233 support
 * 
 * Hydra_Software_Devel/44   12/6/10 1:44p etrudeau
 * SWBLURAY-23579: add 7640 support
 * 
 * Hydra_Software_Devel/44   12/6/10 1:09p etrudeau
 * SWBLURAY-23579: add 7640 support
 * 
 * Hydra_Software_Devel/43   11/30/10 6:15p katrep
 * SW7231-4:add support for 7231,7346,7346
 * 
 * Hydra_Software_Devel/42   11/4/10 3:36p jhaberf
 * SW35230-1: Added 35125 DTV chip support
 * 
 * Hydra_Software_Devel/41   11/2/10 11:13a xhuang
 * SW7552-4: Add 7552 support
 * 
 * Hydra_Software_Devel/40   9/13/10 5:23p hongtaoz
 * SW7425-7: adding 7425 support;
 * 
 * Hydra_Software_Devel/39   8/18/10 11:29a nickh
 * SW7422-12: Add 7422 support
 * 
 * Hydra_Software_Devel/38   8/4/10 7:14p xhuang
 * SW7358-3: Add support for 7358
 * 
 * Hydra_Software_Devel/38   8/4/10 7:12p xhuang
 * SW7358-3: Add support for 7358
 * 
 * Hydra_Software_Devel/37   11/6/09 11:37a gmohile
 * SW7408-1 : Add 7408 support
 * 
 * Hydra_Software_Devel/36   9/29/09 2:22p lwhite
 * SW7468-6: Add 7468 support
 * 
 * Hydra_Software_Devel/35   9/16/09 1:16p nitinb
 * SW7550-7: Add support for 7550
 * 
 * Hydra_Software_Devel/34   9/10/09 5:14p jhaberf
 * SW35230-1: Creating 35230 DTV chip build environment
 * 
 * Hydra_Software_Devel/33   8/27/09 8:07p mward
 * SW7125-4: 7125 needs BINT_NEW_INT_MODEL
 * 
 * Hydra_Software_Devel/32   7/24/09 6:11p pntruong
 * PR55861: Further refactored the new int macro.
 * 
 * Hydra_Software_Devel/32   7/24/09 6:10p pntruong
 * PR55861: Further refactored the new int macro.
 * 
 * Hydra_Software_Devel/32   7/24/09 6:06p pntruong
 * PR55861: Further refactored the new int macro to ease porting of new
 * chips.
 *
 * Hydra_Software_Devel/31   7/24/09 1:07p mward
 * PR55545: Add 7125 to BINT_NEW_INT_MODEL list.
 *
 * Hydra_Software_Devel/30   4/23/09 11:04a jhaberf
 * PR53796: Adding BCM35130 support.
 *
 * Hydra_Software_Devel/29   1/30/09 3:42p jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/28   12/3/08 10:42p pntruong
 * PR49691: Refactored ifdefs for new interrupt model.
 *
 * Hydra_Software_Devel/27   12/3/08 9:16p nickh
 * PR49691: Enable interrupt support for 7420 for new user mode driver
 *
 * Hydra_Software_Devel/26   9/29/08 5:22p pntruong
 * PR47411: Enable interrupt support for 3548/3556 for new user mode
 * driver.
 *
 * Hydra_Software_Devel/25   11/28/07 11:47a katrep
 * PR37430: fixed compiler error.
 *
 * Hydra_Software_Devel/24   11/28/07 10:58a katrep
 * PR37430: Extended interrupt interface to 128 bits for 7405,7325,7335
 *
 * Hydra_Software_Devel/23   11/21/07 1:47p ronchan
 * PR 32395: added compile option to select interrupt array size for 7325
 *
 * Hydra_Software_Devel/22   11/21/07 12:26p ronchan
 * PR 32395: increased L1 interrupt array size from 64 to 70 for 7325
 *
 * Hydra_Software_Devel/21   2/15/07 12:01p erickson
 * PR26657: optimized BINT_Isr. added BINT_IS_STANDARD to allow standard
 * interrupts to be processed inside bint.c.
 *
 * Hydra_Software_Devel/20   5/26/06 3:12p albertl
 * PR21392:  Removed remaining code accessing timers directly.
 *
 * Hydra_Software_Devel/19   5/24/06 6:58p albertl
 * PR21392:  Changed BINT stats tracking to use timers from TMR module.
 * Removed code accessing timers directly.
 *
 * Hydra_Software_Devel/18   2/15/06 5:30p vsilyaev
 * PR 19693: Added support for acquiring interrupt rate
 *
 * Hydra_Software_Devel/17   4/5/05 7:13p albertl
 * PR10596:  Added new statistics tracking functionality.
 *
 * Hydra_Software_Devel/16   12/14/04 4:32p marcusk
 * PR13361: Added more details to L1Shift and L2RegOffset members.
 *
 * Hydra_Software_Devel/15   1/5/04 4:26p marcusk
 * PR9117: Updated to support PI provided L2 interrupt handler (for
 * transport message and overflow interrupts).  Updated documentation.
 *
 * Hydra_Software_Devel/14   12/29/03 3:58p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/13   12/18/03 2:08p marcusk
 * PR8985: Refactored to use single ISR() routine. Removed reserved names.
 * Placed all platform specific defines in bint_plat.h
 *
 * Hydra_Software_Devel/12   9/16/03 10:30a marcusk
 * Updated to comply with DocJet requirements. Fixes for PR8055.
 *
 * Hydra_Software_Devel/11   8/26/03 10:43a marcusk
 * Removed default settings (they are not valid)
 *
 * Hydra_Software_Devel/10   8/22/03 3:00p erickson
 * added BINT_GetDefaultSettings
 *
 * Hydra_Software_Devel/9   6/18/03 3:26p dlwin
 * Added support to allow for more general implementation of Interrupt
 * manager.
 *
 * Hydra_Software_Devel/8   4/2/03 10:39a marcusk
 * Updated to support flag to specify if the interrupt can be triggered by
 * the CPU.
 *
 * Hydra_Software_Devel/7   3/21/03 6:30p marcusk
 * Minor updates.
 *
 * Hydra_Software_Devel/6   3/21/03 10:23a marcusk
 * Removed array and replaced with pointer.
 *
 * Hydra_Software_Devel/5   3/19/03 11:28a marcusk
 * Added module overview.
 *
 * Hydra_Software_Devel/4   3/17/03 9:06a marcusk
 * Updated with const strings and removed un-needed typedef.
 *
 * Hydra_Software_Devel/3   3/13/03 3:24p marcusk
 * Added include to fix build.
 *
 * Hydra_Software_Devel/2   3/12/03 3:07p marcusk
 * Updated comments.
 *
 * Hydra_Software_Devel/1   3/12/03 2:21p marcusk
 * Initial Version.
 *
 ***************************************************************************/

/*= Module Overview *********************************************************

The platform API is used to manage an instance of the interrupt interface.
This includes opening and closing and instance and the actual function that
is called when a L1 interrupt is generated.  Multiple instances of an
InterruptInterface can be used for the same chip if and only if they do not
manage the same L2 interrupt bits.  For example, one InterruptInterface
instance may be used the BSP/kernel code to manage standard peripherals
(IDE, USB, ENET, etc.), while another exists in a driver to manage interrupts
specific to that driver.

The InterruptInterface also supports proprietary L2 interrupt handlers
through the use of the chip specific interrupt definition (see BINT_DONT_PROCESS_L2)
for chip specific definitions.  When using this feature, the InterruptInterface
acts only as the central interrupt dispatcher used for processing all
L1 interrupts.  This feature is mainly used to simplify the platform specific
code (so it only has to worry about calling BINT_Isr() for all L1 interrupts.

In addition the InterruptInterface can be used to notify the platform
specific code regarding the L1 interrupts that are managed by an instance.
This can be done using the BINT_GetL1BitMask() routine.

***************************************************************************/

#ifndef BINT_PLATFORM_H
#define BINT_PLATFORM_H

#include "breg_mem.h"
#include "bint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* New interrupt model!  Avoid doing this in many files that uses
 * new int model. */
#if((BCHP_CHIP==7038) || (BCHP_CHIP==7401) || (BCHP_CHIP==7403) || \
    (BCHP_CHIP==7400) || (BCHP_CHIP==7118) || (BCHP_CHIP==7440) || (BCHP_CHIP==7601) || \
    (BCHP_CHIP==3560) || (BCHP_CHIP==3563) || (BCHP_CHIP==3573))
#define BINT_NEW_INT_MODEL   (0)
#else
/* the #else should always contain the new architecture.
also, if the number of L1 registers changes in the future, consider a macro whose value is the # of registers. */
#define BINT_NEW_INT_MODEL   (1)
#if (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
#define BINT_INTC_SIZE 4
#else
#define BINT_INTC_SIZE 3
#endif
#endif

#if (BINT_NEW_INT_MODEL)
#define BINT_MAX_INTC_SIZE    4  /* interrupt controller size this interface is capable of handling */
#define BINT_P_L1_SIZE        (32*BINT_INTC_SIZE) /* Size of L1 interrupt register */
#else
#define BINT_P_L1_SIZE        64 /* Size of L1 interrupt register */
#endif

#define BINT_DONT_PROCESS_L2    0xFFFFFFFF
#define BINT_IS_STANDARD        0x40000000 /* See BINT_P_IntMap.L1Shift for usage. */

#define BINT_P_STD_STATUS		0x00
#define BINT_P_STD_SET			0x04
#define BINT_P_STD_CLEAR		0x08
#define BINT_P_STD_MASK_STATUS	0x0C
#define BINT_P_STD_MASK_SET		0x10
#define BINT_P_STD_MASK_CLEAR	0x14

#define BINT_P_MAP_MISC_REGULAR 0
#define BINT_P_MAP_MISC_WEAK_MASK   0x20000000
#define BINT_P_MAP_MISC_STANDARD    BINT_IS_STANDARD
#define BINT_P_MAP_PROCESS_ALL(x)   0
#define BINT_P_MAP_PROCESS_NONE(x)  BINT_DONT_PROCESS_L2    
#define BINT_P_MAP_PROCESS_SOME(x)  (~(x))
#define BINT_P_MAP_PROCESS_MASK(x)  x

/*
Summary:
This macro definition used to define the interrupt map entry for 'standard' L2 interrupts

Description:
Standard L2 interrupts allowed to use simple form to specify L2 interrupt entry. Only three arguments are needed:
 - L1 - number of L1 status register (0,1,2, etc) that L1 interrupt belongs to
 - name - name of the L1 interrupts - this is the name in RBD without the '_CPU_INTR' suffix. For example for RDB field name 'MEMC1_CPU_INTR' one should specify 'MEMC1'
 - core - name of the corresponding L2 status register -  this is the name of register in RDB without the '_STATUS' suffix. For example for the register name 'MEMC_L2_0_R5F_STATUS' one should specify 'MEMC_L2_0_R5F'
*/
#define BINT_MAP_STD(L1,name,core) {BINT_IS_STANDARD | (BCHP_HIF_CPU_INTR1_INTR_W##L1##_STATUS_##name##_CPU_INTR_SHIFT+L1*32), BCHP_##core##_STATUS, 0, #name}

/*
Summary:
This macro definition used to define the interrupt map entry for L2 interrupts

Description:
 - L1 - number of L1 status register (0,1,2, etc) that L1 interrupt belongs to
 - name - name of the L1 interrupts - this is the name in RBD without the '_CPU_INTR' suffix. For example for RDB field name 'XPT_STATUS_CPU_INTR' one should specify 'XPT_STATUS'
 - suffix - string that would be appended to the interrupt name - used solely for debug purposes 
 - reg - name of the corresponding L2 status register -  this is the name of register in RDB. For example for the register name 'XPT_PB0_INTR' one should specify 'XPT_PB0_INTR'
 - misc - miscellaneous flags. It has three possible values: REGULAR - regular interrupt, STANDARD - L2 interrupt that uses standard L2 controller, WEAK_MASK - L2 interrupt implements 'weak' type of interrupt masking, e.g. interrupt can't be masked when it's  asserted
 - process - specifies what L2 interrupts described by this entry. it could have the following options: ALL - process all L2 interrupts, NONE - no interrupts are processed, SOME - the next argument would be a bitmask of interrupts that should be processed, MASK - then next argument would be a bitmask of interrupts that should be ignored
 - bits   - if process is 'SOME' then it's bitmask for interrupts that should be processed, for example if only interrupts for bit 0 and 1 should be processed, then bits should be set to 0x03 (1<<0 | 1<<1), and if process is 'MASK' then it's bitmask for interrupts that shouldn't be processed, for example if only interrupts for bit 0 and 1 should be processed, then bits should be set to 0xFFFFFFC ( ~(1<<0 | 1<<1) ).
*/

#define BINT_MAP(L1,name,suffix,reg,misc,process,bits) {BINT_P_MAP_MISC_##misc | (BCHP_HIF_CPU_INTR1_INTR_W##L1##_STATUS_##name##_CPU_INTR_SHIFT+L1*32), BCHP_##reg , BINT_P_MAP_PROCESS_##process(bits), #name suffix}

/*
Summary:
This macro definition used to define the interrupt map entry for 'external' L1 interrupts

Description:
External L1 interrupts used to complement the interrupt map table. Only two arguments are needed:
 - L1 - number of L1 status register (0,1,2, etc) that L1 interrupt belongs to
 - name - name of the L1 interrupts - this is the name in RDB without the '_CPU_INTR' suffix. For example for RDB field name 'MEMC1_CPU_INTR' one should specify 'MEMC1'
*/
#define BINT_MAP_EXT(L1,name) {(BCHP_HIF_CPU_INTR1_INTR_W##L1##_STATUS_##name##_CPU_INTR_SHIFT+L1*32), 0, BINT_DONT_PROCESS_L2, #name}

/*
Summary:
This macro definition used to end the interrupt map
*/
#define BINT_MAP_LAST() {-1, 0, 0, NULL}

#define BINT_MAP_IS_EXTERNAL(m) ((m)->L2RegOffset==0)
#define BINT_MAP_GET_L1SHIFT(m) ((m)->L1Shift & (~(BINT_IS_STANDARD|BINT_P_MAP_MISC_WEAK_MASK)))

/*
Summary:
This structure is used to store the interrupt map supported by a specific
instance of the interrupt interface.
*/
typedef struct BINT_P_IntMap
{
   int L1Shift;             /* L1 shift value (-1 signifies the end of list).
                             * This value must match the L1Shift value passed
                             * into BINT_Isr() when you wish to process interrupts
                             * associated with this entry.
                             * This value, in combination with the L2RegOffset value, creates
                             * a unique L1 shift to L2 register mapping that is used
                             * when BINT_Isr() is called.
                             *
                             * This value can be OR'd with BINT_IS_STANDARD. If this is true,
                             * then this is a "standard" interrupt which can be processed in bint.c
                             * more efficiently. Overall performance improvement is significant.
                             * Each bint_CHIP.c file should set BINT_IS_STANDARD for standard L2's.
                             */
   uint32_t L2RegOffset;    /* L2 Register offset used when the specified L1 triggers.
                             * This value, in combination with the L1Shift value, creates
                             * a unique L1 shift to L2 register mapping that is used
                             * when BINT_Isr() is called.  This value is passed into
                             * BINT_SetIntFunc(), BINT_ClearIntFunc(), BINT_SetMaskFunc(),
                             * BINT_ClearMaskFunc(), BINT_ReadMaskFunc(), and BINT_ReadStatusFunc()
                             * functions as the baseAddr parameter.
                             * All BINT_Id's associated with this L2 register must also
                             * be defined using the same L2RegOffset value.
                             */
   uint32_t L2InvalidMask;  /* Mask that specifies the invalid bits contained in this L2 register
                             * (1 means the interrupt bit is not valid). BINT_DONT_PROCESS_L2 specifies
                             * that these L2 interrupts are processed by a proprietary L2 interrupt
                             * handling routine (so just call the callback and don't touch any of
                             * the interrupt registers).  When an L1 interrupt fires that is defined
                             * with a BINT_DONT_PROCESS_L2 mask, only the most recently created callback
                             * associated with that L1 shift will be called.
                             * Any callbacks associated with BINT_DONT_PROCESS_L2 masks will be called
                             * when the L1 interrupt triggers regardless of whether they are enabled or
                             * disabled.
                             */
   const char *L2Name;      /* Name of L2 interrupt */
} BINT_P_IntMap;

/* Used to Software Trigger an interrupt, used only if target H/W supports it */
typedef void (*BINT_SetIntFunc)(
                                BREG_Handle hRegister, /* [in] Register handle */
                                uint32_t baseAddr, /* [in] Base Register Offset, from device base */
                                int shift /* [in] Bit Shift */
                                );

/* Used to Clear an interrupt */
typedef void (*BINT_ClearIntFunc)(
                                  BREG_Handle hRegister, /* [in] Register handle */
                                  uint32_t baseAddr, /* [in] Base Register Offset, from device base */
                                  int shift /* [in] Bit Shift */
                                  );

/* Used to Mask an interrupt */
typedef void (*BINT_SetMaskFunc)(
                                 BREG_Handle hRegister, /* [in] Register handle */
                                 uint32_t baseAddr, /* [in] Base Register Offset, from device base */
                                 int shift /* [in] Bit Shift */
                                 );

/* Used to Clear an interrupt mask */
typedef void (*BINT_ClearMaskFunc)(
                                   BREG_Handle hRegister, /* [in] Register handle */
                                   uint32_t baseAddr, /* [in] Base Register Offset, from device base */
                                   int shift /* [in] Bit Shift */
                                   );

/* Used to read the L2 interrupt mask */
typedef uint32_t (*BINT_ReadMaskFunc)(
                               BREG_Handle Handle, /* [in] handle created by BINT_Open */
                               uint32_t baseAddr /* [in] Base Register Offset, from device base */
                               );

/* Used to read the L2 interrupt status */
typedef uint32_t (*BINT_ReadStatusFunc)(
                               BREG_Handle Handle, /* [in] handle created by BINT_Open */
                               uint32_t baseAddr /* [in] Base Register Offset, from device base */
                               );

typedef struct
{
    BINT_SetIntFunc pSetInt; /* ptr to Set Interrupt, NULL if none */
    BINT_ClearIntFunc pClearInt; /* ptr to Clear Interrupt, NULL if none */
    BINT_SetMaskFunc pSetMask; /* ptr to Set Interrupt Mask, REQUIRED */
    BINT_ClearMaskFunc pClearMask; /* ptr to Clear Interrupt Mask, REQUIRED */
    BINT_ReadMaskFunc pReadMask; /* ptr to Read Mask, REQUIRED */
    BINT_ReadStatusFunc pReadStatus; /* ptr to Read Status, REQUIRED */
    const BINT_P_IntMap *pIntMap; /* ptr to the interrupt map, REQUIRED */
    const char *name; /* chip name */
} BINT_Settings;

/*
Summary:
This function creates an instance of a interrupt interface.
*/
BERR_Code BINT_Open(
                    BINT_Handle *pHandle, /* [out] Returns handle to instance on interrupt interface */
                    BREG_Handle regHandle, /* [in] handle used for reading and writing registers */
                    const BINT_Settings *pDefSettings /* [in] pointer to default settings */
                    );

/*
Summary:
This function destroys an instance of a interrupt interface.
*/
BERR_Code BINT_Close(
                     BINT_Handle Handle  /* [in] handle created by BINT_Open */
                     );

/*
Summary:
This function returns a bit mask that describes which L1 interrupts the specified
instance of the BINT module is currently handling.

Description:
If a bit is set that means that this instanace of BINT is handling the L2 register
associated with that L1 bit.  This can be used by the platform initialization routine
to automatically create mappings between the L1 ISR handler and the BINT module instance.

Sample Code:
static void BFramework_EnableIsr( BINT_Handle intHandle )
{
    unsigned long i;
    bool enableIsr;
    uint32_t l1masklo, l1maskhi;

    BINT_GetL1BitMask( intHandle, &l1masklo, &l1maskhi );
    for( i=0; i<BINT_P_L1_SIZE; i++ )
    {
        enableIsr = false;
        if( i >=32 )
        {
            if( l1maskhi & 1ul<<(i-32) )
            {
                enableIsr = true;
            }
        }
        else
        {
            if( l1masklo & 1ul<<i )
            {
                enableIsr = true;
            }
        }
        if( enableIsr )
        {
            BDBG_WRN(("Enabling L1 interrupt %ld", i));
            CPUINT1_ConnectIsr(i, (FN_L1_ISR)BINT_Isr, intHandle, i );
            CPUINT1_Enable(i);
        }
    }
}
*/
#if (BINT_NEW_INT_MODEL)
void BINT_GetL1BitMask(
                     BINT_Handle intHandle, /* [in] handle created by BINT_Open */
                     uint32_t   *BitMask  /* [out] Bitmask that specifies which L1 bits are managed by BINT */
                     );
#else
void BINT_GetL1BitMask(
                     BINT_Handle intHandle, /* [in] handle created by BINT_Open */
                     uint32_t *pBitMaskLo,  /* [out] Bitmask that specifies which L1 bits are managed by BINT */
                     uint32_t *pBitMaskHi   /* [out] Bitmask that specifies which L1 bits are managed by BINT */
                     );
#endif

/*
Summary:
This function should be called any time an interrupt occurs.

Description:
This function should be called for each L1 interrupt bit that needs
to be processed by the interrupt interface module.

The interrupt interface does not mask or manage any L1 interrupt registers.
This is to allow the platform/os specific code to share the L1 registers
between platform/os specific code and common code which uses the interrupt
interface.
*/
void BINT_Isr(
              BINT_Handle Handle, /* [in] handle created by BINT_Open */
              int L1Shift /* [in] shift value for L1 interrupt bit to be processed */
              );

#ifdef __cplusplus
}
#endif

#endif
/* End of File */
