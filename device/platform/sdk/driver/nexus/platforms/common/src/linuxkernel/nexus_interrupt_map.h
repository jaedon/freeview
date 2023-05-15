/***************************************************************************
*     (c)2011-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_interrupt_map.h $
* $brcm_Revision: 4 $
* $brcm_Date: 1/6/12 9:36a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/linuxkernel/nexus_interrupt_map.h $
* 
* 4   1/6/12 9:36a erickson
* SW7420-1318: NEXUS_Platform_P_IrqMap still required to manage L1's with
*  no L2's (for instance, external irqs)
* 
* 2   12/22/11 5:56p katrep
* SW7429-1:fixed pointer overflow due long interrupt names
* 
* 1   5/3/11 5:10p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
***************************************************************************/
#ifndef NEXUS_INTERRUPT_MAP_H__
#define NEXUS_INTERRUPT_MAP_H__

#include "nexus_platform_features.h"

struct NEXUS_Platform_P_IrqMap{
    int16_t linux_irq; /* virtual linuxirq number */
    bool requested; /* unused - true if request_irq has been called */
    bool enabled; /* unused - true if enable_irq has been called */
    const char *name;
    bool shared; /* used to set SA_SHIRQ flag */
    void (*special_handler)(int linux_irq); /* for interrupts where L1 cannot be disable. instead, disable L2 interrupts immediately.
        requires PI code to reenable. */
};

/* variable sized array, terminated with name == NULL */
extern struct NEXUS_Platform_P_IrqMap NEXUS_Platform_P_IrqMap[];

#endif /* NEXUS_INTERRUPT_MAP_H__ */
