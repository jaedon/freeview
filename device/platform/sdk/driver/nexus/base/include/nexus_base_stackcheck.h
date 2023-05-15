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
* $brcm_Workfile: nexus_base_stackcheck.h $
* $brcm_Revision: 2 $
* $brcm_Date: 8/1/12 6:32p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/nexus_base_stackcheck.h $
* 
* 2   8/1/12 6:32p vsilyaev
* SW7435-287: Added code to measure stack utilization by callback
*  handlers
* 
* 1   7/31/12 6:36p vsilyaev
* SW7435-287: NEXUS base component for non-destructive test of the stack
*  utilization
* 
***************************************************************************/
#ifndef NEXUS_BASE_STACKCHECK_H__
#define NEXUS_BASE_STACKCHECK_H__

/* granularity of stack depth test */
#define NEXUS_P_BASE_STACKCHECK_ATOM  64
/* that much of stack should be avaliable before any probe, e.g. that much probe would actually test, and guarantee to not fail */
#define NEXUS_P_BASE_STACKSIZE_MIN  4096

typedef struct NEXUS_P_Base_StackCheck_Frame {
    uint8_t crc[NEXUS_P_BASE_STACKSIZE_MIN/NEXUS_P_BASE_STACKCHECK_ATOM];
} NEXUS_P_Base_StackCheck_Frame;
void NEXUS_P_Base_StackCheck_MakeFrame(NEXUS_P_Base_StackCheck_Frame *frame, const void *_stack);
unsigned NEXUS_P_Base_StackCheck_CheckFrame(const NEXUS_P_Base_StackCheck_Frame *frame, const void *_stack);

#endif /* NEXUS_BASE_STACKCHECK_H__ */
