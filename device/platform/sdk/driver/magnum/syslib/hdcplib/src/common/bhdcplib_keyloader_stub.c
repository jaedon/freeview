/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdcplib_keyloader_stub.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 9/11/12 2:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/hdcplib/src/common/bhdcplib_keyloader_stub.c $
 * 
 * Hydra_Software_Devel/9   9/11/12 2:28p rgreen
 * SW7231-956: Exit immediately if authentication is using the hdcplib
 * keyloader stub to authenticate
 * 
 * Hydra_Software_Devel/8   12/12/11 2:30p rgreen
 * SW7552-157: Remove  unused BHDM_CONFIG_40NM_REV2_SUPPORT macro
 * 
 * Hydra_Software_Devel/7   10/11/11 5:21p vle
 * SW7429-5: Add support for 7429
 * 
 * Hydra_Software_Devel/6   1/27/11 6:35p vle
 * SW7422-129: Fix build error when HDCP is not enabled.
 * 
 * Hydra_Software_Devel/5   6/15/10 1:36p vle
 * SW7550-458: Fix build warning
 * 
 * Hydra_Software_Devel/4   3/15/10 5:30p vle
 * SW7408-28: Fix build error for non-HDCP build.
 * 
 * Hydra_Software_Devel/3   12/29/09 3:53p qcheng
 * SW7550-102 : Add Security support for 7550
 * 
 * Hydra_Software_Devel/2   3/18/08 7:18p vle
 * PR 39991: Rename HDMIlib to HDCPlib. Remove unused implementation.
 * 
 * Hydra_Software_Devel/1   2/26/08 5:43p jgarrett
 * PR 39991: Renaming new HDMlib to HDCPlib
 * 
 * Hydra_Software_Devel/2   8/7/06 6:04p rgreen
 * PR22187: Fix stub to use standalone HSM PI
 * 
 * Hydra_Software_Devel/1   6/15/06 1:24p rgreen
 * PR17748: Create separate keyloader source for 740x vs 7038
 * 
 * Hydra_Software_Devel/8   5/2/06 4:23p rgreen
 * PR 20436: Sync up with Aegis 7_0 release
 * 
 * Hydra_Software_Devel/7   3/10/06 1:13p rgreen
 * PR19265: Add BCM97400 HDMI Support; No HDCP yet
 * 
 * Hydra_Software_Devel/6   1/23/06 8:08p rgreen
 * PR17748: Modify HDMI HDCP to use HSM for chips that support AEGIS
 * 
 * Hydra_Software_Devel/5   10/20/05 5:31p erickson
 * PR17108: added temp 7401 hacks
 * 
 * Hydra_Software_Devel/4   8/31/05 6:54p rgreen
 * PR9474: Fix big endian support bug
 * 
 * Hydra_Software_Devel/3   4/26/05 8:55a rgreen
 * PR12116: Fix bug in HDCP stub key loader to used passed argument vs
 * global
 * 
 * Hydra_Software_Devel/2   8/19/04 12:07p rgreen
 * PR12116: Add HDCP Key Loader Support
 * Fix Metrowerks compilation error
 * 
 * Hydra_Software_Devel/1   8/18/04 4:34p rgreen
 * PR 12116: Add HDCP Key Loading Support
 * 
 ***************************************************************************/

#include "bstd.h"

#include "bhsm.h"
#include "bhsm_keyladder.h"
#include "bhdcplib_keyloader.h"
#include "breg_endian.h"

BDBG_MODULE(BHDCPLIB_KEYLOADER) ;


