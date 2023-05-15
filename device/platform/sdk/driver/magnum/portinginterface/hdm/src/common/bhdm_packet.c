/***************************************************************************
*	  (c)2004-2010 Broadcom Corporation
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
*  1.	  This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bhdm_packet.c $
 * $brcm_Revision: Hydra_Software_Devel/28 $
 * $brcm_Date: 7/12/12 6:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/src/common/bhdm_packet.c $
 * 
 * Hydra_Software_Devel/28   7/12/12 6:27p vle
 * SW7422-465: merge Bluray changes
 * 
 * Hydra_Software_Devel/27   6/29/12 3:28p vle
 * SW7425-2851: Fix coverity error
 * 
 * Hydra_Software_Devel/26   5/16/12 9:59a rgreen
 * SW7231-791: Merge Changes
 * 
 * Hydra_Software_Devel/SW7231-791/1   5/5/12 11:52a rgreen
 * SW7231-791: Add support for DVB AFD in HDMI AVI Info Frame
 * 
 * Hydra_Software_Devel/25   3/6/12 6:37p rgreen
 * SW7425-2572: Set AVI Infoframe to <No Data> when unknown aspect ratio
 * is Set.  Do not report uncompatible with HDMI message
 * 
 * Hydra_Software_Devel/24   3/2/12 10:07a rgreen
 * SW7408-320: Fix bug introduced by clearing AudioInfoFrame argument in
 * BHDM_SetAudioInfoFrame.  Argument can be same as internally stored
 * AudioInfoFrame.  See  BHDM_EnableDisplay.  Fix same issue for VSI
 * packet.
 * 
 * Hydra_Software_Devel/23   2/21/12 10:20a rgreen
 * SW7408-320: Merge Changes
 * 
 * Hydra_Software_Devel/SW7408-320/3   2/21/12 9:53a rgreen
 * SW7408-320: Bound extended colorimetry feature with HDMI 1.3 macro
 * 
 * Hydra_Software_Devel/SW7408-320/2   2/20/12 3:34p rgreen
 * SW7408-320: Clear bOverrideDefaults flag after each use;  Caller must
 * specify each instance that settings must be overrwritten
 * 
 * Hydra_Software_Devel/SW7408-320/1   2/20/12 1:20p rgreen
 * SW7408-320:  Fix Set AVI InfoFrame to correctly update/keep all fields;
 * Ensure Get/Set calls initalize packets to 0.
 * 
 * Hydra_Software_Devel/21   1/4/12 3:31p rgreen
 * SW7405-5004: Use BAVC_HDMI_xxx macros when copying InfoFrame contents.;
 * Add debug info packet transmission.
 * 
 * Hydra_Software_Devel/20   11/2/11 1:18p vle
 * SW7420-2117: Save VSI settings in all cases.
 * 
 * Hydra_Software_Devel/19   10/21/11 4:49p rgreen
 * SW7125-1128,SW7422-114: Initialize all RAM Packets to NULL Packets
 * 
 * Hydra_Software_Devel/18   9/15/11 4:27p rgreen
 * SW7425-1296: Fix bug with SPD Infoframe description length
 * 
 * Hydra_Software_Devel/17   8/4/11 2:36p vle
 * SW7422-464: fix typo
 * 
 * Hydra_Software_Devel/16   8/4/11 12:19p vle
 * SW7422-464: Save the VendorSpecificInfoFrame when setting it.
 *
 * Hydra_Software_Devel/15   8/3/11 1:06p rgreen
 * SW7422-461:   Add packet type, version, and length to the printed debug
 * information
 * 
 * Hydra_Software_Devel/14   7/20/11 12:30p vle
 * SW7422-461: Fix incorrect VendorSpecificInfoFrame generated for non-3D
 * formats
 * 
 * Hydra_Software_Devel/13   4/11/11 3:37p rgreen
 * SW7420-579: Remove warnings when dEBUG_AVI_INFOFRAME is set.
 * 
 * Hydra_Software_Devel/12   2/17/11 5:53p vle
 * SW7420-1289: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7420-1289/2   2/17/11 10:02a vle
 * SW7420-1289: Add NEXUS APIs to get/set HDMI AVI InfoFrame, HDMI Audio
 * Infoframe. Share structures for hdmi_input and hdmi_output
 * 
 * Hydra_Software_Devel/SW7420-1289/1   1/27/11 2:35p vle
 * SW7420-1289: Add NEXUS APIs to get/set HDMI AVI InfoFrame, HDMI Audio
 * Infoframe
 * 
 * Hydra_Software_Devel/11   1/17/11 7:12p vle
 * SW7405-5094: Fix error in forming Vendor Specific InfoFrame which
 * causes HDMI compliance test failure on test 7-38
 * 
 * Hydra_Software_Devel/10   10/7/10 7:36p jtna
 * SW7420-972: BHDM_P_WritePacket requires BHCP_PWR_Acquire/Release
 * 
 * Hydra_Software_Devel/9   9/21/10 7:28p rgreen
 * SW7420-737,SW7125-537: Fix Audio InfoFrame Channel Count  Update debug
 * messages for Audio InfoFrame
 * 
 * Hydra_Software_Devel/8   5/13/10 5:18p vle
 * SW7420-579: Fix build error if enable DEBUG_AVI_INFOFRAME debug build.
 * 
 * Hydra_Software_Devel/7   4/15/10 6:59p rgreen
 * SW7405-3997:: Update debug display info for VSI
 * 
 * Hydra_Software_Devel/6   4/14/10 7:19p rgreen
 * SW7405-3997: Fix creation of IEEE RegID for VSI transmission: SW7405-
 * 3997:
 * 
 * Hydra_Software_Devel/5   4/5/10 4:58p rgreen
 * SW7405-3997:Merge changes for debug messages
 * 
 * Hydra_Software_Devel/SW7401-4363/1   4/5/10 4:56p rgreen
 * SW7405-3997:Use debug messages in addition to enums to indicate the VSI
 * information sent
 * 
 * Hydra_Software_Devel/4   4/2/10 6:40p vle
 * SW7601-172: Rename to clearly indicate SetGamutMetadataPacket is a
 * private API.
 * 
 * Hydra_Software_Devel/3   4/2/10 5:31p rgreen
 * SW7405-3994,SW7405-3997: Report required 3D Structure vs 3D optional
 * Ext Data when sending 3D info
 * 
 * Hydra_Software_Devel/2   3/26/10 4:34p vle
 * SW7601-172: Merge xvYCC support with Gamut Metadata Packet transmission
 * from bdvd branch
 * 
 * Hydra_Software_Devel/1   2/23/10 12:41a vle
 * SW7420-579: Refactor HDMI PI code.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bhdm.h"
#include "bhdm_priv.h"
#include "bavc_hdmi.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

BDBG_MODULE(BHDM_PACKET) ;

#define BHDM_CHECK_RC( rc, func )			  \
do												  \
{												  \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{											  \
		goto done;								  \
	}											  \
} while(0)


#define BHDM_P_WORDS_PER_RAM_PACKET 9
#define BHDM_P_NUM_CONFIGURABLE_RAM_PACKETS 14
	
#define BHDM_REFER_TO_STREAM_HEADER 0 
	
static const uint16_t BHDM_Packet_AVIFrameType	  = 0x82 ; 
static const uint8_t  BHDM_Packet_AVIFrameVersion = 0x02 ;
static const uint8_t  BHDM_Packet_AVIFrameLength  = 0x0D ;

static const uint16_t BHDM_Packet_SPDFrameType	  = 0x83 ;
static const uint8_t  BHDM_Packet_SPDFrameVersion = 0x01 ;
static const uint8_t  BHDM_Packet_SPDFrameLength  = 25	;

static const uint16_t BHDM_Packet_AudioFrameType	= 0x84 ; 
static const uint8_t  BHDM_Packet_AudioFrameVersion = 0x01 ;
static const uint8_t  BHDM_Packet_AudioFrameLength	= 0x0A ;


#if BHDM_CONFIG_HDMI_1_3_SUPPORT
/*
 * As per Chris Pasqualino:
 * ========================
 * This GBD specification based on GBD format 1, 12-bit data, defined as an RGB expression 
 * of xvYCC709 coordinates.  This profile provides the min/max non-gamma corrected limited 
 * range RGB values in the transmitted stream.	 
 * 
 * Header Byte 1 = 0x81 = 1_000_0001b:
 *	Affected_Gamut_Seq_Num = 1	This is the first gamut provided
 *	GBD_profile = 0 		P0 Profile
 *	Next_Field = 1			This GBD data applies to the next field
 * 
 * Header Byte 2 = 0x31 = 0_0_11_0001b:
 *	Current_Gamut_Seq_Num = 1	This gamut number being transmitted.
 *	Packet_Seq = 3			The only packet in the sequence
 *	No_Current_GBD = 0		A GBD is applicable to next video field.
 * 
 * From the header, we know this is a profile 0 GBD.  This is important because it means 
 * that there are no length parameters stored in the subpacket data. Thus, the MSB of the 
 * first byte in subpacket 0 indicates the format of the remainder of the packet.
 * 
 * Subpacket 0, Byte 0 = 0x92 = 1_0_0_10_010b:
 *	Format_Flag = 1 		GBD data is a range description
 *	GBD_Color_Precision = 2 	12-bit precision on GBD data
 *	GBD_Color_Space = 2 	RGB Expression of xvYCC709 coordinates
 * 
 * The HDMI spec states in section E.4 that for Format_Flag = 1, the Packet_Range_Data 
 * shall be stored in the following order:
 *	Min_Red_Data 
 *	Max_Red_Data 
 *	Min_Green_Data 
 *	Max_Green_Data 
 *	Min_Blue_Data 
 *	Max_Blue_Data
 */
