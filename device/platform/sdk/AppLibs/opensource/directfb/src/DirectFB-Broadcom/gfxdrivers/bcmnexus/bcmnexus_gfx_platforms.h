/******************************************************************************
 *    (c)2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bcmnexus_gfx_platforms.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/1 $
 * $brcm_Date: 2/14/12 2:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/gfxdrivers/bcmnexus/bcmnexus_gfx_platforms.h $
 *
 * DirectFB_1_4_15_Port/1   2/14/12 2:15p christ
 * SW7231-652: DirectFB-1.4.15: Add chip defines for all AppLibs 3.0
 *  supported platforms.
 *
 *****************************************************************************/
#ifndef __BCMNEXUS_GFX_PLATFORMS_H__
#define __BCMNEXUS_GFX_PLATFORMS_H__

#include "bcmnexus.h"

/* This file contains a list of supported / tested platforms.
 * It is possible that a platform not listed in this file
 * will work but it hasn't been tested by the DFB or chip
 * refsw team. (Chips are arranged below in numerical order)
 */

/********** 7125 **********/

#define BCM7125B1_DEVICE_ID 0x71250011
#define BCM7125C0_DEVICE_ID 0x71250020

#if (BCHP_CHIP == 7125)
#if (BCHP_VER==BCHP_VER_B1)
#define BCM740X_DEVICE_ID BCM7125B1_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_C0)
#define BCM740X_DEVICE_ID BCM7125C0_DEVICE_ID
#else
#warning "Unknown 7125 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7231 **********/

#define BCM7231B0_DEVICE_ID 0x72310010
#define BCM7231B1_DEVICE_ID 0x72310011
#define BCM7231B2_DEVICE_ID 0x72310012

#if (BCHP_CHIP == 7231)
#if (BCHP_VER==BCHP_VER_B0)
#define BCM740X_DEVICE_ID BCM7231B0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B1)
#define BCM740X_DEVICE_ID BCM7231B1_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B2)
#define BCM740X_DEVICE_ID BCM7231B2_DEVICE_ID
#else
#warning "Unknown 7231 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7344 **********/

#define BCM7344B0_DEVICE_ID 0x73440010
#define BCM7344B1_DEVICE_ID 0x73440011
#define BCM7344B2_DEVICE_ID 0x73440012

#if (BCHP_CHIP == 7344)
#if (BCHP_VER==BCHP_VER_B0)
#define BCM740X_DEVICE_ID BCM7344B0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B1)
#define BCM740X_DEVICE_ID BCM7344B1_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B2)
#define BCM740X_DEVICE_ID BCM7344B2_DEVICE_ID
#else
#warning "Unknown 7344 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7346 **********/

#define BCM7346B0_DEVICE_ID 0x73460010
#define BCM7346B1_DEVICE_ID 0x73460011
#define BCM7346B2_DEVICE_ID 0x73460012

#if (BCHP_CHIP == 7346)
#if (BCHP_VER==BCHP_VER_B0)
#define BCM740X_DEVICE_ID BCM7346B0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B1)
#define BCM740X_DEVICE_ID BCM7346B1_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B2)
#define BCM740X_DEVICE_ID BCM7346B2_DEVICE_ID
#else
#warning "Unknown 7346 revision - you may experience unexpected behaviour"
#endif
#endif

/********** 7358 **********/

#define BCM7358A0_DEVICE_ID 0x73580000
#define BCM7358A1_DEVICE_ID 0x73580001

#if (BCHP_CHIP == 7358)
#if (BCHP_VER==BCHP_VER_A0)
#define BCM740X_DEVICE_ID BCM7358A0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_A1)
#define BCM740X_DEVICE_ID BCM7358A1_DEVICE_ID
#else
#warning "Unknown 7358 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7418 **********/

#define BCM7418B0_DEVICE_ID 0x74180010
#define BCM7418B1_DEVICE_ID 0x74180011

#if (BCHP_CHIP == 7418)
#if (BCHP_VER==BCHP_VER_B0)
#define BCM740X_DEVICE_ID BCM7418B0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B1)
#define BCM740X_DEVICE_ID BCM7418B1_DEVICE_ID
#else
#warning "Unknown 7418 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7420 **********/

#define BCM7420C0_DEVICE_ID 0x74200020
#define BCM7420C1_DEVICE_ID 0x74200021

#if (BCHP_CHIP == 7420)
#if (BCHP_VER==BCHP_VER_C0)
#define BCM740X_DEVICE_ID BCM7420C0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_C1)
#define BCM740X_DEVICE_ID BCM7420C1_DEVICE_ID
#else
#warning "Unknown 7420 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7425 **********/

#define BCM7425A1_DEVICE_ID 0x74250001
#define BCM7425B0_DEVICE_ID 0x74250010
#define BCM7425B1_DEVICE_ID 0x74250011
#define BCM7425B2_DEVICE_ID 0x74250012

#if (BCHP_CHIP == 7425)
#if (BCHP_VER==BCHP_VER_A1)
#define BCM740X_DEVICE_ID BCM7425A1_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B0)
#define BCM740X_DEVICE_ID BCM7425B0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B1)
#define BCM740X_DEVICE_ID BCM7425B1_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B2)
#define BCM740X_DEVICE_ID BCM7425B2_DEVICE_ID
#else
#warning "Unknown 7425 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7429 **********/

#define BCM7429A0_DEVICE_ID 0x74290000
#define BCM7429B0_DEVICE_ID 0x74290010

#if (BCHP_CHIP == 7429)
#if (BCHP_VER==BCHP_VER_A0)
#define BCM740X_DEVICE_ID BCM7429A0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B0)
#define BCM740X_DEVICE_ID BCM7429B0_DEVICE_ID
#else
#warning "Unknown 7429 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7435 **********/

#define BCM7435A0_DEVICE_ID 0x74350000
#define BCM7435B0_DEVICE_ID 0x74350010

#if (BCHP_CHIP == 7435)
#if (BCHP_VER == BCHP_VER_A0)
#define BCM740X_DEVICE_ID BCM7435A0_DEVICE_ID
#elif (BCHP_VER == BCHP_VER_B0)
#define BCM740X_DEVICE_ID BCM7435B0_DEVICE_ID
#else
#warning "Unknown 7435 revision - you may experience unexpected behaviour"
#endif
#endif


/********** 7552 **********/

#define BCM7552A0_DEVICE_ID 0x75520000
#define BCM7552B0_DEVICE_ID 0x75520010

#if (BCHP_CHIP == 7552)
#if (BCHP_VER==BCHP_VER_A0)
#define BCM740X_DEVICE_ID BCM7552A0_DEVICE_ID
#elif (BCHP_VER==BCHP_VER_B0)
#define BCM740X_DEVICE_ID BCM7552B0_DEVICE_ID
#else
#warning "Unknown 7552 revision - you may experience unexpected behaviour"
#endif
#endif

/* Generic fall through if chip ID isn't recognised */

#ifndef BCM740X_DEVICE_ID
#warning "Unknown / untested platform - you may experience unexpected behaviour"
#endif

#endif /*__BCMNEXUS_GFX_PLATFORMS_H__ */
