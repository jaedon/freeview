/***************************************************************************
 *     (c)2003-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_tsmf.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 10/3/12 5:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/include/nexus_tsmf.h $
 * 
 * 2   10/3/12 5:02p jtna
 * SW7425-3782: refactor nexus tsmf impl
 * 
 * 1   10/5/11 12:27p jtna
 * SW7425-1189: add transport and frontend TSMF support
 * 
 ***************************************************************************/
#ifndef NEXUS_TSMF_H__
#define NEXUS_TSMF_H__

#include "nexus_types.h"
#include "nexus_parser_band.h"
#include "nexus_playpump.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct NEXUS_TsmfOpenSettings
{
    unsigned unused;
} NEXUS_TsmfOpenSettings;

/*
Summary:
Get default open settings for the structure
*/
void NEXUS_Tsmf_GetDefaultOpenSettings(
    NEXUS_TsmfOpenSettings *pOpenSettings /* [out] */
    );
/*
Summary:
Open a new TSMF
*/
NEXUS_TsmfHandle NEXUS_Tsmf_Open( /* attr{destructor=NEXUS_Tsmf_Close} */
    unsigned index,
    const NEXUS_TsmfOpenSettings* pOpenSettings /* attr{null_allowed=y} */
    );

/*
Summary:
Close a TSMF
*/
void NEXUS_Tsmf_Close(
    NEXUS_TsmfHandle tsmf
    );

/*
Summary:
Get current settings
*/
void NEXUS_Tsmf_GetSettings(
    NEXUS_TsmfHandle tsmf, /* which TSMF */
    NEXUS_TsmfSettings *pSettings
    );

/*
Summary:
Set new settings
*/    
NEXUS_Error NEXUS_Tsmf_SetSettings(
    NEXUS_TsmfHandle tsmf, /* which TSMF */
    const NEXUS_TsmfSettings *pSettings
    );


#ifdef __cplusplus
}
#endif

#endif