static const uint8_t BHDM_Packet_GamutMetadataType = 0x0A ; 
static const uint8_t BHDM_Packet_GamutMetadataHB1  = 0x81 ;
static const uint8_t BHDM_Packet_GamutMetadataHB2  = 0x31 ;
static const uint8_t BHDM_Packet_GamutMetadataSubPkt[] = {
		0x92, 0x9B, 0x52, 0xF4, 0x8D, 
		0x72, 0x96, 0x8C, 0xC2, 0x92
};
#endif

/***************************************************************************
BHDM_P_InitializePacketRAM
Summary: Configure/Initialize the Packet RAM with NULL Packets 
****************************************************************************/
BERR_Code BHDM_InitializePacketRAM(
   BHDM_Handle hHDMI		  /* [in] HDMI handle */
) 
{
	BERR_Code	rc = BERR_SUCCESS;
	uint32_t	Register ;
	uint32_t RegAddr ;
	uint8_t Packet ;
	
	/*CP*  13 Enable/Configure RAM Packets - HDMI_TODO Add PUBLIC FUNCTION(s)??? */ 
	
	/* This will configure a NULL packet for sending.  
	   This is just to get things up and running. */

	/* set	 RAM_ENABLE  */
	/* set	 ACTIVE_RAM_PACKETS = 14'd1   - Enable RAM Packet 0 */


	/* Zero (NULL) out all RAM Packets */
	for (Packet = 0 ; Packet < BHDM_P_NUM_CONFIGURABLE_RAM_PACKETS; Packet++)
	{
		for (Register = 0 ; Register < BHDM_P_WORDS_PER_RAM_PACKET ; Register++)
		{
			RegAddr = BCHP_HDMI_RAM_GCP_0 
				+ Packet * BHDM_P_WORDS_PER_RAM_PACKET * 4 
				+ Register * 4 ;
			
			BREG_Write32(hHDMI->hRegister, RegAddr, (uint32_t) 0) ;
		}
	}

	/* Enable the Packet Ram */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RAM_PACKET_CONFIG) ;
	Register |= BCHP_FIELD_DATA(HDMI_RAM_PACKET_CONFIG, RAM_ENABLE, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_PACKET_CONFIG,  Register) ;

	return rc ;
}


BERR_Code BHDM_EnablePacketTransmission(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BHDM_Packet PacketId)
{
	BERR_Code	   rc = BERR_SUCCESS ;
	uint32_t Register ;
	uint32_t XmitPacketN ;
	uint8_t timeoutMs ;
	
	/* set the transmission bit */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RAM_PACKET_CONFIG) ;
	Register |= 1 << (uint8_t) PacketId ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_PACKET_CONFIG, Register) ;
	
	/* wait until RAM Packet Status starts transmitting (RAM_PACKET_X = 0) */
	timeoutMs = 10 ;
	do
	{
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RAM_PACKET_STATUS) ;
		XmitPacketN = Register & (1 << (uint8_t) PacketId) ;
		if (XmitPacketN)
			break ;

		BDBG_MSG(("Attempting start of RAM PACKET %d transmission", (uint8_t) PacketId)) ;
		BKNI_Sleep(1) ;
	} while ( timeoutMs-- ) ;

	if (!XmitPacketN)
	{
		BDBG_ERR(("Timeout Error starting RAM PACKET %d transmission", (uint8_t) PacketId)) ;
		rc = BERR_TIMEOUT ;
		goto done ;
	}

done:
	return rc ;
}	



BERR_Code BHDM_DisablePacketTransmission(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BHDM_Packet PacketId)
{
	BERR_Code	   rc = BERR_SUCCESS ;
	uint32_t Register ;
	uint32_t XmitPacketN ;
	uint8_t timeoutMs ;

	/* clear the transmission bit */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RAM_PACKET_CONFIG) ;
	Register &= ~(1 << (uint8_t) PacketId) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_PACKET_CONFIG, Register) ;
	
	/* wait until RAM Packet Status stops transmitting (RAM_PACKET_X = 0) */
	timeoutMs = 10 ;
	do
	{
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RAM_PACKET_STATUS) ;
		XmitPacketN = Register & (1 << (uint8_t) PacketId) ;
		if (!XmitPacketN)
			break ;

		BDBG_MSG(("Attempting stop of RAM PACKET %d transmission", (uint8_t) PacketId)) ;
		BKNI_Sleep(1) ;
	} while ( timeoutMs-- ) ;

	if (XmitPacketN)
	{
		BDBG_ERR(("Timeout Error stopping RAM PACKET %d transmission", (uint8_t) PacketId)) ;
		rc = BERR_TIMEOUT ;
		goto done ;
	}
	
done:
	return rc ;
}	



BERR_Code BHDM_P_WritePacket(
	BHDM_Handle hHDMI, BHDM_Packet PhysicalHdmiRamPacketId,
	uint8_t PacketType, uint8_t PacketVersion, uint8_t PacketLength, 
	uint8_t *PacketBytes) 
{
	BERR_Code	   rc = BERR_SUCCESS ;
	uint32_t Register ; 
	
	uint32_t PacketRegisterOffset ;
	uint32_t checksum ;
	uint8_t i ;


	/* zero out the unused packet bytes */
	for (i = PacketLength + 1 ; i < BHDM_NUM_PACKET_BYTES; i++)
		PacketBytes[i] = 0 ;

	/* calculate Packet Checksum only on CEA-861 InfoFrame Packets */
	if (PacketType > BHDM_INFOFRAME_PACKET_TYPE)
	{
		/* calculate checksum */			
		checksum = 0 ;
		checksum = PacketType + PacketVersion + PacketLength ;
		for (i = 1 ; i <= PacketLength ; i++)
			checksum = checksum + PacketBytes[i] ;
	
		PacketBytes[0] = 256 - (uint8_t) (checksum % 256)  ;
#if BHDM_CONFIG_DEBUG_HDMI_PACKETS		
		BDBG_MSG(("Infoframe Checksum Byte: %#X", PacketBytes[0])) ;
#endif	
	}
	else
	{
		/* do not modify PB0; may be used for some other purpose */
		BDBG_WRN(("Writing non-Infoframe Packet Type %#x", PacketType)) ;
	}

#ifdef BCHP_PWR_RESOURCE_HDMI_TX
	BCHP_PWR_AcquireResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX);
#endif
	/* disable transmission of the RAM Packet */
	BHDM_CHECK_RC(rc, BHDM_DisablePacketTransmission(hHDMI, PhysicalHdmiRamPacketId)) ;

	
	/* calculate the offset of where the RAM for Packet ID begins */		
	PacketRegisterOffset = 
		   BCHP_HDMI_RAM_GCP_0 
		+ BHDM_P_WORDS_PER_RAM_PACKET * 4 * PhysicalHdmiRamPacketId ;
	
	/* create/write the header use HDMI_RAM_PACKET_1_0 for Register field names */
	Register = BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_0, HEADER_BYTE_0, PacketType) 
			 | BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_0, HEADER_BYTE_1, PacketVersion)
			 | BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_0, HEADER_BYTE_2, PacketLength) ;
	BREG_Write32(hHDMI->hRegister, PacketRegisterOffset, Register) ;
	
#if BHDM_CONFIG_DEBUG_HDMI_PACKETS		
	BDBG_MSG(("Packet Type %#X Packet %d Loading...", 
		PacketType, PhysicalHdmiRamPacketId)) ;
	BDBG_MSG(("Addr [%#x] = %x", PacketRegisterOffset, Register)) ;
