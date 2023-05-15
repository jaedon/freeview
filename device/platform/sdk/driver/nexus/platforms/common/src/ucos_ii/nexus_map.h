/***************************************************************************
*     (c)2008-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_map.h $
* $brcm_Revision: 2 $
* $brcm_Date: 8/17/11 3:54p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97346/src/ucos_ii/nexus_map.h $
* 
* 2   8/17/11 3:54p agin
* SWNOOS-479:  Add offset and size fields.
* 
* 1   5/9/11 3:04p agin
* SWNOOS-456:  Relevel 97340 no-os to 20110418.
* 
* 1   3/16/11 12:43a agin
* SWNOOS-447:  Support 97231 no-os.
* 
***************************************************************************/
#ifndef NEXUS_MAP_H__
#define NEXUS_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

struct nexus_map_settings
{
    unsigned offset; /* physical address that is being managed */
    unsigned size; /* size of memory in bytes from offset */

    /* real OS map/unmap functions */
    void *(*mmap)(unsigned long offset, unsigned long length, int /*bool*/ cached);
    void (*munmap)(void *address, unsigned long length);
};

void nexus_p_get_default_map_settings(struct nexus_map_settings *p_settings);

/* functions for handling fake + real memory map using wrapper around special BMEM */
int nexus_p_init_map(const struct nexus_map_settings *p_settings);

void nexus_p_uninit_map(void);

typedef enum NEXUS_MemoryMapType {
    NEXUS_MemoryMapType_eUncached,
    NEXUS_MemoryMapType_eCached,
    NEXUS_MemoryMapType_eFake,
    NEXUS_MemoryMapType_eMax
} NEXUS_MemoryMapType;

void *nexus_p_map_memory(unsigned long offset, unsigned long length, NEXUS_MemoryMapType type);

/* unmap memory mapped by nexus_p_map_memory */
void nexus_p_unmap_memory( void *address, unsigned long length );

#ifdef __cplusplus
}
#endif

#endif
