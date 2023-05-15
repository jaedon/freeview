/***************************************************************************
 *     (c)2002-2012 Broadcom Corporation
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
 * $brcm_Workfile: bfpga.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/21/12 3:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/common/src/bfpga.c $
 * 
 * 1   8/21/12 3:39p erickson
 * SW7435-235: remove symlinks
 * 
 * Hydra_Software_Devel/21   11/20/09 4:00p darnstein
 * SW7342-39: add a few asserts for zero pointers. This may help with
 * debugging.
 * 
 * Hydra_Software_Devel/20   2/8/08 11:59a vsilyaev
 * PR 38682: Fixed warning
 * 
 * Hydra_Software_Devel/19   12/20/06 3:31p erickson
 * PR25108: add 7403 support
 * 
 * Hydra_Software_Devel/18   5/12/06 4:37p erickson
 * PR20219: redesign BFPGA_P_GetAddr, remove hardcoded value
 * 
 * Hydra_Software_Devel/17   5/12/06 2:47p erickson
 * PR20219: added PKT4 for 7401/7400
 * 
 * Hydra_Software_Devel/16   2/3/06 5:09p jgarrett
 * PR 19250: Updating for 97400, eliminating warnings at startup
 * 
 * Hydra_Software_Devel/15   12/2/05 4:42p erickson
 * PR17108: added 7401 specific #ifdef to avoid errors
 * 
 * Hydra_Software_Devel/14   10/25/05 3:06p dlwin
 * PR 17809: Added support for handling Stratix FPGA with version 2.x.
 * 97038B2 ships with 2.3 and higher.
 * 
 * Hydra_Software_Devel/13   3/22/05 10:29a mphillip
 * PR14399: Invert the clocks for 3510
 * 
 * Hydra_Software_Devel/12   3/9/05 7:39p vsilyaev
 * PR 14402: Fixed clock configuration for 7042 input.
 * 
 * Hydra_Software_Devel/11   2/7/05 9:34a dlwin
 * PR 13700: Added default setting for FPGA Ver. 2.0 and up.
 * 
 * Hydra_Software_Devel/10   1/14/05 12:29p dlwin
 * PR 13700: Disable resetting for FPGA for 97038C0, if the FPGA is send a
 * reset command, the I2C bus will lock-up.
 * 
 * Hydra_Software_Devel/9   1/12/05 2:58p dlwin
 * PR 13700: Updated to support 97038C0 board, which has the FPGA on I2C
 * bus instead of EBI.
 * 
 * Hydra_Software_Devel/8   9/29/04 12:03p marcusk
 * PR12850: UPdated with support for AVC daughter card setting.
 * 
 * Hydra_Software_Devel/7   2/6/04 8:07p vsilyaev
 * PR 9588: Fixed polarity settings for VSB (daughtecards).
 * 
 * Hydra_Software_Devel/6   2/6/04 8:00p vsilyaev
 * PR 9588: Fixed polarity for 3510 DS1, DS2, and VSB,
 * 
 * Hydra_Software_Devel/5   2/6/04 5:32p marcusk
 * PR9588: Updated to program default polarity values when inputs are
 * selected.
 * 
 * Hydra_Software_Devel/4   2/4/04 3:51p marcusk
 * PR9588: Updated with proper detection of POD and 1394 Ts selects
 * (documentation was wrong)
 * 
 * Hydra_Software_Devel/3   2/4/04 2:52p marcusk
 * PR9588: Updated to use jumper settings when software has not
 * overwritten output selections
 * 
 * Hydra_Software_Devel/2   2/4/04 7:57a marcusk
 * PR9588: Added support for fpga, tested and working.
 * 
 * Hydra_Software_Devel/1   2/2/04 5:38p marcusk
 * PR9588: First version (may not compile yet)
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bfpga.h"

BDBG_MODULE(fpga);

static const BFPGA_Settings defDevSettings =
{
	0x0FFF00,
	0x0E				/* 7bit address, unshifted */
};

#define BOARD_CFG_ADDR			0x00
#define FPGA_VER_ADDR			0x01
#define STRAP_PINS_ADDR			0x02
#define RESETS_ADDR				0x03
#define PKT0_CFG_ADDR			0x04

typedef struct BFPGA_Impl
{
	BREG_Handle 	hReg;
	BREG_I2C_Handle hI2CReg;
	BFPGA_Settings  settings;
} BFPGA_Impl;

