/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: platform_video_formats.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/2 $
 * $brcm_Date: 11/15/11 1:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_video_formats.h $
 * 
 * DirectFB_1_4_15_Port/2   11/15/11 1:51a christ
 * SW7346-556: DirectFB: Display fails to open on 7346 B1 using Phase 4.5
 *  refsw
 *
 * DirectFB_1_4_14_Port/1   9/22/11 4:43p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 *****************************************************************************/
#ifndef __PLATFORM_VIDEO_FORMATS_H__
#define __PLATFORM_VIDEO_FORMATS_H__

#include "platform_init.h"

const NEXUS_VideoFormat
DFB_Platform_P_DFBToNexusVideoFormatTable[] =
{
    [DFB_PlatformVideoFormat_eUnknown]                          = NEXUS_VideoFormat_eUnknown,
    [DFB_PlatformVideoFormat_eNtsc]                             = NEXUS_VideoFormat_eNtsc,
    [DFB_PlatformVideoFormat_eNtsc443]                          = NEXUS_VideoFormat_eNtsc443,
    [DFB_PlatformVideoFormat_eNtscJapan]                        = NEXUS_VideoFormat_eNtscJapan,
    [DFB_PlatformVideoFormat_ePalM]                             = NEXUS_VideoFormat_ePalM,
    [DFB_PlatformVideoFormat_ePalN]                             = NEXUS_VideoFormat_ePalN,
    [DFB_PlatformVideoFormat_ePalNc]                            = NEXUS_VideoFormat_ePalNc,
    [DFB_PlatformVideoFormat_ePalB]                             = NEXUS_VideoFormat_ePalB,
    [DFB_PlatformVideoFormat_ePalB1]                            = NEXUS_VideoFormat_ePalB1,
    [DFB_PlatformVideoFormat_ePalD]                             = NEXUS_VideoFormat_ePalD,
    [DFB_PlatformVideoFormat_ePalD1]                            = NEXUS_VideoFormat_ePalD1,
    [DFB_PlatformVideoFormat_ePalDK1]                           = NEXUS_VideoFormat_ePalDK1,
    [DFB_PlatformVideoFormat_ePalDK2]                           = NEXUS_VideoFormat_ePalDK2,
    [DFB_PlatformVideoFormat_ePalDK3]                           = NEXUS_VideoFormat_ePalDK3,
    [DFB_PlatformVideoFormat_ePalG]                             = NEXUS_VideoFormat_ePalG,
    [DFB_PlatformVideoFormat_ePalH]                             = NEXUS_VideoFormat_ePalH,
    [DFB_PlatformVideoFormat_ePalK]                             = NEXUS_VideoFormat_ePalK,
    [DFB_PlatformVideoFormat_ePalI]                             = NEXUS_VideoFormat_ePalI,
    [DFB_PlatformVideoFormat_ePal60hz]                          = NEXUS_VideoFormat_ePal60hz,
    [DFB_PlatformVideoFormat_eSecamL]                           = NEXUS_VideoFormat_eSecamL,
    [DFB_PlatformVideoFormat_eSecamB]                           = NEXUS_VideoFormat_eSecamB,
    [DFB_PlatformVideoFormat_eSecamG]                           = NEXUS_VideoFormat_eSecamG,
    [DFB_PlatformVideoFormat_eSecamD]                           = NEXUS_VideoFormat_eSecamD,
    [DFB_PlatformVideoFormat_eSecamK]                           = NEXUS_VideoFormat_eSecamK,
    [DFB_PlatformVideoFormat_eSecamH]                           = NEXUS_VideoFormat_eSecamH,
    [DFB_PlatformVideoFormat_e480p]                             = NEXUS_VideoFormat_e480p,
    [DFB_PlatformVideoFormat_e576p]                             = NEXUS_VideoFormat_e576p,
    [DFB_PlatformVideoFormat_e1080i]                            = NEXUS_VideoFormat_e1080i,
    [DFB_PlatformVideoFormat_e1080i50hz]                        = NEXUS_VideoFormat_e1080i50hz,
    [DFB_PlatformVideoFormat_e1080p24hz]                        = NEXUS_VideoFormat_e1080p24hz,
    [DFB_PlatformVideoFormat_e1080p25hz]                        = NEXUS_VideoFormat_e1080p25hz,
    [DFB_PlatformVideoFormat_e1080p30hz]                        = NEXUS_VideoFormat_e1080p30hz,
    [DFB_PlatformVideoFormat_e1080p50hz]                        = NEXUS_VideoFormat_e1080p50hz,
    [DFB_PlatformVideoFormat_e1080p60hz]                        = NEXUS_VideoFormat_e1080p60hz,
    [DFB_PlatformVideoFormat_e1250i50hz]                        = NEXUS_VideoFormat_e1250i50hz,
    [DFB_PlatformVideoFormat_e720p]                             = NEXUS_VideoFormat_e720p,
    [DFB_PlatformVideoFormat_e720p50hz]                         = NEXUS_VideoFormat_e720p50hz,
    [DFB_PlatformVideoFormat_e720p24hz]                         = NEXUS_VideoFormat_e720p24hz,
    [DFB_PlatformVideoFormat_e720p25hz]                         = NEXUS_VideoFormat_e720p25hz,
    [DFB_PlatformVideoFormat_e720p30hz]                         = NEXUS_VideoFormat_e720p30hz,
    [DFB_PlatformVideoFormat_e240p60hz]                         = NEXUS_VideoFormat_e240p60hz,
    [DFB_PlatformVideoFormat_e288p50hz]                         = NEXUS_VideoFormat_e288p50hz,
    [DFB_PlatformVideoFormat_e1440x480p60hz]                    = NEXUS_VideoFormat_e1440x480p60hz,
    [DFB_PlatformVideoFormat_e1440x576p50hz]                    = NEXUS_VideoFormat_e1440x576p50hz,
    [DFB_PlatformVideoFormat_eCustomer1440x240p60hz]            = NEXUS_VideoFormat_eCustomer1440x240p60hz,
    [DFB_PlatformVideoFormat_eCustomer1440x288p50hz]            = NEXUS_VideoFormat_eCustomer1440x288p50hz,
    [DFB_PlatformVideoFormat_eCustomer1366x768p60hz]            = NEXUS_VideoFormat_eCustomer1366x768p60hz,
    [DFB_PlatformVideoFormat_eCustomer1366x768p50hz]            = NEXUS_VideoFormat_eCustomer1366x768p50hz,
    [DFB_PlatformVideoFormat_eVesa640x480p60hz]                 = NEXUS_VideoFormat_eVesa640x480p60hz,

    /* DVI VESA mode for computer monitors */
    [DFB_PlatformVideoFormat_eVesa800x600p60hz]                 = NEXUS_VideoFormat_eVesa800x600p60hz,
    [DFB_PlatformVideoFormat_eVesa1024x768p60hz]                = NEXUS_VideoFormat_eVesa1024x768p60hz,
    [DFB_PlatformVideoFormat_eVesa1280x768p60hz]                = NEXUS_VideoFormat_eVesa1280x768p60hz,
    [DFB_PlatformVideoFormat_eVesa1280x768p60hzRed]             = NEXUS_VideoFormat_eVesa1280x768p60hzRed,
    [DFB_PlatformVideoFormat_eVesa1280x720p50hz]                = NEXUS_VideoFormat_eVesa1280x720p50hz,
    [DFB_PlatformVideoFormat_eVesa1280x720p60hz]                = NEXUS_VideoFormat_eVesa1280x720p60hz,
    [DFB_PlatformVideoFormat_eVesa1280x720pReducedBlank]        = NEXUS_VideoFormat_eVesa1280x720pReducedBlank,
    [DFB_PlatformVideoFormat_eVesa640x350p60hz]                 = NEXUS_VideoFormat_eVesa640x350p60hz,
    [DFB_PlatformVideoFormat_eVesa640x350p70hz]                 = NEXUS_VideoFormat_eVesa640x350p70hz,
    [DFB_PlatformVideoFormat_eVesa640x350p72hz]                 = NEXUS_VideoFormat_eVesa640x350p72hz,
    [DFB_PlatformVideoFormat_eVesa640x350p75hz]                 = NEXUS_VideoFormat_eVesa640x350p75hz,
    [DFB_PlatformVideoFormat_eVesa640x350p85hz]                 = NEXUS_VideoFormat_eVesa640x350p85hz,
    [DFB_PlatformVideoFormat_eVesa640x400p60hz]                 = NEXUS_VideoFormat_eVesa640x400p60hz,
    [DFB_PlatformVideoFormat_eVesa640x400p70hz]                 = NEXUS_VideoFormat_eVesa640x400p70hz,
    [DFB_PlatformVideoFormat_eVesa640x400p72hz]                 = NEXUS_VideoFormat_eVesa640x400p72hz,
    [DFB_PlatformVideoFormat_eVesa640x400p75hz]                 = NEXUS_VideoFormat_eVesa640x400p75hz,
    [DFB_PlatformVideoFormat_eVesa640x400p85hz]                 = NEXUS_VideoFormat_eVesa640x400p85hz,
    [DFB_PlatformVideoFormat_eVesa640x480p66hz]                 = NEXUS_VideoFormat_eVesa640x480p66hz,
    [DFB_PlatformVideoFormat_eVesa640x480p70hz]                 = NEXUS_VideoFormat_eVesa640x480p70hz,
    [DFB_PlatformVideoFormat_eVesa640x480p72hz]                 = NEXUS_VideoFormat_eVesa640x480p72hz,
    [DFB_PlatformVideoFormat_eVesa640x480p75hz]                 = NEXUS_VideoFormat_eVesa640x480p75hz,
    [DFB_PlatformVideoFormat_eVesa640x480p85hz]                 = NEXUS_VideoFormat_eVesa640x480p85hz,
    [DFB_PlatformVideoFormat_eVesa720x400p60hz]                 = NEXUS_VideoFormat_eVesa720x400p60hz,
    [DFB_PlatformVideoFormat_eVesa720x400p70hz]                 = NEXUS_VideoFormat_eVesa720x400p70hz,
    [DFB_PlatformVideoFormat_eVesa720x400p72hz]                 = NEXUS_VideoFormat_eVesa720x400p72hz,
    [DFB_PlatformVideoFormat_eVesa720x400p75hz]                 = NEXUS_VideoFormat_eVesa720x400p75hz,
    [DFB_PlatformVideoFormat_eVesa720x400p85hz]                 = NEXUS_VideoFormat_eVesa720x400p85hz,
    [DFB_PlatformVideoFormat_eVesa800x600p56hz]                 = NEXUS_VideoFormat_eVesa800x600p56hz,
    [DFB_PlatformVideoFormat_eVesa800x600p59hzRed]              = NEXUS_VideoFormat_eVesa800x600p59hzRed,
    [DFB_PlatformVideoFormat_eVesa800x600p70hz]                 = NEXUS_VideoFormat_eVesa800x600p70hz,
    [DFB_PlatformVideoFormat_eVesa800x600p72hz]                 = NEXUS_VideoFormat_eVesa800x600p72hz,
    [DFB_PlatformVideoFormat_eVesa800x600p75hz]                 = NEXUS_VideoFormat_eVesa800x600p75hz,
    [DFB_PlatformVideoFormat_eVesa800x600p85hz]                 = NEXUS_VideoFormat_eVesa800x600p85hz,
    [DFB_PlatformVideoFormat_eVesa848x480p60hz]                 = NEXUS_VideoFormat_eVesa848x480p60hz,
    [DFB_PlatformVideoFormat_eVesa1024x768p66hz]                = NEXUS_VideoFormat_eVesa1024x768p66hz,
    [DFB_PlatformVideoFormat_eVesa1024x768p70hz]                = NEXUS_VideoFormat_eVesa1024x768p70hz,
    [DFB_PlatformVideoFormat_eVesa1024x768p72hz]                = NEXUS_VideoFormat_eVesa1024x768p72hz,
    [DFB_PlatformVideoFormat_eVesa1024x768p75hz]                = NEXUS_VideoFormat_eVesa1024x768p75hz,
    [DFB_PlatformVideoFormat_eVesa1024x768p85hz]                = NEXUS_VideoFormat_eVesa1024x768p85hz,
    [DFB_PlatformVideoFormat_eVesa1064x600p60hz]                = NEXUS_VideoFormat_eVesa1064x600p60hz,
    [DFB_PlatformVideoFormat_eVesa1280x720p70hz]                = NEXUS_VideoFormat_eVesa1280x720p70hz,
    [DFB_PlatformVideoFormat_eVesa1280x720p72hz]                = NEXUS_VideoFormat_eVesa1280x720p72hz,
    [DFB_PlatformVideoFormat_eVesa1280x720p75hz]                = NEXUS_VideoFormat_eVesa1280x720p75hz,
    [DFB_PlatformVideoFormat_eVesa1280x720p85hz]                = NEXUS_VideoFormat_eVesa1280x720p85hz,
    [DFB_PlatformVideoFormat_eVesa1024x768i87hz]                = NEXUS_VideoFormat_eVesa1024x768i87hz,
    [DFB_PlatformVideoFormat_eVesa1152x864p75hz]                = NEXUS_VideoFormat_eVesa1152x864p75hz,
    [DFB_PlatformVideoFormat_eVesa1280x768p75hz]                = NEXUS_VideoFormat_eVesa1280x768p75hz,
    [DFB_PlatformVideoFormat_eVesa1280x768p85hz]                = NEXUS_VideoFormat_eVesa1280x768p85hz,
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X800P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1280x800p_60Hz]               = NEXUS_VideoFormat_eVesa1280x800p_60Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1280x800p_60Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
    [DFB_PlatformVideoFormat_eVesa1280x960p60hz]                = NEXUS_VideoFormat_eVesa1280x960p60hz,
    [DFB_PlatformVideoFormat_eVesa1280x960p85hz]                = NEXUS_VideoFormat_eVesa1280x960p85hz,
    [DFB_PlatformVideoFormat_eVesa1280x1024p60hz]               = NEXUS_VideoFormat_eVesa1280x1024p60hz,
    [DFB_PlatformVideoFormat_eVesa1280x1024p69hz]               = NEXUS_VideoFormat_eVesa1280x1024p69hz,
    [DFB_PlatformVideoFormat_eVesa1280x1024p75hz]               = NEXUS_VideoFormat_eVesa1280x1024p75hz,
    [DFB_PlatformVideoFormat_eVesa1280x1024p85hz]               = NEXUS_VideoFormat_eVesa1280x1024p85hz,
    [DFB_PlatformVideoFormat_eVesa832x624p75hz]                 = NEXUS_VideoFormat_eVesa832x624p75hz,
    [DFB_PlatformVideoFormat_eVesa1360x768p60hz]                = NEXUS_VideoFormat_eVesa1360x768p60hz,
    [DFB_PlatformVideoFormat_eVesa1366x768p60hz]                = NEXUS_VideoFormat_eVesa1366x768p60hz,
    [DFB_PlatformVideoFormat_eVesa1400x1050p60hz]               = NEXUS_VideoFormat_eVesa1400x1050p60hz,
    [DFB_PlatformVideoFormat_eVesa1400x1050p60hzReducedBlank]   = NEXUS_VideoFormat_eVesa1400x1050p60hzReducedBlank,
    [DFB_PlatformVideoFormat_eVesa1400x1050p75hz]               = NEXUS_VideoFormat_eVesa1400x1050p75hz,
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1440X900P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1440x900p60hz]                = NEXUS_VideoFormat_eVesa1440x900p60hz,
#else
    [DFB_PlatformVideoFormat_eVesa1440x900p60hz]                = NEXUS_VideoFormat_eUnknown,
