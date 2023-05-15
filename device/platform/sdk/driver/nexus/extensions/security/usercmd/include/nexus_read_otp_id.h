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
 * $brcm_Workfile: nexus_read_otp_id.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 1/28/11 7:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/security/usercmd/7422/include/nexus_read_otp_id.h $
 * 
 * 1   1/28/11 7:30p atruong
 * SW7422-235: Enable Nexus Security extension modules for 97422
 *
 ******************************************************************************/
#ifndef NEXUS_READ_OTP_ID_H__
#define NEXUS_READ_OTP_ID_H__

#include "nexus_security_datatypes.h"
#include "nexus_security.h"


#ifdef __cplusplus
extern "C" {
#endif


#define NEXUS_MAX_OTP_ID_LENGTH           8


/**************************************************************************************************
Summary:
This enum defines the supported OTP ID types
**************************************************************************************************/
typedef enum NEXUS_OtpIdType
{
    NEXUS_OtpIdType_eA = 0,
    NEXUS_OtpIdType_eB = 1,
    NEXUS_OtpIdType_eC = 2,
	NEXUS_OtpIdType_eD = 3,
	NEXUS_OtpIdType_eE = 4,
	NEXUS_OtpIdType_eF = 5,

    NEXUS_OtpIdType_eMax
}   NEXUS_OtpIdType;

/**************************************************************************************************
Summary:
Output from NEXUS_Security_ReadOtpId
**************************************************************************************************/
typedef struct NEXUS_OtpIdOutput
{
    unsigned char otpId[NEXUS_MAX_OTP_ID_LENGTH];      /* Buffer to hold the OTP ID */
    unsigned int  size;                                /* Actual size of the returned OTP ID in bytes */

}   NEXUS_OtpIdOutput;


/*****************************************************************************
Summary:
Read an OTP ID
*****************************************************************************/
NEXUS_Error NEXUS_Security_ReadOtpId(
    NEXUS_OtpIdType otpId,
    NEXUS_OtpIdOutput *pOutput /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif
