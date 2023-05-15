/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_edid_ram.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/4/11 2:50p $
 *
 * $brcm_Log: Q:/projects/7425/latest/magnum/portinginterface/hdr/7425/bhdr_edid_ram.c $
 * 
 * Hydra_Software_Devel/5   10/4/11 2:50p rgreen
 * SW7425-1390,SW7425-1391: Fix coverity issue; prevent possible
 * dereference of NULL pointer
 * 
 * Hydra_Software_Devel/4   8/3/11 2:08p rgreen
 * SW7425-833: Update to use BDBG_OBJECT_ASSERT
 * 
 * Hydra_Software_Devel/3   6/21/11 4:35p rgreen
 * SWDTV-7092: Reduce error to warning when only 1 EDID block exists vs
 * the required 2 for HDMI
 * 
 * Hydra_Software_Devel/2   1/31/11 11:02a rgreen
 * SW7422-110:Add on-chip EDID RAM support; Update debug messages
 * 
 * Hydra_Software_Devel/1   12/29/10 10:16p rgreen
 * SW7422-110:Add on-chip EDID RAM support
 * 
 ***************************************************************************/

#if BDBG_DEBUG_BUILD
#if defined LINUX && !defined __KERNEL__
#include "ctype.h"
#endif
#endif

#include "bchp_aon_hdmi_rx.h"

#include "bhdr_fe.h"
#include "bhdr_fe_priv.h"

#include "bhdr.h"
#include "bhdr_priv.h"


BDBG_MODULE(BHDR_EDID_RAM) ;

#define BHDR_EDID_BLOCKSIZE 128
#define BHDR_EDID_RAM_P_DEFAULT_BLOCKS 2

static uint8_t BHDR_EDID_RAM_P_DefaultBlocks[ BHDR_EDID_BLOCKSIZE * BHDR_EDID_RAM_P_DEFAULT_BLOCKS] = 
{	
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x48, 0x56, 0xA0, 0xA0, 0xA0, 0xA0,
	0x1A, 0x12, 0x01, 0x03, 0x81, 0x5D, 0x34, 0x78, 0x0A, 0x55, 0x50, 0xA7, 0x55, 0x46, 0x99, 0x24,
	0x12, 0x49, 0x4B, 0x21, 0x08, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 
	0x45, 0x00, 0xA2, 0x08, 0x32, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20,
	0x6E, 0x28, 0x55, 0x00, 0xA2, 0x08, 0x32, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
	0x43, 0x4D, 0x39, 0x33, 0x35, 0x34, 0x38, 0x2F, 0x35, 0x36, 0x0A, 0x20, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x32, 0x4B, 0x0F, 0x50, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x59, 
	
	0x02, 0x03, 0x2B, 0x71, 0x56, 0x05, 0x04, 0x90, 0x02, 0x03, 0x22, 0x20, 0x21, 0x14, 0x1F, 0x13,
	0x11, 0x12, 0x17, 0x18, 0x15, 0x16, 0x19, 0x1A, 0x07, 0x06, 0x01, 0x23, 0x09, 0x07, 0x07, 0x83,
	0x01, 0x00, 0x00, 0x67, 0x03, 0x0C, 0x00, 0x10, 0x00, 0xB8, 0xDE, 0x02, 0x3A, 0x80, 0x18, 0x71,
	0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0x13, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x97 
} ;


/******************************************************************************
Summary:
	Loads the on-chip EDID RAM for the specified HDR module with 128*Blocks bytes 
	stored in the EdidRam array.

	In EDID, two 128 byte Blocks comprise a single segment.  The Broadcom EDIDs supports up 
	to  8 Blocks or equivalently, 4 segments.
	
*******************************************************************************/
BERR_Code BHDR_EDID_RAM_LoadData(
	BHDR_Handle hHDR, const BHDR_EDID_Info *edid)
{
	BREG_Handle hRegister ;	
	uint32_t ulOffset  ;
	uint32_t Register  ;
	
	uint32_t i, k	;
	uint8_t block ;
	uint32_t checksum;
	BERR_Code   rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDR_EDID_RAM_LoadData) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;	

	if (edid->uiNumBlocks < 2)
	{
		BDBG_WRN(("HDMI (CEA 861) EDID requires at least 2 Blocks.  Only %d Blocks were specified",
			edid->uiNumBlocks)) ;
	}
	
	if (!edid->pDataBytes)
	{
		BDBG_ERR(("HDMI EDID bytes not specified; pDataBytes= NULL")) ;
		rc = BERR_INVALID_PARAMETER ;
		goto BHDR_EDID_RAM_Load_Done ;

	}
	
	if (edid->uiNumBlocks > 8)
	{
		BDBG_ERR(("HDMI (CEA 861) EDID allows up to 8 Blocks.  %d Blocks were specified.", 
			edid->uiNumBlocks)) ;
		rc = BERR_INVALID_PARAMETER ;
		goto BHDR_EDID_RAM_Load_Done ;
	}

	i = 0;

	/* Disable I2c access to the  EDID RAM device */
	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset) ;
	Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE) ;
	Register |= BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE, 0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset, Register) ;


	/* enable RBUS access of i2c registers */			
	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL) ;
	Register &= ~(
		  BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, RBUS_ACCESS_ENABLE)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_ENABLE)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_READ_ENABLE)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA)) ;
		
	Register |=
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, RBUS_ACCESS_ENABLE, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_ENABLE, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_READ_ENABLE, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA, 0x0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL, Register) ;
		
		

