/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: test_bsp.c $
 * $brcm_Revision: 16 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/magnum/test_bsp.c $
 * 
 * 16   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop_board.h"
#include "bsettop_os.h"
#include <stdio.h>
#include "bchp_sun_top_ctrl.h"
#include "bchp_timer.h"
#include "bchp_int_id_timer.h"

#if BCHP_CHIP==7400 || BCHP_CHIP==7401 || BCHP_CHIP == 7403 || BCHP_CHIP == 7118 || BCHP_CHIP == 7405 || BCHP_CHIP ==7325 || BCHP_CHIP ==7335
    #include "bchp_memc_0.h"
    #define BCHP_MEMC64_MSA_STATUS BCHP_MEMC_0_MSA_STATUS
    #define BCHP_MEMC64_MSA_CMD_TYPE BCHP_MEMC_0_MSA_CMD_TYPE
    #define BCHP_MEMC64_MSA_CMD_ADDR BCHP_MEMC_0_MSA_CMD_ADDR
    #define BCHP_MEMC64_MSA_RD_DATA(n) BCHP_MEMC_0_MSA_RD_DATA##n
    #define BCHP_MEMC64_MSA_WR_DATA(n) BCHP_MEMC_0_MSA_WR_DATA##n
    #define BCHP_MEMC64_MSA_STATUS_BUSY_MASK BCHP_MEMC_0_MSA_STATUS_BUSY_MASK
    #define BCHP_MEMC64_MSA_STATUS_BUSY_SHIFT BCHP_MEMC_0_MSA_STATUS_BUSY_SHIFT
#else
    #include "bchp_memc.h"
    #define BCHP_MEMC64_MSA_STATUS BCHP_MEMC_MSA_STATUS
    #define BCHP_MEMC64_MSA_CMD_TYPE BCHP_MEMC_MSA_CMD_TYPE
    #define BCHP_MEMC64_MSA_CMD_ADDR BCHP_MEMC_MSA_CMD_ADDR
    #define BCHP_MEMC64_MSA_RD_DATA(n) BCHP_MEMC_MSA_RD_DATA##n
    #define BCHP_MEMC64_MSA_WR_DATA(n) BCHP_MEMC_MSA_WR_DATA##n
    #define BCHP_MEMC64_MSA_STATUS_BUSY_MASK BCHP_MEMC_MSA_STATUS_BUSY_MASK
    #define BCHP_MEMC64_MSA_STATUS_BUSY_SHIFT BCHP_MEMC_MSA_STATUS_BUSY_SHIFT
#endif
#include "bkni.h"

BDBG_MODULE(test_bsp);

void *my_func(void *data)
{
    int i;
    BSTD_UNUSED(data);
    for (i=0;i<10;i++)
    {
        BKNI_Printf("task: %d\n", i);
        BKNI_Sleep(100);
    }
    return NULL;
}

#if BCHP_CHIP==7038
    #define b_XXXX b7038
#elif BCHP_CHIP==3560
    #define b_XXXX b3560
#elif BCHP_CHIP==7401
    #define b_XXXX b7401
#elif BCHP_CHIP==7403
    #define b_XXXX b7403
#elif BCHP_CHIP==7405
    #define b_XXXX b7405
#elif BCHP_CHIP==7400
    #define b_XXXX b7400
#elif BCHP_CHIP==7118
    #define b_XXXX b7118
#elif BCHP_CHIP==7325
    #define b_XXXX b7325
#elif BCHP_CHIP==7335
    #define b_XXXX b7335
#elif
    #error "Not supported"
#endif

static void 
b_timer_isr(void *parm1, int parm2)
{
    BSTD_UNUSED(parm1);
    BSTD_UNUSED(parm2);
    BDBG_MSG(("timer..."));
    return;
}


/* Read Physical Memory through MSA Client.
   It reads 8 32-bit words at a time.
   So uiPhyAddr should be a OWORD address */
