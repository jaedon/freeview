/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: hdmi_input_hdcp_keyloader.c $
* $brcm_Revision: 7 $
* $brcm_Date: 2/10/12 10:44a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/hdmi/hdmi_input_hdcp_keyloader.c $
* 
* 7   2/10/12 10:44a rgreen
* SW7425-2226: Make HDCP Ksv and KeySet declarartions consistent between
*  examples
* 
* 6   1/19/12 4:05p rgreen
* SW7425-2226: Make HDCP Ksv and KeySet declarartions consistent between
*  examples
* 
* 5   11/2/11 4:26p rgreen
* SW7425-1351:  Replace Tx Keyset with the Rx Keyset from the HDCP Spec.
*  Add mode informative message for success/failure of key loading;
* 
* 4   4/18/11 3:53p rgreen
* SW7422-186: Update examples for HDMI Repeater Support
* 
* 3   4/12/11 11:24a rgreen
* SW7422-186: Update hdmiInput HDCP Keyloader app
* 
* 2   2/16/11 7:45p rgreen
* SW7422-129: Update example to remove depracated Nexus HdcpInit call
* 
* 1   2/2/11 6:01p rgreen
* SW7422-129: Add example app for hdmiInput HDCP key set loading
* 
***************************************************************************/


#include "nexus_platform.h"
#if NEXUS_NUM_HDMI_INPUTS

#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#include "nexus_hdmi_input_hdcp.h"
#endif

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(hdmi_input_hdcp_keyloader) ;

static uint8_t SampleEDID[] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x74, 0x22, 0x05, 0x01, 0x11, 0x20,
	0x00, 0x14, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
	0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
	0x58, 0x2C, 0x25, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
	0x43, 0x4D, 0x37, 0x34, 0x32, 0x32, 0x2F, 0x37, 0x34, 0x32, 0x35, 0x0A, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x17, 0x3D, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x89,

	0x02, 0x03, 0x3C, 0x71, 0x7F, 0x03, 0x0C, 0x00, 0x40, 0x00, 0xB8, 0x2D, 0x2F, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE3, 0x05, 0x1F, 0x01, 0x49, 0x90, 0x05, 0x20, 0x04, 0x03, 0x02, 0x07,
	0x06, 0x01, 0x29, 0x09, 0x07, 0x01, 0x11, 0x07, 0x00, 0x15, 0x07, 0x00, 0x01, 0x1D, 0x00, 0x72,
	0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A,
	0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x18,
	0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x0B, 0x88, 0x21, 0x00,
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D
};




/*****************/
/* For HDCP TESTING  */
/*    1) insert the Production Key Set set generated by BCrypt  */
/*    2) set the USE_PRODUCTION_KEYS macro to to 1 */
/*****************/

#define USE_PRODUCTION_KEYS 0

#if USE_PRODUCTION_KEYS

/*****************************/
/* INSERT PRODUCTION KeySet HERE */
/*****************************/

#else


/**************************************/
/* HDCP Specification Test Key Set    */
/*                                    */
/* NOTE: the default declared Test    */
/* KeySet below is from the HDCP Spec */ 
/* and it *IS NOT* compatible with    */
/* production devices                 */
/**************************************/


/* HDCP Specification Test Key Set */

uint8_t encryptedRxKeySetAlg     = 0x01 ;
uint8_t encryptedRxKeySetKeyVar1 = 0x02 ;
uint8_t encryptedRxKeySetKeyVar2 = 0x03 ;
uint8_t encryptedRxKeySetCusKey  = 0x04 ;


