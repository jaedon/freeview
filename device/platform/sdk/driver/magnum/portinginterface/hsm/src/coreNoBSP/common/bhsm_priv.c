/***************************************************************************
 *     (c)2007-2008 Broadcom Corporation
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
 * $brcm_Workfile: bhsm_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/1/12 9:26a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/src/coreNoBSP/common/bhsm_priv.c $
 * 
 * Hydra_Software_Devel/5   10/1/12 9:26a atruong
 * SWSECURITY-53: Code maintenance - fixed BCHP_CHIP typo
 * 
 * Hydra_Software_Devel/4   9/28/12 3:50p atruong
 * SWSECURITY-175: Symbolic link removal for 7408 and 7550 HSM PI
 * 
 * Hydra_Software_Devel/3   2/24/11 4:28p haoboy
 * SW7550-692: fixed a bug
 * 
 * Hydra_Software_Devel/2   2/23/11 3:06p haoboy
 * SW7550-692:added AES support for keyladder operation
 * 
 * Hydra_Software_Devel/1   1/21/11 9:50a haoboy
 * SW7550-666:initial version
 * 
 * Hydra_Software_Devel/10   11/26/10 5:00p jhaberf
 * SW35125-13: Added 35125 DTV chip support
 * 
 * Hydra_Software_Devel/9   6/11/10 10:03a qcheng
 * SW7550-427 : Coverity - fix the 7550 "magnum_pi_hsm" module specific
 * coverity defects
 * 
 * Hydra_Software_Devel/8   6/2/10 2:32p cdisc
 * SW35230-51:   Magnum HSM PI needs to build for the 35230
 * 
 * Hydra_Software_Devel/7   4/2/10 7:29p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 * PVR function
 * 
 * Hydra_Software_Devel/6   3/22/10 7:04p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/5   3/4/10 11:33a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/5   3/4/10 11:31a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/4   2/19/10 6:17p nitinb
 * SW7550-270: Changed printf to BDBG_MSG(). printf causes problem in
 * kernel mode build
 * 
 * 
 * Hydra_Software_Devel/3   1/15/10 6:27p qcheng
 * SW7550-188: Support HDMI/HDCP for 7550
 * 
 * Hydra_Software_Devel/2   12/30/09 1:49a rnayak
 * SW7550-102: 7550 HSM files were not properly added to source control.
 * Adding them again
 * 
 * Hydra_Software_Devel/1   12/4/09 5:26p qcheng
 * SW7550-102 : Add Security support for 7550
 *
 **************************************************************************/

#include "bhsm_priv.h"

#include "bstd.h" 
#include "bkni.h"
#if ( BCHP_CHIP == 7550)
#include "chip_secure_7550.h"
#elif ( BCHP_CHIP == 7408)
#include "xpt_mmscram.h"
#elif ( BCHP_CHIP == 35230) || (BCHP_CHIP == 35125 ) || ( BCHP_CHIP == 35233 )
#include "chip_secure_35230.h"
#endif


BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */

BERR_Code BHSM_P_ProcessKey(
	struct BHSM_P_Handle     *in_handle,
	BHSM_P_ProcessKeyIO_t		*inp_processKeyIO
)
{
	uint32_t  regVal;

	BSTD_UNUSED(in_handle);

	if (  inp_processKeyIO->unCustKey > BHSM_MAX_CUST_KEY) {
		BDBG_WRN(("%s: invalid Customer Key (%d)", __func__, inp_processKeyIO->unCustKey));
		return BHSM_STATUS_INPUT_PARM_ERR;
	}

	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_PROC_IN_HI, inp_processKeyIO->procInHigh);
	BDBG_MSG(("XPT_SECURITY_PROC_IN_HI(0x%08x) = 0x%08x\n", XPT_SECURITY_PROC_IN_HI,inp_processKeyIO->procInHigh ));	

	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_PROC_IN_LO, inp_processKeyIO->procInLow);
	BDBG_MSG(("XPT_SECURITY_PROC_IN_LO(0x%08x) = 0x%08x\n", XPT_SECURITY_PROC_IN_LO,inp_processKeyIO->procInLow ));	


	regVal  = ((inp_processKeyIO->eKeyLadderModule << XPT_SECURITY_KEYGEN_CTRL_PROC_MMSCRAM_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_MMSCRAM_MASK) |
			((inp_processKeyIO->eKeyProcess << XPT_SECURITY_KEYGEN_CTRL_PROC_DEST_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_DEST_MASK) |
			((inp_processKeyIO->eOperation <<XPT_SECURITY_KEYGEN_CTRL_PROC_CA_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_CA_MASK) |
			((inp_processKeyIO->unCustKey << XPT_SECURITY_KEYGEN_CTRL_PROC_CUS_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_CUS_MASK ) |
			((inp_processKeyIO->unKeyVar << XPT_SECURITY_KEYGEN_CTRL_KEY_VAR_SHIFT) & XPT_SECURITY_KEYGEN_CTRL_KEY_VAR_MASK) |
			((inp_processKeyIO->unKeyDest << XPT_SECURITY_KEYGEN_CTRL_KEY_ADDR_SHIFT) & XPT_SECURITY_KEYGEN_CTRL_KEY_ADDR_MASK) ;
	/*Start Key Ladder Operation */
	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_KEYGEN_CTRL, regVal);
	BDBG_MSG(("XPT_SECURITY_KEYGEN_CTRL(0x%08x) = 0x%08x\n", XPT_SECURITY_KEYGEN_CTRL,regVal ));	

#if (BCHP_CHIP != 7408)
	regVal = BREG_Read32( in_handle ->regHandle,XPT_SECURITY_SECURITY_ERROR);
	BDBG_MSG(("XPT_SECURITY_SECURITY_ERROR = 0x%08x !\n", regVal));
#endif	

	if ( inp_processKeyIO->eKeyProcess == BHSM_KeyGenProcess_eDeNonceToProcOut ) {
		inp_processKeyIO->procOutHigh = BREG_Read32 (in_handle ->regHandle, XPT_SECURITY_PROC_IN_HI);
		BDBG_MSG(("XPT_SECURITY_PROC_IN_HI(0x%08x) = 0x%08x\n", XPT_SECURITY_PROC_IN_HI,inp_processKeyIO->procInHigh ));	

		inp_processKeyIO->procOutLow = BREG_Read32 (in_handle ->regHandle, XPT_SECURITY_PROC_IN_LO);
		BDBG_MSG(("XPT_SECURITY_PROC_IN_LO(0x%08x) = 0x%08x\n", XPT_SECURITY_PROC_IN_LO,inp_processKeyIO->procInLow ));	
	}
	
	return BERR_SUCCESS;
}

void BHSM_P_EnableSecurity(
	struct BHSM_P_Handle     *in_handle
	)
{
#if ( BCHP_CHIP != 7408)
	uint32_t regVal;

	BDBG_MSG(("XPT_SECURITY_SECURITY_CTRL Addres = 0x%08x\n", XPT_SECURITY_SECURITY_CTRL));	

	regVal = BREG_Read32( in_handle->regHandle,XPT_SECURITY_SECURITY_CTRL);

#if ( BCHP_CHIP == 7408)
      regVal |= XPT_SECURITY_SECURITY_CTRL_reserved_for_eco1_MASK;
#else
      regVal |= XPT_SECURITY_SECURITY_CTRL_SECURITY_EN_MASK;
#endif

	/* XPT_SECURITY_SECURITY_CTRL: SECURITY_EN = 1); */
	BDBG_MSG(("Enable Security ( XPT_SECURITY_SECURITY_CTRL = 0x%08x ) !\n", regVal));	

	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_SECURITY_CTRL, regVal);		
	BDBG_MSG(("XPT_SECURITY_SECURITY_CTRL(0x%08x) = 0x%08x\n", XPT_SECURITY_SECURITY_CTRL,regVal ));	


	regVal = BREG_Read32( in_handle->regHandle,XPT_SECURITY_SECURITY_CTRL);
	BDBG_MSG(("Read XPT_SECURITY_SECURITY_CTRL = 0x%08x !\n", regVal));	
#else
 	BSTD_UNUSED(in_handle);

	return;
#endif


}

void BHSM_P_DisableSecurity(
	struct BHSM_P_Handle     *in_handle
	)
{
#if ( BCHP_CHIP != 7408)
	uint32_t regVal;

	regVal = BREG_Read32( in_handle->regHandle,XPT_SECURITY_SECURITY_CTRL);

#if ( BCHP_CHIP == 7408)
      regVal &= ~(XPT_SECURITY_SECURITY_CTRL_reserved_for_eco1_MASK);
#else
      regVal &= ~(XPT_SECURITY_SECURITY_CTRL_SECURITY_EN_MASK);
#endif

	/* XPT_SECURITY_SECURITY_CTRL: SECURITY_EN = 0); */
	BDBG_MSG(("Disable Security ( XPT_SECURITY_SECURITY_CTRL = 0x%08x ) !\n", regVal));	
	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_SECURITY_CTRL, regVal);	

#else
 	BSTD_UNUSED(in_handle);

	return;
#endif


}


BERR_Code BHSM_P_ReqCustMode(
	struct BHSM_P_Handle     *in_handle,
	BHSM_CustMode_e	mode
)
{
	uint32_t  procInLow, procInHigh;
	BHSM_P_ProcessKeyIO_t processKeyIO;

	procInLow = 0;
	procInHigh = 0;

	/* starting with BCHP_VER > BCHP_VER_A0 mode is defined in header file as 8 bit quantities */
#if (BCHP_VER <= BCHP_VER_A0)
	procInHigh	|=	(( (mode >> 4) & 0xf  ) << 28 ) ;
#else
	procInHigh  |=  (( mode & 0xff )         << 24 ) ;
#endif
		
	BDBG_MSG(("Request Cust Mode = 0x%x !\n", mode));	

	processKeyIO.eKeyLadderModule = BHSM_KeyLadderModule_eSecurity ;
	processKeyIO.eKeyProcess = BHSM_KeyGenProcess_eCusMode;
	processKeyIO.unCustKey = 0 ;
	processKeyIO.unKeyVar = 0;
	processKeyIO.procInHigh = procInHigh;
	processKeyIO.procInLow = procInLow;
	processKeyIO.eOperation = BHSM_Operation_eCA;
	processKeyIO.unKeyDest = 0 ; 
	
	check(BHSM_P_ProcessKey( in_handle , &processKeyIO ));

	in_handle->eCustMode = mode ;
	return BERR_SUCCESS;

}