static void 
b_read_mem(uint32_t uiPhyAddr, uint32_t* pData)
{
    uint32_t ui32RegVal = 0;
    int iLoopCnt = 0;
    BREG_Handle hReg=b_board.b_XXXX.reg;

    BDBG_ASSERT(hReg);
    BDBG_ASSERT(pData);

    /* write the command */
    BREG_Write32(hReg, BCHP_MEMC64_MSA_CMD_TYPE, 1);

    /* write the physical address */
    BREG_Write32(hReg, BCHP_MEMC64_MSA_CMD_ADDR, uiPhyAddr>>3);

    /* Wait till MSA is busy */
    do
    {
        ui32RegVal = BREG_Read32(hReg, BCHP_MEMC64_MSA_STATUS);
        iLoopCnt++;
        if (iLoopCnt > 1000)
        {
            printf("\nMSA Read: Timeout!!!\n");
            break;
        }
    }while (BCHP_GET_FIELD_DATA(ui32RegVal, MEMC64_MSA_STATUS, BUSY) != 0);

    /* read back the data */
    *pData = BREG_Read32(hReg, BCHP_MEMC64_MSA_RD_DATA(7));
    *(pData+1) = BREG_Read32(hReg, BCHP_MEMC64_MSA_RD_DATA(6));
    *(pData+2) = BREG_Read32(hReg, BCHP_MEMC64_MSA_RD_DATA(5));
    *(pData+3) = BREG_Read32(hReg, BCHP_MEMC64_MSA_RD_DATA(4));
    *(pData+4) = BREG_Read32(hReg, BCHP_MEMC64_MSA_RD_DATA(3));
    *(pData+5) = BREG_Read32(hReg, BCHP_MEMC64_MSA_RD_DATA(2));
    *(pData+6) = BREG_Read32(hReg, BCHP_MEMC64_MSA_RD_DATA(1));
    *(pData+7) = BREG_Read32(hReg, BCHP_MEMC64_MSA_RD_DATA(0));

    return;
}

/* Write Physical Memory through MSA Client.
   It writess 8 32-bit words at a time.
   So uiPhyAddr should be a OWORD address */
void b_write_mem(uint32_t uiPhyAddr, const uint32_t* pData)
{
    uint32_t ui32RegVal = 0;
    int iLoopCnt = 0;
    BREG_Handle hReg=b_board.b_XXXX.reg;

    BDBG_ASSERT(hReg);
    BDBG_ASSERT(pData);

    /* write the data */
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(7), *pData);
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(6), *(pData+1));
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(5), *(pData+2));
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(4), *(pData+3));
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(3), *(pData+4));
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(2), *(pData+5));
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(1), *(pData+6));
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(0), *(pData+7));

    /* write the command */
#if BCHP_CHIP == 7405 || BCHP_CHIP == 7325
    BREG_Write32(hReg, BCHP_MEMC64_MSA_CMD_TYPE, 0x21);
#else
    BREG_Write32(hReg, BCHP_MEMC64_MSA_CMD_TYPE, 0x11);
#endif

    /* write the physical address */
    BREG_Write32(hReg, BCHP_MEMC64_MSA_CMD_ADDR, uiPhyAddr>>3);

    /* Wait till MSA is busy */
    do
    {
        ui32RegVal = BREG_Read32(hReg, BCHP_MEMC64_MSA_STATUS);
        iLoopCnt++;
        if (iLoopCnt > 1000)
        {
            printf("\nMSA Write: Timeout!!!\n");
            break;
        }
    }while (BCHP_GET_FIELD_DATA(ui32RegVal, MEMC64_MSA_STATUS, BUSY) != 0);

    return;
}