#endif
    [DFB_PlatformVideoFormat_eVesa1600x1200p60hz]               = NEXUS_VideoFormat_eVesa1600x1200p60hz,
    [DFB_PlatformVideoFormat_eVesa1920x1080p60hzReducedBlank]   = NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank,
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1680X1050P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1680x1050p_60Hz]              = NEXUS_VideoFormat_eVesa1680x1050p_60Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1680x1050p_60Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X800P_60HZ_RED_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1280x800p_60Hz_Red]           = NEXUS_VideoFormat_eVesa1280x800p_60Hz_Red,
#else
    [DFB_PlatformVideoFormat_eVesa1280x800p_60Hz_Red]           = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1600X1200P_75HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1600x1200p_75Hz]              = NEXUS_VideoFormat_eVesa1600x1200p_75Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1600x1200p_75Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1600X900P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1600x900p_60Hz_Red]           = NEXUS_VideoFormat_eVesa1600x900p_60Hz_Red,
#else
    [DFB_PlatformVideoFormat_eVesa1600x900p_60Hz_Red]           = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1680X1050P_60HZ_RED_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1680x1050p_60Hz_Red]          = NEXUS_VideoFormat_eVesa1680x1050p_60Hz_Red,
#else
    [DFB_PlatformVideoFormat_eVesa1680x1050p_60Hz_Red]          = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1920X1200P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1920x1200p_60Hz]              = NEXUS_VideoFormat_eVesa1920x1200p_60Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1920x1200p_60Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1920X1200P_60HZ_RED_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1920x1200p_60Hz_Red]          = NEXUS_VideoFormat_eVesa1920x1200p_60Hz_Red,
#else
    [DFB_PlatformVideoFormat_eVesa1920x1200p_60Hz_Red]          = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X720P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1152x720p_60Hz]               = NEXUS_VideoFormat_eVesa1152x720p_60Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1152x720p_60Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X720P_75HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1152x720p_75Hz]               = NEXUS_VideoFormat_eVesa1152x720p_75Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1152x720p_75Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X720P_85HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1152x720p_85Hz]               = NEXUS_VideoFormat_eVesa1152x720p_85Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1152x720p_85Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X864P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1152x864p_60Hz]               = NEXUS_VideoFormat_eVesa1152x864p_60Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1152x864p_60Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X864P_85HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1152x864p_85Hz]               = NEXUS_VideoFormat_eVesa1152x864p_85Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1152x864p_85Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X870P_75HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1152x870p_75Hz]               = NEXUS_VideoFormat_eVesa1152x870p_75Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1152x870p_75Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X900P_66HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1152x900p_66Hz]               = NEXUS_VideoFormat_eVesa1152x900p_66Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1152x900p_66Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X900P_76HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1152x900p_76Hz]               = NEXUS_VideoFormat_eVesa1152x900p_76Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1152x900p_76Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1170X584P_50HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1170x584p_50Hz]               = NEXUS_VideoFormat_eVesa1170x584p_50Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1170x584p_50Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_70HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1280x1024p_70Hz]              = NEXUS_VideoFormat_eVesa1280x1024p_70Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1280x1024p_70Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_72HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1280x1024p_72Hz]              = NEXUS_VideoFormat_eVesa1280x1024p_72Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1280x1024p_72Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_76HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1280x1024p_76Hz]              = NEXUS_VideoFormat_eVesa1280x1024p_76Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1280x1024p_76Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X768P_50HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1280x768p_50Hz]               = NEXUS_VideoFormat_eVesa1280x768p_50Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1280x768p_50Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X960P_75HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1280x960p_75Hz]               = NEXUS_VideoFormat_eVesa1280x960p_75Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1280x960p_75Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1600X1024P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1600x1024p_60Hz]              = NEXUS_VideoFormat_eVesa1600x1024p_60Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1600x1024p_60Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1600X1024P_76HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1600x1024p_76Hz]              = NEXUS_VideoFormat_eVesa1600x1024p_76Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1600x1024p_76Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1728X1080P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa1728x1080p_60Hz]              = NEXUS_VideoFormat_eVesa1728x1080p_60Hz,