BERR_Code BHDCPlib_FastLoadEncryptedHdcpKeys(
	BHDCPlib_Handle hHDCPlib)
{
#if (BCHP_CHIP ==7550) || (BCHP_CHIP == 7408) || BHDM_CONFIG_40NM_SUPPORT 
	
	BDBG_WRN(("*********************************************************")) ;
	BDBG_WRN(("HDCP feature is disabled. HDCP authentication will")) ;
	BDBG_WRN(("not succeed. Please rebuild the application with ")) ; 
	BDBG_WRN(("NEXUS_HDCP_SUPPORT=y to enable HDCP")) ;
	BDBG_WRN(("*********************************************************")) ;

	BSTD_UNUSED(hHDCPlib);

	/* HDCP will not authenticate; force the app to exiit */
	BDBG_ASSERT(false) ;
	return BERR_NOT_SUPPORTED;
#else

	uint8_t  i ;
	uint32_t HdcpKey[4] ;
		
	BERR_Code rc = BERR_SUCCESS ;
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_LoadRouteUserKeyIO_t loadRouteUserKeyIO;	
	BHDCPlib_EncryptedHdcpKeyStruct * EncryptedHdcpKeys;
	
	BDBG_ASSERT(hHDCPlib) ;


	/* Get HDCP Encrypted Keys */
	EncryptedHdcpKeys = (BHDCPlib_EncryptedHdcpKeyStruct *) &hHDCPlib->stHdcpConfiguration.TxKeySet.TxKeyStructure;

	BDBG_WRN(("*********************************************************")) ;
	BDBG_WRN(("Loading HDCP Spec 1.1 A1 Transmitter Test Keys...")) ;
	BDBG_WRN(("HDCP Test and Production Keys are not compatible")) ; 
	BDBG_WRN(("Test Keys WILL NOT AUTHENTICATE with production TVs, RXs")) ;
	BDBG_WRN(("Separate Agreement needed for use of Encrypted Key Loader")) ;
	BDBG_WRN(("*********************************************************")) ;

	
	HdcpKey[1] = 0;
	HdcpKey[0] = 0;
	
	BDBG_MSG(("Start HDCP Key Loading")) ;
	for(i= 0; i < BHDM_HDCP_NUM_KEYS ; i++)
	{
		HdcpKey[3] = EncryptedHdcpKeys[i].HdcpKeyLo ;
		HdcpKey[2] = EncryptedHdcpKeys[i].HdcpKeyHi ;
		
		BREG_LE32(HdcpKey[3]) ;
		BREG_LE32(HdcpKey[2]) ;
		
#if 0
		BDBG_MSG(("%08x%08x %08X %08X %08X %08X %d", 
			EncryptedHdcpKeys[i].HdcpKeyHi, EncryptedHdcpKeys[i].HdcpKeyLo,
			HdcpKey[3], HdcpKey[2], HdcpKey[1], HdcpKey[0], i)) ;
#endif			

		loadRouteUserKeyIO.keySource =	BCMD_KeyRamBuf_eSecondRam;	/* BCMD_KeyRamBuf_eFirstRam;*/
		loadRouteUserKeyIO.keySize.eKeySize =  BCMD_KeySize_e64;
		BKNI_Memset(loadRouteUserKeyIO.aucKeyData, 0, sizeof(loadRouteUserKeyIO.aucKeyData));

		loadRouteUserKeyIO.aucKeyData[3] =	(HdcpKey[2] >> 24) & 0xff ;
		loadRouteUserKeyIO.aucKeyData[2] =	(HdcpKey[2] >> 16) & 0xff ;
		loadRouteUserKeyIO.aucKeyData[1] =	(HdcpKey[2] >>	8) & 0xff ;
		loadRouteUserKeyIO.aucKeyData[0] =	(HdcpKey[2]) & 0xff ;
		
		loadRouteUserKeyIO.aucKeyData[7] =	(HdcpKey[3] >> 24) & 0xff ;
		loadRouteUserKeyIO.aucKeyData[6] =	(HdcpKey[3] >> 16) & 0xff ;
		loadRouteUserKeyIO.aucKeyData[5] =	(HdcpKey[3] >>	8) & 0xff ;
		loadRouteUserKeyIO.aucKeyData[4] =	(HdcpKey[3]) & 0xff ;	

		
		loadRouteUserKeyIO.bIsRouteKeyRequired = true ;
		loadRouteUserKeyIO.keyDestBlckType	 =	BCMD_KeyDestBlockType_eHdmi ;
		loadRouteUserKeyIO.keyDestEntryType = BCMD_KeyDestEntryType_eOddKey; /* Aegis does not care */ 
		loadRouteUserKeyIO.caKeySlotType	  = BCMD_XptSecKeySlot_eType0;	   /* Aegis does not care */ 
		loadRouteUserKeyIO.unKeySlotNum = i ;					

		errCode= BHSM_LoadRouteUserKey (hHDCPlib->stDependencies.hHsm, &loadRouteUserKeyIO); 
		if (errCode != 0) 
		{
			BDBG_MSG(("loadRouteUserKeyIO unStatus = 0x%08X", loadRouteUserKeyIO.unStatus)) ;
			BDBG_ERR(("BHSM_LoadRouteUserKey errCode: %x", errCode )) ; 
			return errCode;
		}	
	}

	BDBG_MSG(("END HDCP Key Loading")) ;

	return rc ;
#endif
}