#endif	
	
	/* load the Packet Bytes */
	for (i = 0 ; i < BHDM_NUM_PACKET_BYTES; i = i + 7)
	{
		PacketRegisterOffset += 4 ;
		
		/* create/write the subpacket data use HDMI_RAM_PACKET_1_1 for Register field names */
		Register = 
			  BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_1, SUBPACKET_1_BYTE_0, PacketBytes[i]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_1, SUBPACKET_1_BYTE_1, PacketBytes[i+1]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_1, SUBPACKET_1_BYTE_2, PacketBytes[i+2]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_1, SUBPACKET_1_BYTE_3, PacketBytes[i+3]) ;
		BREG_Write32(hHDMI->hRegister, PacketRegisterOffset, Register) ;
#if BHDM_CONFIG_DEBUG_HDMI_PACKETS		
		BDBG_MSG(("Addr [%#x] = %#x", PacketRegisterOffset, Register)) ;
#endif		
		
		PacketRegisterOffset += 4 ;
		
		/* create/write the subpacket data use HDMI_RAM_PACKET_1_1 for Register field names */
		Register = 
			  BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_2, SUBPACKET_1_BYTE_4, PacketBytes[i+4]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_2, SUBPACKET_1_BYTE_5, PacketBytes[i+5]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_2, SUBPACKET_1_BYTE_6, PacketBytes[i+6]) ;
		BREG_Write32(hHDMI->hRegister, PacketRegisterOffset, Register) ;
#if BHDM_CONFIG_DEBUG_HDMI_PACKETS				
		BDBG_MSG(("Addr [%#x] = %#x", PacketRegisterOffset, Register)) ;
#endif		
	}
	
	/* reenable the packet transmission */
	BHDM_CHECK_RC(rc, BHDM_EnablePacketTransmission(hHDMI, PhysicalHdmiRamPacketId)) ;
#ifdef BCHP_PWR_RESOURCE_HDMI_TX
	BCHP_PWR_ReleaseResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX);
#endif
done:
	return rc ; 
}


