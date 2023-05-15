/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdm_hdcp.h $
 * $brcm_Revision: Hydra_Software_Devel/43 $
 * $brcm_Date: 11/24/10 3:28p $
 *
 * Module Description:
 * This module implements HDCP functionality contained in the HDCP Spec 
 * Version 1.086 (www.digital-cp.com) for HDMI/DVI transmiters and 
 * receivers.  
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/7038/bhdm_hdcp.h $
 * 
 * Hydra_Software_Devel/43   11/24/10 3:28p calvinho
 * SW7401-4468: Update HDCP version definition
 * 
 * Hydra_Software_Devel/42   2/22/10 5:10p vle
 * SW7335-669: add BHDM_HDCP_RX_BKSV_I2C_READ_ERROR and
 * BHDM_HDCP_TX_AKSV_I2C_WRITE_ERROR errors constant.
 * 
 * Hydra_Software_Devel/41   12/3/09 2:24p rgreen
 * SW7405-3521: Fix link erros for HDCP_Get/Set options
 * 
 * Hydra_Software_Devel/40   11/18/09 8:06p vle
 * SW7342-76: Define HDCP debug function to resolve "implicit declaration"
 * warnings.
 * 
 * Hydra_Software_Devel/39   7/14/09 11:28a rgreen
 * PR56776: Prevent HDCP An Timeout; Reduce debug messages
 * 
 * Hydra_Software_Devel/38   5/5/09 5:14p vle
 * PR 54851: Implement a dynamic mode in the PI where HDCP 1.1 feature
 * will be disabled when a pre-define occurance of Pj Link Failures is
 * met.
 * 
 * Hydra_Software_Devel/37   6/26/06 12:04p rgreen
 * PR22342: Detect three successive HDCP Pj failures prior to forcing an
 * un-Authenticated link
 * 
 * Hydra_Software_Devel/36   6/2/06 3:26p rgreen
 * PR20866: Add option to disable Pj checking; default is on
 * Add BHDM_HDCP_Get/SetOptions
 * Modify LIC return codes to report specific Ri
 * (BHDM_HDCP_LINK_RI_FAILURE)
 * or Pj (BHDM_HDCP_LINK_PJ_FAILURE)
 * link failure
 * 
 * BHDM_HDCP_LINK_FAILURE (unused) left for legacy
 * 
 * Hydra_Software_Devel/35   2/3/06 4:36p rgreen
 * PR8896: Remove depracated functions
 * 
 * Hydra_Software_Devel/34   10/21/05 6:02p rgreen
 * PR17750: Add HDCP Abort return code
 * 
 * Hydra_Software_Devel/33   6/27/05 6:21p rgreen
 * PR15217: Add Auth Support for HDCP Repeaters with Device Count 0;
 * Use bhdm_config.h configuration option.
 * Add option for Repeater Simulation Test
 * 
 * Hydra_Software_Devel/32   5/16/05 3:11p rgreen
 * PR8896: Fix typo in definition
 * BDHM_HDCP_RX_BSTATUS_DEPTH to
 * BHDM_HDCP_RX_BSTATUS_DEPTH
 * 
 * Hydra_Software_Devel/31   4/29/05 5:43p rgreen
 * PR14848: Return failure when Repeater downstream devices do not
 * authenticate
 * 
 * Hydra_Software_Devel/30   4/21/05 7:43p rgreen
 * PR14622: Add HDMI API to return KSV values
 * Add functions
 * BHDM_HDCP_GetRxKsv
 * BHDM_HDCP_GetRepeaterKsvFifo
 * 
 * Hydra_Software_Devel/29   2/4/05 1:04p rgreen
 * PR9474: HDCP 1.1 Support
 * Remove depracated   BHDM_HDCP_Version eHdcpVersion from
 * BHDM_XmitEncrypted.  Already part of hdmi handle.
 * 
 * Hydra_Software_Devel/28   12/9/04 2:38p rgreen
 * PR8896: Change BHDM_HDCP_AuthenticateRepeater argument
 * RepeaterAuthenticated from bool to uint8_t.
 * 
 * Clean up debug messages
 * 
 * Hydra_Software_Devel/27   10/21/04 6:26p rgreen
 * PR8896: Remove HDCP Key functions used for debug purposes.
 * 
 * Hydra_Software_Devel/26   10/18/04 11:59a rgreen
 * PR9474: Add HDCP 1.1 Support
 * HDCP 1.1 not fully implemented
 * Modified BHDM_HDCP_GetRxStatus API call (RxStatus is uint16_t not
 * uint8_t)
 * Replace private HDCP_SetVersion with new API call HDCP_SetVersion
 * 
 * Hydra_Software_Devel/25   9/23/04 8:36p rgreen
 * PR12728: Make sure code compiles with -pedantic-errors
 * Clean up documenation
 * 
 * Hydra_Software_Devel/24   5/13/04 4:24p rgreen
 * PR 10273: HDMI / HDCP Revocation/renewability support requirement
 * 
 * Hydra_Software_Devel/23   4/28/04 10:59a rgreen
 * PR8896: HDMI Development
 * Removed code for BHDM_HDCP_VerifyLink; use
 * BHDM_HDCP_RiLinkIntegrityCheck instead
 * 
 * Hydra_Software_Devel/22   2/13/04 7:52p rgreen
 * PR8896: HDMI API Development/Test
 * Add code for HDCP Repeater Support
 * Change BHDM_HDCP_ReadReapeaterKsvFIFO to
 * BHDM_HDCP_ReadRxRepeaterKsvFIFO
 * Add BHDM_HDCP_WriteTxKsvFIFO
 * Add checkForRepeater function
 * 
 * Hydra_Software_Devel/21   1/27/04 1:03p rgreen
 * PR8896: HDMI API Development/Test
 * Move AuthenticateRepeater function to BHDMlib; needs access to XPT
 * handle for authentication with HDCP Repeater
 * 
 * Hydra_Software_Devel/20   1/26/04 11:38a rgreen
 * PR9474: Add HDCP 1.1 functionality
 * Add BHDM_HDCP_Version enumeration for HDCP Version unused/1_0/1_1
 * 
 * Replace BHDM_HDCP_XmitEncrypted bool argument with BHDM_HDCP_Version
 * enumeration.
 * Caller can decide which version of HDCP to used based on Rx Capability
 * 
 * Add BHDM_HDCP_RiLinkIntegrityCheck to replace BHDM_HDCP_VerifyLink...
 * VerifyLink will be eventually removed
 * 
 * Add BHDM_HDCP_PjLinkIntegrityCheck stub for HDCP 1.1 functionality
 * 
 * Add toggle to AUTH_REQUEST bit for GenerateAn
 * 
 * Enable Always Rekey on VSync functionality (HDCP 1.1) to the
 * XmitEncryprpted function
 * 
 * Hydra_Software_Devel/19   1/20/04 6:32p rgreen
 * PR8896: HDMI API Development/Test
 * Add flag to BHDM_XmitEncrypted to enable HDCP 1.1 Features
 * 
 * Hydra_Software_Devel/18   12/15/03 5:08p rgreen
 * PR8896:
 * Fix typo BHDM_HDPC... to BHDM_HDCP on error macro
 * 
 * Hydra_Software_Devel/17   11/24/03 3:03p rgreen
 * Add declarations for private functions switched to public functions
 *   BHDM_HDCP_ReadRxBksv,
 *   BHDM_HDCP_GenerateAn, and
 *   BHDM_HDCP_AuthenticateLink
 * Add new functions BHDM_HDCP_WriteTxAksvToRx and
 * BHDM_HDCP_EnableSerialKeyLoad
 * 
 * Hydra_Software_Devel/16   11/5/03 4:48p rgreen
 * Add BHDM_HDCP_IsLinkAuthenticated declaration;  function
 * can be used to check authentication status of the link
 * 
 * Hydra_Software_Devel/15   10/22/03 5:18p rgreen
 * Remove BHDM_HDCP_SetAvMute declaration;
 * Function is in bhdm.c as BHDM_SetAvMute
 * 
 * Hydra_Software_Devel/14   10/21/03 6:28p rgreen
 * Add #defines for HDCP Rx I2C Registers
 * Remove Make75XX arguments from HDCP Authenticate functions
 * Re-arrange AnSelect enumeration to match register configuration
 * Fix declaration and return values for BHDM_HDCP_ReadRepeaterKsvFIFO
 * 
 * Hydra_Software_Devel/13   10/6/03 8:32p rgreen
 * Incorporate HDMI API Review Comments
 * 
 * Hydra_Software_Devel/12   9/30/03 5:06p lseverin
 * fixed a few documentation (DocJet) wanrings
 * 
 * Hydra_Software_Devel/11   9/24/03 8:37p rgreen
 * Changes to reflect internal HDMI review comments.
 * 
 * Hydra_Software_Devel/10   7/25/03 4:42p rgreen
 * Add/Modify Function names
 * 
 * Hydra_Software_Devel/9   7/24/03 2:49p rgreen
 * Modifications for HDCP 1.1
 * 
 * Hydra_Software_Devel/8   4/15/03 6:01p rgreen
 * Documentation Upates
 * 
 * Hydra_Software_Devel/7   4/14/03 2:53p rgreen
 * Documentation Updates
 * 
 * Hydra_Software_Devel/6   3/27/03 4:54p rgreen
 * Documentation Updates
 * 
 * Hydra_Software_Devel/5   3/25/03 5:49p rgreen
 * Documentation Fix
 * 
 * Hydra_Software_Devel/3   3/20/03 6:01p rgreen
 * Use StandardNamingConventions on enumeration types
 * 
 * Hydra_Software_Devel/2   3/14/03 11:23a rgreen
 * add mofule overview
 * 
 * Hydra_Software_Devel/1   3/7/03 1:23p rgreen
 * Beta Release of HDMI API
 *
 ***************************************************************************/