#if BHDR_CONFIG_DEBUG_EDID_RAM
	BDBG_WRN(("Start loading %d EDID Blocks...", edid->uiNumBlocks)) ;
#endif
	for (block = 0 ; block < edid->uiNumBlocks ; block++)
	{
		checksum = 0;
		for (k = 0; k < BHDR_EDID_BLOCKSIZE; k++, i++)
		{
		
			/* Mask off the address and data*/
			Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL + ulOffset) ;
			Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS) ;
			Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA) ;
			
			/* Write the address and data*/
			Register |= BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS, 
				i) ;
			Register |= BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA, 
				edid->pDataBytes[i]) ;
			BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL + ulOffset, Register) ;

#if BHDR_CONFIG_DEBUG_EDID_RAM
			BDBG_WRN(("%08x Writing %02x to offset %03d", Register,  edid->pDataBytes[i], i)) ;
#endif

			/* Load the data into the RAM */
			Register |= BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_ENABLE, 1) ;
			BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL + ulOffset, Register) ;

 			Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_ENABLE) ;
			BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL + ulOffset, Register) ;

 
			checksum += edid->pDataBytes[i] ;
		}

		if (checksum & 0xFF)
		{
			BDBG_ERR(("Block %d checksum %#02X is INVALID; Computed %#02X",
				block, edid->pDataBytes[i-1], 
				((256 - (checksum - edid->pDataBytes[i-1]) ) & 0xFF)));
			rc = BERR_INVALID_PARAMETER;
		}
		else
		{
			BDBG_MSG(("Block %d checksum %#x", block, edid->pDataBytes[i-1])) ;
		}
	}

	BDBG_MSG(("Finished loading %d EDID Blocks", block));
 
	
#if BHDR_CONFIG_DEBUG_EDID_RAM
	{
		uint32_t EdidByte ;

		BDBG_WRN(("Verifying EDID Data."));

		/* enable RBUS access of i2c registers */			
		Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL) ;
		Register &= ~(
			  BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, RBUS_ACCESS_ENABLE)
			| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_ENABLE)
			| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_READ_ENABLE)
			| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS)
			| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA)) ;
			
		Register |= 
			  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, RBUS_ACCESS_ENABLE, 0x1)
			| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_ENABLE, 0x0)
			| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_READ_ENABLE, 0x0)
			| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS, 0x0)
			| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA, 0x0) ;
		BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL, Register) ;
			
			
		
		i = 0 ;				
		for (block = 0 ; block < edid->uiNumBlocks ; block++)
		{
			for (k = 0 ; k < BHDR_EDID_BLOCKSIZE ; k++, i++)
			{
				/* Specify the address to be read */
				Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL + ulOffset) ;
				Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS) ;
				Register |=   BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS, i) ;
				BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL + ulOffset, Register) ;

				/* Enable the EDID for read */
				Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL + ulOffset) ;
				Register |= BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_READ_ENABLE, 1) ;
				BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL + ulOffset, Register) ;
								
 
				/* Read the data */
				Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_STATUS + ulOffset) ;
				EdidByte = BCHP_GET_FIELD_DATA(Register, AON_HDMI_RX_EDID_STATUS, EDID_READ_DATA) ;

#if 0				
				BDBG_WRN(("Read offset %3d: %3d (%02x)", i, EdidByte, EdidByte));
#endif
				/* Check the data */
				if (EdidByte != edid->pDataBytes[i])
				{
					if (rc == BERR_SUCCESS)
					{
						rc = BERR_UNKNOWN;
					}
					BDBG_ERR(("RAM Byte %03d Block %d Byte %03d write of %02x failed;  Read back %02x",
			                  i, block, k, edid->pDataBytes[i],  EdidByte)) ;
				}
			}
		}
	}
#endif

