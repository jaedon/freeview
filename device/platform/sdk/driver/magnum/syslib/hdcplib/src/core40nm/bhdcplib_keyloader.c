/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdcplib_keyloader.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 1/14/12 10:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/hdcplib/7422/bhdcplib_keyloader.c $
 * 
 * Hydra_Software_Devel/6   1/14/12 10:04p rgreen
 * SWSECURITY-71:  Move keyloading setup to HSM; Use
 * BHSM_FastLoadEncryptedHdcpKey to load keys
 * 
 * Mot_MTN1.xx_7425Bx_Int/ericz_Mot_MTN1.xx_7425Bx_3/1   12/14/11 12:35p ericz
 * CsP465507: Build errors with "export NEXUS_HDCP_SUPPORT=y" set for
 * DCX3600_7425 target
 * 
 * Hydra_Software_Devel/5   10/23/11 12:30p rgreen
 * SW7425-1583:  Add BERR_TRACE when HSM reports a HDCP key load error
 * 
 * Hydra_Software_Devel/4   10/17/11 5:38p vle
 * SW7425-1351: update hdcplib keyloader for 40nm B0 parts
 * 
 * Hydra_Software_Devel/3   1/31/11 7:33p vle
 * SW7422-129: Fix build warnings.
 * 
 * Hydra_Software_Devel/2   1/7/11 5:26p vle
 * SW7422-129: Add HDCP keyloader support for 7422 and other 40nm
 * platforms.
 * 
 * Hydra_Software_Devel/1   12/15/10 6:18p rgreen
 * SW7422-129: Initial checkin
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bhdcplib_keyloader.h"

#include "bhsm.h"
#include "bhsm_keyladder.h"


#define BHDCPLib_KEY_OFFSET 0x80 

BDBG_MODULE(BHDCPLIB_KEYLOADER) ;



BERR_Code BHDCPlib_FastLoadEncryptedHdcpKeys(
	BHDCPlib_Handle hHDCPlib)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint8_t i;
	BHSM_EncryptedHdcpKeyStruct * EncryptedHdcpKeys ;
	
	BDBG_ASSERT(hHDCPlib) ;

	BDBG_WRN(("$brcm_Revision: Hydra_Software_Devel/6 $")) ; 

	/* Get HDCP Encrypted Keys */
	EncryptedHdcpKeys = (BHSM_EncryptedHdcpKeyStruct *) 
		&hHDCPlib->stHdcpConfiguration.TxKeySet.TxKeyStructure ;

	for (i =0; i< BAVC_HDMI_HDCP_N_PRIVATE_KEYS; i++)
	{
		/* skip keys that are not specified to be used by the RxBksv */
		if (!(hHDCPlib->stHdcpConfiguration.RxInfo.RxBksv[i / 8] & (1 << (i % 8))))
			continue ;

		errCode = BHSM_FastLoadEncryptedHdcpKey( 
			hHDCPlib->stDependencies.hHsm, i + BHDCPLib_KEY_OFFSET, &(EncryptedHdcpKeys[i]) ) ;

		if (errCode != BERR_SUCCESS) 
		{
			BDBG_ERR(("BHSM_FastLoadEncryptedHdcpKey errCode: %x", errCode )) ;
			BERR_TRACE(errCode) ;
			break ;
		}	
	}

	return( errCode );
}

