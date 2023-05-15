/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_hdmitx.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/10/12 9:25a $
 *
 * Module Description: HDMI Abstraction for APE audio testing
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_hdmitx.h $
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 * 
 * Hydra_Software_Devel/1   4/11/12 7:25p jgarrett
 * SW7425-2075: Adding HDMITX support and HBR test tone app
 *
 ***************************************************************************/
#ifndef APETEST_HDMITX_H_
#define APETEST_HDMITX_H_

#include "framework.h"
#include "bfmt.h"
#include "bavc.h"
#include "bavc_hdmi.h"
#include "bape.h"

/***************************************************************************
Summary:
Settings availble for the HDMI output library
****************************************************************************/
typedef struct APETEST_HDMITX_Settings
{
    BFMT_VideoFmt videoFormat;
    BFMT_AspectRatio aspectRatio;
    BAVC_HDMI_AudioInfoFrame_ChannelCount channelCount;
    uint8_t speakerAllocation;
} APETEST_HDMITX_Settings;

/***************************************************************************
Summary:
Get Default HDMI output library settings
****************************************************************************/
void APETEST_HDMITX_GetDefaultSettings(
    APETEST_HDMITX_Settings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
Initialize the HDMI outuput
****************************************************************************/
BERR_Code APETEST_HDMITX_Init(
    BFramework_Info *pFrmInfo,
    const APETEST_HDMITX_Settings *pSettings,
    BAPE_MaiOutputHandle hMaiOutput
    );

/***************************************************************************
Summary:
Un-Initialize the HDMI outuput
****************************************************************************/
void APETEST_HDMITX_Uninit(
    void
    );

#endif /* #ifndef APE_HDMI_LIB_H_ */