#ifndef BHDM_HDCP_H__
#define BHDM_HDCP_H__

/*=************************* Module Overview ********************************
  The HDCP (High-bandwidth Data Content Protection) functions provide support 
  for implementing the HDCP Spec Version 1.086 as of 12/31/2002.

  HDCP requires the use of purchased production type keys or the use of test
  keys contained in the specification.  Both the transmitting and receiving 
  devices must have keys.

  The specification can be found at www.digital-cp.com 
***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "bhdm.h"

#define BHDM_HDCP_RX_NO_HDCP_SUPPORT  BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 1)
#define BHDM_HDCP_RX_BKSV_ERROR		  BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 2)
#define BHDM_HDCP_RX_BKSV_REVOKED     BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 3)
#define BHDM_HDCP_TX_AKSV_ERROR       BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 4)
#define BHDM_HDCP_RECEIVER_AUTH_ERROR BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 5)
#define BHDM_HDCP_REPEATER_AUTH_ERROR BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 6)

#define BHDM_HDCP_LINK_FAILURE		  BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 7)

#define BHDM_HDCP_RX_DEVICES_EXCEEDED BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 8)
#define BHDM_HDCP_REPEATER_DEPTH_EXCEEDED \
	                                  BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 9)

#define BERR_HDCP_NOT_ENOUGH_MEM           BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 10)
#define BHDM_HDCP_NO_AUTHENTICATED_LINK    BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 11)
#define BHDM_HDCP_REPEATER_FIFO_NOT_READY  BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 12)

#define BHDM_HDCP_REPEATER_DEVCOUNT_0      BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 13)
#define BHDM_HDCP_AUTH_ABORTED     BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 14)

#define BHDM_HDCP_LINK_RI_FAILURE		  BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 15)
#define BHDM_HDCP_LINK_PJ_FAILURE		  BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 16)

#define BHDM_HDCP_PLL_PWRDN 	BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 17)
#define BHDM_HDCP_FIFO_UNDERFLOW 	BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 18)
#define BHDM_HDCP_FIFO_OVERFLOW  	BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 19)
#define BHDM_HDCP_MULTIPLE_AN_REQUEST 	BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 20)

#define BHDM_HDCP_RX_BKSV_I2C_READ_ERROR BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 21)
#define BHDM_HDCP_TX_AKSV_I2C_WRITE_ERROR BERR_MAKE_CODE(BERR_HDM_ID, BHDM_HDCP_ERRS+ 22)


/* keep older definition in case in use */
#define BHDM_HDCP_AUTHENTICATE_ERROR  BHDM_HDCP_RECEIVER_AUTH_ERROR 