#if BCHP_CHIP==7405 || BCHP_CHIP==7325 || BCHP_CHIP==7335
    #include "bchp_memc_1.h"
    #define BCHP_MEMC16_MSA_STATUS BCHP_MEMC_1_MSA_STATUS
    #define BCHP_MEMC16_MSA_CMD_TYPE BCHP_MEMC_1_MSA_CMD_TYPE
    #define BCHP_MEMC16_MSA_CMD_ADDR BCHP_MEMC_1_MSA_CMD_ADDR
    #define BCHP_MEMC16_MSA_RD_DATA(n) BCHP_MEMC_1_MSA_RD_DATA##n
    #define BCHP_MEMC16_MSA_WR_DATA(n) BCHP_MEMC_1_MSA_WR_DATA##n
    #define BCHP_MEMC16_MSA_STATUS_BUSY_MASK BCHP_MEMC_0_MSA_STATUS_BUSY_MASK
    #define BCHP_MEMC16_MSA_STATUS_BUSY_SHIFT BCHP_MEMC_0_MSA_STATUS_BUSY_SHIFT

/* Read Physical Memory through MSA Client.
   It reads 8 32-bit words at a time.
   So uiPhyAddr should be a OWORD address */
static void 
b_read_mem1(uint32_t uiPhyAddr, uint32_t* pData)
{
    uint32_t ui32RegVal = 0;
    int iLoopCnt = 0;
    BREG_Handle hReg=b_board.b_XXXX.reg;

    BDBG_ASSERT(hReg);
    BDBG_ASSERT(pData);

    /* write the command */
    BREG_Write32(hReg, BCHP_MEMC16_MSA_CMD_TYPE, 1);

    /* write the physical address */
    BREG_Write32(hReg, BCHP_MEMC16_MSA_CMD_ADDR, uiPhyAddr>>3);

    /* Wait till MSA is busy */
    do
    {
        ui32RegVal = BREG_Read32(hReg, BCHP_MEMC16_MSA_STATUS);
        iLoopCnt++;
        if (iLoopCnt > 1000)
        {
            printf("\nMSA Read: Timeout!!!\n");
            break;
        }
    }while (BCHP_GET_FIELD_DATA(ui32RegVal, MEMC16_MSA_STATUS, BUSY) != 0);

    /* read back the data */
    *pData = BREG_Read32(hReg, BCHP_MEMC16_MSA_RD_DATA(7));
    *(pData+1) = BREG_Read32(hReg, BCHP_MEMC16_MSA_RD_DATA(6));
    *(pData+2) = BREG_Read32(hReg, BCHP_MEMC16_MSA_RD_DATA(5));
    *(pData+3) = BREG_Read32(hReg, BCHP_MEMC16_MSA_RD_DATA(4));
    *(pData+4) = BREG_Read32(hReg, BCHP_MEMC16_MSA_RD_DATA(3));
    *(pData+5) = BREG_Read32(hReg, BCHP_MEMC16_MSA_RD_DATA(2));
    *(pData+6) = BREG_Read32(hReg, BCHP_MEMC16_MSA_RD_DATA(1));
    *(pData+7) = BREG_Read32(hReg, BCHP_MEMC16_MSA_RD_DATA(0));

    return;
}

/* Write Physical Memory through MSA Client.
   It writess 8 32-bit words at a time.
   So uiPhyAddr should be a OWORD address */
