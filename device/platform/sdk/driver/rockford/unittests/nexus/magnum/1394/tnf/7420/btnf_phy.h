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
 * $brcm_Workfile: btnf_phy.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/6/12 6:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/magnum/1394/tnf/7420/btnf_phy.h $
 * 
 * Hydra_Software_Devel/2   9/6/12 6:08p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#ifndef __PHY_H__
#define __PHY_H__

/*
** PHY registers (they don't label registers but do define bits).
*/

/* Address 0 */
#define PHY_PHYSICAL_ID                     0xfc
#define PHY_ROOT                            0x02
#define PHY_CABLE_POWER_ACTIVE              0x01

/* Address 1 */
#define PHY_ROOT_HOLD_OFF_BIT               0x80
#define PHY_INITIATE_BUS_RESET              0x40
#define PHY_MAX_GAP_COUNT                   0x3f

/* Address 2 */
#define PHY_EXTENDED                        0xE0
#define PHY_TOTAL_PORTS                     0x0F

/* Address 3 */
#define PHY_MAX_SPEED                       0xE0
#define PHY_DELAY                           0x0F

/* Address 4 */
#define PHY_LINK_CONTROL_ENABLE             0x80
#define PHY_CONTENDER_ENABLE                0x40 
#define PHY_JITTER                          0x38
#define PHY_POWER_CLASS                     0x07

/* Address 5 */
#define PHY_RESUME_INT_ENABLE               0x80
#define PHY_INITIATE_SHORT_BUS_RESET        0x40
#define PHY_LOOP_DETECT                     0x20
#define PHY_POWER_FAILURE                   0x10
#define PHY_TIMEOUT                         0x08
#define PHY_PORT_EVENT_DETECTED             0x04
#define PHY_ENABLE_ARBITRATION_ACCELERATION 0x02
#define PHY_ENABLE_MULTISPEED               0x01

#endif /* __PHY_H__ */