#else
    [DFB_PlatformVideoFormat_eVesa1728x1080p_60Hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA800X600P_100HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa800x600p_100Hz]               = NEXUS_VideoFormat_eVesa800x600p_100Hz,
#else
    [DFB_PlatformVideoFormat_eVesa800x600p_100Hz]               = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA800X600P_90HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa800x600p_90Hz]                = NEXUS_VideoFormat_eVesa800x600p_90Hz,
#else
    [DFB_PlatformVideoFormat_eVesa800x600p_90Hz]                = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA848X480P_75HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa848x480p_75Hz]                = NEXUS_VideoFormat_eVesa848x480p_75Hz,
#else
    [DFB_PlatformVideoFormat_eVesa848x480p_75Hz]                = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA848X480P_85HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa848x480p_85Hz]                = NEXUS_VideoFormat_eVesa848x480p_85Hz,
#else
    [DFB_PlatformVideoFormat_eVesa848x480p_85Hz]                = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA852X480P_60HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eVesa852x480p_60Hz]                = NEXUS_VideoFormat_eVesa852x480p_60Hz,
#else
    [DFB_PlatformVideoFormat_eVesa852x480p_60Hz]                = NEXUS_VideoFormat_eUnknown,
#endif

#ifdef BCMNEXUS_VIDEOFORMAT_720P_3DOU_AS_SUPPORTED
    [DFB_PlatformVideoFormat_e720p_3DOU_AS]                     = NEXUS_VideoFormat_e720p_3DOU_AS,
#else
    [DFB_PlatformVideoFormat_e720p_3DOU_AS]                     = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_720P50HZ_3DOU_AS_SUPPORTED
    [DFB_PlatformVideoFormat_e720p50hz_3DOU_AS]                 = NEXUS_VideoFormat_e720p50hz_3DOU_AS,
#else
    [DFB_PlatformVideoFormat_e720p50hz_3DOU_AS]                 = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_720P30HZ_3DOU_AS_SUPPORTED
    [DFB_PlatformVideoFormat_e720p30hz_3DOU_AS]                 = NEXUS_VideoFormat_e720p30hz_3DOU_AS,
#else
    [DFB_PlatformVideoFormat_e720p30hz_3DOU_AS]                 = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_720P24HZ_3DOU_AS_SUPPORTED
    [DFB_PlatformVideoFormat_e720p24hz_3DOU_AS]                 = NEXUS_VideoFormat_e720p24hz_3DOU_AS,
#else
    [DFB_PlatformVideoFormat_e720p24hz_3DOU_AS]                 = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_1080P24HZ_3DOU_AS_SUPPORTED
        [DFB_PlatformVideoFormat_e1080p24hz_3DOU_AS]                = NEXUS_VideoFormat_e1080p24hz_3DOU_AS,
#else
        [DFB_PlatformVideoFormat_e1080p24hz_3DOU_AS]                = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_1080P30HZ_3DOU_AS_SUPPORTED
    [DFB_PlatformVideoFormat_e1080p30hz_3DOU_AS]                = NEXUS_VideoFormat_e1080p30hz_3DOU_AS,
#else
    [DFB_PlatformVideoFormat_e1080p30hz_3DOU_AS]                = NEXUS_VideoFormat_eUnknown,
#endif

    /* dynamics: custom format (see Nexus for more detail) */
    [DFB_PlatformVideoFormat_eCustom0]                          = NEXUS_VideoFormat_eCustom0,
    [DFB_PlatformVideoFormat_eCustom1]                          = NEXUS_VideoFormat_eCustom1,
    [DFB_PlatformVideoFormat_eCustom2]                          = NEXUS_VideoFormat_eCustom2,

    /* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
    [DFB_PlatformVideoFormat_eCustom1920x2160i_48Hz]            = NEXUS_VideoFormat_eCustom1920x2160i_48Hz,

    /* Legacy 3D timing and display formats */
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_SUPPORTED
    [DFB_PlatformVideoFormat_eCustom_3D_720p]                   = NEXUS_VideoFormat_eCustom_3D_720p,
#else
    [DFB_PlatformVideoFormat_eCustom_3D_720p]                   = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_50HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eCustom_3D_720p_50hz]              = NEXUS_VideoFormat_eCustom_3D_720p_50hz,
#else
    [DFB_PlatformVideoFormat_eCustom_3D_720p_50hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_30HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eCustom_3D_720p_30hz]              = NEXUS_VideoFormat_eCustom_3D_720p_30hz,
#else
    [DFB_PlatformVideoFormat_eCustom_3D_720p_30hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_24HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eCustom_3D_720p_24hz]              = NEXUS_VideoFormat_eCustom_3D_720p_24hz,
#else
    [DFB_PlatformVideoFormat_eCustom_3D_720p_24hz]              = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_1080P_24HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eCustom_3D_1080p_24hz]             = NEXUS_VideoFormat_eCustom_3D_1080p_24hz,
#else
    [DFB_PlatformVideoFormat_eCustom_3D_1080p_24hz]             = NEXUS_VideoFormat_eUnknown,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_1080P_30HZ_SUPPORTED
    [DFB_PlatformVideoFormat_eCustom_3D_1080p_30hz]             = NEXUS_VideoFormat_eCustom_3D_1080p_30hz,
#else
    [DFB_PlatformVideoFormat_eCustom_3D_1080p_30hz]             = NEXUS_VideoFormat_eUnknown,
#endif
};

