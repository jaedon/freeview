/******************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsid_img.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 9/17/12 2:59p $
*
* Module Description:
*   See Module Overview below.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/sid/src/bsid_img.c $
* 
* Hydra_Software_Devel/8   9/17/12 2:59p fbasso
* SW7445-34: added support for 7445 to sid.
*
* Hydra_Software_Devel/7   8/27/12 12:35p fbasso
* SW7425-3620: fixed build warning.
*
* Hydra_Software_Devel/6   6/15/12 11:34a fbasso
* SW7425-3239: fixed computation, part of text segment was not be
* included in the math.
*
* Hydra_Software_Devel/5   6/13/12 3:06p fbasso
* SW7425-3239: fixed calculation of memory region for SID boot and text
* segments to be authenticated during secure boot
*
* Hydra_Software_Devel/4   5/1/12 1:08p fbasso
* SW7584-18: added support for 7584 to SID
*
* Hydra_Software_Devel/3   4/19/12 12:13p fbasso
* SW7429-128: added SID PI support.
*
* Hydra_Software_Devel/2   3/5/12 10:45a fbasso
* SW7435-26: added support for 7435 to SID PI.
*
* Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-1619: release SID multi channel interface to
* Hydra_Software_Devel
*
* Hydra_Software_Devel/sid_channel_interface_devel/2   1/3/12 4:53p fbasso
* SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* for 7346
*
* Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-1619: add queue based interface and multi channel support
*
* Hydra_Software_Devel/22   9/13/11 4:47p katrep
* SW7231-320:sw init on the block moved to sun top??
*
* Hydra_Software_Devel/21   8/23/11 10:33a fbasso
* SW7231-320: Add SID PI support for 7231 B0
*
* Hydra_Software_Devel/20   8/22/11 4:47p fbasso
* SWDTV-8264: returning Transparency RGB color for PNG decode when
* present.
*
* Hydra_Software_Devel/19   6/21/11 11:13a parijat
* SW7425-674: Added $brcm_Log: /magnum/portinginterface/sid/src/bsid_img.c $
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/8   9/17/12 2:59p fbasso
* SW7425-674: Added SW7445-34: added support for 7445 to sid.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/7   8/27/12 12:35p fbasso
* SW7425-674: Added SW7425-3620: fixed build warning.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/6   6/15/12 11:34a fbasso
* SW7425-674: Added SW7425-3239: fixed computation, part of text segment was not be
* SW7425-674: Added included in the math.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/5   6/13/12 3:06p fbasso
* SW7425-674: Added SW7425-3239: fixed calculation of memory region for SID boot and text
* SW7425-674: Added segments to be authenticated during secure boot
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/4   5/1/12 1:08p fbasso
* SW7425-674: Added SW7584-18: added support for 7584 to SID
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/3   4/19/12 12:13p fbasso
* SW7425-674: Added SW7429-128: added SID PI support.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/2   3/5/12 10:45a fbasso
* SW7425-674: Added SW7435-26: added support for 7435 to SID PI.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-674: Added SW7425-1619: release SID multi channel interface to
* SW7425-674: Added Hydra_Software_Devel
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/2   1/3/12 4:53p fbasso
* SW7425-674: Added SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* SW7425-674: Added for 7346
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-674: Added SW7425-1619: add queue based interface and multi channel support
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/22   9/13/11 4:47p katrep
* SW7425-674: Added SW7231-320:sw init on the block moved to sun top??
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/21   8/23/11 10:33a fbasso
* SW7425-674: Added SW7231-320: Add SID PI support for 7231 B0
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/20   8/22/11 4:47p fbasso
* SW7425-674: Added SWDTV-8264: returning Transparency RGB color for PNG decode when
* SW7425-674: Added present.
*
******************************************************************************/

#include "bchp_sun_top_ctrl.h"

