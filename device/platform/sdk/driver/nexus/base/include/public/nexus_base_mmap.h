/***************************************************************************
*     (c)2007-2010 Broadcom Corporation
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
* $brcm_Workfile: nexus_base_mmap.h $
* $brcm_Revision: 3 $
* $brcm_Date: 11/3/10 4:16p $
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/nexus_base_mmap.h $
* 
* 3   11/3/10 4:16p erickson
* SW7420-1223: refactor for application use of pointer/offset conversion
*  functions
* 
* 2   10/19/10 4:24p erickson
* SW7420-1155: clarify that offset is a system-wide unique physical
*  address
* 
* 1   9/24/10 9:23a erickson
* SW7420-943: add files
*
***************************************************************************/
#ifndef NEXUS_BASE_MMAP_H__
#define NEXUS_BASE_MMAP_H__

#include "nexus_base_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
This API is available to Nexus and applications.
Only Nexus is able to submit new memory mappings (see nexus_base.h), but applications can query this mapping information.

This API assumes that "physical offset" is a physical address.
That means, for a system with multiple memory controllers (MEMC's), offsets will be unique in the system.
**/

/**
Summary:
Convert a cached or uncached virtual address to a physical offset.
**/
uint32_t NEXUS_AddrToOffset(const void *addr);

/**
Summary:
Convert a physical offset to a cached address.
**/
void *NEXUS_OffsetToCachedAddr(uint32_t offset);

/**
Summary:
Convert a physical offset to an uncached address.
**/
void *NEXUS_OffsetToUncachedAddr(uint32_t offset);

#ifdef __cplusplus
}
#endif

#endif