/******************************************************************************
Summary:
HDCP Definitions
*******************************************************************************/

#define BHDM_HDCP_RX_I2C_ADDR       0x3A   /* 7 Bit Addr */

#define BHDM_HDCP_NUM_KEYS  40
#define BHDM_HDCP_KSV_LENGTH 5
#define BHDM_HDCP_AN_LENGTH 8

#define BHDM_HDCP_MAX_PJ_MISMATCH 3
#define BHDM_HDCP_MAX_I2C_RETRY 3

#define BHDM_HDCP_MAX_PJ_LINK_FAILURES_BEFORE_DISABLE_HDCP_1_1 5

/******************************************************************************
Summary:
I2C Rx HDCP Registers 
*******************************************************************************/


/* Rx Bksv value (Read Only) */
#define BHDM_HDCP_RX_BKSV0			0x00
#define BHDM_HDCP_RX_BKSV1			0x01
#define BHDM_HDCP_RX_BKSV2			0x02
#define BHDM_HDCP_RX_BKSV3			0x03
#define BHDM_HDCP_RX_BKSV4			0x04

/* Rx Link verification value (Read Only) */
#define BHDM_HDCP_RX_RI0		    0x08
#define BHDM_HDCP_RX_RI1		    0x09

/*  Rx Enhanced Link Verification Response (Read Only) */
#define BHDM_HDCP_RX_PJ             0x0A


/* Tx Aksv value (Write Only) */
#define BHDM_HDCP_RX_AKSV0			0x10
#define BHDM_HDCP_RX_AKSV1			0x11
#define BHDM_HDCP_RX_AKSV2			0x12
#define BHDM_HDCP_RX_AKSV3			0x13
#define BHDM_HDCP_RX_AKSV4			0x14

/*  Rx HDCP Enable HDCP 1.1 features (Write Only) */
#define BHDM_HDCP_RX_AINFO             0x15
#define BHDM_HDCP_RX_ENABLE_1_1_FEATURES 0x02