const NEXUS_VideoFormat
DFB_Platform_P_NexusToDFBVideoFormatTable[] =
{
    [NEXUS_VideoFormat_eUnknown]                                = DFB_PlatformVideoFormat_eUnknown,
    [NEXUS_VideoFormat_eNtsc]                                   = DFB_PlatformVideoFormat_eNtsc,
    [NEXUS_VideoFormat_eNtsc443]                                = DFB_PlatformVideoFormat_eNtsc443,
    [NEXUS_VideoFormat_eNtscJapan]                              = DFB_PlatformVideoFormat_eNtscJapan,
    [NEXUS_VideoFormat_ePalM]                                   = DFB_PlatformVideoFormat_ePalM,
    [NEXUS_VideoFormat_ePalN]                                   = DFB_PlatformVideoFormat_ePalN,
    [NEXUS_VideoFormat_ePalNc]                                  = DFB_PlatformVideoFormat_ePalNc,
    [NEXUS_VideoFormat_ePalB]                                   = DFB_PlatformVideoFormat_ePalB,
    [NEXUS_VideoFormat_ePalB1]                                  = DFB_PlatformVideoFormat_ePalB1,
    [NEXUS_VideoFormat_ePalD]                                   = DFB_PlatformVideoFormat_ePalD,
    [NEXUS_VideoFormat_ePalDK1]                                 = DFB_PlatformVideoFormat_ePalDK1,
    [NEXUS_VideoFormat_ePalDK2]                                 = DFB_PlatformVideoFormat_ePalDK2,
    [NEXUS_VideoFormat_ePalDK3]                                 = DFB_PlatformVideoFormat_ePalDK3,
    [NEXUS_VideoFormat_ePalG]                                   = DFB_PlatformVideoFormat_ePalG,
    [NEXUS_VideoFormat_ePalH]                                   = DFB_PlatformVideoFormat_ePalH,
    [NEXUS_VideoFormat_ePalK]                                   = DFB_PlatformVideoFormat_ePalK,
    [NEXUS_VideoFormat_ePalI]                                   = DFB_PlatformVideoFormat_ePalI,
    [NEXUS_VideoFormat_ePal60hz]                                = DFB_PlatformVideoFormat_ePal60hz,
    [NEXUS_VideoFormat_eSecamL]                                 = DFB_PlatformVideoFormat_eSecamL,
    [NEXUS_VideoFormat_eSecamB]                                 = DFB_PlatformVideoFormat_eSecamB,
    [NEXUS_VideoFormat_eSecamG]                                 = DFB_PlatformVideoFormat_eSecamG,
    [NEXUS_VideoFormat_eSecamD]                                 = DFB_PlatformVideoFormat_eSecamD,
    [NEXUS_VideoFormat_eSecamK]                                 = DFB_PlatformVideoFormat_eSecamK,
    [NEXUS_VideoFormat_eSecamH]                                 = DFB_PlatformVideoFormat_eSecamH,
    [NEXUS_VideoFormat_e480p]                                   = DFB_PlatformVideoFormat_e480p,
    [NEXUS_VideoFormat_e576p]                                   = DFB_PlatformVideoFormat_e576p,
    [NEXUS_VideoFormat_e1080i]                                  = DFB_PlatformVideoFormat_e1080i,
    [NEXUS_VideoFormat_e1080i50hz]                              = DFB_PlatformVideoFormat_e1080i50hz,
    [NEXUS_VideoFormat_e1080p24hz]                              = DFB_PlatformVideoFormat_e1080p24hz,
    [NEXUS_VideoFormat_e1080p25hz]                              = DFB_PlatformVideoFormat_e1080p25hz,
    [NEXUS_VideoFormat_e1080p30hz]                              = DFB_PlatformVideoFormat_e1080p30hz,
    [NEXUS_VideoFormat_e1080p50hz]                              = DFB_PlatformVideoFormat_e1080p50hz,
    [NEXUS_VideoFormat_e1080p60hz]                              = DFB_PlatformVideoFormat_e1080p60hz,
    [NEXUS_VideoFormat_e1250i50hz]                              = DFB_PlatformVideoFormat_e1250i50hz,
    [NEXUS_VideoFormat_e720p]                                   = DFB_PlatformVideoFormat_e720p,
    [NEXUS_VideoFormat_e720p50hz]                               = DFB_PlatformVideoFormat_e720p50hz,
    [NEXUS_VideoFormat_e720p24hz]                               = DFB_PlatformVideoFormat_e720p24hz,
    [NEXUS_VideoFormat_e720p25hz]                               = DFB_PlatformVideoFormat_e720p25hz,
    [NEXUS_VideoFormat_e720p30hz]                               = DFB_PlatformVideoFormat_e720p30hz,
    [NEXUS_VideoFormat_e240p60hz]                               = DFB_PlatformVideoFormat_e240p60hz,
    [NEXUS_VideoFormat_e288p50hz]                               = DFB_PlatformVideoFormat_e288p50hz,
    [NEXUS_VideoFormat_e1440x480p60hz]                          = DFB_PlatformVideoFormat_e1440x480p60hz,
    [NEXUS_VideoFormat_e1440x576p50hz]                          = DFB_PlatformVideoFormat_e1440x576p50hz,
    [NEXUS_VideoFormat_eCustomer1440x240p60hz]                  = DFB_PlatformVideoFormat_eCustomer1440x240p60hz,
    [NEXUS_VideoFormat_eCustomer1440x288p50hz]                  = DFB_PlatformVideoFormat_eCustomer1440x288p50hz,
    [NEXUS_VideoFormat_eCustomer1366x768p60hz]                  = DFB_PlatformVideoFormat_eCustomer1366x768p60hz,
    [NEXUS_VideoFormat_eCustomer1366x768p50hz]                  = DFB_PlatformVideoFormat_eCustomer1366x768p50hz,
    [NEXUS_VideoFormat_eVesa640x480p60hz]                       = DFB_PlatformVideoFormat_eVesa640x480p60hz,

    /* DVI VESA mode for computer monitors */
    [NEXUS_VideoFormat_eVesa800x600p60hz]                       = DFB_PlatformVideoFormat_eVesa800x600p60hz,
    [NEXUS_VideoFormat_eVesa1024x768p60hz]                      = DFB_PlatformVideoFormat_eVesa1024x768p60hz,
    [NEXUS_VideoFormat_eVesa1280x768p60hz]                      = DFB_PlatformVideoFormat_eVesa1280x768p60hz,
    [NEXUS_VideoFormat_eVesa1280x768p60hzRed]                   = DFB_PlatformVideoFormat_eVesa1280x768p60hzRed,
    [NEXUS_VideoFormat_eVesa1280x720p50hz]                      = DFB_PlatformVideoFormat_eVesa1280x720p50hz,
    [NEXUS_VideoFormat_eVesa1280x720p60hz]                      = DFB_PlatformVideoFormat_eVesa1280x720p60hz,
    [NEXUS_VideoFormat_eVesa1280x720pReducedBlank]              = DFB_PlatformVideoFormat_eVesa1280x720pReducedBlank,
    [NEXUS_VideoFormat_eVesa640x350p60hz]                       = DFB_PlatformVideoFormat_eVesa640x350p60hz,
    [NEXUS_VideoFormat_eVesa640x350p70hz]                       = DFB_PlatformVideoFormat_eVesa640x350p70hz,
    [NEXUS_VideoFormat_eVesa640x350p72hz]                       = DFB_PlatformVideoFormat_eVesa640x350p72hz,
    [NEXUS_VideoFormat_eVesa640x350p75hz]                       = DFB_PlatformVideoFormat_eVesa640x350p75hz,
    [NEXUS_VideoFormat_eVesa640x350p85hz]                       = DFB_PlatformVideoFormat_eVesa640x350p85hz,
    [NEXUS_VideoFormat_eVesa640x400p60hz]                       = DFB_PlatformVideoFormat_eVesa640x400p60hz,
    [NEXUS_VideoFormat_eVesa640x400p70hz]                       = DFB_PlatformVideoFormat_eVesa640x400p70hz,
    [NEXUS_VideoFormat_eVesa640x400p72hz]                       = DFB_PlatformVideoFormat_eVesa640x400p72hz,
    [NEXUS_VideoFormat_eVesa640x400p75hz]                       = DFB_PlatformVideoFormat_eVesa640x400p75hz,
    [NEXUS_VideoFormat_eVesa640x400p85hz]                       = DFB_PlatformVideoFormat_eVesa640x400p85hz,
    [NEXUS_VideoFormat_eVesa640x480p66hz]                       = DFB_PlatformVideoFormat_eVesa640x480p66hz,
    [NEXUS_VideoFormat_eVesa640x480p70hz]                       = DFB_PlatformVideoFormat_eVesa640x480p70hz,
    [NEXUS_VideoFormat_eVesa640x480p72hz]                       = DFB_PlatformVideoFormat_eVesa640x480p72hz,
    [NEXUS_VideoFormat_eVesa640x480p75hz]                       = DFB_PlatformVideoFormat_eVesa640x480p75hz,
    [NEXUS_VideoFormat_eVesa640x480p85hz]                       = DFB_PlatformVideoFormat_eVesa640x480p85hz,
    [NEXUS_VideoFormat_eVesa720x400p60hz]                       = DFB_PlatformVideoFormat_eVesa720x400p60hz,
    [NEXUS_VideoFormat_eVesa720x400p70hz]                       = DFB_PlatformVideoFormat_eVesa720x400p70hz,
    [NEXUS_VideoFormat_eVesa720x400p72hz]                       = DFB_PlatformVideoFormat_eVesa720x400p72hz,
    [NEXUS_VideoFormat_eVesa720x400p75hz]                       = DFB_PlatformVideoFormat_eVesa720x400p75hz,
    [NEXUS_VideoFormat_eVesa720x400p85hz]                       = DFB_PlatformVideoFormat_eVesa720x400p85hz,
    [NEXUS_VideoFormat_eVesa800x600p56hz]                       = DFB_PlatformVideoFormat_eVesa800x600p56hz,
    [NEXUS_VideoFormat_eVesa800x600p59hzRed]                    = DFB_PlatformVideoFormat_eVesa800x600p59hzRed,
    [NEXUS_VideoFormat_eVesa800x600p70hz]                       = DFB_PlatformVideoFormat_eVesa800x600p70hz,
    [NEXUS_VideoFormat_eVesa800x600p72hz]                       = DFB_PlatformVideoFormat_eVesa800x600p72hz,
    [NEXUS_VideoFormat_eVesa800x600p75hz]                       = DFB_PlatformVideoFormat_eVesa800x600p75hz,
    [NEXUS_VideoFormat_eVesa800x600p85hz]                       = DFB_PlatformVideoFormat_eVesa800x600p85hz,
    [NEXUS_VideoFormat_eVesa848x480p60hz]                       = DFB_PlatformVideoFormat_eVesa848x480p60hz,
    [NEXUS_VideoFormat_eVesa1024x768p66hz]                      = DFB_PlatformVideoFormat_eVesa1024x768p66hz,
    [NEXUS_VideoFormat_eVesa1024x768p70hz]                      = DFB_PlatformVideoFormat_eVesa1024x768p70hz,
    [NEXUS_VideoFormat_eVesa1024x768p72hz]                      = DFB_PlatformVideoFormat_eVesa1024x768p72hz,
    [NEXUS_VideoFormat_eVesa1024x768p75hz]                      = DFB_PlatformVideoFormat_eVesa1024x768p75hz,
    [NEXUS_VideoFormat_eVesa1024x768p85hz]                      = DFB_PlatformVideoFormat_eVesa1024x768p85hz,
    [NEXUS_VideoFormat_eVesa1064x600p60hz]                      = DFB_PlatformVideoFormat_eVesa1064x600p60hz,
    [NEXUS_VideoFormat_eVesa1280x720p70hz]                      = DFB_PlatformVideoFormat_eVesa1280x720p70hz,
    [NEXUS_VideoFormat_eVesa1280x720p72hz]                      = DFB_PlatformVideoFormat_eVesa1280x720p72hz,
    [NEXUS_VideoFormat_eVesa1280x720p75hz]                      = DFB_PlatformVideoFormat_eVesa1280x720p75hz,
    [NEXUS_VideoFormat_eVesa1280x720p85hz]                      = DFB_PlatformVideoFormat_eVesa1280x720p85hz,
    [NEXUS_VideoFormat_eVesa1024x768i87hz]                      = DFB_PlatformVideoFormat_eVesa1024x768i87hz,
    [NEXUS_VideoFormat_eVesa1152x864p75hz]                      = DFB_PlatformVideoFormat_eVesa1152x864p75hz,
    [NEXUS_VideoFormat_eVesa1280x768p75hz]                      = DFB_PlatformVideoFormat_eVesa1280x768p75hz,
    [NEXUS_VideoFormat_eVesa1280x768p85hz]                      = DFB_PlatformVideoFormat_eVesa1280x768p85hz,
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X800P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1280x800p_60Hz]                     = DFB_PlatformVideoFormat_eVesa1280x800p_60Hz,
#endif
    [NEXUS_VideoFormat_eVesa1280x960p60hz]                      = DFB_PlatformVideoFormat_eVesa1280x960p60hz,
    [NEXUS_VideoFormat_eVesa1280x960p85hz]                      = DFB_PlatformVideoFormat_eVesa1280x960p85hz,
    [NEXUS_VideoFormat_eVesa1280x1024p60hz]                     = DFB_PlatformVideoFormat_eVesa1280x1024p60hz,
    [NEXUS_VideoFormat_eVesa1280x1024p69hz]                     = DFB_PlatformVideoFormat_eVesa1280x1024p69hz,
    [NEXUS_VideoFormat_eVesa1280x1024p75hz]                     = DFB_PlatformVideoFormat_eVesa1280x1024p75hz,
    [NEXUS_VideoFormat_eVesa1280x1024p85hz]                     = DFB_PlatformVideoFormat_eVesa1280x1024p85hz,
    [NEXUS_VideoFormat_eVesa832x624p75hz]                       = DFB_PlatformVideoFormat_eVesa832x624p75hz,
    [NEXUS_VideoFormat_eVesa1360x768p60hz]                      = DFB_PlatformVideoFormat_eVesa1360x768p60hz,
    [NEXUS_VideoFormat_eVesa1366x768p60hz]                      = DFB_PlatformVideoFormat_eVesa1366x768p60hz,
    [NEXUS_VideoFormat_eVesa1400x1050p60hz]                     = DFB_PlatformVideoFormat_eVesa1400x1050p60hz,
    [NEXUS_VideoFormat_eVesa1400x1050p60hzReducedBlank]         = DFB_PlatformVideoFormat_eVesa1400x1050p60hzReducedBlank,
    [NEXUS_VideoFormat_eVesa1400x1050p75hz]                     = DFB_PlatformVideoFormat_eVesa1400x1050p75hz,
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1440X900P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1440x900p60hz]                      = DFB_PlatformVideoFormat_eVesa1440x900p60hz,
#endif
    [NEXUS_VideoFormat_eVesa1600x1200p60hz]                     = DFB_PlatformVideoFormat_eVesa1600x1200p60hz,
    [NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank]         = DFB_PlatformVideoFormat_eVesa1920x1080p60hzReducedBlank,
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1680X1050P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1680x1050p_60Hz]                    = DFB_PlatformVideoFormat_eVesa1680x1050p_60Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X800P_60HZ_RED_SUPPORTED
    [NEXUS_VideoFormat_eVesa1280x800p_60Hz_Red]                 = DFB_PlatformVideoFormat_eVesa1280x800p_60Hz_Red,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1600X1200P_75HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1600x1200p_75Hz]                    = DFB_PlatformVideoFormat_eVesa1600x1200p_75Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1600X900P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1600x900p_60Hz_Red]                 = DFB_PlatformVideoFormat_eVesa1600x900p_60Hz_Red,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1680X1050P_60HZ_RED_SUPPORTED
    [NEXUS_VideoFormat_eVesa1680x1050p_60Hz_Red]                = DFB_PlatformVideoFormat_eVesa1680x1050p_60Hz_Red,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1920X1200P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1920x1200p_60Hz]                    = DFB_PlatformVideoFormat_eVesa1920x1200p_60Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1920X1200P_60HZ_RED_SUPPORTED
    [NEXUS_VideoFormat_eVesa1920x1200p_60Hz_Red]                = DFB_PlatformVideoFormat_eVesa1920x1200p_60Hz_Red,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X720P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1152x720p_60Hz]                     = DFB_PlatformVideoFormat_eVesa1152x720p_60Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X720P_75HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1152x720p_75Hz]                     = DFB_PlatformVideoFormat_eVesa1152x720p_75Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X720P_85HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1152x720p_85Hz]                     = DFB_PlatformVideoFormat_eVesa1152x720p_85Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X864P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1152x864p_60Hz]                     = DFB_PlatformVideoFormat_eVesa1152x864p_60Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X864P_85HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1152x864p_85Hz]                     = DFB_PlatformVideoFormat_eVesa1152x864p_85Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X870P_75HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1152x870p_75Hz]                     = DFB_PlatformVideoFormat_eVesa1152x870p_75Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X900P_66HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1152x900p_66Hz]                     = DFB_PlatformVideoFormat_eVesa1152x900p_66Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1152X900P_76HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1152x900p_76Hz]                     = DFB_PlatformVideoFormat_eVesa1152x900p_76Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1170X584P_50HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1170x584p_50Hz]                     = DFB_PlatformVideoFormat_eVesa1170x584p_50Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_70HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1280x1024p_70Hz]                    = DFB_PlatformVideoFormat_eVesa1280x1024p_70Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_72HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1280x1024p_72Hz]                    = DFB_PlatformVideoFormat_eVesa1280x1024p_72Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X1024P_76HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1280x1024p_76Hz]                    = DFB_PlatformVideoFormat_eVesa1280x1024p_76Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X768P_50HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1280x768p_50Hz]                     = DFB_PlatformVideoFormat_eVesa1280x768p_50Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1280X960P_75HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1280x960p_75Hz]                     = DFB_PlatformVideoFormat_eVesa1280x960p_75Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1600X1024P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1600x1024p_60Hz]                    = DFB_PlatformVideoFormat_eVesa1600x1024p_60Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1600X1024P_76HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1600x1024p_76Hz]                    = DFB_PlatformVideoFormat_eVesa1600x1024p_76Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA1728X1080P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa1728x1080p_60Hz]                    = DFB_PlatformVideoFormat_eVesa1728x1080p_60Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA800X600P_100HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa800x600p_100Hz]                     = DFB_PlatformVideoFormat_eVesa800x600p_100Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA800X600P_90HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa800x600p_90Hz]                      = DFB_PlatformVideoFormat_eVesa800x600p_90Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA848X480P_75HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa848x480p_75Hz]                      = DFB_PlatformVideoFormat_eVesa848x480p_75Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA848X480P_85HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa848x480p_85Hz]                      = DFB_PlatformVideoFormat_eVesa848x480p_85Hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_VESA852X480P_60HZ_SUPPORTED
    [NEXUS_VideoFormat_eVesa852x480p_60Hz]                      = DFB_PlatformVideoFormat_eVesa852x480p_60Hz,
