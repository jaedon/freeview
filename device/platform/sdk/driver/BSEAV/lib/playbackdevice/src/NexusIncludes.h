/******************************************************************************
 *    (c)2009-2011 Broadcom Corporation
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
 * $brcm_Workfile: NexusIncludes.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 4/30/12 4:25p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/playbackdevice/src/NexusIncludes.h $
 * 
 * 2   4/30/12 4:25p gayatriv
 * SW7425-2967:copy protection support in playbackdevice api
 * 
 * 2   4/30/12 4:24p gayatriv
 * SW7425-2967:copy protection support in playbackdevice api
 * 
 * 1   8/16/11 2:24p cdisc
 * SW7425-1114: adding playback device
 * 
 * 6   8/24/09 11:57a sgundime
 * SW3548-2400: Netflix SDK release v2.0.3 release.
 * 
 * 5   6/17/09 7:54p kcoyle
 * PR 56111:
 * Add support for 7405
 * 
 * 4   5/26/09 11:52a sgundime
 * PR55051:Mute audio until first video pts . This is  work around to
 *  avoid audio drop bacause of  pcr- lib.
 * 
 * 3   4/20/09 5:29p sgundime
 * PR54401:  Netflix uses NexusMGR , initial check-in
 * 
 * 2   4/8/09 7:39p sgundime
 * PR54028: netflix-checkin
 * 
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_video_input.h"
#include "nexus_video_output.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_output.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_input.h"
#include "nexus_video_window.h"
#include "nexus_parser_band.h"
#include "nexus_playpump.h"
#include "nexus_playback.h"
#include "nexus_video_types.h"
#include "nexus_audio_types.h"
#include "nexus_security.h"
#include "nexus_display_vbi.h"
#ifndef PLAYBACKDEVICE
#include "nexus_random_number.h"

#if (B_HAS_MSDRM_PRDY==1)
  #warning "MSDRM Playready support is enabled"
  #include "prdy_core.h"
  #include "prdy_decryptor.h"
#else
  #warning "MSDRM WMDRM support is enabled"
  #include "drmcore.h"
  #include "drmdecryptor.h"
#endif

#endif
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "nexusMgr.h"