/******************************************************************************
Summary:
Set/Enable the Auxillary Video Information Info frame to be sent to the HDMI Rx
*******************************************************************************/
BERR_Code BHDM_SetAVIInfoFramePacket(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BAVC_HDMI_AviInfoFrame *stAviInfoFrame
)
{
	BERR_Code	   rc = BERR_SUCCESS ;
	
	BHDM_Packet PhysicalHdmiRamPacketId ;
	BAVC_HDMI_AviInfoFrame newAviInfoFrame ;
	uint8_t PacketType ;
	uint8_t PacketVersion ;
	uint8_t PacketLength ;
	uint8_t M1M0 ;
	uint8_t VideoID = 1 ;
	uint8_t PixelRepeat ;
	uint8_t C1C0 ;
	uint8_t Y1Y0 ;
	uint8_t EC2EC1EC0 = 0 ;

	/* make a local copy of AVI  */
	BKNI_Memset(&newAviInfoFrame, 0, sizeof(BAVC_HDMI_AviInfoFrame)) ;
	BKNI_Memcpy(&newAviInfoFrame, stAviInfoFrame,
		sizeof(BAVC_HDMI_AviInfoFrame)) ;

	/* initialize packet information to zero */ 
	BKNI_Memset(hHDMI->PacketBytes, 0, BHDM_NUM_PACKET_BYTES) ;

	PhysicalHdmiRamPacketId = BHDM_PACKET_eAVI_ID  ;
	
	PacketType	  = BHDM_Packet_AVIFrameType ;
	PacketVersion = BHDM_Packet_AVIFrameVersion ;
	PacketLength  = BHDM_Packet_AVIFrameLength ;


	/* Override AVI infoFrame info. Use settings from the AVI InfoFrame passed in */
	if (stAviInfoFrame->bOverrideDefaults)
	{
		/* Set RGB or YCrCb Colorspace */	
		Y1Y0 = stAviInfoFrame->ePixelEncoding;	
		
		/* Set Colorimetry */	
		C1C0 = stAviInfoFrame->eColorimetry;
		
#if BHDM_CONFIG_HDMI_1_3_SUPPORT
		if (stAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended) 
			EC2EC1EC0 = stAviInfoFrame->eExtendedColorimetry ;
#endif			
		/* Picture Aspect Ratio*/
		M1M0 = stAviInfoFrame->ePictureAspectRatio; 	

		/* Video ID Code & Pixel Repetition */
		VideoID = stAviInfoFrame->VideoIdCode;
		PixelRepeat = stAviInfoFrame->PixelRepeat;		
	}
	else
	{
		/* Set RGB or YCrCb Colorspace */	
		switch (hHDMI->DeviceSettings.eColorimetry)
		{
		default :
		case BAVC_MatrixCoefficients_eHdmi_RGB :
			Y1Y0 =	BAVC_HDMI_AviInfoFrame_Colorspace_eRGB ;
			break ;
			
		/* YCbCr output is always YCbCr 4:4:4 */		
		case BAVC_MatrixCoefficients_eItu_R_BT_709 :
		case BAVC_MatrixCoefficients_eSmpte_170M :
		case BAVC_MatrixCoefficients_eXvYCC_601 :
		case BAVC_MatrixCoefficients_eXvYCC_709 :		
			Y1Y0 =	BAVC_HDMI_AviInfoFrame_Colorspace_eYCbCr444 ;  
			break ;
		}

		
		/* Set Colorimetry */	
		switch (hHDMI->DeviceSettings.eColorimetry)
		{
		default :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eNoData ;
			break ;
			
		case BAVC_MatrixCoefficients_eItu_R_BT_709 :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eItu709 ;
			break ;
		
		case BAVC_MatrixCoefficients_eSmpte_170M :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eSmpte170 ;
			break ;
			
		case BAVC_MatrixCoefficients_eXvYCC_601 :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended ;
			EC2EC1EC0 = BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvYCC601 ;
			break ;

		case BAVC_MatrixCoefficients_eXvYCC_709 :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended ;
			EC2EC1EC0 = BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvYCC709 ;
			break ; 	
		}

			
		/* convert specified Aspect Ratio to use for AVI Infoframe values */
		switch (hHDMI->DeviceSettings.eAspectRatio)
		{
		default :
		case BFMT_AspectRatio_eUnknown	 : /* Unknown/Reserved */
			BDBG_WRN(("Unknown AspectRatio <%d> passed in AVI Frame",
				(uint8_t) hHDMI->DeviceSettings.eAspectRatio)) ;
			M1M0 = BAVC_HDMI_AviInfoFrame_PictureAspectRatio_eNoData ;
			break ;

		case BFMT_AspectRatio_eSquarePxl : /* square pixel */
		case BFMT_AspectRatio_e221_1	 :	   /* 2.21:1 */
			BDBG_WRN(("Specified BAVC Aspect Ratio %d, not compatible with HDMI" ,
				(uint8_t) hHDMI->DeviceSettings.eAspectRatio)) ;
			M1M0 = BAVC_HDMI_AviInfoFrame_PictureAspectRatio_eNoData ;
			break ;
		
		case BFMT_AspectRatio_e4_3		 :	
			M1M0 = BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e4_3 ;
			break ;
			
		case BFMT_AspectRatio_e16_9 	 :	
			M1M0 = BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e16_9 ;
			break ;
		}


		/* As per HDMI1.4, if HD and Aspect Ratio is 4:3, then indicate No-Data. */
		switch (hHDMI->DeviceSettings.eInputVideoFmt)
		{
		case BFMT_VideoFmt_e720p	  :
		case BFMT_VideoFmt_e720p_50Hz :
		case BFMT_VideoFmt_e720p_24Hz :
		case BFMT_VideoFmt_e720p_25Hz :
		case BFMT_VideoFmt_e720p_30Hz :
		case BFMT_VideoFmt_e1080i	  :
		case BFMT_VideoFmt_e1080i_50Hz:
		case BFMT_VideoFmt_e1080p	  :
		case BFMT_VideoFmt_e1080p_24Hz:
		case BFMT_VideoFmt_e1080p_25Hz:
		case BFMT_VideoFmt_e1080p_30Hz:
		case BFMT_VideoFmt_e1080p_50Hz:
			if (hHDMI->DeviceSettings.eAspectRatio == BFMT_AspectRatio_e4_3)
					M1M0 = BAVC_HDMI_AviInfoFrame_PictureAspectRatio_eNoData ;
		break ;
			default :
		break ;
		}


		/* Video Id Code & pixel repetitions */
		BHDM_P_VideoFmt2CEA861Code(hHDMI->DeviceSettings.eInputVideoFmt, 
			hHDMI->DeviceSettings.eAspectRatio, hHDMI->DeviceSettings.ePixelRepetition, 
			&VideoID) ;
		
		switch (VideoID) 
		{
		case  6 :  	case  7 :  
		case 14 :  	case 15 :  
		case 21 :  	case 22 :  
		case 29 :  	case 30 :  
			PixelRepeat = BAVC_HDMI_PixelRepetition_e2x;
			break;
			
		case 10 :  	case 11 :  
		case 25 :  	case 26 :  
		case 35 :  	case 36 :  
		case 37 :  	case 38 :  
			PixelRepeat = BAVC_HDMI_PixelRepetition_e4x;
			break;
			
		default:
			PixelRepeat = BAVC_HDMI_PixelRepetition_eNone;
			break;
		}	
	}	

	/* Update derived or overridden AVI fields */

	/* Pixel Encoding  */
	newAviInfoFrame.ePixelEncoding = Y1Y0 ;
	
	/* Update AVI Colorimetry */
	newAviInfoFrame.eColorimetry = C1C0;
#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	if (C1C0 == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended)
		newAviInfoFrame.eExtendedColorimetry = EC2EC1EC0;
#endif

	/* Picture Aspect Ratio */
	newAviInfoFrame.ePictureAspectRatio = M1M0 ;
	
	
	/* Video ID and Pixel Repeat */
	newAviInfoFrame.VideoIdCode = VideoID;
	newAviInfoFrame.PixelRepeat = PixelRepeat;

	/* always set the bOverrideDefaults to false */
	/* so subsequent calls to Get/Set AVI do not force override of settings */
	/* all overrides must be explicitly forced by the caller */
	newAviInfoFrame.bOverrideDefaults = false ;

	
	/* Construct AVI InfoFrame pay loads */
	hHDMI->PacketBytes[1] = 		 
		  newAviInfoFrame.ePixelEncoding << 5 /* Y1Y0 Colorspace */
		| newAviInfoFrame.eActiveInfo << 4   /*    A0 Active Information Valid */
		| newAviInfoFrame.eBarInfo << 2      /* B1B0 Bar Info Valid */
		| newAviInfoFrame.eScanInfo ;	      /* S1S0 Scan Information */

	hHDMI->PacketBytes[2] =
		  newAviInfoFrame.eColorimetry << 6   /* C1C0 Colorimetry */
		| newAviInfoFrame.ePictureAspectRatio << 4     /* M1M0 */
		| newAviInfoFrame.eActiveFormatAspectRatio ;  /* R3..R0 */

	hHDMI->PacketBytes[3] = newAviInfoFrame.eScaling ;
	if (newAviInfoFrame.eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended)
	{
#if BHDM_CONFIG_HDMI_1_3_SUPPORT
		hHDMI->PacketBytes[3] |= 
			newAviInfoFrame.eExtendedColorimetry << 4;  /* EC2EC1EC0 */
#endif
	}
	
#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	hHDMI->PacketBytes[3] |= 
		  newAviInfoFrame.eITContent << 7  /* ITC */
		| newAviInfoFrame.eRGBQuantizationRange << 2;  /* Q1Q0 */
#endif


	hHDMI->PacketBytes[4] = VideoID ;
	hHDMI->PacketBytes[5] = PixelRepeat;
#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	hHDMI->PacketBytes[5] |= 
		  newAviInfoFrame.eContentType << 4  /* CN1CN0 */
		| newAviInfoFrame.eYccQuantizationRange << 6; /* YQ1YQ0 */
#endif
 

	hHDMI->PacketBytes[6]  = (uint8_t) (newAviInfoFrame.TopBarEndLineNumber & 0x00FF) ;  
	hHDMI->PacketBytes[7]  = (uint8_t) (newAviInfoFrame.TopBarEndLineNumber >> 8) ;   
	
	hHDMI->PacketBytes[8]  = (uint8_t) (newAviInfoFrame.BottomBarStartLineNumber & 0x00FF) ;		 
	hHDMI->PacketBytes[9]  = (uint8_t) (newAviInfoFrame.BottomBarStartLineNumber >> 8) ;   
	
	hHDMI->PacketBytes[10] = (uint8_t) (newAviInfoFrame.LeftBarEndPixelNumber & 0x00FF) ;		  
	hHDMI->PacketBytes[11] = (uint8_t) (newAviInfoFrame.LeftBarEndPixelNumber >> 8) ;	
	
	hHDMI->PacketBytes[12] = (uint8_t) (newAviInfoFrame.RightBarEndPixelNumber & 0x00FF) ;		   
	hHDMI->PacketBytes[13] = (uint8_t) (newAviInfoFrame.RightBarEndPixelNumber >> 8) ;	 
	
	BHDM_P_WritePacket(
		hHDMI, PhysicalHdmiRamPacketId,
		PacketType, PacketVersion, PacketLength, hHDMI->PacketBytes) ;
		
	/* update current device settings with new AviInfoFrame settings */
	BKNI_Memcpy(&(hHDMI->DeviceSettings.stAviInfoFrame), &newAviInfoFrame, 
		sizeof(BAVC_HDMI_AviInfoFrame)) ;


#if BHDM_CONFIG_DEBUG_AVI_INFOFRAME
/*
		Y0, Y1		
			RGB or YCBCR indicator. See EIA/CEA-861B table 8 for details.
		A0			
			Active Information Present. Indicates whether field R0..R3 is valid. 
			See EIA/CEA-861B table 8 for details.
		B0, B1		
			Bar Info data valid. See EIA/CEA-861B table 8 for details.	  
		S0, S1		
			Scan Information (i.e. overscan, underscan). See EIA/CEA-861B table 8
		C0, C1		
			Colorimetry (ITU BT.601, BT.709 etc.). See EIA/CEA-861B table 9
		M0, M1		
			Picture Aspect Ratio (4:3, 16:9). See EIA/CEA-861B table 9 
		R0.R3		
			Active Format Aspect Ratio. See EIA/CEA-861B table 10 and Annex H 
		VIC0..VIC6	
			Video Format Identification Code. See EIA/CEA-861B table 13
		PR0..PR3	
			Pixel Repetition factor. See EIA/CEA-861B table 14
		SC1, SC0
			Non-uniform Picture Scaling. See EIA/CEA-861B table 11 
		EC2..EC0
			Extended Colorimetry. See EIA/CEA-861D tables 7 & 11
*/					  

	BDBG_WRN(("-------------------- NEW  AVI INFOFRAME -------------------")) ;
	BDBG_WRN(("Packet Type: 0x%02x  Version %d  Length: %d", 
		PacketType, PacketVersion, PacketLength)) ;

	/* display original bOverrideDefaults setting */
	BDBG_WRN(("Override Default: %d", stAviInfoFrame->bOverrideDefaults));

	BDBG_WRN(("ColorSpace (%d): %s", 
		newAviInfoFrame.ePixelEncoding, 
		BAVC_HDMI_AviInfoFrame_ColorspaceToStr(newAviInfoFrame.ePixelEncoding)));
	BDBG_WRN(("Active Info (%d): %s ", 
		newAviInfoFrame.eActiveInfo, BAVC_HDMI_AviInfoFrame_ActiveFormatToStr(newAviInfoFrame.eActiveInfo)));
	BDBG_WRN(("Bar Info (%d): %s ", 
		newAviInfoFrame.eBarInfo, BAVC_HDMI_AviInfoFrame_BarInfoToStr(newAviInfoFrame.eBarInfo)));
	BDBG_WRN(("Scan Info (%d): %s", newAviInfoFrame.eScanInfo, BAVC_HDMI_AviInfoFrame_ScanInfoToStr(newAviInfoFrame.eScanInfo))) ;

	BDBG_WRN(("Colorimetry (%d): %s", 
		newAviInfoFrame.eColorimetry, 
		BAVC_HDMI_AviInfoFrame_ColorimetryToStr(newAviInfoFrame.eColorimetry))) ; 

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	BDBG_WRN(("Extended Colorimetry (%d): %s", 
		newAviInfoFrame.eExtendedColorimetry, 
		BAVC_HDMI_AviInfoFrame_ExtendedColorimetryToStr(newAviInfoFrame.eExtendedColorimetry))) ;
#endif

	BDBG_WRN(("Picture AR (%d): %s", 
		newAviInfoFrame.ePictureAspectRatio, 
		BAVC_HDMI_AviInfoFrame_PictureAspectRatioToStr(newAviInfoFrame.ePictureAspectRatio))) ;
	
	if ((newAviInfoFrame.eActiveFormatAspectRatio >= 8) 
	&&	(newAviInfoFrame.eActiveFormatAspectRatio <= 11))
		BDBG_WRN(("Active Format AR (%d): %s", 
			newAviInfoFrame.eActiveFormatAspectRatio,
			BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatioToStr(newAviInfoFrame.eActiveFormatAspectRatio - 8))) ;
	else if  ((newAviInfoFrame.eActiveFormatAspectRatio > 12)
	&&	(newAviInfoFrame.eActiveFormatAspectRatio <= 15))
		BDBG_WRN(("Active Format AR (%d): %s", 
			newAviInfoFrame.eActiveFormatAspectRatio,
			BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatioToStr(newAviInfoFrame.eActiveFormatAspectRatio - 9))) ;		
	else
		BDBG_WRN(("Active Format AR (%d): Other", 
			newAviInfoFrame.eActiveFormatAspectRatio)) ;
			
	BDBG_WRN(("Picture Scaling (%d): %s ", 
		newAviInfoFrame.eScaling, BAVC_HDMI_AviInfoFrame_ScalingToStr(newAviInfoFrame.eScaling))) ;
		
	BDBG_WRN(("Video ID Code = %d", newAviInfoFrame.VideoIdCode )) ;
	BDBG_WRN(("Pixel Repeat: %d", newAviInfoFrame.PixelRepeat)) ;

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	BDBG_WRN(("IT Content (%d): %s", newAviInfoFrame.eITContent, 
		BAVC_HDMI_AviInfoFrame_ITContentToStr(newAviInfoFrame.eITContent)));
	BDBG_WRN(("IT Content Type (%d): %s", newAviInfoFrame.eContentType,
		BAVC_HDMI_AviInfoFrame_ContentTypeToStr(newAviInfoFrame.eContentType)));
	BDBG_WRN(("RGB Quantization Range (%d): %s", newAviInfoFrame.eRGBQuantizationRange,
		BAVC_HDMI_AviInfoFrame_RGBQuantizationRangeToStr(newAviInfoFrame.eRGBQuantizationRange)));
	BDBG_WRN(("Ycc Quantization Range (%d): %s", newAviInfoFrame.eYccQuantizationRange,
		BAVC_HDMI_AviInfoFrame_YccQuantizationRangeToStr(newAviInfoFrame.eYccQuantizationRange)));
#endif 
	
	BDBG_WRN(("Top Bar End Line Number: %d", newAviInfoFrame.TopBarEndLineNumber)) ;
	BDBG_WRN(("Bottom Bar Stop Line Number: %d", newAviInfoFrame.BottomBarStartLineNumber)) ;
	
	BDBG_WRN(("Left Bar End Pixel Number: %d", newAviInfoFrame.LeftBarEndPixelNumber )) ; 
	BDBG_WRN(("Right Bar End Pixel Number: %d", newAviInfoFrame.RightBarEndPixelNumber )) ;
	BDBG_WRN(("--------------------- END AVI INFOFRAME ---------------------")) ;
#endif		
		
		
	return rc ;
}	