#endif

#ifdef BCMNEXUS_VIDEOFORMAT_720P_3DOU_AS_SUPPORTED
    [NEXUS_VideoFormat_e720p_3DOU_AS]                           = DFB_PlatformVideoFormat_e720p_3DOU_AS,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_720P50HZ_3DOU_AS_SUPPORTED
    [NEXUS_VideoFormat_e720p50hz_3DOU_AS]                       = DFB_PlatformVideoFormat_e720p50hz_3DOU_AS,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_720P30HZ_3DOU_AS_SUPPORTED
    [NEXUS_VideoFormat_e720p30hz_3DOU_AS]                       = DFB_PlatformVideoFormat_e720p30hz_3DOU_AS,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_720P24HZ_3DOU_AS_SUPPORTED
    [NEXUS_VideoFormat_e720p24hz_3DOU_AS]                       = DFB_PlatformVideoFormat_e720p24hz_3DOU_AS,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_1080P24HZ_3DOU_AS_SUPPORTED
        [NEXUS_VideoFormat_e1080p24hz_3DOU_AS]                      = DFB_PlatformVideoFormat_e1080p24hz_3DOU_AS,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_1080P30HZ_3DOU_AS_SUPPORTED
    [NEXUS_VideoFormat_e1080p30hz_3DOU_AS]                      = DFB_PlatformVideoFormat_e1080p30hz_3DOU_AS,
