/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_version_number.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/13/06 11:06a $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7403/a0/common/aegis/bsp_s_version_number.h $
 * 
 * Hydra_Software_Devel/2   12/13/06 11:06a lyang
 * PR 24864: HSM: Add HSM support for 7403, PR 25689: add 7403 support to
 * HSM/SCD
 * 
 * Hydra_Software_Devel/1   3/1/06 10:40a btan
 * PR 19931: Added HSM support for 7400A0
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
 * bit [05] is reserved
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
#define BSP_S_CHIP_VERSION_SHIFT		16
#define BSP_S_MAJOR_VERSION_SHIFT		12
#define BSP_S_MINOR_VERSION_SHIFT		6
#define BSP_S_CUSTOMER_MODE_SHIFT	0

 
#define BSP_S_CHIP_NUMBER_7401		0x00L
#define BSP_S_CHIP_NUMBER_7400		0x01L
#define BSP_S_CHIP_NUMBER_7118		0x02L
#define BSP_S_CHIP_NUMBER_7440		0x03L
#define BSP_S_CHIP_NUMBER_3563		0x04L
#define BSP_S_CHIP_NUMBER_7403		0x05L


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




#endif /* BSP_S_VERSION_NUMBER_H__ */

