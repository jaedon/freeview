/******************************************************************************
 *    (c)2011 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_decoder_register_protection_settings.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/20/11 3:21p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/extensions/video_decoder/security/7422/src/nexus_video_decoder_register_protection_settings.c $
 * 
 * 3   7/20/11 3:21p yili
 * SW7422-404:Code resturcture
 * 
 * 2   6/29/11 5:14p yili
 * SW7422-404:Clean up
 * 
 * 1   3/23/11 11:19a yili
 * SW7422-404:Video decoder security
 * 
 *****************************************************************************/

#include "bchp_common.h"
#ifndef NEXUS_VIDEO_DECODER_SECURITY_NO_BLD
#include "bchp_bld_decode_cpudma_0.h"    
#endif
#include "nexus_bsp_config.h"       
#include "bxvd.h"
#include "nexus_video_decoder_register_protection.h"

#define AVD_RESGISTER_ADDRESS(x) (0x10000000+x)

#if (NEXUS_NUM_XVD_DEVICES)

static const AVDRegRegion gAVD0_Regions[] =
{
#ifndef NEXUS_VIDEO_DECODER_SECURITY_NO_BLD
	{
		AVD_RESGISTER_ADDRESS(BCHP_BLD_DECODE_CPUDMA_0_REG_DMA0_SD_ADDR), /* 0x10101800 */
		AVD_RESGISTER_ADDRESS(BCHP_BLD_DECODE_CPUDMA_0_REG_DMA0_SD_ADDR+0xFB) /* 0x101018FB */
	},

	{
		AVD_RESGISTER_ADDRESS(BCHP_BLD_DECODE_IND_SDRAM_REGS_0_REG_SDRAM_INC), /* 0x10141000 */
		AVD_RESGISTER_ADDRESS(BCHP_BLD_DECODE_IND_SDRAM_REGS_0_REG_SDRAM_INC+0xEFFB) /* 0x1014FFFB */
	},
#endif		
	{
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUDMA_0_REG_START), /* 0x10001800 */
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUDMA_0_REG_START+0xFB)  /* 0x100018FB */
	},
	
	{
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS_0_REG_START), /* 0x10041000 */
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS_0_REG_START+0xEFFB) /* 0x1004FFFB */
	},
	{
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUDMA2_0_REG_START), /* 0x10051800 */
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUDMA2_0_REG_START+0xFB) /* 0x100518fb */
	}

};

#if (NEXUS_NUM_XVD_DEVICES>1)
static const AVDRegRegion gAVD1_Regions[] =
{
	{
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUDMA_1_REG_START), /* 0x10001800 */
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUDMA_1_REG_START+0xFB) /* 0x100018FB */
	},

	{
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS_1_REG_START), /* 0x10041000 */
		AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS_1_REG_START+0xEFFB) /* 0x1004FFFB */
	},
};
#endif


static const AVDRegAddrValue gStartAVDRegAddrValues[NEXUS_NUM_XVD_DEVICES][NEXUS_VIDEO_REG_PROTECTION_NUM_START_AVD_REGISTERS] =
{
	{
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG), /* 0x10051010 */
			0x00000001,

		},
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUAUX2_0_CPUAUX_REG+0x18), /* 0x10055018 */
			0x00000000,

		},

		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUAUX2_0_CPUAUX_REG+0x28), /* 0x10055028 */
			0x00000000,
		}
	}
#if (NEXUS_NUM_XVD_DEVICES>1)
	,
	{
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS2_1_REG_CPU_DBG), /* 0x10051010 */
			0x00000001,

		},
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUAUX2_1_REG_START+0x18), /* 0x10055018 */
			0x00000000,

		},
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUAUX2_1_REG_START+0x28), /* 0x10055028 */
			0x00000000,

		}
	}
#endif
};


static const AVDRegAddrValue gResetAVDRegAddrValues[NEXUS_NUM_XVD_DEVICES][NEXUS_VIDEO_REG_PROTECTION_NUM_RESET_AVD0_REGISTERS] =
{
	{
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG),
			0x00000001,

		},
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUAUX_0_CPUAUX_REG+0x28),
			0x00000001,

		},
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG),
			0x00000001,

		},
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUAUX2_0_CPUAUX_REG+0x28),
			0x00000001,

		},
#ifndef NEXUS_VIDEO_DECODER_SECURITY_NO_BLD
		{
			AVD_RESGISTER_ADDRESS(BCHP_BLD_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG),			/*  0x10141010*/
			0x00000001,
		},
		{
			AVD_RESGISTER_ADDRESS(BCHP_BLD_DECODE_CPUAUX_0_CPUAUX_REG+0x28),			/*  0x10145028*/
			0x00000001,

		},
#endif
	}
#if (NEXUS_NUM_XVD_DEVICES>1)
	,
	{
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_IND_SDRAM_REGS_1_REG_CPU_DBG), /* 0x10041010 */
			0x00000001,

		},
		{
			AVD_RESGISTER_ADDRESS(BCHP_DECODE_CPUAUX_1_CPUAUX_REG+0x28), /* 0x10045028 */
			0x00000001,

		},
		{
			0x00000000,		/* Dummy */
			0x00000000,

		},
		{
			0x00000000,		/* Dummy */
			0x00000000,

		},
		{
			0x00000000,		/* Dummy */
			0x00000000,

		},
		{
			0x00000000,		/* Dummy */
			0x00000000,

		}

	}
#endif
};


#endif