#if ((BCHP_CHIP==3548) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==3556) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==35230) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==35125) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==35233) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_gfx_rgrb.h"
#elif ((BCHP_CHIP==7422) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7425) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7231) && (BCHP_VER >= BCHP_VER_B0)) || \
      ((BCHP_CHIP==7344) && (BCHP_VER >= BCHP_VER_B0)) || \
      ((BCHP_CHIP==7346) && (BCHP_VER >= BCHP_VER_B0)) || \
      ((BCHP_CHIP==7429) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_gfx_gr.h"
#elif ((BCHP_CHIP==7435) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7584) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7445) && (BCHP_VER >= BCHP_VER_A0))
/* sundry is used to boot up SID arc */
#else
#include "bchp_gfx_grb.h"
#endif

#include "bchp_sid_arc.h"
#include "bchp_sid_arc_core.h"
#include "bchp_sid_arc_dbg.h"

#include "bsid.h"
#include "bsid_priv.h"
#include "bsid_img.h"

BDBG_MODULE(BSID_IMG);

#define BERR_SID_EOF                                  BERR_MAKE_CODE(0xFF, 1)
#define BSID_P_TEST_CHUNK                                                  16
#define BIMG_SKIP                                              (uint32_t)(-2)
#define BSID_IMG_CHUNK_SIZE                                         (48*1024)
#define BSID_IMG_TEXT_SEGMENT_OFFSET                               0x00000400 /* from FW linker */
#define BSID_IMG_BOOT_SEGMENT_SIZE                                 0x00000400 /* from FW linker */
#define BSID_IMG_DATA_SEGMENT_OFFSET                               0x00040000 /* from FW linker */
#define BSID_IMG_BOOT_AND_TEXT_SEGMENT_EST_SIZE                      100*1024
#define SID_FW_DUMP_TO_FILE                                                 0
#if SID_FW_DUMP_TO_FILE

#include <stdio.h>

#endif

extern const void *BSID_IMG_Sid[];

/******************************************************************************
* Function name: BSID_P_LoadOne
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_LoadOne(BSID_Handle hSid, const uint32_t *data, unsigned size)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t  i;
    uint32_t  len, addr;
    uint32_t  testData[BSID_P_TEST_CHUNK];
    uint32_t  *AuxVirtAddr = NULL;

    size /= 4;
    while(size)
    {
        addr = *data++;
        size--;
        if (addr==BIMG_SKIP) {
            continue;
        }
        if (addr==BIMG_EOF) {
            return BERR_SID_EOF;
        }
        len = *data++;
        size--;
        if (len>size) {
            return BERR_TRACE(BERR_OS_ERROR);
        }

#ifdef BSID_USE_CACHED_MEMORY
        AuxVirtAddr = (uint32_t*)((uint32_t)hSid->sFwHwConfig.sCodeMemory.pv_CachedAddr + (uint32_t)addr);
#else
        AuxVirtAddr = (uint32_t*)((uint32_t)hSid->sFwHwConfig.sCodeMemory.pui32_VirtAddr + (uint32_t)addr);
#endif

        /* Accumulate the size of the boot segment and text segment together.
           The combined segment will be authenticate by security module. */
        if (addr >= BSID_IMG_TEXT_SEGMENT_OFFSET && addr < BSID_IMG_DATA_SEGMENT_OFFSET)
        {
            hSid->sBootInfo.ui32_Size += (len << 2);
        }

#if SID_FW_DUMP_TO_FILE
        {
            FILE *SIDFwCode = NULL;
            SIDFwCode = fopen("firmware", "wb");
            if (!SIDFwCode)
            {
                BKNI_Printf("\nCould not open file for dumping the SID firmware. Make sure to do chmod 777 -R <directory from which the app executable is being run>\n");
            }
            else
            {
                fwrite((void*)data, 1, (len * 4), SIDFwCode);
                fclose(SIDFwCode);
            }
        }
#endif

        BKNI_Memcpy((void*)(AuxVirtAddr), (void*)data, (len * 4));

        if (hSid->sFwHwConfig.bSelfTest)
        {
            for(i=0;i<len;)
            {
                unsigned testSize;
                unsigned j;

                if (i+BSID_P_TEST_CHUNK <= len) {
                    testSize = BSID_P_TEST_CHUNK;
                } else {
                    testSize = len - i;
                }

                {
                    uint32_t *tempAddr = (uint32_t *)((uint32_t)AuxVirtAddr + (uint32_t)(i*4));
                    BKNI_Memcpy((void*)testData, (void*)(tempAddr), (4 * testSize));
                }

                for(j=0;j<testSize;j++) {
                    if ( testData[j] !=  data[i+j]) {
                        BDBG_ERR(("DDRAM failure at[%d] %#x->%#x(%#x)", i, addr+(i+j)*4, testData[j], data[i+j]));
                        return BERR_TRACE(BERR_UNKNOWN);
                    }
                }
                i+=testSize;
            }
        }

        size -= len;
        data += len;
    }
    return BERR_TRACE(retCode);

}