/******************************************************************************
Summary:
Get the Auxillary Video Information Info frame sent to the HDMI Rx
*******************************************************************************/
BERR_Code BHDM_GetAVIInfoFramePacket(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BAVC_HDMI_AviInfoFrame *stAviInfoFrame
)
{
	BERR_Code	   rc = BERR_SUCCESS ;
#if BHDM_CONFIG_DEBUG_AVI_INFOFRAME 	
	uint8_t 
		Y1Y0, C1C0, M1M0, 
		EC2EC1EC0=2,
		VideoID,
		PixelRepeat;
#endif	

	BKNI_Memset(stAviInfoFrame, 0, sizeof(BAVC_HDMI_AviInfoFrame)) ;
	BKNI_Memcpy(stAviInfoFrame, &(hHDMI->DeviceSettings.stAviInfoFrame), 
		sizeof(BAVC_HDMI_AviInfoFrame)) ;


/* translate to AVI Data for debug/display purposes */		
#if BHDM_CONFIG_DEBUG_AVI_INFOFRAME 	

	if (stAviInfoFrame->bOverrideDefaults)
	{
		/* RGB or YCrCb Colorspace */	
		Y1Y0 = stAviInfoFrame->ePixelEncoding;	
		
		/* Colorimetry */	
		C1C0 = stAviInfoFrame->eColorimetry;
		
#if BHDM_CONFIG_HDMI_1_3_SUPPORT
		if (stAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended) {
			EC2EC1EC0 = stAviInfoFrame->eExtendedColorimetry;
		}
#endif	

		/* Picture Aspect Ratio*/
		M1M0 = stAviInfoFrame->ePictureAspectRatio; 	
		
		/* Video ID Code & Pixel Repetition */
		VideoID = stAviInfoFrame->VideoIdCode;
		PixelRepeat = stAviInfoFrame->PixelRepeat;
	}
	else 
	{	
		/* Set RGB or YCrCb Colorspace */	
		switch (hHDMI->DeviceSettings.eColorimetry)
		{
		default :
		case BAVC_MatrixCoefficients_eHdmi_RGB :
			Y1Y0 =	BAVC_HDMI_AviInfoFrame_Colorspace_eRGB ;
			break ;
			
		/* YCbCr output is always YCbCr 4:4:4 */		
		case BAVC_MatrixCoefficients_eItu_R_BT_709 :
		case BAVC_MatrixCoefficients_eSmpte_170M :
		case BAVC_MatrixCoefficients_eXvYCC_601 :
		case BAVC_MatrixCoefficients_eXvYCC_709 :		
			Y1Y0 =	BAVC_HDMI_AviInfoFrame_Colorspace_eYCbCr444 ;  
			break ;
		}
		
		/* Set Colorimetry */	
		switch (hHDMI->DeviceSettings.eColorimetry)
		{
		default :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eNoData ;
			break ;
			
		case BAVC_MatrixCoefficients_eItu_R_BT_709 :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eItu709 ;
			break ;
		
		case BAVC_MatrixCoefficients_eSmpte_170M :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eSmpte170 ;
			break ;

		case BAVC_MatrixCoefficients_eXvYCC_601 :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended ;
			EC2EC1EC0 = BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvYCC601;
			break ;

		case BAVC_MatrixCoefficients_eXvYCC_709 :
			C1C0 = BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended ;
			EC2EC1EC0 = BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvYCC709;
		}
		

		/* convert specified Aspect Ratio to use for AVI Infoframe values */
		switch (hHDMI->DeviceSettings.eAspectRatio)
		{
		default :
		case BFMT_AspectRatio_eUnknown	 : /* Unknown/Reserved */
			BDBG_WRN(("Unknown AspectRatio passed in AVI Frame")) ;
			/* fall through */
			
		case BFMT_AspectRatio_eSquarePxl : /* square pixel */
		case BFMT_AspectRatio_e221_1	 :	   /* 2.21:1 */
			BDBG_WRN(("Specified BAVC Aspect Ratio %d, not compatible with HDMI" ,
				hHDMI->DeviceSettings.eAspectRatio)) ;
			M1M0 = BAVC_HDMI_AviInfoFrame_PictureAspectRatio_eNoData ;
			break ;
		
		case BFMT_AspectRatio_e4_3		 :	
			M1M0 = BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e4_3 ;
			break ;
			
		case BFMT_AspectRatio_e16_9 	 :	
			M1M0 = BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e16_9 ;
			break ;
		}

		BHDM_P_VideoFmt2CEA861Code(hHDMI->DeviceSettings.eInputVideoFmt, 
			hHDMI->DeviceSettings.eAspectRatio, hHDMI->DeviceSettings.ePixelRepetition, 
			&VideoID) ;		

		PixelRepeat = (uint8_t) hHDMI->DeviceSettings.ePixelRepetition;
	}
	
	BDBG_WRN(("*** Current AVI Info Frame Settings ***")) ;
	BDBG_WRN(("ColorSpace (%d): %s", Y1Y0, BAVC_HDMI_AviInfoFrame_ColorspaceToStr(Y1Y0))) ;
	BDBG_WRN(("Active Info (%d): %s ", stAviInfoFrame->eActiveInfo, 
		BAVC_HDMI_AviInfoFrame_ActiveFormatToStr(stAviInfoFrame->eActiveInfo))) ;
	BDBG_WRN(("Bar Info (%d): %s ", stAviInfoFrame->eBarInfo, 
		BAVC_HDMI_AviInfoFrame_BarInfoToStr(stAviInfoFrame->eBarInfo))) ;
	BDBG_WRN(("Scan Info (%d): %s", stAviInfoFrame->eScanInfo, 
		BAVC_HDMI_AviInfoFrame_ScanInfoToStr(stAviInfoFrame->eScanInfo))) ;	
	BDBG_WRN(("Colorimetry (%d): %s", C1C0, BAVC_HDMI_AviInfoFrame_ColorimetryToStr(C1C0))) ;
	BDBG_WRN(("Extended Colorimetry (%d): %s", EC2EC1EC0, 
		BAVC_HDMI_AviInfoFrame_ExtendedColorimetryToStr(EC2EC1EC0))) ; 
	BDBG_WRN(("Picture AR (%d): %s", M1M0, 
		BAVC_HDMI_AviInfoFrame_PictureAspectRatioToStr(M1M0))) ;
	
	if ((stAviInfoFrame->eActiveFormatAspectRatio >= 8) 
	&&	(stAviInfoFrame->eActiveFormatAspectRatio <= 11))
		BDBG_WRN(("Active Format AR (%d): %s", stAviInfoFrame->eActiveFormatAspectRatio,
			BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatioToStr(stAviInfoFrame->eActiveFormatAspectRatio - 8))) ;
	else if  ((stAviInfoFrame->eActiveFormatAspectRatio > 12)
	&&	(stAviInfoFrame->eActiveFormatAspectRatio <= 15))
		BDBG_WRN(("Active Format AR (%d): %s", 
			stAviInfoFrame->eActiveFormatAspectRatio,
			BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatioToStr(stAviInfoFrame->eActiveFormatAspectRatio - 9))) ;
	else
		BDBG_WRN(("Active Format AR (%d): Other", 
			stAviInfoFrame->eActiveFormatAspectRatio)) ;
			
	BDBG_WRN(("Picture Scaling (%d): %s ", stAviInfoFrame->eScaling, 
		BAVC_HDMI_AviInfoFrame_ScalingToStr(stAviInfoFrame->eScaling))) ;
	
	BDBG_WRN(("Video ID Code = %d", VideoID)) ;
	BDBG_WRN(("Pixel Repeat: %d", PixelRepeat)) ;

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	BDBG_WRN(("IT Content (%d): %s", stAviInfoFrame->eITContent, 
		BAVC_HDMI_AviInfoFrame_ITContentToStr(stAviInfoFrame->eITContent)));
	BDBG_WRN(("IT Content Type (%d): %s", stAviInfoFrame->eContentType,
		BAVC_HDMI_AviInfoFrame_ContentTypeToStr(stAviInfoFrame->eContentType)));
	BDBG_WRN(("RGB Quantization Range (%d): %s", stAviInfoFrame->eRGBQuantizationRange,
		BAVC_HDMI_AviInfoFrame_RGBQuantizationRangeToStr(stAviInfoFrame->eRGBQuantizationRange)));
	BDBG_WRN(("YCC Quantization Range (%d): %s", stAviInfoFrame->eYccQuantizationRange,
		BAVC_HDMI_AviInfoFrame_YccQuantizationRangeToStr(stAviInfoFrame->eYccQuantizationRange)));
#endif 
		
	BDBG_WRN(("Top Bar End Line Number: %d", stAviInfoFrame->TopBarEndLineNumber)) ;
	BDBG_WRN(("Bottom Bar Stop Line Number: %d", stAviInfoFrame->BottomBarStartLineNumber)) ;
	
	BDBG_WRN(("Left Bar End Pixel Number: %d", stAviInfoFrame->LeftBarEndPixelNumber )) ; 
	BDBG_WRN(("Right Bar End Pixel Number: %d\n", stAviInfoFrame->RightBarEndPixelNumber )) ;
	
#endif		

	return rc ;
}