void b_write_mem1(uint32_t uiPhyAddr, const uint32_t* pData)
{
    uint32_t ui32RegVal = 0;
    int iLoopCnt = 0;
    BREG_Handle hReg=b_board.b_XXXX.reg;

    BDBG_ASSERT(hReg);
    BDBG_ASSERT(pData);

    /* write the data */
    BREG_Write32(hReg, BCHP_MEMC16_MSA_WR_DATA(7), *pData);
    BREG_Write32(hReg, BCHP_MEMC16_MSA_WR_DATA(6), *(pData+1));
    BREG_Write32(hReg, BCHP_MEMC16_MSA_WR_DATA(5), *(pData+2));
    BREG_Write32(hReg, BCHP_MEMC16_MSA_WR_DATA(4), *(pData+3));
    BREG_Write32(hReg, BCHP_MEMC16_MSA_WR_DATA(3), *(pData+4));
    BREG_Write32(hReg, BCHP_MEMC16_MSA_WR_DATA(2), *(pData+5));
    BREG_Write32(hReg, BCHP_MEMC16_MSA_WR_DATA(1), *(pData+6));
    BREG_Write32(hReg, BCHP_MEMC64_MSA_WR_DATA(0), *(pData+7));

    /* write the command */
    BREG_Write32(hReg, BCHP_MEMC16_MSA_CMD_TYPE, 0x21);

    /* write the physical address */
    BREG_Write32(hReg, BCHP_MEMC16_MSA_CMD_ADDR, uiPhyAddr>>3);

    /* Wait till MSA is busy */
    do
    {
        ui32RegVal = BREG_Read32(hReg, BCHP_MEMC16_MSA_STATUS);
        iLoopCnt++;
        if (iLoopCnt > 1000)
        {
            printf("\nMSA Write: Timeout!!!\n");
            break;
        }
    }while (BCHP_GET_FIELD_DATA(ui32RegVal, MEMC16_MSA_STATUS, BUSY) != 0);

    return;
}

/* run memory test on the 2nd memory controller */
void b_test_mem2()
{
    uint32_t data[8]= {0x0EADBEEF,0x1EADBEEF,0x2EADBEEF,0x3EADBEEF,0x4EADBEEF,0x5EADBEEF,0x6EADBEEF,0x7EADBEEF};
    uint32_t test[8]={0,0,0,0,0,0,0,0};
    uint32_t paddr = 0x6000000;
    uint32_t i;

    BDBG_WRN(("Starting memc1 test"));

    BDBG_WRN(("writing at address%#x", paddr));
    for (i=0;i<8;i++)
        BDBG_WRN(("offset=%#x=%#x", paddr+i*4,data[i]));

    b_write_mem1(paddr, data);
    BDBG_WRN(("reading  from %#x", paddr));
    b_read_mem1(paddr, test);
    for (i=0;i<8;i++)
    {
        if (data[i]==test[i])
            BDBG_WRN(("offset=%#x=%#x", paddr+i*4,test[i]));
        else
        {
            BDBG_WRN(("offset=%#x=%#x", paddr+i*4,test[i]));
            break;
        }
            
    }
    if (i==8)
        BDBG_WRN(("2nd moemory controller test passed"));
    else
        BDBG_WRN(("2nd moemory controller test failed"));

}
#endif

