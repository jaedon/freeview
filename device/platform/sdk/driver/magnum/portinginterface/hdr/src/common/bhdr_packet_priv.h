/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_packet_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 8/26/11 2:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/3548/bhdr_packet_priv.h $
 * 
 * Hydra_Software_Devel/4   8/26/11 2:00p rbshah
 * SWDTV-5426: 35230: Fix compiler errors for Gamut Metadata Packet
 * support.
 * 
 * Hydra_Software_Devel/3   6/22/11 11:14a rgreen
 * SWDTV-6771: Add compile time option support for ISRC packets
 * 
 * Hydra_Software_Devel/2   2/3/11 11:18a rgreen
 * SW35230-2882: Restore support for parsing Gamut Meta Data packet;
 * Support is disabled by default (see bhdr_config.h to enable)   Fix
 * naming convention for isr functions
 * 
 * Hydra_Software_Devel/1   1/8/10 4:01p rgreen
 * SW3548-2642: Move packet processing to separate bhdr_packet_priv.c/h
 * file
 * 
 ***************************************************************************/

#ifndef BHDR_PACKET_PRIV_H__
#define BHDR_PACKET_PRIV_H__


#ifdef __cplusplus
extern "C" {
#endif


#define BHDR_P_ENABLE_PACKET_WRITES(hRegister, Register, ulOffset) \
	/* Enable write access to RAM packets  */ \
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CONTROL + ulOffset) ; \
	Register |= BCHP_MASK(HDMI_RX_0_PACKET_PROCESSOR_CONTROL, ENABLE_RAM_PACKET_WRITES) ;  \
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CONTROL + ulOffset, Register) ; 

#define BHDR_P_DISABLE_PACKET_WRITES(hRegister, Register, ulOffset) \
	/* Disable write access to RAM packets  */ \
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CONTROL + ulOffset) ; \
	Register &= ~BCHP_MASK(HDMI_RX_0_PACKET_PROCESSOR_CONTROL, ENABLE_RAM_PACKET_WRITES) ;  \
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CONTROL + ulOffset, Register) ;

BERR_Code BHDR_P_InitializePacketRAM_isr(BHDR_Handle hHDR) ;
BERR_Code BHDR_P_ProcessReceivedPackets_isr(BHDR_Handle hHDR) ;


void BHDR_P_GetPacketRAM(
	BREG_Handle hRegister, uint32_t PacketRegisterOffset, uint8_t * PacketBytes) ;


BERR_Code BHDR_P_GetPacketRamIndexByType(
	BHDR_Handle hHDR, BAVC_HDMI_PacketType ePacketType,  uint8_t *PacketIndex) ;


void BHDR_P_ClearPacketMemory_isr(BHDR_Handle hHDR) ;

BERR_Code BHDR_P_ClearPacketRAM_isr(
   BHDR_Handle hHDR, uint8_t PacketNumber) ;

BERR_Code BHDR_P_ClearPacketRAMLock_isr(BHDR_Handle hHDR, uint8_t PacketNumber) ;


BERR_Code BHDR_P_ParseGeneralControlPacket_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;

BERR_Code BHDR_P_ParseAviInfoFrameData_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;

BERR_Code BHDR_P_ParseAudioInfoFrameData_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;

BERR_Code BHDR_P_ParseSPDInfoFrameData_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;

BERR_Code BHDR_P_ParseVendorSpecificInfoFrameData_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;

BERR_Code BHDR_P_ParseAudioContentProtection_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;

BERR_Code BHDR_P_ParseAudioClockRegeneration_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;	

	
BERR_Code BHDR_P_GetGamutPacketData_isr(
	BHDR_Handle hHDR, uint8_t PacketNum, BAVC_HDMI_GamutPacket *Packet ) ;

BERR_Code BHDR_P_ParseGamutMetadataPacket_isr(
	BHDR_Handle hHDR, BAVC_HDMI_GamutPacket *Packet) ;

BERR_Code BHDR_P_ParseISRC1_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;

BERR_Code BHDR_P_ParseISRC2_isr(
	BHDR_Handle hHDR, BAVC_HDMI_Packet *Packet) ;

BERR_Code BHDR_P_ProcessStoppedPackets_isr(BHDR_Handle hHDR) ;

#ifdef __cplusplus
}
#endif
 
#endif
/* end bhdr_packet_priv.h */