static const NEXUS_HdmiHdcpKsv hdcpRxBksv =
{
   {0xCD, 0x1A, 0xF2, 0x1E, 0x51} 
} ;

   
static const 
    NEXUS_HdmiInputHdcpKey encryptedRxKeySet[NEXUS_HDMI_HDCP_NUM_KEYS] =
{
	{ 0, 0, 0, 0, 0xFDF05BC7, 0xE013BC00},  /* 00 */
	{ 0, 0, 0, 0, 0x3B44767F, 0x2C0DAE00},  /* 01 */
	{ 0, 0, 0, 0, 0x606CA385, 0x21BF2400},  /* 02 */
	{ 0, 0, 0, 0, 0x2FA3D7BC, 0x6CBCF400},  /* 03 */
	{ 0, 0, 0, 0, 0x8863EBC5, 0x692EA700},  /* 04 */
	{ 0, 0, 0, 0, 0xF8D9377A, 0xD2A27F00},  /* 05 */
	{ 0, 0, 0, 0, 0xD1A3DE29, 0x35FD3200},  /* 06 */
	{ 0, 0, 0, 0, 0xAE9BCC40, 0xC25F4800},  /* 07 */
	{ 0, 0, 0, 0, 0x03517D79, 0x57983B00},  /* 08 */
	{ 0, 0, 0, 0, 0x505261BE, 0x70D10D00},  /* 09 */
	{ 0, 0, 0, 0, 0xB16B86E4, 0x8B741A00},  /* 10 */
	{ 0, 0, 0, 0, 0xCA8C347C, 0x6A60F900},  /* 11 */
	{ 0, 0, 0, 0, 0xA1EE9978, 0x03BB4B00},  /* 12 */
	{ 0, 0, 0, 0, 0xA995C09C, 0xCF0E1900},  /* 13 */
	{ 0, 0, 0, 0, 0x7F449768, 0xC421A800},  /* 14 */
	{ 0, 0, 0, 0, 0x418A29C4, 0x0B8A1A00},  /* 15 */
	{ 0, 0, 0, 0, 0x8220E653, 0x08FCAE00},  /* 16 */
	{ 0, 0, 0, 0, 0xA47B490C, 0x4A5DF700},  /* 17 */
	{ 0, 0, 0, 0, 0xD8068AFC, 0x9564AD00},  /* 18 */
	{ 0, 0, 0, 0, 0x022E2B0C, 0x02C26700},  /* 19 */
	{ 0, 0, 0, 0, 0x8DAEF418, 0x6B118F00},  /* 20 */
	{ 0, 0, 0, 0, 0x69FAE9A3, 0x3F05E300},  /* 21 */
	{ 0, 0, 0, 0, 0xD1C78128, 0x00D83700},  /* 22 */
	{ 0, 0, 0, 0, 0x9C66151C, 0xFDA5C300},  /* 23 */
	{ 0, 0, 0, 0, 0xF711081E, 0xD4939E00},  /* 24 */
	{ 0, 0, 0, 0, 0x6CEC9E50, 0x74402C00},  /* 25 */
	{ 0, 0, 0, 0, 0x619B2719, 0xD87F8B00},  /* 26 */
	{ 0, 0, 0, 0, 0xE96CA0A0, 0xADCAD700},  /* 27 */
	{ 0, 0, 0, 0, 0xDBC1F8A1, 0xDC979200},  /* 28 */
	{ 0, 0, 0, 0, 0x89A4DE99, 0xAA1A5D00},  /* 29 */
	{ 0, 0, 0, 0, 0xD9A1BADD, 0x56CB6000},  /* 30 */
	{ 0, 0, 0, 0, 0xE0F25F5E, 0xADD48500},  /* 31 */
	{ 0, 0, 0, 0, 0x6DDF2112, 0x16801200},  /* 32 */
	{ 0, 0, 0, 0, 0x896540F2, 0xA531CA00},  /* 33 */
	{ 0, 0, 0, 0, 0x6F8E19CB, 0xE8301D00},  /* 34 */
	{ 0, 0, 0, 0, 0xFAD307ED, 0x8BC1D100},  /* 35 */
	{ 0, 0, 0, 0, 0x435B2409, 0xECC7CE00},  /* 36 */
	{ 0, 0, 0, 0, 0x83D5EDEF, 0x2981B000},  /* 37 */
	{ 0, 0, 0, 0, 0xE586E24C, 0xCF342100},  /* 38 */
	{ 0, 0, 0, 0, 0x8CB799D0, 0xF9EEED00}
} ;

#endif


static void displayKeyLoadStatus(uint8_t success) 
{
	
    BDBG_WRN(("*************************")) ;
    BDBG_WRN(("HDCP Key Loading: %s", success ? "SUCCESS" : " FAILED")) ;
    BDBG_WRN(("*************************")) ;
}