static uint8_t BFPGA_P_Read(
	BFPGA_Handle hFpga,
	uint32_t regOffset
	)
{
	uint8_t data;

	if( hFpga->hReg != NULL )
	{
		BDBG_ASSERT (hFpga->hReg);
		data = BREG_Read8(hFpga->hReg, (hFpga->settings.regBase + regOffset));
	}
	else
	{
		BERR_Code retCode;

		BDBG_ASSERT (hFpga->hI2CReg);
 		retCode = BREG_I2C_Read( hFpga->hI2CReg, hFpga->settings.i2cAddr, regOffset, &data, 1 );
		BDBG_ASSERT( retCode == BERR_SUCCESS );
	}
	return( data );
}

static void BFPGA_P_Write(
	BFPGA_Handle hFpga,
	uint32_t regOffset,
	uint8_t data
	)
{
	if( hFpga->hReg != NULL )
	{
		BREG_Write8(hFpga->hReg, (hFpga->settings.regBase + regOffset), data);
	}
	else
	{
		BERR_Code retCode;

		retCode = BREG_I2C_Write( hFpga->hI2CReg, hFpga->settings.i2cAddr, regOffset, &data, 1 );
		BDBG_ASSERT( retCode == BERR_SUCCESS );
	}
}

BERR_Code BFPGA_Open(
	BFPGA_Handle	*phFpga, 			/* [out] returns handle to fpga */
	BREG_Handle 	hReg, 				/* [in] Handle to Memory Mapped register */
	BREG_I2C_Handle hI2CReg,			/* [in] I2C Register handle */
	const BFPGA_Settings *pDefSettings	/* [in] Default settings */
	)
{
	*phFpga = BKNI_Malloc( sizeof(BFPGA_Impl) ); 
	if( *phFpga == NULL )
		return BERR_OUT_OF_SYSTEM_MEMORY;

	BKNI_Memset( *phFpga, 0, sizeof(BFPGA_Impl) );
	(*phFpga)->hReg = hReg;
	(*phFpga)->hI2CReg = hI2CReg;
	(*phFpga)->settings = *pDefSettings;

	return BERR_SUCCESS;
}

void BFPGA_Close(
	BFPGA_Handle hFpga /* handle to fpga */
	)
{
	BKNI_Free( hFpga );
}

BERR_Code BFPGA_GetDefaultSettings(
	BFPGA_Settings *pDefSettings	/* [output] Returns default setting */
	)
{
	BERR_Code retCode = BERR_SUCCESS;

	
	*pDefSettings = defDevSettings;

	return( retCode );
}

BERR_Code BFPGA_GetInfo( 
	BFPGA_Handle hFpga, /* handle to fpga */
	BFPGA_info *pInfo /* [out] pointer to info structure that will be filled with data */
	)
{
	pInfo->board_cfg = BFPGA_P_Read(hFpga, BOARD_CFG_ADDR);
	pInfo->fpga_ver = BFPGA_P_Read(hFpga, FPGA_VER_ADDR);
	pInfo->strap_pins = BFPGA_P_Read(hFpga, STRAP_PINS_ADDR);

	return BERR_SUCCESS;
}

BERR_Code BFPGA_Reset( 
	BFPGA_Handle hFpga /* handle to fpga */
	)
{
	uint8_t data = 0x1F;

	/* I2C version of FPGA doesn't support reset correctly */
	if( hFpga->hReg != NULL )
	{
		BFPGA_P_Write(hFpga, RESETS_ADDR, data);
	}

	return BERR_SUCCESS;
}

struct BFPGA_P_TsSelectDefault
{
	/* BFPGA_TsSelect		tsSelect; */
	uint8_t				defaultSetting;
};


/* This table is for Stratix based FPGA V1.x, it exist on 97038B0/B1 boards */
static const struct BFPGA_P_TsSelectDefault BFPGA_P_Stratix_V1X_TsDefault[] = {
	{ /* BFPGA_TsSelect_eQam_Ds1,	*/  0x20 }, 
	{ /* BFPGA_TsSelect_eQam_Oob,	*/  0x40 }, 	  
	{ /* BFPGA_TsSelect_eHsx_1, 	*/  0x00 }, 	   
	{ /* BFPGA_TsSelect_eHsx_2, 	*/  0x00 }, 	   
	{ /* BFPGA_TsSelect_eEnc_Ts0,	*/  0x00 }, 	  
	{ /* BFPGA_TsSelect_e1394,		*/  0x00 },
	{ /* BFPGA_TsSelect_eLvds_2,	*/  0x40 }, 	   
	{ /* BFPGA_TsSelect_eLvds_1,	*/  0x40 }, 	   
	{ /* BFPGA_TsSelect_eQam_Ds2,	*/  0x20 }, 	  
	{ /* BFPGA_TsSelect_eEnc_Ts1,	*/  0x00 }, 	  
	{ /* BFPGA_TsSelect_ePod,		*/  0x00 }, 	     
	{ /* BFPGA_TsSelect_eVsb_1,		*/  0x40 },   
	{ /* BFPGA_TsSelect_eVsb_2,		*/  0x40 },    
	{ /* BFPGA_TsSelect_eReserved_1,*/	0x00 }, 	
	{ /* BFPGA_TsSelect_eReserved_2,*/	0x00 }, 	
	{ /* BFPGA_TsSelect_eDisable, 	*/ 	0x00 }  	 
};

