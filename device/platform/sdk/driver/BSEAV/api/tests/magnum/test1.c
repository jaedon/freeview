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
 * $brcm_Workfile: test1.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/magnum/test1.c $
 * 
 * 6   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop_board.h"
#include "bsettop_os.h"
#include "bvdc.h" /* specific magnum api required by this app */
#include <stdio.h>

void *my_func(void *data)
{
    int i;
    BSTD_UNUSED(data);
    for (i=0;i<10;i++) {
        BKNI_Printf("task: %d\n", i);
        BKNI_Sleep(100);
    }
    return NULL;
}

int main(void) 
{
    bresult rc;
    BERR_Code mrc;
    BVDC_Settings cfg;
    BVDC_Handle vdc;
    BRDC_Handle rdc;
    b_task_t task;
    b_task_params task_params;
    BRDC_Settings RdcSettings;

    rc = bsettop_boot_board();
    BDBG_ASSERT(!rc);

    mrc = BRDC_Open(&rdc, b_board.B_CORE_CHIP.chip, b_board.B_CORE_CHIP.reg, b_board.B_CORE_CHIP.mem,&RdcSettings);
    BDBG_ASSERT(!mrc);
    
    BVDC_GetDefaultSettings(&cfg);
    mrc = BVDC_Open(&vdc, b_board.B_CORE_CHIP.chip, b_board.B_CORE_CHIP.reg, b_board.B_CORE_CHIP.mem, 
        b_board.B_CORE_CHIP.irq, 
        rdc, 
        b_board.B_CORE_CHIP.tmr,
        &cfg);
    BDBG_ASSERT(!mrc);

    /* Here's an example of using bsettop_os.h functions */ 
    b_task_params_init(&task_params);
    rc = b_start_task(&task, &task_params, my_func, NULL);
    b_stop_task(task);  
    
    getchar();
    
    bsettop_shutdown_board();
    return 0;
}