int main(int argc, char **argv)
{
#if NEXUS_HAS_HDMI_INPUT
    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings;
    NEXUS_HdmiInputHdcpStatus hdcpStatus ;
    
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings ;
    NEXUS_Error errCode  ;
    
    unsigned hdmiInputIndex = 0;
    
    BSTD_UNUSED(argc) ;
    BSTD_UNUSED(argv) ;
    
    /* Bring up all modules for a platform in a default configuration for this platform */

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
  
    NEXUS_Platform_Init(&platformSettings); 

    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
		
        /* set hpdDisconnected to true if a HDMI switch is in front of the Broadcom HDMI Rx.  
             -- The NEXUS_HdmiInput_ConfigureAfterHotPlug should be called to inform the hw of 
             -- the current state,  the Broadcom SV reference boards have no switch so 
             -- the value should always be false 
             */
       hdmiInputSettings.frontend.hpdDisconnected = false ;
        
        /* use NEXUS_HdmiInput_OpenWithEdid ()
              if EDID PROM (U1304 and U1305) is NOT installed; 
                reference boards usually have the PROMs installed.
                this example assumes Port1 EDID has been removed 
            */
    
        /* all HDMI Tx/Rx combo chips have EDID RAM */
        hdmiInputSettings.useInternalEdid = true ;
    hdmiInput = NEXUS_HdmiInput_OpenWithEdid(hdmiInputIndex, &hdmiInputSettings, 
        &SampleEDID[0], (uint16_t) sizeof(SampleEDID));
 
    if (!hdmiInput) 
    {
        fprintf(stderr, "Can't get hdmi input\n");
        return -1;
    }

    NEXUS_HdmiInput_GetSettings(hdmiInput, &hdmiInputSettings) ;
    NEXUS_HdmiInput_SetSettings(hdmiInput, &hdmiInputSettings) ;

    {
        NEXUS_HdmiInputHdcpKeyset hdmiInputKeyset ;

        NEXUS_HdmiInput_HdcpGetDefaultKeyset(hdmiInput, &hdmiInputKeyset) ;

            /* Intialize/Load HDCP Key Set	*/
            hdmiInputKeyset.alg = encryptedRxKeySetAlg ;
            hdmiInputKeyset.custKeyVarL = encryptedRxKeySetKeyVar1	;
            hdmiInputKeyset.custKeyVarH = encryptedRxKeySetKeyVar2	;
            hdmiInputKeyset.custKeySel =  encryptedRxKeySetCusKey	;
            
            BKNI_Memcpy(&hdmiInputKeyset.rxBksv, &hdcpRxBksv,
                NEXUS_HDMI_HDCP_KSV_LENGTH) ;
            
            BKNI_Memcpy(&hdmiInputKeyset.privateKey, &encryptedRxKeySet,
                 sizeof(NEXUS_HdmiInputHdcpKey) * NEXUS_HDMI_HDCP_NUM_KEYS) ;
    
            
        errCode = NEXUS_HdmiInput_HdcpSetKeyset(hdmiInput, &hdmiInputKeyset ) ;
        if (errCode)
        {
            /* display message informing of result of HDCP Key Load */
            displayKeyLoadStatus(0) ;
            goto done ;
        }

        NEXUS_HdmiInput_HdcpGetStatus(hdmiInput, &hdcpStatus) ;

        /* display message informing of result of HDCP Key Load */
	 /* NOTE: use of otpState is overloaded... refers to status of key load */
        if (hdcpStatus.eOtpState != NEXUS_HdmiInputHdcpKeySetOtpState_eCrcMatch)
            displayKeyLoadStatus(0) ;
        else
            displayKeyLoadStatus(1) ;
     }

done:
    NEXUS_HdmiInput_Close(hdmiInput) ;  
    
    NEXUS_Platform_Uninit(); 

#else
    BSTD_UNUSED(argc);
    printf("%s not supported on this platform", argv[0]) ;
#endif

    return 0;
}

#else

#include "bstd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NEXUS_NUM_HDMI_INPUTS
#define NEXUS_NUM_HDMI_INPUTS 0
#endif

int main(int argc, char **argv)
{
    BSTD_UNUSED(argc);
	printf("%d Platform has %d HDMI Inputs ; App requires one HDMI Input \n",
		NEXUS_PLATFORM, NEXUS_NUM_HDMI_INPUTS);
	
    printf("%s not supported on the %d platform.\n", argv[0], BCHP_CHIP) ;
    return 0 ;
}
#endif