/* This table is for Stratix based FPGA V2.x, it exist on 97038B2 boards */
static const struct BFPGA_P_TsSelectDefault BFPGA_P_Stratix_V2X_TsDefault[] = {
	{ /* BFPGA_TsSelect_eQam_Ds1,	*/  0x20 }, 
	{ /* BFPGA_TsSelect_eQam_Oob,	*/  0x40 }, 	  
	{ /* BFPGA_TsSelect_eHsx_1, 	*/  0x00 }, 	   
	{ /* BFPGA_TsSelect_eHsx_2, 	*/  0x00 }, 	   
	{ /* BFPGA_TsSelect_eEnc_Ts0,	*/  0x00 }, 	  
	{ /* BFPGA_TsSelect_e1394,		*/  0x00 },
	{ /* BFPGA_TsSelect_eLvds_2,	*/  0x40 }, 	   
	{ /* BFPGA_TsSelect_eLvds_1,	*/  0x40 }, 	   
	{ /* BFPGA_TsSelect_eQam_Ds2,	*/  0x20 }, 	  
	{ /* BFPGA_TsSelect_eEnc_Ts1,	*/  0x00 }, 	  
	{ /* BFPGA_TsSelect_ePod,		*/  0x00 }, 	     
#if (VSB_CHIP==3510)
	{ /* BFPGA_TsSelect_eVsb_1,		*/  0x40 },   
	{ /* BFPGA_TsSelect_eVsb_2,		*/  0x40 },    
#else
	{ /* BFPGA_TsSelect_eVsb_1,		*/  0x20 },   
	{ /* BFPGA_TsSelect_eVsb_2,		*/  0x20 },    
#endif
	{ /* BFPGA_TsSelect_eReserved_1,*/	0x00 }, 	
	{ /* BFPGA_TsSelect_eReserved_2,*/	0x00 }, 	
	{ /* BFPGA_TsSelect_eDisable, 	*/ 	0x00 }  	 
};

/* This table is for non-stratix based FPGA V2.x, it exist on 97038Cx, 97401, etc boards */
static const struct BFPGA_P_TsSelectDefault BFPGA_P_NonStratix_V2X_TsDefault[] = {
	{ /* BFPGA_TsSelect_eQam_Ds1,	*/  0x20 }, 
	{ /* BFPGA_TsSelect_eQam_Oob,	*/  0x20 }, 	  
	{ /* BFPGA_TsSelect_eHsx_1, 	*/  0x00 }, 	   
	{ /* BFPGA_TsSelect_eHsx_2, 	*/  0x00 }, 	   
	{ /* BFPGA_TsSelect_eEnc_Ts0,	*/  0x40 }, 	  
	{ /* BFPGA_TsSelect_e1394,		*/  0x00 },
	{ /* BFPGA_TsSelect_eLvds_2,	*/  0x20 }, 	   
	{ /* BFPGA_TsSelect_eLvds_1,	*/  0x20 }, 	   
	{ /* BFPGA_TsSelect_eQam_Ds2,	*/  0x20 }, 	  
	{ /* BFPGA_TsSelect_eEnc_Ts1,	*/  0x00 }, 	  
	{ /* BFPGA_TsSelect_ePod,		*/  0x00 }, 	     
#if (VSB_CHIP==3510)
	{ /* BFPGA_TsSelect_eVsb_1,		*/  0x40 },   
	{ /* BFPGA_TsSelect_eVsb_2,		*/  0x40 },    
#else
	{ /* BFPGA_TsSelect_eVsb_1,		*/  0x20 },   
	{ /* BFPGA_TsSelect_eVsb_2,		*/  0x20 },    
#endif
	{ /* BFPGA_TsSelect_eReserved_1,*/	0x00 }, 	
	{ /* BFPGA_TsSelect_eReserved_2,*/	0x00 }, 	
	{ /* BFPGA_TsSelect_eDisable, 	*/ 	0x00 }  	 
};