/******************************************************************************
* Function name: BSID_P_LoadUnit
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_LoadCode(BSID_Handle hSid)
{
    BERR_Code retCode;
    uint32_t  ui32_i;
#ifndef BSID_USE_CACHED_MEMORY
    uint32_t  ui32_ReadBack;
#endif

#ifdef BSID_USE_CACHED_MEMORY
    retCode = BMEM_ConvertAddressToCached(hSid->hMem, hSid->sFwHwConfig.sCodeMemory.pui32_VirtAddr, &hSid->sFwHwConfig.sCodeMemory.pv_CachedAddr);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_P_SetMailboxInfo: BMEM_ConvertAddressToCached for CmdQ failed"));
       return BERR_TRACE(BERR_UNKNOWN);
    }
#endif

#ifdef BSID_USE_CACHED_MEMORY
    hSid->sBootInfo.ui32_Address = (uint32_t)hSid->sFwHwConfig.sCodeMemory.pv_CachedAddr;
#else
    hSid->sBootInfo.ui32_Address = (uint32_t)hSid->sFwHwConfig.sCodeMemory.pui32_VirtAddr;
#endif
    hSid->sBootInfo.ui32_Size = BSID_IMG_BOOT_SEGMENT_SIZE;

    /* Init to zero boot plus text segment region. In some platform this region is authenticated,
       the default zeroes will help avoid incorrect math in the gap area between the boot segment
       and the text segment due to otherwise random values.
       The value in BSID_IMG_TEXT_SEGMENT_EST_SIZE is a a pure estimate, it is currently twice as
       much the real combined size between boot and text segments. */
    BKNI_Memset((void *)hSid->sBootInfo.ui32_Address, 0x00, BSID_IMG_BOOT_AND_TEXT_SEGMENT_EST_SIZE);

    for(ui32_i = 0; ;ui32_i++)
    {
        retCode = BSID_P_LoadOne(hSid, BSID_IMG_Sid[ui32_i], BSID_IMG_CHUNK_SIZE);
        if (retCode==BERR_SID_EOF)
        {
            break;
        }
        else if (retCode!=BERR_SUCCESS)
        {
            return BERR_TRACE(retCode);
        }
    }

#ifdef BSID_USE_CACHED_MEMORY
    BMEM_FlushCache( hSid->hMem, hSid->sFwHwConfig.sCodeMemory.pv_CachedAddr, BSID_ARC_CODE_SIZE);
#else
    ui32_ReadBack = *hSid->sFwHwConfig.sCodeMemory.pui32_VirtAddr;
#endif

    return BERR_TRACE(BERR_SUCCESS);
}

/******************************************************************************
* Function name: BSID_P_ChipEnable
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_ChipEnable(BSID_Handle hSid)
{
    BERR_Code retCode = BERR_SUCCESS;

#if 0 /* 7440 */

#if ((BCHP_CHIP == 7440) && (BCHP_VER >= BCHP_VER_A0))

    uint32_t ui32_reg = BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_44));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, ui32_reg | (3<<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_44_SHIFT));

#if (BCHP_VER == BCHP_VER_A0)
    ui32_reg =  BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_60));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, ui32_reg | (4<<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_60_SHIFT) );
#else
    ui32_reg =  BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, sgpio_08));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, ui32_reg | (4<<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_sgpio_08_SHIFT) );
#endif

    ui32_reg =  BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
    ui32_reg &= ~0xF00;
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL, ui32_reg | (BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_3_cpu_sel_SID << BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_3_cpu_sel_SHIFT));

#elif (((BCHP_CHIP==3548) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==3556) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==35230) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==35125) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==7422) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==7425) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==35233) && (BCHP_VER >= BCHP_VER_A0)) ||\
       ((BCHP_CHIP==7231) && (BCHP_VER >= BCHP_VER_B0)) || \
       ((BCHP_CHIP==7344) && (BCHP_VER >= BCHP_VER_B0)) || \
       ((BCHP_CHIP==7346) && (BCHP_VER >= BCHP_VER_B0)))

#if ((BCHP_CHIP==7425) && (BCHP_VER >= BCHP_VER_A0))
    uint32_t ui32_reg;

    ui32_reg = BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_6_cpu_sel) | BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_5_cpu_sel));
    ui32_reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_6_cpu_sel, SID);
    ui32_reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_5_cpu_sel, SID);
    BREG_Write32(hSid->hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,ui32_reg);

    ui32_reg = BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
    ui32_reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SYS);
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, ui32_reg);
#endif

