/***************************************************************************
*     (c)2008-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_base_os_types.h $
* $brcm_Revision: 8 $
* $brcm_Date: 7/26/12 4:48p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxkernel/nexus_base_os_types.h $
* 
* 8   7/26/12 4:48p vsilyaev
* SW7435-280: Use Linux jiffies for the timestamp
* 
* 7   5/3/11 5:08p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 6   9/23/10 2:10p erickson
* SW7420-943: generic impl of NEXUS_Time
*
* 5   9/11/09 9:55a erickson
* SW7420-228: switch to autoconf.h
*
* 4   9/22/08 3:24p vsilyaev
* PR 42318: Use linux specific files for linux specific prototypes
*
* 3   7/9/08 2:39p erickson
* PR44643: switch from gettimeofday to jiffies, allows for settimeofday
*  changes to calendar time
*
* 2   2/8/08 6:46p vsilyaev
* PR 38682: Added support for 97405 platform
*
* 1   1/18/08 2:14p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/1   10/10/07 4:14p vsilyaev
* PR 35824: Added NEXUS_Time type
*
***************************************************************************/
#ifndef NEXUS_BASE_OS_TYPES_H__
#define NEXUS_BASE_OS_TYPES_H__

/**
Summary:
Time or time duration in milliseconds.

Description:
This is not wall-time, so it is only useful to calculate duration. 
**/
typedef unsigned long NEXUS_Time;

#define B_TIME_ZERO {0}

#endif /* NEXUS_BASE_OS_TYPES_H__ */