/* Session Random Number (An) value generated by the Tx (Write Only) */
#define BHDM_HDCP_RX_AN0			0x18
#define BHDM_HDCP_RX_AN1			0x19
#define BHDM_HDCP_RX_AN2			0x1a
#define BHDM_HDCP_RX_AN3			0x1b
#define BHDM_HDCP_RX_AN4			0x1c
#define BHDM_HDCP_RX_AN5			0x1d
#define BHDM_HDCP_RX_AN6			0x1e
#define BHDM_HDCP_RX_AN7			0x1f

/* HDCP Repeater SHA-1 Hash value V' */
#define BHDM_HDCP_REPEATER_SHA1_V_H0 0x20
#define BHDM_HDCP_REPEATER_SHA1_V_H1 0x24
#define BHDM_HDCP_REPEATER_SHA1_V_H2 0x28
#define BHDM_HDCP_REPEATER_SHA1_V_H3 0x2c
#define BHDM_HDCP_REPEATER_SHA1_V_H4 0x30

/* Rx Capabilities Register (Read Only) */
#define BHDM_HDCP_RX_BCAPS          0x40 

/* Rx Status Registers (Read Only) */
#define BHDM_HDCP_RX_BSTATUS        0x41 /* 2 Bytes */
#define BHDM_HDCP_RX_BSTATUS_DEPTH        0x0700
#define BHDM_HDCP_RX_BSTATUS_DEVICE_COUNT 0x007F

#define BHDM_HDCP_REPEATER_KSV_FIFO 0x43 /* 2 Bytes */

/* HDCP Repeater Registers */
#define BHDM_HDCP_REPEATER_MAX_DEVICE_COUNT 127
#define BHDM_HDCP_REPEATER_MAX_DEPTH 7
												 						  


/******************************************************************************
Summary:
Enumerated Type containing the type of HDCP An value to be generated

Description:
The DVI/HDMI transmitter is capable of generating a psuedo-random number (An)
which is used as a initial seed for the HDCP calculations.  This Enumerated 
Type specifies the type of An value which can be generated.

   BHDM_HDCP_AnSelect_eRandomAn   - generate random An value 
   BHDM_HDCP_AnSelect_eTestA1B1An - generate fixed A1/B1 HDCP Spec An value 
   BHDM_HDCP_AnSelect_eTestA1B2An - generate fixed A1/B2 HDCP Spec An value 
   BHDM_HDCP_AnSelect_eTestA2B1An - generate fixed A2/B1 HDCP Spec An value 
   BHDM_HDCP_AnSelect_eTestA2B2An - generate fixed A2/B2 HDCP Spec An value 


*******************************************************************************/
typedef enum BHDM_HDCP_AnSelect
{
   BHDM_HDCP_AnSelect_eTestA1B1An, /* generate fixed A1/B1 HDCP Spec An value */
   BHDM_HDCP_AnSelect_eTestA1B2An, /* generate fixed A1/B2 HDCP Spec An value */
   BHDM_HDCP_AnSelect_eTestA2B1An, /* generate fixed A2/B1 HDCP Spec An value */
   BHDM_HDCP_AnSelect_eTestA2B2An, /* generate fixed A2/B2 HDCP Spec An value */
   BHDM_HDCP_AnSelect_eRandomAn   /* generate random An value */
} BHDM_HDCP_AnSelect ;


/******************************************************************************
Summary:
Enumerated Type specifying the capabilites of the attached receiver

Description:
The enumeration types listed can be compared with the value of the returned
BCaps register from the Receiver.

See Also:
	BHDM_HDCP_GetRxCaps

*******************************************************************************/
typedef enum BHDM_HDCP_RxCaps
{
   BHDM_HDCP_RxCaps_eHdmiCapable        = 0x80,  /* Rx is HDMI Capable      */
   BHDM_HDCP_RxCaps_eHdcpRepeater       = 0x40,  /* Rx is Repeater          */
   BHDM_HDCP_RxCaps_eKsvFifoReady       = 0x20,  /* Rx Ksv FIFO is ready    */
   BHDM_HDCP_RxCaps_eI2c400KhzSupport   = 0x10,  /* Rx I2C supports 400KHz  */
   BHDM_HDCP_RxCaps_eHDCP_1_1_Features  = 0x02,  /* Rx has HDCP1.1 Features 
												     EESS
													 Advance Cipher
													 Enhanced Link Verification
												   */
   BHDM_HDCP_RxCaps_eFastReauth         = 0x01   /* Rx can receive un-encrypted
												  video during Re-authemtication
												  */
} BHDM_HDCP_RxCaps  ;