#if 0 /* 35230C0 as of 082111 */
    /* rxd */
    ui32_reg = BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, rdb));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, (ui32_reg | (BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_rdb_TP_IN_26 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_rdb_SHIFT)));

    /* txd */
    ui32_reg = BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, tdb));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, (ui32_reg | (BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_tdb_TP_OUT_00 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_tdb_SHIFT)));

    /* uart selector: tp 1 selected */
    ui32_reg =  BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
    ui32_reg &= ~BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL,port_1_cpu_sel);
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL, (ui32_reg | (BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_1_cpu_sel_SID << BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_1_cpu_sel_SHIFT)));

    /* enable SUNDRY block from the test port ctlr */
    ui32_reg = BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, (ui32_reg | BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SUN)));
#endif

#if 0
    /* rxd */
    uint32_t ui32_reg = BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_46));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, ui32_reg | (6<<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_46_SHIFT));

    /* txd */
    ui32_reg = BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
    ui32_reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_43));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, ui32_reg | (6<<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_43_SHIFT));

    /* uart selector: tp 1 selected */
    ui32_reg =  BREG_Read32(hSid->hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
    ui32_reg &= ~0xF0;
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL, ui32_reg | (BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_1_cpu_sel_SID << BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_1_cpu_sel_SHIFT));
#endif

#endif

#endif

    /* wake up sid arc */
    BREG_Write32(hSid->hReg, BCHP_SID_ARC_CPU_INST_BASE, hSid->sFwHwConfig.sCodeMemory.ui32_PhisAddr);
    BREG_Write32(hSid->hReg, BCHP_SID_ARC_CPU_END_OF_CODE, 0x80000);
    BREG_Write32(hSid->hReg, BCHP_SID_ARC_DBG_CPU_DBG, 1);
    BREG_Write32(hSid->hReg, BCHP_SID_ARC_CORE_CPU_PC, 0);
    BREG_Write32(hSid->hReg, BCHP_SID_ARC_DBG_CPU_DBG, 0);

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_BootArc
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_BootArc(BSID_Handle hSid)
{
    BERR_Code retCode;

    /* sid sw reset */
#if ((BCHP_CHIP==3548) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==3556) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==35230) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==35125) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==35233) && (BCHP_VER >= BCHP_VER_A0))
    BREG_Write32(hSid->hReg, BCHP_GFX_RGRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_RGRB_SW_RESET_0, SID_SW_RESET, ASSERT));
    BREG_Write32(hSid->hReg, BCHP_GFX_RGRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_RGRB_SW_RESET_0, SID_SW_RESET, DEASSERT));
#elif ((BCHP_CHIP==7422) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7425) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7344) && (BCHP_VER >= BCHP_VER_B0)) || \
      ((BCHP_CHIP==7346) && (BCHP_VER >= BCHP_VER_B0))
    BREG_Write32(hSid->hReg, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, SID_CLK_108_SW_INIT, ASSERT));
    BREG_Write32(hSid->hReg, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, SID_CLK_108_SW_INIT, DEASSERT));
#elif ((BCHP_CHIP==7231) && (BCHP_VER >= BCHP_VER_B0)) || \
      ((BCHP_CHIP==7435) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7584) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7429) && (BCHP_VER >= BCHP_VER_A0)) || \
      ((BCHP_CHIP==7445) && (BCHP_VER >= BCHP_VER_A0))
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_SW_INIT_1_SET,BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_1_SET, sid_sw_init, 1));
    BREG_Write32(hSid->hReg, BCHP_SUN_TOP_CTRL_SW_INIT_1_CLEAR,BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_1_CLEAR, sid_sw_init, 1));
#else
    BREG_Write32(hSid->hReg, BCHP_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, SID_SW_RESET, ASSERT));
    BREG_Write32(hSid->hReg, BCHP_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, SID_SW_RESET, DEASSERT));
#endif


    /* load sid code onto its memory */
    retCode = BSID_P_LoadCode(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_P_BootArc: error in BSID_P_LoadCode 0x%x", retCode));
        return BERR_TRACE(retCode);
    }

    /*SID fw code authenticate*/
    if (hSid->pExternalBootCallback)
    {
        retCode = (*hSid->pExternalBootCallback)(hSid->pExternalBootCallbackData, &hSid->sBootInfo);

        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_P_BootArc: error in SID fw authentication 0x%x", retCode));
            return BERR_TRACE(retCode);
        }
    }

    retCode = BSID_P_ChipEnable(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_P_BootArc: error in BSID_P_ChipEnable 0x%x", retCode));
        return BERR_TRACE(retCode);
    }

    /* boot sid arc */
    return BERR_TRACE(BERR_SUCCESS);
}
