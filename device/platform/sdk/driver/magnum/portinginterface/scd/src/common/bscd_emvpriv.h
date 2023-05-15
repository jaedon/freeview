/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bscd_emvpriv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 11:00a $
 *
 * Module Description: This file contains Broadcom smart card Porting 
 *                     Interface EMV private data structures, enums, 
 *                     definitions and functions prototypes.           
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/scd/src/common/bscd_emvpriv.h $
 * 
 * Hydra_Software_Devel/1   9/7/12 11:00a qxie
 * SW7429-283:  Smartcard: remove symlink in PI code
 * 
 * Hydra_Software_Devel/5   7/20/04 2:12p btan
 * PR 10898: Pass EMV 2000 tests with either event interrupt or WWT to
 * detect CWT +4
 * 
 * Hydra_Software_Devel/4   12/31/03 3:37p btan
 * PR 9168: Fixed compilation warnings.
 * 
 * Hydra_Software_Devel/3   11/26/03 5:00p btan
 * Fixes for EMV T=0
 * 
 * Hydra_Software_Devel/1   10/13/03 12:18p btan
 * Initial Creation.
 * 
 * Hydra_Software_Devel/4   7/21/03 6:28p btan
 * Updated after 1st review.
 * 
 * Hydra_Software_Devel/2   6/10/03 5:56p btan
 * Fixed some comments.
 * 
 * Hydra_Software_Devel/1   6/5/03 4:58p btan
 * Initial creation for 7038
 *
 ***************************************************************************/
#ifndef BSCD_EMV_PRIV_H__
#define BSCD_EMV_PRIV_H__

#include "berr_ids.h"
#include "bscd.h"

#ifdef __cplusplus
extern "C" {
#endif


BERR_Code BSCD_Channel_P_EMVATRReadNextByte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char *outp_ucData, 
		unsigned long *outp_ulTotalAtrByteTimeInETU, 
		unsigned char *outp_ucParityErrorDetected 
);

BERR_Code BSCD_Channel_P_EMVATRCheckForAdditionalATRBytes(
		BSCD_ChannelHandle	in_channelHandle
);

BERR_Code BSCD_Channel_P_EMVATRReceiveAndDecode(
		BSCD_ChannelHandle	in_channelHandle
);


BERR_Code BSCD_Channel_P_EMVValidateTSByte(
		unsigned char in_ucTSByte
);

BERR_Code BSCD_Channel_P_EMVValidateT0Byte(
      unsigned char ucT0Byte, 
      unsigned int  *unNumHistoricalBytes
);

BERR_Code BSCD_Channel_P_EMVValidateTA1Byte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char in_ucTA1Byte
);

BERR_Code BSCD_Channel_P_EMVValidateTB1Byte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char in_ucTB1Byte
);

BERR_Code BSCD_Channel_P_EMVValidateTC1Byte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char in_ucTC1Byte
);

BERR_Code BSCD_Channel_P_EMVValidateTD1Byte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char in_ucTD1Byte, 
		bool *outp_bTCKRequired
);


BERR_Code BSCD_Channel_P_EMVValidateTA2Byte(
		unsigned char in_ucTA2Byte
);


BERR_Code BSCD_Channel_P_EMVValidateTB2Byte(
		unsigned char in_ucTB2Byte
);

BERR_Code BSCD_Channel_P_EMVValidateTC2Byte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char in_ucTC2Byte		
);

BERR_Code BSCD_Channel_P_EMVValidateTD2Byte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char in_ucTD2Byte, 
		unsigned char in_ucTD1Byte, 
		bool * outp_bTCKRequired
);

BERR_Code BSCD_Channel_P_EMVValidateTA3Byte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char in_ucTA3Byte
);

BERR_Code BSCD_Channel_P_EMVValidateTB3Byte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char in_ucTB3Byte, 
		unsigned char in_ucFFactor, 
		unsigned char in_ucDFactor
);

BERR_Code BSCD_Channel_P_EMVValidateTC3Byte(
		unsigned char in_ucTC3Byte
);

BERR_Code BSCD_Channel_P_EMVValidateTCKByte(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char *inp_ucATR, 
		unsigned int in_unATRLength
);

BERR_Code BSCD_Channel_P_EMVATRByteRead(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char *inoutp_ucData,
		unsigned long in_ulMaxAtrByteTimeInETU,
		long in_lMaxTotalAtrByteTimeInETU,
		unsigned long *inoutp_ultotalAtrByteTimeInETU 
);



#ifdef __cplusplus
}
#endif

#endif /* BSCD_PRIV_H__ */
