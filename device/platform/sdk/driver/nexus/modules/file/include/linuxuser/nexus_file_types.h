/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_file_types.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 11/30/11 1:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file/include/linuxuser/nexus_file_types.h $
 * 
 * 4   11/30/11 1:37p vsilyaev
 * SW7425-1790: Added support for S/G file mux I/O
 * 
 * SW7425-324/1   11/21/11 1:01p vsilyaev
 * SW7425-1790: Added support for S/G file mux I/O
 * 
 * 3   8/8/11 2:38p vsilyaev
 * SW7420-1849: Include bfile_types.h
 * 
 * 2   8/5/11 12:05p vsilyaev
 * SW7420-1849: Fixed off_t compatibility issue on Android Bionic C.
 * 
 * SW7420-1849/1   8/5/11 11:39a franktcc
 * SW7420-1849: Fixed off_t compatibility issue on Android Bionic C.
 * 
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/1   10/10/07 4:13p vsilyaev
 * PR 35824: File I/O module
 * 
 **************************************************************************/
#ifndef NEXUS_FILE_TYPES_H__
#define NEXUS_FILE_TYPES_H__

#include "bfile_types.h"

typedef struct NEXUS_FileMuxIoWriteAtom {
    const void *base;
    size_t len;
} NEXUS_FileMuxIoWriteAtom;

typedef struct NEXUS_FileMuxIoReadAtom {
    const void *base;
    size_t len;
} NEXUS_FileMuxIoReadAtom;



#endif /* NEXUS_FILE_TYPES_H__ */