/******************************************************************************
Summary:
Set/Enable the Audio Info Frame packet to be sent to the HDMI Rx 
*******************************************************************************/
BERR_Code BHDM_SetAudioInfoFramePacket(
   BHDM_Handle hHDMI,		  /* [in] HDMI handle */
   BAVC_HDMI_AudioInfoFrame *pstAudioInfoFrame 
)
{
	BERR_Code	   rc = BERR_SUCCESS ;

	BHDM_Packet PhysicalHdmiRamPacketId ;
	
	uint8_t PacketType ;
	uint8_t PacketVersion ;
	uint8_t PacketLength ;
	
	/* BHDM_SetAudioInfoFramePacket also called from BHDM_EnableDisplay 
	    using the AudioInfoFrame stored in the HDMI handle 
	*/

	/* initialize packet information to zero */ 
	BKNI_Memset(hHDMI->PacketBytes, 0, BHDM_NUM_PACKET_BYTES) ;

	PhysicalHdmiRamPacketId = BHDM_PACKET_eAudioFrame_ID ;
	
	PacketType	  = BHDM_Packet_AudioFrameType ;
	PacketVersion = BHDM_Packet_AudioFrameVersion ;
	PacketLength  = BHDM_Packet_AudioFrameLength ;

	if ((!pstAudioInfoFrame->bOverrideDefaults) 
	&& ((pstAudioInfoFrame->CodingType)
	||  (pstAudioInfoFrame->SampleFrequency) 
	||  (pstAudioInfoFrame->SampleSize)))
	{
		BDBG_WRN(("Audio Coding Type, Sample Size, and Frequency are obtained from stream header")) ;
		
		/* set the audio coding type, sample frequency, and sample size ALL to 0 */
		pstAudioInfoFrame->CodingType =
		pstAudioInfoFrame->SampleFrequency =
		pstAudioInfoFrame->SampleSize
			=  BHDM_REFER_TO_STREAM_HEADER ;
	}
	

	hHDMI->PacketBytes[1] = 
		   pstAudioInfoFrame->CodingType << 4  
		| (pstAudioInfoFrame->ChannelCount) ;  
	hHDMI->PacketBytes[2] = 
		   pstAudioInfoFrame->SampleFrequency << 2 
		| pstAudioInfoFrame->SampleSize ;
	
	
	hHDMI->PacketBytes[3] = 0 ; /* Per HDMI Spec... Set to 0  */
	
	hHDMI->PacketBytes[4] = 
		pstAudioInfoFrame->SpeakerAllocation ;
	
	hHDMI->PacketBytes[5] = 
		pstAudioInfoFrame->DownMixInhibit << 7 
		| pstAudioInfoFrame->LevelShift << 3 ;

	/* adjust the Audio Input Configuration to reflect any changes */
	BHDM_P_ConfigureInputAudioFmt(hHDMI, pstAudioInfoFrame) ;

	BHDM_CHECK_RC(rc, BHDM_P_WritePacket(
		hHDMI, PhysicalHdmiRamPacketId,
		PacketType, PacketVersion, PacketLength, hHDMI->PacketBytes)) ;

	/* update current device settings with new information on AudioInfoFrame */
	BKNI_Memcpy(&hHDMI->DeviceSettings.stAudioInfoFrame, pstAudioInfoFrame, 
		sizeof(BAVC_HDMI_AudioInfoFrame)) ;

#if BHDM_CONFIG_DEBUG_AUDIO_INFOFRAME		
	BDBG_WRN(("------------------- NEW  AUDIO INFOFRAME ------------------")) ;
	BDBG_WRN(("Packet Type: 0x%02x  Version %d  Length: %d", 
		PacketType, PacketVersion, PacketLength)) ;
	BDBG_WRN(("Checksum            %#02x", hHDMI->PacketBytes[0])) ;

	BDBG_WRN(("Audio Coding Type     %s", 
		BAVC_HDMI_AudioInfoFrame_CodingTypeToStr(pstAudioInfoFrame->CodingType))) ;

	BDBG_WRN(("Audio Channel Count   %s", 
		BAVC_HDMI_AudioInfoFrame_ChannelCountToStr(pstAudioInfoFrame->ChannelCount))) ;
	
	BDBG_WRN(("Sampling Frequency    %s", 
		BAVC_HDMI_AudioInfoFrame_SampleFrequencyToStr(pstAudioInfoFrame->SampleFrequency))) ;

	BDBG_WRN(("Sample Size           %s", 
		BAVC_HDMI_AudioInfoFrame_SampleSizeToStr(pstAudioInfoFrame->SampleSize))) ;

	BDBG_WRN(("Speaker Allocation    %02x", pstAudioInfoFrame->SpeakerAllocation)) ;

	BDBG_WRN(("Level Shift           %s",
		BAVC_HDMI_AudioInfoFrame_LevelShiftToStr(pstAudioInfoFrame->LevelShift))) ;

	BDBG_WRN(("Down-mix Inhibit Flag %s", 
		BAVC_HDMI_AudioInfoFrame_DownMixInhibitToStr(pstAudioInfoFrame->DownMixInhibit))) ;
	
	{
		uint8_t i ;

		for (i = 1 ; i <= 10 ; i++)
		{
			BDBG_WRN(("Data Byte %02d = %#02x h", i, hHDMI->PacketBytes[i])) ;
		}
	}	
	BDBG_WRN(("-------------------- END AUDIO INFOFRAME --------------------")) ;
#endif
	
done:
	return rc ; 
}