#endif

    /* dynamics: custom format (see Nexus for more detail) */
    [NEXUS_VideoFormat_eCustom0]                                = DFB_PlatformVideoFormat_eCustom0,
    [NEXUS_VideoFormat_eCustom1]                                = DFB_PlatformVideoFormat_eCustom1,
    [NEXUS_VideoFormat_eCustom2]                                = DFB_PlatformVideoFormat_eCustom2,

    /* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
    [NEXUS_VideoFormat_eCustom1920x2160i_48Hz]                  = DFB_PlatformVideoFormat_eCustom1920x2160i_48Hz,

    /* Legacy 3D timing and display formats */
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_SUPPORTED
    [NEXUS_VideoFormat_eCustom_3D_720p]                         = DFB_PlatformVideoFormat_eCustom_3D_720p,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_50HZ_SUPPORTED
    [NEXUS_VideoFormat_eCustom_3D_720p_50hz]                    = DFB_PlatformVideoFormat_eCustom_3D_720p_50hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_30HZ_SUPPORTED
    [NEXUS_VideoFormat_eCustom_3D_720p_30hz]                    = DFB_PlatformVideoFormat_eCustom_3D_720p_30hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_720P_24HZ_SUPPORTED
    [NEXUS_VideoFormat_eCustom_3D_720p_24hz]                    = DFB_PlatformVideoFormat_eCustom_3D_720p_24hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_1080P_24HZ_SUPPORTED
    [NEXUS_VideoFormat_eCustom_3D_1080p_24hz]                   = DFB_PlatformVideoFormat_eCustom_3D_1080p_24hz,