BHDR_EDID_RAM_Load_Done:

	/* disable RBUS access of i2c registers */			
	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL) ;
	Register &= ~(
		  BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, RBUS_ACCESS_ENABLE)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA)) ;
		
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, RBUS_ACCESS_ENABLE, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA, 0x0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL, Register) ;


	/* Enable I2c access to the  EDID RAM device */
	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset) ;
	Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE) ;
	Register |= BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE, 1) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset, Register) ;

	BDBG_LEAVE(BHDR_EDID_RAM_LoadData) ;
	
	return rc ;
}


 /******************************************************************************
 Summary:
	 Loads 256 byte (2 blocks/1 Segment) into an array for loading via BHDR_EDID_RAM_Load

	 To load, run with the following syntax:

		uint8_t EdidRam[256];

		BHDR_EDID_RAM_Default256Data(EdidRam);
		BHDR_EDID_RAM_Load(hHDR,EdidRam, 2);
	    
	    
	BHDR_Handle hHDR, uint8_t *EdidRam, uint8_t Blocks)
	 
 *******************************************************************************/

 void BHDR_EDID_RAM_GetDefaultBlocks(BHDR_EDID_Info *edid)
{
	uint16_t uiNumEdidBytes  ;

	edid->uiNumBlocks = BHDR_EDID_RAM_P_DEFAULT_BLOCKS ;
	edid->pDataBytes = (uint8_t *) &BHDR_EDID_RAM_P_DefaultBlocks[0] ;
	
	uiNumEdidBytes  = sizeof(uint8_t) * BHDR_EDID_BLOCKSIZE * BHDR_EDID_RAM_P_DEFAULT_BLOCKS;
	BDBG_MSG(("Default EDID contains %d blocks; %d uiNumEdidBytes", 
		edid->uiNumBlocks,  uiNumEdidBytes)) ;	
}