#if (BCHP_CHIP != 7408)

BERR_Code BHSM_P_ProcessKeyAes(
	struct BHSM_P_Handle        *in_handle,
	BHSM_P_ProcessKeyIO_t	*inp_processKeyIO
)
{
	uint32_t  regVal;

	BSTD_UNUSED(in_handle);

	if (  inp_processKeyIO->unCustKey > BHSM_MAX_CUST_KEY) {
		BDBG_WRN(("%s: invalid Customer Key (%d)", __func__, inp_processKeyIO->unCustKey));
		return BHSM_STATUS_INPUT_PARM_ERR;
	}

	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_PROC_IN_AES_127_96, inp_processKeyIO->procInAesHigh);
	BDBG_MSG(("XPT_SECURITY_PROC_IN_AES_127_96(0x%08x) = 0x%08x\n", XPT_SECURITY_PROC_IN_AES_127_96,inp_processKeyIO->procInAesHigh ));	

	
	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_PROC_IN_AES_95_64, inp_processKeyIO->procInAesLow);
	BDBG_MSG(("XPT_SECURITY_PROC_IN_AES_95_64(0x%08x) = 0x%08x\n", XPT_SECURITY_PROC_IN_AES_95_64,inp_processKeyIO->procInAesLow ));	
	 
	
	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_PROC_IN_HI, inp_processKeyIO->procInHigh);
	BDBG_MSG(("XPT_SECURITY_PROC_IN_HI(0x%08x) = 0x%08x\n", XPT_SECURITY_PROC_IN_HI,inp_processKeyIO->procInHigh ));	
	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_PROC_IN_LO, inp_processKeyIO->procInLow);
	BDBG_MSG(("XPT_SECURITY_PROC_IN_LO(0x%08x) = 0x%08x\n", XPT_SECURITY_PROC_IN_LO,inp_processKeyIO->procInLow ));	


	regVal  = ((inp_processKeyIO->eKeyLadderModule << XPT_SECURITY_KEYGEN_CTRL_PROC_MMSCRAM_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_MMSCRAM_MASK) |
			((1 << XPT_SECURITY_KEYGEN_CTRL_PROC_DEST_AES_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_DEST_AES_MASK) |
			((inp_processKeyIO->eKeyProcess << XPT_SECURITY_KEYGEN_CTRL_PROC_DEST_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_DEST_MASK) |
			((inp_processKeyIO->eOperation <<XPT_SECURITY_KEYGEN_CTRL_PROC_CA_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_CA_MASK) |
			((inp_processKeyIO->unCustKey << XPT_SECURITY_KEYGEN_CTRL_PROC_CUS_SHIFT ) & XPT_SECURITY_KEYGEN_CTRL_PROC_CUS_MASK ) |
			((inp_processKeyIO->unKeyVar << XPT_SECURITY_KEYGEN_CTRL_KEY_VAR_SHIFT) & XPT_SECURITY_KEYGEN_CTRL_KEY_VAR_MASK) |
			((inp_processKeyIO->unKeyDest << XPT_SECURITY_KEYGEN_CTRL_KEY_ADDR_SHIFT) & XPT_SECURITY_KEYGEN_CTRL_KEY_ADDR_MASK) ;


	/*for AES KL, if the destination is M2M, route to SHARF*/
	if (inp_processKeyIO->eKeyLadderModule == BHSM_KeyLadderModule_eMmscram)
		{
		printf("regVal=%x \n", regVal);
		regVal = regVal | ( (1<<XPT_SECURITY_KEYGEN_CTRL_PROC_SHARF_SHIFT) & XPT_SECURITY_KEYGEN_CTRL_PROC_SHARF_MASK);
				printf("regVal=%x \n",  ( (1<<XPT_SECURITY_KEYGEN_CTRL_PROC_SHARF_SHIFT) & XPT_SECURITY_KEYGEN_CTRL_PROC_SHARF_MASK));
		}
		
	/*Start Key Ladder Operation */
	BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_KEYGEN_CTRL, regVal);
	BDBG_MSG(("XPT_SECURITY_KEYGEN_CTRL(0x%08x) = 0x%08x\n", XPT_SECURITY_KEYGEN_CTRL,regVal ));	

	regVal = BREG_Read32( in_handle ->regHandle,XPT_SECURITY_SECURITY_ERROR);
	BDBG_MSG(("XPT_SECURITY_SECURITY_ERROR = 0x%08x !\n", regVal));	

	return BERR_SUCCESS;
}

#endif