/******************************************************************************
Summary:
Enumerated Type specifying the current status iof the attached receiver.

Description:
The enumeration types listed can be compared with the value of the returned
BStatus register from the Receiver

See Also:
	BHDM_HDCP_GetRxStatus

*******************************************************************************/
typedef enum BHDM_HDCP_RxStatus
{
   BHDM_HDCP_RxStatus_eHdmiMode             = 0x1000, /* Rx in HDMI mode        */
   BHDM_HDCP_RxStatus_eMaxRepeatersExceeded = 0x0800, /* Rx has too many repeaters */
   BHDM_HDCP_RxStatus_eMaxDevicesExceeded   = 0x0080  /* Rx has too many devices */
} BHDM_HDCP_RxStatus ;


/******************************************************************************
Summary:
Enumerated Type specifying which version of HDCP to use

Description:
The enumeration types listed can be used to configure the core to use features
of HDCP 1.1 or HDCP 1.1 with optional features. BHDM_HDCP_Version_e1_0 is deprecated.

*******************************************************************************/
typedef enum BHDM_HDCP_Version
{
   BHDM_HDCP_Version_eUnused,
   BHDM_HDCP_Version_e1_0,
   BHDM_HDCP_Version_e1_1 = BHDM_HDCP_Version_e1_0,
   BHDM_HDCP_Version_e1_1_Optional_Features
} BHDM_HDCP_Version ;


typedef struct _BHDM_HDCP_OPTIONS_
{
    bool PjChecking ;       /* Set to true to enable HDCP 1.1 feature when applicable */
    uint8_t numPjFailures;  /* Number of Pj Link failures. 
			                            If greater than BHDM_HDCP_MAX_PJ_LINK_FAILURES_BEFORE_DISABLE_HDCP_1_1,
  				                          HDCP 1.1 feature will not be enabled */
 	   
} BHDM_HDCP_OPTIONS ;

/******************************************************************************
Summary:
Read the HDCP BKsv value from the Receiver

Description:
This function 
1) reads the Bksv value from the receiver,
2) verifies it is a valid Ksv (20 zeros & 20 ones)
3) writes the Bksv value to the HDMI transmitter


Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.
	
	pRevokedKsvList - List of Revoked KSVs obtained from DCP LLC SRM message
	
	uiNumRevokedKsvs - Number of Ksvs contained in the pRevokedKsvList

Output:
	<none>

Returns:
	BERR_SUCCESS                  - Bksv value read and successfully written
	BERR_INVALID_PARAMETER        - Invalid function parameter.
	BHDM_HDCP_RX_BKSV_ERROR       - Attached Rx Device has invalid Bksv
	
Note:
The pRevokedKsvList should be passed in as big-endian (the same endian-ness 
contained in the original SRM message).

Use NULL and 0 for pRevokedKsvList and uiNumRevokedKsvs if no DCP LLC SRM 
messages are available.

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_ReadRxBksv(
   BHDM_Handle hHDMI,              /* [in] HDMI handle */
   const uint8_t *pRevokedKsvList, /* [in] pointer to Revoked KSV List */
   const uint16_t uiNumRevokedKsvs /* [in] number of KSVs in Revoked Ksv List */
) ;


/******************************************************************************
Summary: 
Generate the HDCP An value for HDCP calculations

Description:
This function generates the random HDCP An value.  This function also supports
generation of the fixed An values contained in the HDCP Specification


Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

	AnSelection - HDCP An value to be generated by the HDCP core

Output:
	<none>

Returns:
	BERR_SUCCESS                  - Generated and wrote HDCP An value
	BERR_INVALID_PARAMETER        - Invalid function parameter.

See Also:
	o BHDM_Handle
	o BHDM_HDCP_AnSelect

*******************************************************************************/
BERR_Code BHDM_HDCP_GenerateAn(
	BHDM_Handle hHDMI,              /* [in] HDMI handle */
	BHDM_HDCP_AnSelect AnSelection  /* [in] HDCP An type value to use */
) ;
		   


/******************************************************************************
Summary: 
Configure the HDMI Core to receive the HDCP Keys from the XPT Key serializer.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	<none>

Returns:
	BERR_SUCCESS                  - HDCP protected link created.
	BERR_INVALID_PARAMETER        - Invalid function parameter.

See Also:
	o BHDM_Handle
*******************************************************************************/
BERR_Code BHDM_HDCP_EnableSerialKeyLoad(
	BHDM_Handle hHDMI              /* [in] HDMI handle */
) ;


/******************************************************************************
Summary: Write the HDCP Aksv value to the receiver 

Description:
This function 
1) verifies the Aksv is a valid Ksv (20 zeros & 20 ones)
1) writes the Aksv value to the receiver,


Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

	pTxAksv  - pointer to the Transmitter Key Selection Vector (Aksv) shipped 
	with the HDCP key set from Digital-CP (www.digital-cp.com)

Output:
	<none>

Returns:
	BERR_SUCCESS                  - HDCP protected link created.
	BERR_INVALID_PARAMETER        - Invalid function parameter.
	BHDM_HDCP_TX_AKSV_ERROR       - Transmtter Aksv value is invalid

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_WriteTxAksvToRx(
   BHDM_Handle hHDMI,           /* [in] HDMI handle */
   const uint8_t *pTxAksv       /* [in] pointer HDCP Key Set Aksv Value */
)  ;


