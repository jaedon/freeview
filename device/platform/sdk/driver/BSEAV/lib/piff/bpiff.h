/*********************************************************************************
*     Copyright (c) 2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*   This program is the proprietary software of Broadcom Corporation and/or its licensors, 
*   and may only be used, duplicated, modified or distributed pursuant to the terms and 
*   conditions of a separate, written license agreement executed between you and Broadcom 
*   (an "Authorized License").  Except as set forth in an Authorized License, 
*   Broadcom grants no license (express or implied), right to use, or waiver of any kind 
*   with respect to the Software, and Broadcom expressly reserves all rights in and to the 
*   Software and all intellectual property rights therein.  
* 
*   IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, 
*   AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*   Except as expressly set forth in the Authorized License,
*   1.     This program, including its structure, sequence and organization, constitutes the 
*       valuable trade secrets of Broadcom, and you shall use all reasonable efforts to protect 
*       the confidentiality thereof, and to use this information only in connection with your use 
*       of Broadcom integrated circuit products.
*   2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH ALL 
*       FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, 
*       STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND 
*       ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*       LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO 
*       DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
*   3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS BE LIABLE 
*       FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT 
*       OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN 
*       ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID 
*       FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING 
*       ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
*
*********************************************************************************/

#ifndef BPIFF_H
#define BPIFF_H

#include "bstd.h"
#include "bioatom.h"

#include "nexus_wmdrmpd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NB_OF_TRACKS (5)

typedef struct bpiff_mp4_headers {
    NEXUS_WmDrmPdMp4PsshBoxInfo         psshSystemId;     /* Protection System Specific Header Box */
    uint8_t  *pPsshData;                                  /* Set to NULL if no Playready DRM has been found in the moov  */
    uint32_t  psshDataLength;

    NEXUS_WmDrmPdMp4ProtectionSchemeInfo    scheme[MAX_NB_OF_TRACKS]; /* Pool of protection scheme info */
    uint32_t                                nbOfSchemes; /* Number of protection scheme info used in the pool. This number
                                                           correspond to the number of encrypted track we have discovered in the
                                                           moov. */
} bpiff_mp4_headers;
/**
Summary:
WMDRMPD get default mp4 header

Description:
Initialize a mp4 header structure to its default state
**/ 
void bpiff_GetDefaultMp4Header(
    bpiff_mp4_headers *pMp4
    );

/**
Summary:
WMDRMPD free mp4 header

Description:
Helper function dynamically allocation information created during MP4 parsing. 
**/ 
void bpiff_FreeMp4Header(
    bpiff_mp4_headers *pMp4
    );

/**
Summary:
Parse Moov box

Description:
This function is used to parse a moov box and extract all the DRM information
**/ 
int bpiff_parse_moov(
    bpiff_mp4_headers *header,
    batom_t atom
    );


#ifdef __cplusplus
}
#endif


#endif /*BPIFF_H*/