#endif
#ifdef BCMNEXUS_VIDEOFORMAT_CUSTOM_3D_1080P_30HZ_SUPPORTED
    [NEXUS_VideoFormat_eCustom_3D_1080p_30hz]                   = DFB_PlatformVideoFormat_eCustom_3D_1080p_30hz,
#endif
};

const NEXUS_DisplayType
DFB_Platform_P_DFBToNexusDisplayTypeTable[] =
{
    [DFB_PlatformDisplayType_eAuto]                             = NEXUS_DisplayType_eAuto,
    [DFB_PlatformDisplayType_eLvds]                             = NEXUS_DisplayType_eLvds,
    [DFB_PlatformDisplayType_eDvo]                              = NEXUS_DisplayType_eDvo,
    [DFB_PlatformDisplayType_eBypass]                           = NEXUS_DisplayType_eBypass,
};

const DFB_PlatformDisplayType
DFB_Platform_P_NexusToDFBDisplayTypeTable[] =
{
    [NEXUS_DisplayType_eAuto]                                   = DFB_PlatformDisplayType_eAuto,
    [NEXUS_DisplayType_eLvds]                                   = DFB_PlatformDisplayType_eLvds,
    [NEXUS_DisplayType_eDvo]                                    = DFB_PlatformDisplayType_eDvo,
    [NEXUS_DisplayType_eBypass]                                 = DFB_PlatformDisplayType_eBypass,
};

const NEXUS_AspectRatio
DFB_Platform_P_DFBToNexusDisplayAspectRatioTable[] =
{
    [DFB_PlatformDisplayAspectRatio_eAuto]   = NEXUS_DisplayAspectRatio_eAuto,
    [DFB_PlatformDisplayAspectRatio_e4x3]    = NEXUS_DisplayAspectRatio_e4x3,
    [DFB_PlatformDisplayAspectRatio_e16x9]   = NEXUS_DisplayAspectRatio_e16x9,
};

const DFB_PlatformDisplayAspectRatio
DFB_Platform_P_NexusToDFBDisplayAspectRatioTable[] =
{
    [NEXUS_DisplayAspectRatio_eAuto]   = DFB_PlatformDisplayAspectRatio_eAuto,
    [NEXUS_DisplayAspectRatio_e4x3]    = DFB_PlatformDisplayAspectRatio_e4x3,
    [NEXUS_DisplayAspectRatio_e16x9]   = DFB_PlatformDisplayAspectRatio_e16x9,
};

const DFB_PlatformTimebase
DFB_Platform_P_NexusToDFBTimebaseTable[] =
{
    [NEXUS_Timebase_e0]       = DFB_PlatformTimebase_e0,
    [NEXUS_Timebase_e1]       = DFB_PlatformTimebase_e1,
    [NEXUS_Timebase_e2]       = DFB_PlatformTimebase_e2,
    [NEXUS_Timebase_e3]       = DFB_PlatformTimebase_e3,
    [NEXUS_Timebase_eMax]     = DFB_PlatformTimebase_eMax

};

const NEXUS_Timebase
DFB_Platform_P_DFBToNexusTimebaseTable[] =
{
    [DFB_PlatformTimebase_eInvalid] = NEXUS_Timebase_eInvalid,
    [DFB_PlatformTimebase_e0]       = NEXUS_Timebase_e0,
    [DFB_PlatformTimebase_e1]       = NEXUS_Timebase_e1,
    [DFB_PlatformTimebase_e2]       = NEXUS_Timebase_e2,
    [DFB_PlatformTimebase_e3]       = NEXUS_Timebase_e3,
    [DFB_PlatformTimebase_eMax]     = NEXUS_Timebase_eMax
};


#endif /* __PLATFORM_VIDEO_FORMATS_H__ */