/******************************************************************************
BERR_Code BHDM_HDCP_AuthenticateLink
Summary: Authenticate the HDCP Link; verify TxR0 and RxR0 values are equal

*******************************************************************************/
BERR_Code BHDM_HDCP_AuthenticateLink
(
	BHDM_Handle hHDMI              /* [in] HDMI handle */
) ;



/******************************************************************************
Summary:
Clear the Authenticated link between the Transmitter and the Receiver.

Description:
This function sets the transmitted in an un-authenticated state

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	<none>

Returns:
	BERR_SUCCESS                  - HDCP protected link created.
	BERR_INVALID_PARAMETER        - Invalid function parameter.

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_ClearAuthentication(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) ;


/******************************************************************************
Summary:
Start transmitting video encrypted with HDCP.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	<none>

Returns:
	BERR_SUCCESS - transmission of HDCP protected video has begun
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_XmitEncrypted(
   BHDM_Handle hHDMI             /* [in] HDMI handle */
) ;


/******************************************************************************
Summary:
Start transmitting video without HDCP.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	<none>

Returns:
	BERR_SUCCESS - transmission of video is now done without HDCP encryption
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_XmitClear(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) ;

/******************************************************************************
Summary:
Use the HDCP Ri value to verify a protected HDCP link is still valid.

Description:
This function should be called approximately every two seconds (~128 frames)
to verify the HDCP link is still valid.  The validation is done by reading
the Ri values from the Rx and the Tx.  The Ri values are calculated using the 
HDCP Key Set.  The Ri values should be equal and should also change 
approximately every two seconds.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	<none>

Returns:
	BERR_SUCCESS            - HDCP Link is still valid
	BERR_INVALID_PARAMETER  - Invalid function parameter.
	BHDM_HDCP_LINK_FAILURE  - HDCP Link has failed

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_RiLinkIntegrityCheck(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) ;

/******************************************************************************
Summary:
Use the HDCP Pj value to verify the protected HDCP link is still valid.

Description:
This function should be called approximately every TBD (16 frames)
to verify the HDCP link is still valid.  The validation is done by reading
the Pj values from the Rx and the Tx.  The Pj values are calculated using the 
HDCP Key Set.  The Pj values should be equal and should also change 
every 16 frames.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	<none>

Returns:
	BERR_SUCCESS            - HDCP Link is still valid
	BERR_INVALID_PARAMETER  - Invalid function parameter.
	BHDM_HDCP_LINK_FAILURE  - HDCP Link has failed

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_PjLinkIntegrityCheck(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) ;



/******************************************************************************
Summary:
Retrieve the Rx Capabilities.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	pRxCapsReg - contents of Rx capabilities; 
	Use logical AND with RxCapsRegister and BHDM_HDCP_RxCaps feature
	enumeration type to check for specific capability:

Example:
	feature - Rx Capabilities

	BHDM_HDCP_RxCaps_eHdmiCapable		- Rx is HDMI Capable          
	BHDM_HDCP_RxCaps_eHdcpRepeater		- Rx is a Repeater              
	BHDM_HDCP_RxCaps_eKsvFifoReady		- Rx Ksv FIFO is ready        
	BHDM_HDCP_RxCaps_eI2c400KhzSupport	- Rx I2C supports 400KHz Xfer 
	BHDM_HDCP_RxCaps_eHDCP_1_1_Features - Rx supports HDCP 1.1 Features 
	                                       EESS, Advance Cipher, and
	                                       Enhanced Link Verification
    BHDM_HDCP_RxCaps_eFastReauth        - Rx supports fast Re-authemtication

	BHDM_HDCP_GetRxCaps(hHDMI, &RxCaps) ;
	if (RxCaps & BHDM_HDCP_RxCaps_eHdmiCapable)
		HDMI Capable
	else
		DVI Only


Returns:
	BERR_SUCCESS 
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	o BHDM_Handle
	o BHDM_HDCP_RxCaps

*******************************************************************************/
BERR_Code BHDM_HDCP_GetRxCaps(
   BHDM_Handle hHDMI,         /* [in] HDMI handle */
   uint8_t *pRxCapsReg     /* [out] HDCP Rx Capability */
) ;


/******************************************************************************
Summary:
Check the status of the attached receiver.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	pRxStatusReg - contents of Rx Status Register

	Use logical AND with RxStatusReg and BHDM_HDCP_RxStatus
	enumeration type to check for specific capability:

	BHDM_HDCP_RxStatus_eHdmiMode			 -  Rx in HDMI mode        
	BHDM_HDCP_RxStatus_eMaxRepeatersExceeded -  Rx has too many repeaters 
	BHDM_HDCP_RxStatus_eMaxDevicesExceeded	 -  Rx has too many devices 

Returns:
	BERR_SUCCESS 

See Also:
	o BHDM_Handle
	o BHDM_HDCP_RxStatus

*******************************************************************************/
BERR_Code BHDM_HDCP_GetRxStatus(
   BHDM_Handle hHDMI,                /* [in] HDMI handle */
   uint16_t *pRxStatusReg          /* [out] HDCP Rx Status */
) ;


/******************************************************************************
Summary:
Get the depth (number of levels) of HDCP Repeaters

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	pRepeaterDepth - pointer to integer containing the number of levels (depth)
	of receive devices.

Returns:
	BERR_SUCCESS - Repeater Depth successfully retrieved
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	o BHDM_Handle
	o BHDM_HDCP_GetRepeaterDeviceCount

*******************************************************************************/
BERR_Code BHDM_HDCP_GetRepeaterDepth(
   BHDM_Handle hHDMI,      /* [in] HDMI handle */
   uint8_t *pRepeaterDepth  /* [out] pointer to hold Levels of HDCP Repeaters */
) ;


/******************************************************************************
Summary:
Get the number of devices attached to the HDCP Repeater.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	pNumDevices - Number of devices attached to the HDCP Receiver

Returns:
	BERR_SUCCESS - Number of Repeaters successfully retrieved
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	o BHDM_Handle
	o BHDM_HDCP_GetRepeaterDepth

*******************************************************************************/
BERR_Code BHDM_HDCP_GetRepeaterDeviceCount(
   BHDM_Handle hHDMI,   /* [in] HDMI handle */
   uint8_t *pNumDevices /* [out] pointer to # of devices attached to the HDCP 
                           Repeater */
) ;


/******************************************************************************
Summary:
Read the KSV list from the attached repeater.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.
	uiDeviceCount - Number of KSVs to be read from the Ksv FIFO
	pRevokedKsvList - List of Revoked KSVs obtained from DCP LLC SRM message
	uiNumRevokedKsvs - Number of Ksvs contained in the pRevokedKsvList


Output:
	pKsvList - pointer to memory location to load the KSV list read from the 
	HDCP repeater KSV fifo.

Returns:
	BERR_SUCCESS 
	BERR_INVALID_PARAMETER    - Invalid function parameter.
	BERR_HDCP_NOT_ENOUGH_MEM  - Not enough memory to hold results

Note:
	The pKsvList must point to a buffer (allocated and freed by the calling 
	function) that is DEVICE_COUNT * 5 bytes long.  The DEVICE_COUNT can be 
	retrieved using the BHDM_HDCP_GetRepeaterDeviceCount function.  


See Also:
	o BHDM_Handle
	o BHDM_HDCP_GetRepeaterDeviceCount 

*******************************************************************************/
BERR_Code BHDM_HDCP_ReadRxRepeaterKsvFIFO(
   BHDM_Handle hHDMI,     /* [in] HDMI handle */
   uint8_t *pKsvList,     /* [out] pointer to memory to hold KSV List */
   uint16_t uiDeviceCount, /* [in ] number of devices size of memory in bytes to hold KSV List */
   const uint8_t *pRevokedKsvList, /* [in] pointer to Revoked KSV List */
   const uint16_t uiNumRevokedKsvs /* [in] number of KSVs in Revoked Ksv List */
) ;



/******************************************************************************
Summary:
Write the KSV listread  from the attached repeater to the transmitter core

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.
	pKsvList - pointer to memory location holding the KSV list from the 
	HDCP repeater KSV fifo.
	uiDeviceCount - number of Ksvs from the downstream Rx Devices  
	
Output:
	<none>

Returns:
	BERR_SUCCESS 
	BERR_INVALID_PARAMETER    - Invalid function parameter.
	BERR_HDCP_NOT_ENOUGH_MEM  - Not enough memory to hold results

Note:
	The pKsvList must point to a buffer (allocated and freed by the calling 
	function) that is DEVICE_COUNT * 5 bytes long.  The DEVICE_COUNT can be 
	retrieved using the BHDM_HDCP_GetRepeaterDeviceCount function.  


See Also:
	o BHDM_Handle
	o BHDM_HDCP_GetRepeaterDeviceCount 

*******************************************************************************/
BERR_Code BHDM_HDCP_WriteTxKsvFIFO(
	BHDM_Handle hHDMI,     /* [in] HDMI handle */
	uint8_t *pKsvList,     /* [in] pointer to memory to holding KSV List */
	uint16_t uiDeviceCount /* [in] number of Ksvs from the downstream Rx 
	                          Devices */
) ;



/******************************************************************************
Summary:
Check if the current link is authenticated.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.


Output:
	<none>

Returns:
	BERR_SUCCESS 
	BERR_INVALID_PARAMETER    - Invalid function parameter.
	BHDM_HDCP_NO_AUTHENTICATED_LINK  - Link is not Authenticated

Note:
	The pKsvList must point to a buffer (allocated and freed by the calling 
	function) that is DEVICE_COUNT * 5 bytes long.  The DEVICE_COUNT can be 
	retrieved using the BHDM_HDCP_GetRepeaterDeviceCount function.  


See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_IsLinkAuthenticated(
   BHDM_Handle hHDMI      /* [in] HDMI handle */
) ;