#if BCHP_CHIP == 7401 || BCHP_CHIP == 7400 || BCHP_CHIP == 7403
/* PKT4 is available on 97038V4 boards. We have no way to detect version of board. Customers can turn this
on if they wish. For other boards, default it on. */
#define B_HAS_PKT4 1
#endif

static BERR_Code BFPGA_P_GetAddr(BFPGA_OutputSelect outputSelect, uint32_t *addr)
{
	if (outputSelect == BFPGA_OutputSelect_ePkt4) {
        *addr = 0;
#if B_HAS_PKT4
		*addr = 0x0B;
#else
		return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
	}
	else {
		/* here's where the OutputSelect enum maps to an address */
		*addr = PKT0_CFG_ADDR + outputSelect;
	}
	return 0;
}

BERR_Code BFPGA_SetTsOutput( 
	BFPGA_Handle hFpga, /* handle to fpga */
	BFPGA_OutputSelect outputSelect, /* Selects which of the FPGA TS outputs to configure */
	BFPGA_TsSelect tsSelect /* Selects which stream to send to selected output */
	)
{
	uint8_t	data;
	uint32_t addr;
	
	if (BFPGA_P_GetAddr(outputSelect, &addr)) {
		return BERR_NOT_SUPPORTED;
	}

#if BCHP_CHIP == 7401 || BCHP_CHIP == 7400 || BCHP_CHIP == 7403
	/* There is currently only one FPGA type on 97401, so select it without an error message. */
	data = (BFPGA_P_NonStratix_V2X_TsDefault[tsSelect].defaultSetting | (uint8_t)tsSelect);
#else
	switch( BFPGA_P_Read(hFpga, FPGA_VER_ADDR) & 0xF0 )
	{
		case 0x10:
			if( hFpga->hReg != NULL )
			{
				/* Stratix FPGA, controlled over EBI */
				data = (BFPGA_P_Stratix_V1X_TsDefault[tsSelect].defaultSetting | (uint8_t)tsSelect);
			}
			else
			{
				/* All non-Stratix FPGA so far support 2.x settings */
				data = (BFPGA_P_NonStratix_V2X_TsDefault[tsSelect].defaultSetting | (uint8_t)tsSelect);
			}
			break;
		case 0x20:
			if( hFpga->hReg != NULL )
			{
				/* Stratix FPGA, controlled over EBI */
				data = (BFPGA_P_Stratix_V2X_TsDefault[tsSelect].defaultSetting | (uint8_t)tsSelect);
			}
			else
			{
				data = (BFPGA_P_NonStratix_V2X_TsDefault[tsSelect].defaultSetting | (uint8_t)tsSelect);
			}
			break;
		default:
			if( hFpga->hReg != NULL )
			{
				/* Stratix FPGA, controlled over EBI */
				BDBG_ERR(("Unknown FPGA version, using Stratix Ver. 2.x settings"));
				data = (BFPGA_P_Stratix_V2X_TsDefault[tsSelect].defaultSetting | (uint8_t)tsSelect);
			}
			else
			{
				BDBG_ERR(("Unknown FPGA version, using NonStratix Ver. 2.x settings"));
				data = (BFPGA_P_NonStratix_V2X_TsDefault[tsSelect].defaultSetting | (uint8_t)tsSelect);
			}
			break;
	}
#endif
	BFPGA_P_Write(hFpga, addr, data);

	return BERR_SUCCESS;
}

struct BFPGA_P_BoardConfig
{
	/* BFPGA_OutputSelect outputSelect; */
	BFPGA_TsSelect TsSelect_JumperOff;
	BFPGA_TsSelect TsSelect_JumperOn;
	int BoardConfigBit;
};

static const struct BFPGA_P_BoardConfig BFPGA_P_gBoardConfig[] = {
	{ /* BFPGA_OutputSelect_ePkt0, */ BFPGA_TsSelect_eQam_Ds1, BFPGA_TsSelect_eQam_Ds1, -1 },
	{ /* BFPGA_OutputSelect_ePkt1, */ BFPGA_TsSelect_eQam_Ds2, BFPGA_TsSelect_eLvds_1,   1 },
	{ /* BFPGA_OutputSelect_ePkt2, */ BFPGA_TsSelect_eHsx_1,   BFPGA_TsSelect_eLvds_2,   2 },
	{ /* BFPGA_OutputSelect_ePkt3, */ BFPGA_TsSelect_eEnc_Ts0, BFPGA_TsSelect_e1394,     3 },
	{ /* BFPGA_OutputSelect_e1394, */ BFPGA_TsSelect_eHsx_1,   BFPGA_TsSelect_eHsx_1,   -2 },
	{ /* BFPGA_OutputSelect_eTest, */ BFPGA_TsSelect_eHsx_2,   BFPGA_TsSelect_eEnc_Ts1,  0 },
	{ /* BFPGA_OutputSelect_ePod,  */ BFPGA_TsSelect_eHsx_1,   BFPGA_TsSelect_eHsx_1,   -2 },
	{ /* BFPGA_OutputSelect_eAvc,  */ BFPGA_TsSelect_eHsx_1,   BFPGA_TsSelect_eHsx_1,   -2 },
	{ /* BFPGA_OutputSelect_ePkt4, */ BFPGA_TsSelect_eVsb_1,   BFPGA_TsSelect_eVsb_1,   -1 },
};
	