/******************************************************************************
Summary:
Get the Audio Information Info frame sent to the HDMI Rx
*******************************************************************************/
BERR_Code BHDM_GetAudioInfoFramePacket(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BAVC_HDMI_AudioInfoFrame *stAudioInfoFrame
)
{
	BERR_Code	   rc = BERR_SUCCESS ;

	BKNI_Memset(stAudioInfoFrame, 0, sizeof(BAVC_HDMI_AudioInfoFrame)) ;

	BKNI_Memcpy(stAudioInfoFrame, &(hHDMI->DeviceSettings.stAudioInfoFrame), 
		sizeof(BAVC_HDMI_AudioInfoFrame)) ;

	return rc ;
}



/******************************************************************************
Summary:
Get the length of  a string
*******************************************************************************/
static int _strlen(const unsigned char *s) {
	int i=0;
	while (*s++) i++;
	return i;
}



/******************************************************************************
Summary:
Set/Enable the Source Product Description Info Frame packet to be sent to the HDMI Rx 
*******************************************************************************/
BERR_Code BHDM_SetSPDInfoFramePacket(
   BHDM_Handle hHDMI		  /* [in] HDMI handle */
)
{
	BERR_Code	   rc = BERR_SUCCESS ;

	BHDM_Packet PhysicalHdmiRamPacketId ;
	
	uint8_t PacketType ;
	uint8_t PacketVersion ;
	uint8_t PacketLength ;


	/* check length of vendor and description */
	PacketLength = _strlen(hHDMI->DeviceSettings.SpdVendorName) ;
	if (PacketLength > BAVC_HDMI_SPD_IF_VENDOR_LEN)
	{
		BDBG_ERR(("SPD Vendor Name Length %d larger than MAX: %d", 
			PacketLength, BAVC_HDMI_SPD_IF_VENDOR_LEN)) ;
		return BERR_INVALID_PARAMETER ;
	}
	
	PacketLength = _strlen(hHDMI->DeviceSettings.SpdDescription) ;
	if (PacketLength > BAVC_HDMI_SPD_IF_DESC_LEN)
	{
		BDBG_ERR(("SPD Description Length %d larger than MAX: %d", 
			PacketLength, BAVC_HDMI_SPD_IF_DESC_LEN)) ;
		return BERR_INVALID_PARAMETER ;
	}
	
	
	/* initialize packet information to zero */ 
	BKNI_Memset(hHDMI->PacketBytes, 0, BHDM_NUM_PACKET_BYTES) ;

	PhysicalHdmiRamPacketId = BHDM_PACKET_eSPD_ID ;
	
	PacketType	  = BHDM_Packet_SPDFrameType	;
	PacketVersion = BHDM_Packet_SPDFrameVersion ;
	PacketLength  = BHDM_Packet_SPDFrameLength	;
	
	BKNI_Memcpy(hHDMI->PacketBytes+1, hHDMI->DeviceSettings.SpdVendorName, 
		BAVC_HDMI_SPD_IF_VENDOR_LEN) ;
	BKNI_Memcpy(hHDMI->PacketBytes+9, hHDMI->DeviceSettings.SpdDescription, 
		BAVC_HDMI_SPD_IF_DESC_LEN) ;
	hHDMI->PacketBytes[25] = hHDMI->DeviceSettings.eSpdSourceDevice ;
																							   
			
	BHDM_CHECK_RC(rc, BHDM_P_WritePacket(
		hHDMI, PhysicalHdmiRamPacketId,
		PacketType, PacketVersion, PacketLength, hHDMI->PacketBytes)) ;
	
done:
#if BHDM_CONFIG_DEBUG_SPD_INFOFRAME
	BDBG_WRN(("-------------------- NEW  SPD INFOFRAME -------------------")) ;
	BDBG_WRN(("Packet Type: 0x%02x  Version %d  Length: %d", 
		PacketType, PacketVersion, PacketLength)) ;

	BDBG_WRN(("Vendor Name: %s",
		hHDMI->DeviceSettings.SpdVendorName)) ;

	BDBG_WRN(("Vendor Description: %s",
		hHDMI->DeviceSettings.SpdDescription)) ;
		
	BDBG_WRN(("Device Type: %s",
		BAVC_HDMI_SpdInfoFrame_SourceTypeToStr(
			hHDMI->DeviceSettings.eSpdSourceDevice))) ;
	BDBG_WRN(("--------------------- END SPD INFOFRAME ---------------------")) ;
		
#endif	
	return rc ; 
}


/******************************************************************************
Summary:
	Get currently transmitted Vendor Specific Info Frame
*******************************************************************************/
void  BHDM_GetVendorSpecificInfoFrame(
	BHDM_Handle hHDMI,			/* [in] HDMI handle */
	BAVC_HDMI_VendorSpecificInfoFrame *pVendorSpecficInfoFrame)
{
	BKNI_Memset(pVendorSpecficInfoFrame, 
		0, sizeof(BAVC_HDMI_VendorSpecificInfoFrame)) ;

	BKNI_Memcpy(pVendorSpecficInfoFrame, &(hHDMI->DeviceSettings.stVendorSpecificInfoFrame), 
		sizeof(BAVC_HDMI_VendorSpecificInfoFrame)) ;
}

/******************************************************************************
Summary:
	Create/Set a Vendor Specific Info Frame
*******************************************************************************/
BERR_Code BHDM_SetVendorSpecificInfoFrame(
	BHDM_Handle hHDMI,			/* [in] HDMI handle */
	const BAVC_HDMI_VendorSpecificInfoFrame *pVendorSpecificInfoFrame)
{
	BERR_Code	   rc = BERR_SUCCESS ;
	uint8_t packetByte5 ;

	BHDM_Packet PhysicalHdmiRamPacketId = BHDM_Packet_eVendorSpecific_ID ;
	
	uint8_t PacketType = BAVC_HDMI_PacketType_eVendorSpecificInfoframe ;
	uint8_t PacketVersion = 0x01 ; 
	uint8_t PacketLength = 0 ;

	/* BHDM_SetVendorSpecificInfoFrame packet also called from BHDM_EnableDisplay 
	    using the VSI stored in the HDMI handle 
	*/
	
	/* initialize packet information to zero */ 
	BKNI_Memset(hHDMI->PacketBytes, 0, BHDM_NUM_PACKET_BYTES) ;


	/* ready the Packet Data structure used for transmissIon of the Vendor Specific Data */
	/* Copy the IEEE Reg ID; skip over the checksum byte */
	BKNI_Memcpy(hHDMI->PacketBytes + 1, 
		&pVendorSpecificInfoFrame->uIEEE_RegId, BAVC_HDMI_IEEE_REGID_LEN) ;
	PacketLength = BAVC_HDMI_IEEE_REGID_LEN;

	/* Set the HDMI Video Format */
	hHDMI->PacketBytes[4] = pVendorSpecificInfoFrame->eHdmiVideoFormat << 5 ;
	PacketLength++ ;

	/* Set the Extended Resolution or 3D Format */
	switch (pVendorSpecificInfoFrame->eHdmiVideoFormat)
	{
	default :
	case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone :
		goto done;
		
	case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eExtendedResolution :
		packetByte5 = pVendorSpecificInfoFrame->eHdmiVic ;
		break ;
		
	case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat :
		packetByte5 = pVendorSpecificInfoFrame->e3DStructure << 4 ;
		break ;
	}
	hHDMI->PacketBytes[5] = packetByte5 ;
	PacketLength++ ;

	/* if 3D_Structure = sidebysideHalf, 3D_Ext_Data is added to HDMI VSI for additional info */
	if ((pVendorSpecificInfoFrame->eHdmiVideoFormat == BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat)
		&& (pVendorSpecificInfoFrame->e3DStructure == BAVC_HDMI_VSInfoFrame_3DStructure_eSidexSideHalf))
	{
		hHDMI->PacketBytes[6] = pVendorSpecificInfoFrame->e3DExtData << 4;
		PacketLength++;
	}


done:
	
#if BHDM_CONFIG_DEBUG_VENDOR_SPECIFIC_INFOFRAME 

	BDBG_WRN(("-------------------- NEW  VS  INFOFRAME -------------------")) ;
	BDBG_WRN(("Packet Type: 0x%02x  Version %d  Length: %d", 
		PacketType, PacketVersion, PacketLength)) ;

	BDBG_WRN(("IEEE Registration ID 0x%02x%02x%02x",
		pVendorSpecificInfoFrame->uIEEE_RegId[2],
		pVendorSpecificInfoFrame->uIEEE_RegId[1],
		pVendorSpecificInfoFrame->uIEEE_RegId[0])) ; 
	
    
	BDBG_WRN(("HDMI VideoFormat: %s   (PB4: %#x)", 
		BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(pVendorSpecificInfoFrame->eHdmiVideoFormat),
		pVendorSpecificInfoFrame->eHdmiVideoFormat)) ;

  	if (pVendorSpecificInfoFrame->eHdmiVideoFormat == BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eExtendedResolution)
  	{
		BDBG_WRN(("HDMI_VIC: %d (PB5: %#x)",
				pVendorSpecificInfoFrame->eHdmiVic, pVendorSpecificInfoFrame->eHdmiVic));
  	}
	else if (pVendorSpecificInfoFrame->eHdmiVideoFormat == BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat)
	{
		BDBG_WRN(("HDMI 2D/3D Structure: %s (PB5: %#x)",
			BAVC_HDMI_VsInfoFrame_3DStructureToStr(pVendorSpecificInfoFrame->e3DStructure),
			pVendorSpecificInfoFrame->e3DStructure));

		if (pVendorSpecificInfoFrame->e3DStructure == BAVC_HDMI_VSInfoFrame_3DStructure_eSidexSideHalf)
		{		
			BDBG_WRN(("HDMI 3D_Ext_Data: %s (PB6: %#x)",
				BAVC_HDMI_VsInfoFrame_3DExtDataToStr(pVendorSpecificInfoFrame->e3DExtData),
				pVendorSpecificInfoFrame->e3DExtData));
		}
  	}
	BDBG_WRN(("--------------------- END VS  INFOFRAME ---------------------")) ;
	
#endif	

	/* update current device settings with new information on VendorSpecificInfoFrame */
	BKNI_Memcpy(&hHDMI->DeviceSettings.stVendorSpecificInfoFrame, pVendorSpecificInfoFrame, 
		sizeof(BAVC_HDMI_VendorSpecificInfoFrame)) ;

	BHDM_P_WritePacket(
		hHDMI, PhysicalHdmiRamPacketId,
		PacketType, PacketVersion, PacketLength, hHDMI->PacketBytes) ;

	return rc ; 
}