/******************************************************************************
Summary:
Check if the current Receiver is a repeater

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.


Output:
	uiRepeater - flag indicating the attached receiver is a repeater

Returns:
	BERR_SUCCESS 
	BERR_INVALID_PARAMETER    - Invalid function parameter.

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_CheckForRepeater(
   BHDM_Handle hHDMI,           /* [in] HDMI handle */
   uint8_t *uiRepeater          /* [out] HDCP Rx Status */
) ;


/******************************************************************************
Summary:
Create an authenticated link with the attached repeater.

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	bRepeaterAuthenticated - flag indicating the success/failure of the repeater
	authentication

Returns:
	BERR_SUCCESS 
	BERR_INVALID_PARAMETER    - Invalid function parameter.

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_RepeaterAuthenticateLink(
	BHDM_Handle hHDMI, 
	uint8_t *RepeaterAuthenticated
) ;

/******************************************************************************
Summary:
Initialize Authentication with the attached repeater

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.


Output:
	<none>

Returns:
	BERR_SUCCESS 
	BERR_INVALID_PARAMETER    - Invalid function parameter.

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_InitializeRepeaterAuthentication(
   BHDM_Handle hHDMI           /* [in] HDMI handle */
) ;


/******************************************************************************
Summary:
Get a copy of the attached Rx device's KSV

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	AttachedDeviceKsv - Ksv of the Attached DVI/HDMI Rx

Returns:
	BERR_SUCCESS 
	BHDM_RX_NO_DEVICE    - No attached device(s).

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_GetRxKsv(BHDM_Handle hHDMI, 
	uint8_t *AttachedDeviceKsv
) ;


/******************************************************************************
Summary:
Get a copy of the KSV(s) attached to the HDMI Transmitter

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:
	DownstreamKsvsBuffer - pointer to buffer to hold KSV information
	DownstreamKsvsBufferSize - size in bytes of the DownstreamKsvsBuffer 

Returns:
	BERR_SUCCESS 
	BERR_INVALID_PARAMETER - Invalid buffer or buffer size
	BHDM_RX_NO_DEVICE    - No attached device(s).

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_GetRepeaterKsvFifo(BHDM_Handle hHDMI, 
	uint8_t *DownstreamKsvsBuffer, uint16_t DownstreamKsvsBufferSize 
) ;


/******************************************************************************
Summary:
Force the V Calculation for attached repeaters with device count zero 

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:

Returns:

See Also:
	o BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_HDCP_ForceVCalculation(BHDM_Handle hHDMI) ;


/******************************************************************************
Summary:
Get the options used for HDCP Authentication 

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.
	HdcpOptions - pointer to structure containing HDCP Options

Output:

Returns:

See Also:
	o BHDM_Handle
	o BHDM_HDCP_OPTIONS

*******************************************************************************/
BERR_Code BHDM_HDCP_GetOptions(
	BHDM_Handle hHDMI, BHDM_HDCP_OPTIONS *HdcpOptions) ;


/******************************************************************************
Summary:
Set the options used for HDCP Authentication 

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.
	HdcpOptions - pointer to structure containing HDCP Options to set

Output:

Returns:

See Also:
	o BHDM_Handle
	o BHDM_HDCP_OPTIONS

*******************************************************************************/
BERR_Code BHDM_HDCP_SetOptions(
	BHDM_Handle hHDMI, BHDM_HDCP_OPTIONS *HdcpOptions) ;



/******************************************************************************
Summary:
Force all video pixel a particular value, 0 or 1. This function is meant to be used for 
debugging purposes only. 

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.
	value  - value to force video pixels, should be 0 or 1

Output:

Returns:

See Also:
	o BHDM_Handle
	o BHDM_HDCP_P_DEBUG_PjCleanVideo

*******************************************************************************/
BERR_Code BHDM_HDCP_P_DEBUG_PjForceVideo(BHDM_Handle hHDMI, uint8_t value);


/******************************************************************************
Summary:
Clean video pixel which was forced by BHDM_HDCP_P_DEBUG_PjCleanVideo. 
This function is meant to be used for debugging purposes only. 

Input:
	hHDMI - The HDMI device handle that the application created earlier 
	during the system initialization sequence.

Output:

Returns:

See Also:
	o BHDM_Handle
	o BHDM_HDCP_P_DEBUG_PjForceVideo

*******************************************************************************/
BERR_Code BHDM_HDCP_P_DEBUG_PjCleanVideo(BHDM_Handle hHDMI, uint8_t value);

#ifdef __cplusplus
}
#endif



#endif /* BHDM_HDCP_H__ */

