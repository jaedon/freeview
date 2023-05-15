/***************************************************************************
 *    (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: netapp_zeroconf.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/14/11 10:41a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/netapp_zeroconf.h $
 * 
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief NetApp Zero Configuration (Bonjour) Private APIs
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal Zeroconf Implementation using Avahi
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#ifndef NETAPP_ZEROCONF_H_
#define NETAPP_ZEROCONF_H_

typedef struct sNETAPP_ZEROCONF *NETAPP_ZEROCONF_HANDLE;

NETAPP_RETCODE NetAppZeroConfPrivateInit(
    NETAPP_ZEROCONF_HANDLE  *phNetAppZeroConf,
    NETAPP_PRIV_HANDLE      hNetAppPriv,
    NETAPP_INIT_SETTINGS    *pInitSettings,
    NETAPP_SETTINGS         *pSettings);


NETAPP_RETCODE NetAppZeroConfPrivateDeInit(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf);


NETAPP_RETCODE NetAppZeroConfPrivatePublish(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf,
    char                    *pName,
    char                    *pType,
    uint32_t                ulPort,
    char                    *pTxtRecord,
    uint32_t                ulTxtLength);


NETAPP_RETCODE NetAppZeroConfPrivateStart(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf);


NETAPP_RETCODE NetAppZeroConfPrivateStop(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf);


NETAPP_RETCODE NetAppZeroConfPrivateBrowse(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf,
    char                    *pType);


NETAPP_RETCODE NetAppZeroConfPrivateGetBrowseResults(
    NETAPP_ZEROCONF_HANDLE  hNetAppZeroConf
    /* TODO: Send the data discovered! */);
#endif /* NETAPP_ZEROCONF_H_ */
