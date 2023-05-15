/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_input_crc.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 10/11/12 12:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/include/nexus_video_input_crc.h $
 * 
 * 4   10/11/12 12:29p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
 *  added object verification
 * 
 * 3   6/11/12 1:07p erickson
 * SW7346-815: add right eye values for NEXUS_VideoInputCrcData
 * 
 * 2   3/2/11 3:12p erickson
 * SW7420-1491: fix attributes
 *
 * 1   4/7/08 4:10p erickson
 * PR32888: added NEXUS_CRC_CAPTURE
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_INPUT_CRC_H__
#define NEXUS_VIDEO_INPUT_CRC_H__

#include "nexus_types.h"
#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
CRC data for video at the MFD (MPEG feeder)

Description:
This API is used for test and is subject to change.
**/
typedef struct NEXUS_VideoInputCrcData
{
    uint32_t idrPictureId;
    uint32_t pictureOrderCount;
    uint32_t lumaCrc;
    uint32_t chromaCrc;
    uint32_t lumaCrcR; /* 3D right eye */
    uint32_t chromaCrcR; /* 3D right eye */
    uint32_t isField;
} NEXUS_VideoInputCrcData;

/**
Summary:
Get CRC data

Description:
This API is used for test and is subject to change.
**/
NEXUS_Error NEXUS_VideoInput_GetCrcData(
    NEXUS_VideoInputHandle input,
    NEXUS_VideoInputCrcData *pData, /* attr{nelem=numEntries;nelem_out=pNumEntriesReturned} [out] array of crc data structures */
    unsigned numEntries,
    unsigned *pNumEntriesReturned /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_INPUT_CRC_H__ */
