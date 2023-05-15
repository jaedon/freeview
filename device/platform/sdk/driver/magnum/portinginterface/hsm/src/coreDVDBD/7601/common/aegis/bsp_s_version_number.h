/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_version_number.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/30/09 12:17p $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/common/aegis/bsp_s_version_number.h $
 * 
 * Hydra_Software_Devel/2   3/30/09 12:17p atruong
 * PR52979: Support for 7635A0
 * 
 * Hydra_Software_Devel/1   8/12/08 4:43p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * 3   7/23/08 1:43p rjejuri
 * PR 3 : Update 7443 chip number as 0x0d
 * 
 * 2   2/21/08 1:39p bryanl
 * PR3: Modified version information for 7443A0
 * 
 * Aegis_Firmware_Devel/4   9/13/07 5:50p sshekhar
 * PR 826: customer mode version num for MSIPTV
 * 
 * Aegis_Firmware_Devel/3   8/27/07 3:40p wychen
 * PR793: FW BSECK version Control
 * 
 * Aegis_Firmware_Devel/2   8/22/07 10:11p wychen
 * PR760: 7335A0 code support
 * 
 * Aegis_Firmware_Devel/4   8/7/07 3:53p sshekhar
 * PR 57: Adding 3573A0 defines to the fullcode rom codebase.
 * 
 * Aegis_Firmware_Devel/3   6/4/07 11:25a ksekar
 * PR 656: Add 7325A0 compile flags
 * 
 * Aegis_Firmware_Devel/2   4/19/07 6:05p kostas
 * PR619:Update 7405 chip number
 * 
 * Aegis_Firmware_Devel/2   1/26/07 10:59a whou
 * PR525: add 7405 chip version number.
 * 
 * Aegis_Firmware_Devel/1   1/5/06 9:21p btan
 * PR 254: Added version number.
 * 
 *
 ***************************************************************************/

#ifndef BSP_S_VERSION_NUMBER_H__
#define BSP_S_VERSION_NUMBER_H__

/* 
 * 32-bit Version Number for Aegis: 
 * bit [31 -24] is the chip number. For eg, 0x0 = 7401 , 0x1 = 7400, 0x2  = 7118 
 * bit [23 -16] is the chip version. For eg, 0xA0 = A0, 0xB1 = B1 
 * bit [15 -12] is the major version number. For eg, 0x1, 0x2.
 * bit [11 -06] is the minor version number. For eg, 0x1, 0x2.
 * bit [05] Reserved
 * bit [04 - 00] is the customer mode. For eg, All customers = 0x1F, GENERIC = 0x00, SVPDSS = 0x0B

 * 32-bit Version Number for Host: 
 * bit [31 -24] is the chip number. For eg, 0x0 = 7401 , 0x1 = 7400, 0x2  = 7118 
 * bit [23 -16] is the chip version. For eg, 0xA0 = A0, 0xB1 = B1 
 * bit [15 -12] is the major version number. For eg, 0x1, 0x2.
 * bit [11 -06] is the minor version number. For eg, 0x1, 0x2.
 * bit [05 - 00] is reserved.	


 1)	Aegis and Host both increase major number by one if we decide to have a different command structure.

 2)	Aegis should increment the minor number of specific chip revision by 1, if we decide to release a ROM or patch
 	codes for that specific chip revision.  Host can have a different minor number from Aegis.
 	
 3) 	Host will check if bit[31-12] of Aegis verision number matches with it version number.  
 	If so, then host will continue 	process the next command. Otherwise, host stop processing command.
 	Aegis does not have to perform the any version number checking.

 4)	Host should not have those customer codes in any header files. 	

 	By default, the customer mode is ALL since the code is built for all customers.
 	So we #define BSP_S_CUSTOMER_MODE			0x1F
 	
 	If the PATCH codes is built for certain customer, then someone has to
 	change the definition of BSP_S_CUSTOMER_MODE by modifying
 	"bsp_customer_code.h"

 5)	For ROM codes, bit[15 - 00] should be all 0s.  For PATCH codes, bit[15 - 00]
 	should contain major, minor and customer codes.

 6)	Label for ClearCase:
 	ChipNum_ChipVersion_MajorNum_MinorNum_ROM/PATCH_CustomerMode
 	For Eg:
 	7401_A0_1_0_ROM_ALL (32 bit value = 0x00A00000)
 	7401_B0_1_0_ROM_ALL (32 bit value = 0x00B00000)
 	7401_B0_1_0_PATCH_ALL (32 bit value = 0x00B0103F)
 	7400_A0_1_0_ROM_ALL (32 bit value = 0x01A00000)
 	7400_A0_1_0_PATCH_ALL (32 bit value = 0x01A0103F)

 	7401_B0_1_0_PATCH_SVPDSS (32 bit value = 0x00B0102B)
 	7401_B0_1_1_PATCH_SVPDSS (32 bit value = 0x00B0106B)

	 For each label, we should be able to refer to a list of PR or release note. 

 */

#define BSP_S_CHIP_NUMBER_SHIFT		24
#define BSP_S_CHIP_VERSION_SHIFT	16
#define BSP_S_MAJOR_VERSION_SHIFT	12
#define BSP_S_MINOR_VERSION_SHIFT	6
#define BSP_S_CUSTOMER_MODE_SHIFT	0

/* Defines used for chip numbers */
#define BSP_S_CHIP_NUMBER_7443      0x0dL
#define BSP_S_CHIP_NUMBER_7601      0x0dL
#define BSP_S_CHIP_NUMBER_7635      0x0eL

#define BSP_S_A0_CHIP_VERSION			0xA0L
#define BSP_S_A1_CHIP_VERSION			0xA1L
#define BSP_S_A2_CHIP_VERSION			0xA2L
#define BSP_S_A3_CHIP_VERSION			0xA3L
#define BSP_S_B0_CHIP_VERSION			0xB0L
#define BSP_S_B1_CHIP_VERSION			0xB1L
#define BSP_S_B2_CHIP_VERSION			0xB2L
#define BSP_S_B3_CHIP_VERSION			0xB3L
#define BSP_S_C0_CHIP_VERSION			0xC0L
#define BSP_S_C1_CHIP_VERSION			0xC1L
#define BSP_S_C2_CHIP_VERSION			0xC2L
#define BSP_S_C3_CHIP_VERSION			0xC3L
#define BSP_S_D0_CHIP_VERSION			0xD0L
#define BSP_S_D1_CHIP_VERSION			0xD1L
#define BSP_S_D2_CHIP_VERSION			0xD2L
#define BSP_S_D3_CHIP_VERSION			0xD3L

#define BSP_S_BSECK_GENERIC				0x00
#define BSP_S_BSECK_ALL_CUSTOMER		0x1F

#endif /* BSP_S_VERSION_NUMBER_H__ */
