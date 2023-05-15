/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_base_ioctl.h $
* $brcm_Revision: 1 $
* $brcm_Date: 5/3/11 5:09p $
*
* API Description:
*   API name: Platform (private)
*    This file enumerates linuxkernel driver supported on the given platform
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxkernel/nexus_base_ioctl.h $
* 
* 1   5/3/11 5:09p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 2   7/31/08 5:19p vsilyaev
* PR 45137: Initial support for multi application mode
* 
* 1   7/24/08 6:06p vsilyaev
* PR 45137: IOCTL that common for all modules
* 
* 
***************************************************************************/

#ifndef __NEXUS_BASE_IOCTL_H_
#define __NEXUS_BASE_IOCTL_H_

/* This file cannot #include linux header files. It defines a Broadcom-only API to the driver. */

#define NEXUS_PLATFORM_P_DRIVER_MAJOR 33

/* ioctl dispatch is faster if ioctl number doesn't include size of the structure */
#define NEXUS_IOCTL(type, num, data) ((type)<<16|(num))
#define NEXUS_IOCTL_NUM(id)          ((id)&0xFFFF)
#define NEXUS_IOCTL_TYPE(id)         ((id)>>16)

typedef struct PROXY_NEXUS_ModuleInit {
    unsigned version;
} PROXY_NEXUS_ModuleInit;

#endif /* __NEXUS_BASE_IOCTL_H_ */