BERR_Code BFPGA_GetTsOutput( 
	BFPGA_Handle hFpga,	/* handle to fpga */
	BFPGA_OutputSelect outputSelect, /* Selects which of the FPGA TS outputs to configure */
	BFPGA_TsSelect *p_tsSelect, /* [out] Returns the ts stream that is routed to selected output */
	bool *p_softwareConfigured /* [out] Returns if the ts stream is configured by software (if not it is configured by the jumper setting) */
	)
{
	uint8_t	data;
	uint32_t addr;
	
	if (BFPGA_P_GetAddr(outputSelect, &addr)) {
		return BERR_NOT_SUPPORTED;
	}


	data = BFPGA_P_Read(hFpga, addr);
	
	*p_softwareConfigured = (data&0x80)?false:true;
	if( *p_softwareConfigured )
	{
		*p_tsSelect = (BFPGA_TsSelect)(data & 0x0F);
	}
	else
	{
		bool jumperState = false;
		uint8_t	board_cfg = BFPGA_P_Read(hFpga, BOARD_CFG_ADDR);

		if( BFPGA_P_gBoardConfig[outputSelect].BoardConfigBit == -2 )
		{
			*p_tsSelect = (BFPGA_TsSelect)(board_cfg&0xF);
		}
		else
		{
			if( BFPGA_P_gBoardConfig[outputSelect].BoardConfigBit >= 0 )
			{
				jumperState = (board_cfg&(1<<BFPGA_P_gBoardConfig[outputSelect].BoardConfigBit))?true:false;
			}
			*p_tsSelect = jumperState?BFPGA_P_gBoardConfig[outputSelect].TsSelect_JumperOn:BFPGA_P_gBoardConfig[outputSelect].TsSelect_JumperOff;
		}
	}

	return BERR_SUCCESS;
}

BERR_Code BFPGA_SetClockPolarity( 
	BFPGA_Handle hFpga, /* handle to fpga */
	BFPGA_OutputSelect outputSelect, /* Selects which of the FPGA TS outputs to configure */
	BFPGA_ClockPolarity inputClockPolarity, /* Selects the input clock polarity */
	BFPGA_ClockPolarity outputClockPolarity /* Selects the output clock polarity */
	)
{
	uint8_t	data;
	uint32_t addr;
	
	if (BFPGA_P_GetAddr(outputSelect, &addr)) {
		return BERR_NOT_SUPPORTED;
	}


	data = BFPGA_P_Read(hFpga, addr);
	data &= 0x9F;
	data |= (inputClockPolarity?0x40:0x00)|(outputClockPolarity?0x20:0x00);
	BFPGA_P_Write(hFpga, addr, data);

	return BERR_SUCCESS;
}

BERR_Code BFPGA_GetClockPolarity( 
	BFPGA_Handle hFpga, /* handle to fpga */
	BFPGA_OutputSelect outputSelect, /* Selects which of the FPGA TS outputs to configure */
	BFPGA_ClockPolarity *p_inputClockPolarity, /* [out] Returns the input clock polarity */
	BFPGA_ClockPolarity *p_outputClockPolarity /* [out] Returns the output clock polarity */
	)
{
	uint8_t	data;
	uint32_t addr;
	
	if (BFPGA_P_GetAddr(outputSelect, &addr)) {
		return BERR_NOT_SUPPORTED;
	}


	data = BFPGA_P_Read(hFpga, addr);
	*p_inputClockPolarity = (data&0x40)?BFPGA_ClockPolarity_eNegativeEdge:BFPGA_ClockPolarity_ePositiveEdge;
	*p_outputClockPolarity = (data&0x20)?BFPGA_ClockPolarity_eNegativeEdge:BFPGA_ClockPolarity_ePositiveEdge;

	return BERR_SUCCESS;
}