int main(void) 
{
    bresult rc;
    BERR_Code mrc=BERR_SUCCESS;
    b_task_t task;
    b_task_params task_params;
    uint32_t reg;
    BINT_CallbackHandle int_cb;
    unsigned i;


    BSTD_UNUSED(mrc);

    rc = bsettop_boot_board();
    BDBG_ASSERT(!rc);

    printf("CHIP revision %#x\n", BREG_Read32(b_board.b_XXXX.reg, BCHP_SUN_TOP_CTRL_PROD_REVISION));
    /* Here's an example of using bsettop_os.h functions */ 
    b_task_params_init(&task_params);
    rc = b_start_task(&task, &task_params, my_func, NULL);
    b_stop_task(task);  

    reg = BREG_Read32(b_board.b_XXXX.reg, BCHP_TIMER_TIMER0_CTRL);
    reg =
    BCHP_FIELD_DATA(TIMER_TIMER0_CTRL, ENA, 1) |
    BCHP_FIELD_DATA(TIMER_TIMER0_CTRL, MODE, 0) | /* periodic */
    BCHP_FIELD_DATA(TIMER_TIMER0_CTRL, TIMEOUT_VAL, 100*27*1000 ); /* 100 msec */
    BREG_Write32(b_board.b_XXXX.reg, BCHP_TIMER_TIMER0_CTRL,reg);

    BINT_CreateCallback(&int_cb, b_board.b_XXXX.irq, BCHP_INT_ID_TMR0TO, b_timer_isr, NULL, 0);
    BINT_EnableCallback(int_cb);

#if 1
    {
        for (i=0;i<100;i++)
        {
            reg = BREG_Read32(b_board.b_XXXX.reg, BCHP_TIMER_TIMER0_STAT);
            BDBG_WRN(("timer %u", reg));
            BKNI_Sleep(20);
        }
    }
#endif

    {
        uint32_t data[8]= {0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,0xDEADBEEF};
        uint32_t test[8]={0,0,0,0,0,0,0,0};
        uint32_t paddr = b_board.sys.mem.offset+0x10000;
        uint32_t *vaddr = (uint32_t *)(((uint8_t*)b_board.sys.mem.addr)+0x10000);

        BDBG_WRN(("write to %#x", paddr));
        getchar();
        for (i=0;i<8;i++)
            BDBG_WRN(("offset=%#x=%#x", paddr+i*4,data[i]));

        b_write_mem(paddr, data);
        BDBG_WRN(("read from %#x", paddr));
        getchar();
        b_read_mem(paddr, test);
        for (i=0;i<8;i++)
            BDBG_WRN(("offset=%#x=%#x", paddr+i*4,test[i]));

        BDBG_WRN(("test[0]=%#x", test[0]));
        BDBG_WRN(("read from %#x", vaddr));
        getchar();
        BDBG_WRN(("vaddr[0]=%#x", vaddr[0]));
        BDBG_WRN(("write to %#x", vaddr));
        getchar();
        vaddr[1] = 0xdeadbeef;
        vaddr[0] = 0xbeefdead;
#if 0
        {
            for (i=0;;i+=4096/4)
            {
                if (vaddr[i]==0xdeadbeef)
                {
                    BDBG_WRN(("vaddr[%d]=%#x", i, vaddr[i]));
                }
            }   
        }
#endif
        {
            for (i=0;i<128*1024*1024;i+=4096)
            {
                b_read_mem(i, test);    
                /* BDBG_WRN(("%#x:%#x", test[0],i)); */
                if (test[0]==0xbeefdead && test[1]==0xdeadbeef)
                {
                    BDBG_WRN(("paddr[%#x:%#x]=%#x", i, paddr, test[0]));
                }
            }   
        }
    }
#if 1
    {
        uint32_t *vaddr = (uint32_t *)b_board.sys.mem.addr;
        printf("presee enter to run sdram test \n");
        getchar();

        /* write block of data */
        for ( vaddr=(uint32_t *)b_board.sys.mem.addr;vaddr < (uint32_t *)((uint8_t*)b_board.sys.mem.addr+(128*1024*1024)/*b_board.sys.mem.size*/);vaddr+=8192)
        {
            printf("writing 0x%08x=%08x\r",(uint32_t)vaddr,(uint32_t)vaddr);
            *vaddr=(uint32_t)vaddr;

        }
        /* read and verify */
        for ( vaddr=(uint32_t *)b_board.sys.mem.addr;vaddr < (uint32_t *)((uint8_t*)b_board.sys.mem.addr+(128*1024*1024)/*b_board.sys.mem.size*/);vaddr+=8192)
        {
            if (*vaddr != (uint32_t)vaddr)
            {
                printf("verify failed at 0x%08x=%08x\r",(uint32_t)vaddr,(uint32_t)*vaddr);
                break;
            }

        }
        if (vaddr >= (uint32_t *)((uint8_t*)b_board.sys.mem.addr+(128*1024*1024)/*b_board.sys.mem.size*/))
            printf("\nmem test passed\n");
        else
            printf("\nmem test failed\n");


    }
#endif
#if BCHP_CHIP==7405
    b_test_mem2();
#endif
    getchar();

    bsettop_shutdown_board();
    return 0;
}