/******************************************************************************
Summary:
	Create/Set a user defined packet
*******************************************************************************/
BERR_Code BHDM_SetUserDefinedPacket(
	BHDM_Handle hHDMI,			/* [in] HDMI handle */
	BHDM_Packet PhysicalHdmiRamPacketId,
	
	uint8_t PacketType,
	uint8_t PacketVersion, 
	uint8_t PacketLength,
	
	uint8_t *PacketData
)
{
	BERR_Code	   rc = BERR_SUCCESS ;
	uint8_t i ;

	
	if (PacketLength > BHDM_NUM_PACKET_BYTES)
	{
		rc = BERR_INVALID_PARAMETER ;
		BDBG_ERR(("User Defined Packet Length larger than HDMI Packet Size %d", 
			BHDM_NUM_PACKET_BYTES)) ;
		goto done ;
	}
	
	if (PhysicalHdmiRamPacketId < BHDM_Packet_eUser1)
	{
		rc = BERR_INVALID_PARAMETER ;
		BDBG_ERR(("Pre-defined Packets cannot be modified; use BHDM_Packet_eUserX")) ;
		goto done ;
		
	}
	
	for (i = 0 ; i < BHDM_Packet_eUser1;  i++)
	{
		if (PacketType == (BHDM_Packet) i)
		{
			BDBG_ERR(("User Packet Type cannot be pre-defined Packet Type: %d", i)) ;
			rc = BERR_INVALID_PARAMETER ;
		}
	}
	
	
	/* initialize packet information to zero */ 
	BKNI_Memset(hHDMI->PacketBytes, 0, BHDM_NUM_PACKET_BYTES) ;
	
	
	/* copy the user defined data */
	for (i = 0 ; i < PacketLength;	i++)
		hHDMI->PacketBytes[i] = PacketData[i] ;
		
	BDBG_MSG(("Packet Bytes: %s", hHDMI->PacketBytes)) ;
	
	BHDM_P_WritePacket(
		hHDMI, PhysicalHdmiRamPacketId,
		PacketType, PacketVersion, PacketLength, hHDMI->PacketBytes) ;
	
done:		
	return rc ; 
}


#if BHDM_CONFIG_HDMI_1_3_SUPPORT
/******************************************************************************
Summary:
Set/Enable the Gamut Metadata packet to be sent to the HDMI Rx 
*******************************************************************************/
BERR_Code BHDM_P_SetGamutMetadataPacket(
	BHDM_Handle hHDMI		  /* [in] HDMI handle */
)
{
	BERR_Code rc = BERR_SUCCESS ;

	BHDM_Packet PhysicalHdmiRamPacketId ;
	uint32_t PacketRegisterOffset ;
	uint32_t Register;
	uint32_t i;

	/* initialize packet information to zero */ 
	BKNI_Memset(hHDMI->PacketBytes, 0, BHDM_NUM_PACKET_BYTES) ;

	PhysicalHdmiRamPacketId = BHDM_PACKET_eGamutMetadata_ID;

	hHDMI->PacketBytes[0] = BHDM_Packet_GamutMetadataType;
	hHDMI->PacketBytes[1] = BHDM_Packet_GamutMetadataHB1;
	hHDMI->PacketBytes[2] = BHDM_Packet_GamutMetadataHB2;

	for (i=0; i<sizeof(BHDM_Packet_GamutMetadataSubPkt); i++) {
		hHDMI->PacketBytes[i+3] = BHDM_Packet_GamutMetadataSubPkt[i];
	}

	/* disable transmission of the RAM Packet */
	BHDM_CHECK_RC(rc, BHDM_DisablePacketTransmission(hHDMI, PhysicalHdmiRamPacketId)) ;

	/* calculate the offset of where the RAM for Packet ID begins */		
	PacketRegisterOffset = 
		  BCHP_HDMI_RAM_GCP_0 
		+ BHDM_P_WORDS_PER_RAM_PACKET * 4 * PhysicalHdmiRamPacketId ;

	/* load the Packet Bytes */
	for (i = 0 ; i < BHDM_NUM_PACKET_BYTES; i = i + 7)
	{
		/* create/write the subpacket data use HDMI_RAM_PACKET_1_0 for Register field names */
		Register = 
			  BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_0, HEADER_BYTE_0, hHDMI->PacketBytes[i+0]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_0, HEADER_BYTE_1, hHDMI->PacketBytes[i+1]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_0, HEADER_BYTE_2, hHDMI->PacketBytes[i+2]) ;
		BREG_Write32(hHDMI->hRegister, PacketRegisterOffset, Register) ;
		
#if BHDM_CONFIG_DEBUG_HDMI_PACKETS				
		BDBG_MSG(("Addr [%#x] = %#x", PacketRegisterOffset, Register)) ;
#endif				
		PacketRegisterOffset += 4 ;

		/* create/write the subpacket data use HDMI_RAM_PACKET_1_1 for Register field names */
		Register = 
			  BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_1, SUBPACKET_1_BYTE_0, hHDMI->PacketBytes[i+3]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_1, SUBPACKET_1_BYTE_1, hHDMI->PacketBytes[i+4]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_1, SUBPACKET_1_BYTE_2, hHDMI->PacketBytes[i+5]) 
			| BCHP_FIELD_DATA(HDMI_RAM_PACKET_1_1, SUBPACKET_1_BYTE_3, hHDMI->PacketBytes[i+6]) ;
		BREG_Write32(hHDMI->hRegister, PacketRegisterOffset, Register) ;

#if BHDM_CONFIG_DEBUG_HDMI_PACKETS		
		BDBG_MSG(("Addr [%#x] = %#x", PacketRegisterOffset, Register)) ;
#endif				
		PacketRegisterOffset += 4 ; 	
	}
	
	/* reenable the packet transmission */
	BHDM_CHECK_RC(rc, BHDM_EnablePacketTransmission(hHDMI, PhysicalHdmiRamPacketId)) ;
	
done:
	return rc ; 
}

#endif