void BHDR_EDID_RAM_Disable(BHDR_Handle hHDR)
{
	BREG_Handle hRegister ;	
	uint32_t ulOffset  ;
	uint32_t Register  ;
	
	BDBG_ENTER(BHDR_EDID_RAM_Disable) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;
	
	/* Disable EDID RAM device */
	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset) ;
	Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset, Register) ;

	BDBG_LEAVE(BHDR_EDID_RAM_Disable) ;
	
}

 
void BHDR_EDID_RAM_Enable(BHDR_Handle hHDR)
{
	BREG_Handle hRegister ;	
	uint32_t ulOffset  ;
	uint32_t Register  ;
	
	BDBG_ENTER(BHDR_EDID_RAM_Enable) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;	

 	/* Disable I2c access to the  EDID RAM device */
	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset) ;
	Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE) ;
	Register |= BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE, 0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset, Register) ;



	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_I2C_DELAY_CFG) ;
	Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_I2C_DELAY_CFG, I2C_WRITE_DELAY) ;
	Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_I2C_DELAY_CFG, I2C_HOLD_DELAY) ;
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_I2C_DELAY_CFG, I2C_WRITE_DELAY, 0xF)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_I2C_DELAY_CFG, I2C_HOLD_DELAY, 0x1C) ;
	BREG_Write32(hRegister,BCHP_AON_HDMI_RX_EDID_I2C_DELAY_CFG, Register) ;
	

	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL) ;
	Register &= ~ (
		  BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, EDID_I2C_ADDR)
		| BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, EDID_OUTRANGE_DATA)
		| BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, EDID_BLOCK_COUNT)
		| BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, EDID_ENABLE_INC_BB)
		| BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, EDID_ENABLE_INC_SB)
		| BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, FORCE_ACK)) ;
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, EDID_I2C_ADDR, 0x50)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, EDID_OUTRANGE_DATA, 0xEE)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, EDID_BLOCK_COUNT, 0x2)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, EDID_ENABLE_INC_BB, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, EDID_ENABLE_INC_SB, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, FORCE_ACK, 0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL, Register) ;
	
	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_SEGMENT_POINTER) ;
	Register &= ~ (
		  BCHP_MASK(AON_HDMI_RX_EDID_SEGMENT_POINTER, SP_I2C_ADDR)
		| BCHP_MASK(AON_HDMI_RX_EDID_SEGMENT_POINTER, ENABLE_SP_DEVICE)
		| BCHP_MASK(AON_HDMI_RX_EDID_SEGMENT_POINTER, SP_MAX_RANGE)) ;
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_SEGMENT_POINTER, SP_I2C_ADDR, 0x30)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_SEGMENT_POINTER, ENABLE_SP_DEVICE, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_SEGMENT_POINTER, SP_MAX_RANGE, 0x3) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_SEGMENT_POINTER, Register) ;


	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_CONFIG) ;
	Register &= ~ (
		  BCHP_MASK(AON_HDMI_RX_EDID_CONFIG, CR_I2C_ADDR)
		| BCHP_MASK(AON_HDMI_RX_EDID_CONFIG, ENABLE_CR_DEVICE)
		| BCHP_MASK(AON_HDMI_RX_EDID_CONFIG, EDID_CONFIGURATION)
		| BCHP_MASK(AON_HDMI_RX_EDID_CONFIG, ENABLE_MASTER_WRITE_EDID)) ;
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONFIG, CR_I2C_ADDR, 0x31)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONFIG, ENABLE_CR_DEVICE, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONFIG, EDID_CONFIGURATION, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONFIG, ENABLE_MASTER_WRITE_EDID, 0x0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_CONFIG, Register) ;



	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_DISPLAYID_CONTROL) ;
	Register &= ~ (
		  BCHP_MASK(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_I2C_ADDR)
		| BCHP_MASK(AON_HDMI_RX_DISPLAYID_CONTROL, ENABLE_DISPLAYID_DEVICE)
		| BCHP_MASK(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_OUTRANGE_DATA)
		| BCHP_MASK(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_START_BLOCK)
		| BCHP_MASK(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_BLOCK_COUNT)
		| BCHP_MASK(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_ENABLE_INC_BB)
		| BCHP_MASK(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_ENABLE_INC_SB)) ;
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_I2C_ADDR, 0x32)
		| BCHP_FIELD_DATA(AON_HDMI_RX_DISPLAYID_CONTROL, ENABLE_DISPLAYID_DEVICE, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_OUTRANGE_DATA, 0xCC)
		| BCHP_FIELD_DATA(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_START_BLOCK, 0x6)
		| BCHP_FIELD_DATA(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_BLOCK_COUNT, 0x4)
		| BCHP_FIELD_DATA(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_ENABLE_INC_BB, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_DISPLAYID_CONTROL, DISPLAYID_ENABLE_INC_SB, 0x1) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_DISPLAYID_CONTROL, Register) ;


	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL) ;
	Register &= ~ (
		  BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_ENABLE)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_READ_ENABLE)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS)
		| BCHP_MASK(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA)) ;
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_ENABLE, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_READ_ENABLE, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_ACCESS_ADDRESS, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_RAM_CONTROL, EDID_WRITE_DATA, 0x0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_RAM_CONTROL, Register) ;
		


	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_ENERGYSTAR_CONTROL) ;
	Register &= ~ (
		  BCHP_MASK(AON_HDMI_RX_EDID_ENERGYSTAR_CONTROL, FORCE_EDID_CLK)
		| BCHP_MASK(AON_HDMI_RX_EDID_ENERGYSTAR_CONTROL, ENABLE_EDID_ES)
		| BCHP_MASK(AON_HDMI_RX_EDID_ENERGYSTAR_CONTROL, CLEAR_EDID_ES)) ;
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_ENERGYSTAR_CONTROL, FORCE_EDID_CLK, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_ENERGYSTAR_CONTROL, ENABLE_EDID_ES, 0x1)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_ENERGYSTAR_CONTROL, CLEAR_EDID_ES, 0x0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_ENERGYSTAR_CONTROL, Register) ;
	
	

	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_I2C_DELAY_CFG) ;
	Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_I2C_DELAY_CFG, I2C_LPF_FILTER_THRESHOLD) ;
	Register |= 
		BCHP_FIELD_DATA(AON_HDMI_RX_EDID_I2C_DELAY_CFG, I2C_LPF_FILTER_THRESHOLD, 0x1) ;
	BREG_Write32(hRegister,BCHP_AON_HDMI_RX_EDID_I2C_DELAY_CFG, Register) ;

	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL) ;
	Register &= ~ (
		  BCHP_MASK(AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, SCL_DELAY_START)
		| BCHP_MASK(AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, SCL_DELAY_STOP)
		| BCHP_MASK(AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, SDA_UP_DELAY)
		| BCHP_MASK(AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, SDA_DN_DELAY)) ;
	Register |= 
		  BCHP_FIELD_DATA(AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, SCL_DELAY_START, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, SCL_DELAY_STOP, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, SDA_UP_DELAY, 0x0)
		| BCHP_FIELD_DATA(AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, SDA_DN_DELAY, 0x0) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_I2C_GEN_START_STOP_CONTROL, Register) ;


	/* Enable I2c access to the  EDID RAM device */
	Register = BREG_Read32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset) ;
	Register &= ~ BCHP_MASK(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE) ;
	Register |= BCHP_FIELD_DATA(AON_HDMI_RX_EDID_CONTROL, ENABLE_EDID_DEVICE, 1) ;
	BREG_Write32(hRegister, BCHP_AON_HDMI_RX_EDID_CONTROL + ulOffset, Register) ;
		

	BDBG_LEAVE(BHDR_EDID_RAM_Enable) ;
	
}
  

