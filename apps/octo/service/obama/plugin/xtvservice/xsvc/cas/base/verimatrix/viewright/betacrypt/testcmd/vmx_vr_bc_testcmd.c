/* (c) 2011-2015 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/**
	@file     vmx_vr_bc_usercmd.c
	@brief    vmx_vr_bc_usercmd.c (Verimatrix BC USER CMD)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#include <mwc_rm.h>
#include "cmdtool.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "_xsvc_cas_vmx_bc_main.h"
#include "vmx_mgr.h"
#include "vmx_ui.h"
#include "pal_fs.h"

#ifdef CONFIG_DEBUG
HUINT32 g_BcCmd_Level = (VMX_DBG_VR_BC_USERCMD == 1 ? 0xff : 0);

#define BcCmdPrint(level, message)		(((level) & g_BcCmd_Level) ? ((VK_Print("\tVMX_CMD [%d][%s], ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define BcCmdError(level, message)		(((level) & g_BcCmd_Level) ? ((VK_Print("\tVMX_ERR_CMD [%d][%s] ERROR, ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define BcCmdAssert(c) 					((c) ? 0 : (VK_Print("\n CMD assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define BcCmdData(level, message)		(((level) & g_BcCmd_Level) ? ( (VK_Print message)) : 0)
#else
#define BcCmdPrint(level, message)
#define BcCmdError(level, message)
#define BcCmdAssert(c)
#define BcCmdData(level, message)
#endif


HUINT8 *OSM_Trigger_Printf_Buf[6] =
{
		// UPGRADE
	"Trigger_Upgrade_Forced_Without_Descriptor_Buf", //  = eOSM_UPGRADE", ///*\006UF0000000100100012*/
	"Trigger_Upgrade_Forced_Buf", ///*\006UF0000000100100012440B04340000FFF2030068750F*/
	"Trigger_Upgrade_Now_Buf", ///*006UN0000000100100012440B04340000FFF2030068750F*/
	#if 1
		// WATERMARKING
	"Trigger_Watermarking_10sec_Buf", // = eOSM_WATERMARKING1", ///*\006W0000000A*/
	"Trigger_Watermarking_100sec_Buf", // = eOSM_WATERMARKING2", ///*\006W00000064001c02*/	

		// PIN_NUMBER_RESET
	"Trigger_PinNumberReset_Buf", // = eOSM_PINNUMBERRESET/*\006P1234*/	
	#else
	"Trigger_Upgrade_NowAfterUserConfirmation_Buf", ///*\006Un0000000100100012440B04340000FFF2030068750F*/	
	"Trigger_Upgrade_OnPowerDown_Buf", ///*006UD0000000100100012440B04340000FFF2030068750F*/	
	"Trigger_Upgrade_OnPowerDownAfterUserConfirmation_Buf", ///*006Ud0000000100100012440B04340000FFF2030068750F*/
	"Trigger_Upgrade_OnPowerUp_Buf", ///*006UU0000000100100012440B04340000FFF2030068750F*/
	"Trigger_Upgrade_OnPowerUpAfterUserConfirmation_Buf", ///*\006Uu0000000100100012440B04340000FFF2030068750F*/
	"Trigger_Upgrade_OnNextServiceChange_Buf", ///*\006US0000000100100012440B04340000FFF2030068750F*/
	"Trigger_Upgrade_OnNextNonDescramblingPeriod_Buf", ///*\006UC0000000100100012440B04340000FFF2030068750F*/

		// RESCAN
	"Trigger_Rescan_Now_EraseAll_Buf", // = eOSM_RESCAN", ///*\006RN00000001E440B04340000FFF2030068750F*/
	"Trigger_Rescan_Now_EraseCAOnly_Buf", ///*\006RN00000001e440B04340000FFF2030068750F*/
	"Trigger_Rescan_Now_AddAll_Buf", ///*006RN00000001A440B04340000FFF2030068750F*/
	"Trigger_Rescan_Now_AddCAOnly_Buf", ///*\006RN00000001a440B04340000FFF2030068750F*/
	"Trigger_Rescan_NowAfterUserConfirmation_EraseAll_Buf", ///*\006Rn00000001E440B04340000FFF2030068750F*/
	"Trigger_Rescan_NowAfterUserConfirmation_EraseCAOnly_Buf", ///*\006Rn00000001e440B04340000FFF2030068750F*/
	"Trigger_Rescan_NowAfterUserConfirmation_AddAll_Buf", ///*\006Rn00000001A440B04340000FFF2030068750F*/
	"Trigger_Rescan_NowAfterUserConfirmation_AddCAOnly_Buf", ///*\006Rn00000001a440B04340000FFF2030068750F*/

	"Trigger_Rescan_OnPowerDown_EraseAll_Buf", ///*\006RD00000001E440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerDown_EraseCAOnly_Buf", ///*\006RD00000001e440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerDown_AddAll_Buf", ///*\006RD00000001A440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerDown_AddCAOnly_Buf", ///*\006RD00000001a440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerDownAfterUserConfirmation_EraseAll_Buf", ///*\006Rd00000001E440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerDownAfterUserConfirmation_EraseCAOnly_Buf", ///*\006Rd00000001e440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerDownAfterUserConfirmation_AddAll_Buf", ///*\006Rd00000001A440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerDownAfterUserConfirmation_AddCAOnly_Buf", ///*\006Rd00000001a440B04340000FFF2030068750F*/

	"Trigger_Rescan_OnPowerUp_EraseAll_Buf", ///*\006RUE00000001440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerUp_EraseCAOnly_Buf", ///*\006RUe00000001440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerUp_AddAll_Buf", ///*\006RU00000001A440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerUp_AddCAOnly_Buf", ///*\006RU00000001a440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerUpAfterUserConfirmation_EraseAll_Buf", ///*\006Ru00000001E440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerUpAfterUserConfirmation_EraseCAOnly_Buf", ///*\006Ru00000001e440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerUpAfterUserConfirmation_AddAll_Buf", ///*\006Ru00000001A440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnPowerUpAfterUserConfirmation_AddCAOnly_Buf", ///*006Ru00000001a440B04340000FFF2030068750F*/

	"Trigger_Rescan_OnNextServiceChange_EraseAll_Buf", ///*\006RS00000001E440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnNextServiceChange_EraseCAOnly_Buf", ///*\006RS00000001e440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnNextServiceChange_AddAll_Buf", ///*\006RS00000001A440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnNextServiceChange_AddCAOnly_Buf", ///*\006RS00000001a440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnNextNonDescramblingPeriod_EraseAll_Buf", ///*\006RC00000001E440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnNextNonDescramblingPeriod_EraseCAOnly_Buf", ///*\006RC00000001E440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnNextNonDescramblingPeriod_AddAll_Buf", ///*\006RC00000001A440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnNextNonDescramblingPeriod_AddCAOnly_Buf", ///*\006RC00000001a440B04340000FFF2030068750F*/
	"Trigger_Rescan_OnNextNonDescramblingPeriod_Without_Descriptor_Buf", ///*\006RC00000001a*/

		// SWITCH
	"Trigger_Switch_Now", // = eOSM_SWITCH", ///*\006SN0000000100010035*/
	"Trigger_Switch_AtTime_Buf", ///*\006SA0000000100010035D594170000*/
	"Trigger_Switch_ForTimedPeriod_Buf", ///*\006SP000000010001003500000E10*/
	"Trigger_Switch_AtTimeForTimedPeriod_Buf", ///*006Sp0000000100010035D59417000000000E10*/
	"Trigger_Switch_IfServiceCannotbeDescrambled_Buf", ///*\006SC0000000100010035*/
	"Trigger_Switch_UntilTimeReached_Buf", ///*\006SU0000000100010035D594170000*/
	"Trigger_Switch_BackNow_Buf", ///*\006SB00000001*/
	"Trigger_Switch_BackinxSeconds_Buf", ///*\006Sb0000000100000E10*/
	"Trigger_Switch_BackAtTime_Buf", ///*\006Sa00000001D594170000*/

		// WATERMARKING
	"Trigger_Watermarking_10sec_Buf", // = eOSM_WATERMARKING1", ///*\006W0000000A*/
	"Trigger_Watermarking_100sec_Buf", // = eOSM_WATERMARKING2", ///*\006W00000064001c02*/	

		// PIN_NUMBER_RESET
	"Trigger_PinNumberReset_Buf", // = eOSM_PINNUMBERRESET/*\006P1234*/


	// Customer Binding to customer id12345678 with date 20120812
	"Trigger_CustomerBinding_Buf", // \006C123456782012812

		// EAS
	"Trigger_EAS_TriggerBegin_Buf", // = eOSM_EAS", ///*\006EB00000001NPT00010035*/
	"Trigger_EAS_TriggerEnd_Buf", ///*006EE00000001*/		
	#endif
};

HUINT8 *OSM_Trigger_Buf[7] =
{
		// UPGRADE
	/* 0 Trigger_Upgrade_Forced_Without_Descriptor_Buf*/ "\006UF0000000100100012",
	/* 1 Trigger_Upgrade_Forced_Buf*/ "\006UF0000000100100012440B04340000FFF2030068750F",
	/* 2 Trigger_Upgrade_Now_Buf*/ "\006UN0000000100100012440B04340000FFF2030068750F",
	#if 1
	// WATERMARKING
	/* 3 Trigger_Watermarking_10sec_Buf*/ "\006W0000000A",
	/* 4 Trigger_Watermarking_100sec_Buf*/  "\006W00000064001c02",

	// PIN_NUMBER_RESET
	/* 5 Trigger_PinNumberReset_Buf*/ "\006P1234",	
	#else
	/* 3 Trigger_Upgrade_NowAfterUserConfirmation_Buf*/ "\006Un0000000100100012440B04340000FFF2030068750F",	
	/* 4 Trigger_Upgrade_OnPowerDown_Buf*/ "006UD0000000100100012440B04340000FFF2030068750F",
	/* 5 Trigger_Upgrade_OnPowerDownAfterUserConfirmation_Buf*/ "006Ud0000000100100012440B04340000FFF2030068750F",
	/* 6 Trigger_Upgrade_OnPowerUp_Buf*/ "006UU0000000100100012440B04340000FFF2030068750F",
	/* 7 Trigger_Upgrade_OnPowerUpAfterUserConfirmation_Buf*/ "\006Uu0000000100100012440B04340000FFF2030068750F",
	/* 8 Trigger_Upgrade_OnNextServiceChange_Buf*/ "\006US0000000100100012440B04340000FFF2030068750F",
	/* 9 Trigger_Upgrade_OnNextNonDescramblingPeriod_Buf*/ "\006UC0000000100100012440B04340000FFF2030068750F",

		// RESCAN
	/* 10 Trigger_Rescan_Now_EraseAll_Buf*/ "\006RN00000001E440B04340000FFF2030068750F",
	/* 11 Trigger_Rescan_Now_EraseCAOnly_Buf*/ "\006RN00000001e440B04340000FFF2030068750F",
	/* 12 Trigger_Rescan_Now_AddAll_Buf*/ "006RN00000001A440B04340000FFF2030068750F",
	/* 13 Trigger_Rescan_Now_AddCAOnly_Buf*/ "\006RN00000001a440B04340000FFF2030068750F",
	/* 14 Trigger_Rescan_NowAfterUserConfirmation_EraseAll_Buf*/ "\006Rn00000001E440B04340000FFF2030068750F",
	/* 15 Trigger_Rescan_NowAfterUserConfirmation_EraseCAOnly_Buf*/ "\006Rn00000001e440B04340000FFF2030068750F",
	/* 16 Trigger_Rescan_NowAfterUserConfirmation_AddAll_Buf*/ "\006Rn00000001A440B04340000FFF2030068750F",
	/* 17 Trigger_Rescan_NowAfterUserConfirmation_AddCAOnly_Buf*/ "\006Rn00000001a440B04340000FFF2030068750F",

	/* 18 Trigger_Rescan_OnPowerDown_EraseAll_Buf*/ "\006RD00000001E440B04340000FFF2030068750F",
	/* 19 Trigger_Rescan_OnPowerDown_EraseCAOnly_Buf*/ "\006RD00000001e440B04340000FFF2030068750F",
	/* 20 Trigger_Rescan_OnPowerDown_AddAll_Buf*/ "\006RD00000001A440B04340000FFF2030068750F",
	/* 21 Trigger_Rescan_OnPowerDown_AddCAOnly_Buf*/ "\006RD00000001a440B04340000FFF2030068750F",
	/* 22 Trigger_Rescan_OnPowerDownAfterUserConfirmation_EraseAll_Buf*/ "\006Rd00000001E440B04340000FFF2030068750F",
	/* 23 Trigger_Rescan_OnPowerDownAfterUserConfirmation_EraseCAOnly_Buf*/ "\006Rd00000001e440B04340000FFF2030068750F",
	/* 24 Trigger_Rescan_OnPowerDownAfterUserConfirmation_AddAll_Buf*/ "\006Rd00000001A440B04340000FFF2030068750F",
	/* 25 Trigger_Rescan_OnPowerDownAfterUserConfirmation_AddCAOnly_Buf*/ "\006Rd00000001a440B04340000FFF2030068750F",

	/* 26 Trigger_Rescan_OnPowerUp_EraseAll_Buf*/ "\006RUE00000001440B04340000FFF2030068750F",
	/* 27 Trigger_Rescan_OnPowerUp_EraseCAOnly_Buf*/ "\006RUe00000001440B04340000FFF2030068750F",
	/* 28 Trigger_Rescan_OnPowerUp_AddAll_Buf*/ "\006RU00000001A440B04340000FFF2030068750F",
	/* 29 Trigger_Rescan_OnPowerUp_AddCAOnly_Buf*/ "\006RU00000001a440B04340000FFF2030068750F",
	/* 30 Trigger_Rescan_OnPowerUpAfterUserConfirmation_EraseAll_Buf*/ "\006Ru00000001E440B04340000FFF2030068750F",
	/* 31 Trigger_Rescan_OnPowerUpAfterUserConfirmation_EraseCAOnly_Buf*/ "\006Ru00000001e440B04340000FFF2030068750F",
	/* 32 Trigger_Rescan_OnPowerUpAfterUserConfirmation_AddAll_Buf*/ "\006Ru00000001A440B04340000FFF2030068750F",
	/* 33 Trigger_Rescan_OnPowerUpAfterUserConfirmation_AddCAOnly_Buf*/ "006Ru00000001a440B04340000FFF2030068750F",

	/* 34 Trigger_Rescan_OnNextServiceChange_EraseAll_Buf*/ "\006RS00000001E440B04340000FFF2030068750F",
	/* 35 Trigger_Rescan_OnNextServiceChange_EraseCAOnly_Buf*/ "\006RS00000001e440B04340000FFF2030068750F",
	/* 36 Trigger_Rescan_OnNextServiceChange_AddAll_Buf*/ "\006RS00000001A440B04340000FFF2030068750F",
	/* 37 Trigger_Rescan_OnNextServiceChange_AddCAOnly_Buf*/ "\006RS00000001a440B04340000FFF2030068750F",
	/* 38 Trigger_Rescan_OnNextNonDescramblingPeriod_EraseAll_Buf*/ "\006RC00000001E440B04340000FFF2030068750F",
	/* 39 Trigger_Rescan_OnNextNonDescramblingPeriod_EraseCAOnly_Buf*/ "\006RC00000001E440B04340000FFF2030068750F",
	/* 40 Trigger_Rescan_OnNextNonDescramblingPeriod_AddAll_Buf*/ "\006RC00000001A440B04340000FFF2030068750F",
	/* 41 Trigger_Rescan_OnNextNonDescramblingPeriod_AddCAOnly_Buf*/ "\006RC00000001a440B04340000FFF2030068750F",
	/* 42 Trigger_Rescan_OnNextNonDescramblingPeriod_Without_Descriptor_Buf*/ "\006RC00000001a",



	// SWITCH
	/* 43 Trigger_Switch_Now*/ "\006SN0000000100010035",
	/* 44 Trigger_Switch_AtTime_Buf*/ "\006SA0000000100010035D594170000",
	/* 45 Trigger_Switch_ForTimedPeriod_Buf*/ "\006SP000000010001003500000E10",
	/* 46 Trigger_Switch_AtTimeForTimedPeriod_Buf*/ "006Sp0000000100010035D59417000000000E10",
	/* 47 Trigger_Switch_IfServiceCannotbeDescrambled_Buf*/ "\006SC0000000100010035",
	/* 48 Trigger_Switch_UntilTimeReached_Buf*/ "\006SU0000000100010035D594170000",
	/* 49 Trigger_Switch_BackNow_Buf*/ "\006SB00000001",
	/* 50 Trigger_Switch_BackinxSeconds_Buf*/ "\006Sb0000000100000E10",
	/* 51 Trigger_Switch_BackAtTime_Buf*/ "\006Sa00000001D594170000",
	
	// WATERMARKING
	/* 52 Trigger_Watermarking_10sec_Buf*/ "\006W0000000A",
	/* 53 Trigger_Watermarking_100sec_Buf*/  "\006W00000064001c02",

	// PIN_NUMBER_RESET
	/* 54 Trigger_PinNumberReset_Buf*/ "\006P1234",

	// Customer Binding to customer id12345678 with date 20120812
	/* 55 Trigger_PinNumberReset_Buf*/ "\006C123456782012812",

	// EAS
	/* 56 Trigger_EAS_TriggerBegin_Buf*/ "\006EB00000001NPT00010035",
	/* 57 Trigger_EAS_TriggerEnd_Buf*/ "006EE00000001",
	#endif
};

HUINT8 EMM_Paket[188] = 
{
	/*0x47, 0x57, 0x00, 0x10, 0x00,*/ 0x82, 0x70, 0x52, 0x00, 0x01, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x70, 0x53, 0x01, 0x02, 0xd1,
	0xa1, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff,/**/
};

HUINT8 STBData[360] =
{
	0x1f, 0xf9, 0x7b, 0xa3, 0x05, 0x96, 0xee, 0x61, 0x24, 0xe3, 0x83, 0x0f, 0xb9, 0xed, 0x17, 0xb8, 
	0x6b, 0x6c, 0xe9, 0x7e, 0x47, 0xe7, 0xc6, 0x8a, 0xc2, 0x1a, 0xd0, 0x2c, 0x45, 0x86, 0xb1, 0x5f, 
	0x20, 0x55, 0x8a, 0xea, 0x64, 0x47, 0xd0, 0xb8, 0xde, 0xb9, 0xa5, 0x91, 0x3e, 0x84, 0x99, 0x68, 
	0x26, 0xd0, 0xea, 0x2b, 0x88, 0x95, 0xa8, 0xe6, 0x88, 0xf3, 0xf3, 0xe6, 0x67, 0xcc, 0xa3, 0xc2, 
	0xa8, 0xa5, 0x2c, 0x54, 0x2d, 0x2d, 0xf5, 0x61, 0xf8, 0xa4, 0x0d, 0x09, 0x5b, 0x25, 0xcf, 0xfd, 
	0x08, 0xa6, 0x8a, 0xbe, 0x27, 0x24, 0x16, 0xdb, 0xea, 0xa3, 0xa2, 0xb7, 0x74, 0x7a, 0x97, 0xce, 
	0x69, 0x0a, 0x69, 0xab, 0x80, 0x3f, 0x46, 0x63, 0xd7, 0x52, 0xe6, 0x10, 0x66, 0xa2, 0x2d, 0xb7, 
	0xb6, 0x8c, 0x29, 0xcf, 0xa9, 0x4f, 0x19, 0xac, 0x80, 0x1f, 0xa1, 0x6c, 0x9e, 0xe3, 0x11, 0x21, 
	0x4c, 0xcb, 0xba, 0x19, 0x71, 0xd1, 0x21, 0xa4, 0x41, 0xdc, 0x71, 0x65, 0xae, 0x29, 0x52, 0x92, 
	0x1f, 0xd8, 0xff, 0x13, 0x4b, 0xfa, 0x6c, 0x87, 0xdd, 0xe3, 0x70, 0xf4, 0xb7, 0xe6, 0xf5, 0x06, 
	0xfb, 0x62, 0x62, 0x16, 0xc3, 0x84, 0x08, 0x24, 0xff, 0x5f, 0x46, 0xec, 0x98, 0x95, 0x40, 0x2b, 
	0x90, 0xaf, 0x7c, 0x5c, 0x49, 0xb0, 0x19, 0x11, 0x95, 0x2f, 0x23, 0x17, 0xd6, 0x07, 0x63, 0x0a, 
	0xe7, 0x60, 0x98, 0xcb, 0xbf, 0x60, 0x8f, 0xc6, 0x0e, 0x59, 0x0d, 0x3d, 0x4b, 0x48, 0xde, 0xd7, 
	0x8b, 0xe9, 0x0f, 0x6d, 0x52, 0x92, 0xd5, 0xbc, 0x53, 0xed, 0xad, 0xa9, 0x0f, 0x5f, 0x84, 0xe1, 
	0x23, 0x53, 0xd0, 0xe3, 0x31, 0x94, 0xe6, 0x3c, 0x3e, 0xae, 0x8c, 0x4a, 0x78, 0x3b, 0x65, 0xe6, 
	0xdc, 0x0f, 0x7f, 0x9b, 0x53, 0xcf, 0x30, 0x8b, 0x1c, 0x32, 0xe1, 0xa4, 0x5d, 0x9c, 0xa6, 0xf2, 
	0x79, 0xb8, 0xce, 0x3d, 0x4d, 0x48, 0xd5, 0x63, 0x0c, 0xb8, 0x13, 0xd3, 0x6e, 0x2f, 0x48, 0xed, 
	0x7a, 0xc2, 0x3b, 0xac, 0xc4, 0x9c, 0xc4, 0xf4, 0x4f, 0xe6, 0xe0, 0xcf, 0xf7, 0x62, 0x21, 0x42, 
	0x74, 0x53, 0x23, 0x96, 0x4a, 0x92, 0x61, 0x66, 0x25, 0x45, 0x7c, 0x9d, 0xfc, 0x94, 0x62, 0xc9, 
	0xe5, 0xc7, 0x09, 0x5c, 0x29, 0x8f, 0x74, 0xbf, 0x37, 0x44, 0x62, 0xe9, 0xb1, 0xe9, 0xbf, 0x84, 
	0x4e, 0xd6, 0x30, 0x75, 0x56, 0x62, 0x26, 0xbf, 0x3e, 0xee, 0xe0, 0x53, 0x46, 0x13, 0xa3, 0x10, 
	0xfe, 0xfb, 0x01, 0x47, 0x76, 0x20, 0xf2, 0x56, 0x29, 0xee, 0x75, 0x3b, 0xd5, 0x86, 0xc7, 0xfa, 
	0x25, 0x85, 0x2e, 0x0e, 0x91, 0xc2, 0x4f, 0x9f  
};

#define VMX_NSC_DATA	"/mnt/nfs/nsc.txt"
static HUINT8 Temp_NSc_Data[2048];


static HERROR VmxCmd_GetNScFromNFS(unsigned char * pData)
{
	int i, j = 0;
	int result_data = 0;
	int nFd, nErr;
	unsigned char 	recv_data[2048];
	HUINT8 copy[2];

	HxLOG_Print ("\n++ VmxCmd_GetNScFromUsb ++\n");

	memset( &recv_data[0], 0x00, sizeof( recv_data ) );
	memset( &Temp_NSc_Data[0], 0x00, sizeof( Temp_NSc_Data ) );

	nFd = open(VMX_NSC_DATA, O_RDONLY | O_SYNC);
	if (nFd < 0)
	{
		HxLOG_Print ("* %s() ERROR 1 < file is not exist >\n", __FUNCTION__);
		return ERR_FAIL;
	}
	nErr = read(nFd, recv_data, sizeof( recv_data ));
	if (nErr < 0)
	{
		HxLOG_Print ("* %s() ERROR 2 < file is not read >\n", __FUNCTION__);
		close(nFd);
		return ERR_FAIL;
	}
	HxLOG_Print ("Recv Total Byte = %d\n", nErr);

	HxLOG_Print ("TotalByte = %d\n", j);
	for(i=0; i<1000; i++)
	{
		if( recv_data[i] == 0xff)
			break;
		if(i%16==0)
			HxLOG_Print ("\n");

		HxLOG_Print ("0x%02x, ", recv_data[i]);
	}

	for(i=0;i<nErr;i++)
	{
		if(recv_data[i]==0x20)
			continue;
//		if(recv_data[i]==0x2c)
//			continue;
		if(recv_data[i]==0x40) // 0x40 @를 last char로본다.
		{
			HxLOG_Print ("\nlast string............\n");
			break;
		}

		if(i%16==0)
			HxLOG_Print ("\n");		

		copy[0] = recv_data[i];
		i++;
		copy[1] = recv_data[i];
		result_data = strtoul(&copy[0], NULL, 16);
		Temp_NSc_Data[j] = result_data;
		j++;
	}
	HxLOG_Print ("TotalByte = %d\n", j);
	for(i=0; i<j; i++)
	{
		if(i%16==0)
			HxLOG_Print ("\n");		

		HxLOG_Print ("0x%02x, ", Temp_NSc_Data[i]);
	}
	HxLOG_Print ("\n");

	fsync(nFd);
	close(nFd);

	return ERR_OK;
}

void VmxCmd_GetProcInfo(void)
{
	char line[1024];
	return;
	sprintf(line, "/proc/%d/status", getpid());

	FILE *  fp = fopen(line, "r");

	if (fp == NULL)
	    return; 

	int nVmSize = 0;
	int nVmRss  = 0;

	while (fgets(line, 1024, fp) != NULL) {
          if (strstr(line,"VmRSS"))
		 {

	        char tmp[32];
	        char size[32];
	        sscanf(line, "%s%s", tmp, size);
	        nVmRss = atoi(size);
	        break;
	    }
	}
	HxLOG_Print ("* %s\n", line);
	fclose(fp);
}

static int VmxCmd(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[100+1]={0,};	
	HUINT32 ulCmd;
	static  HUINT8 buf[256];
	static HBOOL firstCmd = 0;
	static  HINT16 wMode = 0;
	static HINT16 x = 0;
	static HINT16 y = 0;
	static HINT16 w = 0;
	static HINT16 h = 0;
	static HINT8 bBackground = 0;
	static HINT8 bAlpha = 0;
	static HINT8 bForeGround = 0;
	static HUINT8 bDisplayMode = 0;
	static HUINT16 wDuration = 0;

	/* 1st argument */
	aucArg = (char *)CMD_NextArg((char **)&szArgList);
	if( aucArg == NULL )
	{
		BcCmdError(DBG_ASSERT, ("aucArg is null\n"));
		return CMD_ERR;
	}
	if( snprintf(aucStr, 100, "%s", aucArg) == 0 )
	{
		BcCmdError(DBG_ASSERT, ("cmd is null\n"));
		return CMD_ERR;
	}

	VK_Print("\n/*********** Verimatrix Test Command = %s***************/\n", aucStr);

	if(firstCmd == 0)
	{ 
		VK_memset(buf, 0, sizeof(buf));
		wMode = 0;
		x = 0;
		y = 0;
		w = 100;
		h = 100;
		bBackground = 10;
		bAlpha = 10;
		bForeGround = 3;
		bDisplayMode = 0;
		wDuration = 0;
		sprintf(buf, "%s",  "Hellow !\nUser Command");

		firstCmd = 1;
	}


	if (strcmp(aucStr, "vmxinit")== 0)
	{
		vmx_vr_bc_mgr_InitContext();
	}
	else if (strcmp(aucStr, "vmxver")== 0)
	{
		HUINT8 ver[32];
		HUINT8 date[32];
		HUINT8 time[32];
	   	 vmx_vr_bc_api_GetVersion(&ver[0], &date[0], &time[0]);
		BcCmdPrint(DBG_PRINT, ("GetLibraryVersion : Ver(%s), Date(%s), Time(%s)\n",ver, date, time));
	}
	else if (strcmp(aucStr, "scver")== 0)
	{
		HUINT8	wbuf[2] = { 0xd1, 0x28 };
		HUINT8	rbuf[256];
		HUINT16 wlen = 2;
		HUINT16 rlen = 256;
		HUINT16 timeout = 1500;

		#if 0	// SC에 Version info를 요청하는 command이다.
		/*
			- T1 Protol test
			Get Version Info Command
			- Request: 6 Bytes including T=1 header and trailer
			  00 00 02 d1 28 fb
			- Reply: 127 Bytes including T=1 header and trailer for card with
			    0  0 7b d1 28  b 10 50 38 57 45 35 30 31 37 2d 
	                31 11 11 42 65 74 61 43 72 79 70 74 20 49 49 2d 
	                31 2e 32  b 12 36 31 30 30 30 30 32 34 31 39 31 
	                13 28 43 29 20 42 65 74 61 52 65 73 65 61 72 63 
	                68 2c 20 62 65 74 61 63 72 79 70 74 20 62 79 20 
	                42 65 74 61 52 65 73 65 61 72 63 68 20 32 30 30 
	                30 1a  f 31 2e 32 2e 30 2e 39 38 20 32 30 30 32 
	                2d 30 32 2d 32 31 20 31 37 3a 33 38  0  0 86
		**/
		ucWriteData[0] = 0;		// NAD (Verimatrix do not use Node Address Byte)
		ucWriteData[1] = 0;       	// PCB (Verimatrix use only I-Block for send command to smartcard)
		ucWriteData[2] = 0x02;   	// LEN (Information length)
		ucWriteData[3] = 0xd1;   	// INF
		ucWriteData[4] = 0x28;   	// INF
		ucWriteData[5] = 0xfb;	// vmx_bc_sc_GenerateLRC(&ucWriteData[0], 5);//0xfb;   	// LRC
		usWriteLen = 6;
		#endif

		SC_Write(wbuf, &wlen, timeout);
	 	SC_Read(rbuf, &rlen);
		xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_USERCMD, "GetSCVersion", rbuf, rlen);
	}	
#if 0
	else if (strcmp(aucStr, "m")== 0)
	{
		char line[1024]; 
		sprintf(line, "/proc/%d/status", getpid());

		FILE *  fp = fopen(line, "r");

		if (fp == NULL)
		    return; 

		int nVmSize = 0;
		int nVmRss  = 0;

		while (fgets(line, 1024, fp) != NULL) {
	          if (strstr(line,"VmRSS")) 
			 {

		        char tmp[32];
		        char size[32];
		        sscanf(line, "%s%s", tmp, size);
		        nVmRss = atoi(size);
		        break;
		    }
		}
		HxLOG_Print ("==> line = %s\n", line);
		fclose(fp);
	}
#endif
	else if (strcmp(aucStr, "scinit")== 0)
	{
		aucArg = CMD_NextArg( (char **)&szArgList );
		if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;

		vmx_vr_bc_mgr_InitContext();
	}
	else if (strcmp(aucStr, "sctask")== 0)
	{
		aucArg = CMD_NextArg( (char **)&szArgList );
		if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
		HxLOG_Print ("task+\n");
		BC_Task();
		HxLOG_Print ("task-\n");
	}
	else if (strcmp(aucStr, "ecm")== 0)
	{
		aucArg = (char *)CMD_NextArg((char **)&szArgList);
		if(aucArg == NULL)
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
		if( snprintf(aucStr, 100, "%s", aucArg) == 0  )
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}

		BcCmdPrint(DBG_PRINT, ("- command osmdisplay [%s]-\n", aucStr));

		if(strcmp(aucStr, "ps") == 0) // PageSearch
		{
			HUINT16 usEcmPId = 0;

			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;

			if(ulCmd	 == 1) // N24
				usEcmPId = 0x602;
			else if(ulCmd	 == 2) // ProSieben
				usEcmPId = 0x603;
			else if(ulCmd	 == 3) // SAT.1
				usEcmPId = 0x604;
			else if(ulCmd	 == 4) // KABEL1
				usEcmPId = 0x605;
			else
			{
				BcCmdError(DBG_ASSERT, ("cmd error\n"));
				return CMD_ERR;
			}
						
			FS_SetECMFilter(0, 0, usEcmPId, 0, 0, 0);
 		}
		else if(strcmp(aucStr, "df") == 0) // DisableFilter
		{
			HUINT16 usEcmPId = 0;

			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;

			if(ulCmd	 == 1) // N24
				usEcmPId = 0x602;
			else if(ulCmd	 == 2) // ProSieben
				usEcmPId = 0x603;
			else if(ulCmd	 == 3) // SAT.1
				usEcmPId = 0x604;
			else if(ulCmd	 == 4) // KABEL1
				usEcmPId = 0x605;
			else
			{
				BcCmdError(DBG_ASSERT, ("cmd error\n"));
				return CMD_ERR;
			}			
			FS_SetECMFilter(0, 2, usEcmPId, 0, 0, 0);
		}		
		else if(strcmp(aucStr, "pl") == 0) // PageLock
		{
			HUINT16 usEcmPId = 0;
			HUINT8 ucTableId = 0;
			HUINT8 ucVersion = 0;
			HUINT8 ucPage = 0;

			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			// 10 sec ~ 40 sec 구간
			ucTableId = 0x80;
			if(ulCmd	 == 1) // N24
			{
				usEcmPId = 0x602;
			}
			else if(ulCmd	 == 2) // ProSieben
			{
				usEcmPId = 0x603;
			}
			else if(ulCmd	 == 3) // SAT.1
			{
				usEcmPId = 0x604;
			}
			else if(ulCmd	 == 4) // KABEL1
			{
				usEcmPId = 0x605;
				ucVersion = 0x13;
				ucPage = 2;
			}
			else // 여기는 version change로 change하자
			{

			}

			FS_SetECMFilter(0, 1, usEcmPId, ucTableId, ucVersion, ucPage);
		}
		else if(strcmp(aucStr, "get") == 0) // Get from Ecm Buf
		{
			HUINT8 bcBuf[1024];
			HUINT16 size = 0;
			memset(bcBuf, 0, sizeof(bcBuf));
			FS_ReadECM(0, &bcBuf[0], &size);
			xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_USERCMD, "ReadECM", &bcBuf[0], size);
		}
		else if(strcmp(aucStr, "flush") == 0) // Init Ecm Buf
		{
			FS_FlushECM_Buffer(0);
		}
		else if(strcmp(aucStr, "wtest") == 0) // Init Ecm Buf
		{
			int i;
			HUINT8 buf[2];
			buf[1] = 0xff;
			for(i=0;i<10;i++)
			{
				buf[0] = i+1;
				vmx_vr_bc_ecm_WriteEcmBuf(0, &buf[0], 2);
			}
		}
		else if(strcmp(aucStr, "rtest") == 0) // Init Ecm Buf
		{
			HUINT8 buf[2];
			HUINT16 len;
			buf[0] = 0;
			buf[1] = 0;
			if(vmx_vr_bc_ecm_ReadEcmBuf(0, &buf[0], &len) == ERR_OK)
				HxLOG_Print ("size[%d] - %02x, %02x\n", len, buf[0], buf[1]);
			else
				HxLOG_Print (" invalid data\n");
		}		
		else if(strcmp(aucStr, "ralltest") == 0) // Init Ecm Buf
		{
			int i;
			HUINT8 buf[2];
			HUINT16 len;
			buf[0] = 0;
			buf[1] = 0;
			for(i=0;i<10;i++)
			{
				if(vmx_vr_bc_ecm_ReadEcmBuf(0, &buf[0], &len) == ERR_OK)
					HxLOG_Print ("cnt[%d] size[%d] - %02x, %02x\n", i, len, buf[0], buf[1]);
				else
					HxLOG_Print ("cnt[%d] - invalid data\n", i);
			}
		}
		else if(strcmp(aucStr, "flush") == 0) // Init Ecm Buf
		{
			vmx_vr_bc_ecm_FlushEcmBuf(0);
		}		
		else
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
 	}
	else if(strcmp(aucStr, "emm") == 0)
	{
		aucArg = (char *)CMD_NextArg((char **)&szArgList);
		if(aucArg == NULL)
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
		if( snprintf(aucStr, 100, "%s", aucArg) == 0  )
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}

		BcCmdPrint(DBG_PRINT, ("- command osmdisplay [%s]-\n", aucStr));

		if(strcmp(aucStr, "set") == 0) // set emm filter
		{
			HUINT8 bFilterIndex;
			HUINT8 bAddressLength;
			HUINT8 abAddress[4];

			bFilterIndex = 0;
			bAddressLength = 4;
			abAddress[0] = 0;
			abAddress[1] = 0;
			abAddress[2] = 0;
			abAddress[3] = 0;
			vmx_vr_bc_emm_SetEmmFilter(bFilterIndex, bAddressLength, &abAddress[0]);
		}
		else if(strcmp(aucStr, "paket") == 0)
		{
			aucArg = (char *)CMD_NextArg((char **)&szArgList);
			if(aucArg == NULL)
			{
				BcCmdError(DBG_ASSERT, ("cmd error\n"));
				return CMD_ERR;
			}
			if( snprintf(aucStr, 100, "%s", aucArg) == 0  )
			{
				BcCmdError(DBG_ASSERT, ("cmd error\n"));
				return CMD_ERR;
			}
			if(strcmp(aucStr, "set") == 0) // set emm filter
			{
				HUINT8 bFilterIndex;
				HUINT8 bAddressLength;
				HUINT8 abAddress[4];

				bFilterIndex = 0;
				bAddressLength = 1;
				abAddress[0] = 0xd0;
				vmx_vr_bc_emm_SetEmmFilter(bFilterIndex, bAddressLength, &abAddress[0]);

				bFilterIndex = 1;
				bAddressLength = 2;
				abAddress[0] = 0xd1;
				abAddress[1] = 0xa1;
				vmx_vr_bc_emm_SetEmmFilter(bFilterIndex, bAddressLength, &abAddress[0]);	
			}
			else if(strcmp(aucStr, "get") == 0)
			{
				Vmx_Emm_t		*pEmm = NULL;
				pEmm = xsvc_vmx_psi_emm_GetEmmInfo();
				vmx_vr_bc_emm_SaveEmmData(pEmm, &EMM_Paket[0], 188);
			}
			else
			{
				BcCmdError(DBG_ASSERT, ("cmd error - packet -\n"));
			}
		}		
		else if(strcmp(aucStr, "get") == 0) // spec에 명시된 test
		{
			HUINT8 bcBuf[1024];
			HUINT16 size = 0;
			memset(bcBuf, 0, sizeof(bcBuf));
			FS_ReadEMM(0, &bcBuf[0], &size);
			xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_USERCMD, "ReadEMM", &bcBuf[0], size);
		}
		else if(strcmp(aucStr, "flush") == 0)
		{
			FS_FlushEMM_Buffer();
		}
		else if(strcmp(aucStr, "wtest") == 0)
		{
			int i;
			HUINT8 buf[2];
			buf[1] = 0xff;
			for(i=0;i<10;i++)
			{
				buf[0] = i+1;
				vmx_vr_bc_emm_WriteEmmBuf(&buf[0], 2);
			}
		}
		else if(strcmp(aucStr, "rtest") == 0)
		{
			HUINT8 buf[2];
			HUINT16 len = 2;
			HBOOL bError;
			buf[0] = 0;
			buf[1] = 0;			
			if(vmx_vr_bc_emm_ReadEmmBuf(&buf[0], &len, &bError) == ERR_OK)
				HxLOG_Print ("size[%d] - %02x, %02x\n", len, buf[0], buf[1]);
			else
				HxLOG_Print (" invalid data\n");
		}		
		else if(strcmp(aucStr, "ralltest") == 0)
		{
			int i;
			HUINT8 buf[2];
			HUINT16 len = 2;
			HBOOL bError;
			buf[0] = 0;
			buf[1] = 0;			
			for(i=0;i<10;i++)
			{
				if(vmx_vr_bc_emm_ReadEmmBuf(&buf[0], &len, &bError) == ERR_OK)
					HxLOG_Print ("cnt[%d] size[%d] - %02x, %02x\n", i, len, buf[0], buf[1]);
				else
					HxLOG_Print ("cnt[%d] - invalid data\n", i);
			}
		}
		else if(strcmp(aucStr, "add") == 0)
		{
			bc_emm_printEmmAddress(TRUE, 0);
		}		
		else
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
	}	
	else if (strcmp(aucStr, "rnsc")== 0)
	{
		VmxCmd_GetNScFromNFS(NULL);
	}	
	else if (strcmp(aucStr, "wnsc")== 0)
	{
		int i;
		HUINT16 pusLen = 360;//97;//96;//360;
#if 1 // from mnt/nfs
		vmx_vr_bc_indiv_WriteIndivData(&Temp_NSc_Data[0], &pusLen);
#else // from fixed stb data
//setwnsc();
	//	vmx_vr_bc_indiv_WriteIndivData(&STBData[0], &pusLen);
#endif
	}
	else if (strcmp(aucStr, "osminit")== 0)
	{
		BcCmdPrint(DBG_PRINT, ("- command osm init -\n"));
		VK_memset(buf, 0, sizeof(buf));
		wMode = 0;
		x = 0;
		y = 0;
		w = 100;
		h = 100;
		bBackground = 10;
		bAlpha = 10;
		bForeGround = 3;
		bDisplayMode = 0;
		wDuration = 0;
		sprintf(buf, "%s",  "Hellow !\nUser Command");
	}
	else if (strcmp(aucStr, "osd")== 0)
	{
		aucArg = (char *)CMD_NextArg((char **)&szArgList);
		if(aucArg == NULL)
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
		if( snprintf(aucStr, 100, "%s", aucArg) == 0  )
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}

		BcCmdPrint(DBG_PRINT, ("- command osmmsg [%s] -\n", aucStr));

		if(strcmp(aucStr, "mode") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;

			wMode = ulCmd;	/* 0 ~ 15 */
		}
		else if(strcmp(aucStr, "x") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			x = ulCmd;
		}
		else if(strcmp(aucStr, "y") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			y= ulCmd;
		}
		else if(strcmp(aucStr, "w") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			w= ulCmd;
			wMode |= 0x100;
		}
		else if(strcmp(aucStr, "h") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			h= ulCmd;
			wMode |= 0x100;
		}	
		else if(strcmp(aucStr, "bg") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			bBackground= ulCmd;
		}
		else if(strcmp(aucStr, "alpha") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			bAlpha= ulCmd;
		}
		else if(strcmp(aucStr, "fg") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			bForeGround= ulCmd;
		}
		else if(strcmp(aucStr, "x_align") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;

			//clear x_pos align bits
			wMode &= 0xFFF3;

			/*
				0: left side as reference (default)
				1: center side as reference
				2: right side as reference
			*/
			if(ulCmd < 3)
			{
				wMode |= ulCmd << 2;
			}
		}
		else if(strcmp(aucStr, "y_align") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;

			//clear y_pos align bits
			wMode &= 0xFFFC;

			/*
				0: bottom side as reference (default)
				1: center side as reference
				2: upper side as reference
			*/
			if(ulCmd < 3)
			{
				wMode |= ulCmd;
			}
		}
		else if(strcmp(aucStr, "text_align") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;

			//clear text align bits
			wMode &= 0xFFCF;
			/*
				0: align left
				1: align right
				2: align center
				3: align justified
			*/
			if(ulCmd <= 3)
			{
				wMode |= ulCmd << 4;
			}
		}
		else
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			VK_memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), aucStr);
		}

		BcCmdPrint(DBG_PRINT, ("- ulCmd  [%d] -\n", ulCmd));
		BcCmdPrint(DBG_PRINT, ("- OSD  * wMode(%d)\n", wMode));
		BcCmdPrint(DBG_PRINT, ("- OSD  * x(%d), y(%d), w(%d), h(%d)\n", x, y, w, h));
		BcCmdPrint(DBG_PRINT, ("- OSD  * bBackground(%d)\n", bBackground));
		BcCmdPrint(DBG_PRINT, ("- OSD  * bAlpha(%d)\n", bAlpha));
		BcCmdPrint(DBG_PRINT, ("- OSD  * bForeGround(%d)\n", bForeGround));
		BcCmdPrint(DBG_PRINT, ("- OSD  * msg = %s\n", buf));

		vmx_vr_bc_ui_SetOsdBuildWindow(buf, wMode, x, y, w, h, bBackground, bAlpha, bForeGround);				
	}	
	else if (strcmp(aucStr, "trigger") == 0)
	{
		BcCmdPrint(DBG_PRINT, ("- command osmtrigger -\n"));
		aucArg = CMD_NextArg( (char **)&szArgList );
		if( aucArg == NULL )
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
		sscanf(aucArg, "%d", &ulCmd) ;
		sprintf(buf, "%s",  OSM_Trigger_Buf[ulCmd]);

		BcCmdPrint(DBG_PRINT, ("- TRIGGER - ulCmd[%d] - %s : %s -\n", ulCmd, OSM_Trigger_Printf_Buf[ulCmd], buf));

		vmx_vr_bc_ui_SetOsdBuildWindow(buf, wMode, x, y, w, h, bBackground, bAlpha, bForeGround);
	}
	else if (strcmp(aucStr, "show") == 0)
	{
		aucArg = (char *)CMD_NextArg((char **)&szArgList);
		if(aucArg == NULL)
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
		if( snprintf(aucStr, 100, "%s", aucArg) == 0  )
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}

		BcCmdPrint(DBG_PRINT, ("- command osmdisplay [%s]-\n", aucStr));

		if(strcmp(aucStr, "mode") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			bDisplayMode= ulCmd; // 1 ~ 8 
		}
		else if(strcmp(aucStr, "dur") == 0)
		{
			aucArg = CMD_NextArg( (char **)&szArgList );
			if( aucArg != NULL ) sscanf(aucArg, "%d", &ulCmd) ;
			wDuration= ulCmd; // 0 ~
		}
		else if(strcmp(aucStr, "on") == 0)
		{
			vmx_vr_bc_ui_SetOsdDisplayWindow(bDisplayMode, wDuration);
		}
		else
		{
			BcCmdError(DBG_ASSERT, ("\n"));
		}

		BcCmdPrint(DBG_PRINT, ("- SHOW  * wMode = %d\n", wMode));
		BcCmdPrint(DBG_PRINT, ("- SHOW  * x(%d), y(%d), w(%d), h(%d)\n", x, y, w, h));
		BcCmdPrint(DBG_PRINT, ("- SHOW  * bBackground = %d\n", bBackground));
		BcCmdPrint(DBG_PRINT, ("- SHOW  * bAlpha = %d\n", bAlpha));
		BcCmdPrint(DBG_PRINT, ("- SHOW  * bForeGround = %d\n", bForeGround));
		BcCmdPrint(DBG_PRINT, ("- SHOW  * bDisplayMode = %d\n", bDisplayMode));
		BcCmdPrint(DBG_PRINT, ("- SHOW  * wDuration = %d\n", wDuration));
		BcCmdPrint(DBG_PRINT, ("- SHOW  * msg = %s\n", buf));
		
		
	}
	else if (strcmp(aucStr, "pinorder") == 0)
	{
		HUINT8 bPinIndex = 0;
		HUINT32 lPurse = 0;
		HUINT32 lCost = 0;
		HUINT16 wMult = 0;
		HUINT16 wDiv = 0;
		HUINT8 bLocation = 0;
		HUINT8 bSign0 = '.';
		HUINT8 bSign1000 = ',';
		HUINT8 bCount = 0;
		HUINT8 bLen = 0;
		HUINT8 abText[100];
		HUINT32 lEventId = 0;
		HUINT8 bServiceIdx = 0;
		
		aucArg = (char *)CMD_NextArg((char **)&szArgList);
		if(aucArg == NULL)
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
		if( snprintf(aucStr, 100, "%s", aucArg) == 0  )
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}

		BcCmdPrint(DBG_PRINT, ("- command osmdisplay [%s]-\n", aucStr));

		if(strcmp(aucStr, "test1") == 0)
		{
			// test를 위해 아래의 값을 변경시켜주자.
			/* resulting in Credit: $3,000.00 Cost: $1.50 */
			bPinIndex = 0;
			lPurse = 6000;
			lCost = 3;
			wMult = 1;
			wDiv = 2;
			bLocation = 1;
			bSign0 = '.';
			bSign1000 = ',';
			bCount = 2;
			bLen = 1;
			memset(abText, 0, sizeof(abText));
			sprintf(abText, "%s", "$");
			lEventId = 0;
			bServiceIdx = 0;

			BcCmdPrint(DBG_PRINT, ("- command pinorder - \n"));
			BcCmdPrint(DBG_PRINT, ("- Expect Result : %s - \n", "Credit: $3,000.00 Cost: $1.50 "));		
		}
		else if(strcmp(aucStr, "test2") == 0)
		{
			/* resulting in Credit: 4.000,00 EUR Cost: 2,00 EUR */
			bPinIndex = 0;
			lPurse = 6000;
			lCost = 3;
			wMult = 2;
			wDiv = 3;
			bLocation = 0;
			bSign0 = ',';
			bSign1000 = '.';
			bCount = 2;
			bLen = 4;
			memset(abText, 0, sizeof(abText));
			sprintf(abText, "%s", "EUR");
			lEventId = 0;
			bServiceIdx = 0;
			BcCmdPrint(DBG_PRINT, ("- command pinorder - \n"));
			BcCmdPrint(DBG_PRINT, ("- Expect Result : %s - \n", "Credit: 4.000,00 EUR Cost: 2,00 EUR"));				
		}
		else
		{
			BcCmdError(DBG_ASSERT, ("- command pinorder - \n"));
		}

		vmx_vr_bc_ui_SetOrderPin(bPinIndex, lPurse, lCost, wMult, wDiv, bLocation, bSign0, bSign1000, bCount, bLen, abText, lEventId, bServiceIdx);
	}
	else if (strcmp(aucStr, "pincheck") == 0)
	{
		VmxBc_PinReason_e eReason;
		HUINT8 bPinIndex;
		HUINT8 bServiceIdx;
		// test를 위해 아래의 값을 변경시켜주자.
		aucArg = (char *)CMD_NextArg((char **)&szArgList);
		if(aucArg == NULL)
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
		if( snprintf(aucStr, 100, "%s", aucArg) == 0  )
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}

		if(strcmp(aucStr, "pc") == 0)
			eReason = eVmxBc_PinReason_PC;
		else if(strcmp(aucStr, "ippv") == 0)
			eReason = eVmxBc_PinReason_IPPV;
		else if(strcmp(aucStr, "pcnsc") == 0)
			eReason = eVmxBc_PinReason_PC_UsingNonSmartcardPin;
		else if(strcmp(aucStr, "resume") == 0)
			eReason = eVmxBc_PinReason_ResumingOfEvent;
		else if(strcmp(aucStr, "select") == 0)
			eReason = eVmxBc_PinReason_SelectiingOfEvent;
		else
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}

		BcCmdPrint(DBG_PRINT, ("- command pinorder [%s] Reason[0x%x]-\n", aucStr,eReason));
		
		bPinIndex = 0;
		bServiceIdx = 0;

		vmx_vr_bc_ui_SetCheckPin(bPinIndex, eReason, bServiceIdx);
	}
	else if (strcmp(aucStr, "fpincheck") == 0)
	{
		VmxBc_VerificationPin_t pin;
		VmxBc_BCD_Pin_t stBcdPin;

		aucArg = (char *)CMD_NextArg((char **)&szArgList);
		if(aucArg == NULL)
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}
		if( snprintf(aucStr, 100, "%s", aucArg) == 0  )
		{
			BcCmdError(DBG_ASSERT, ("cmd error\n"));
			return CMD_ERR;
		}

		if(strcmp(aucStr, "ippv") == 0) // channel : SAT.1 - 강제로 pincode를 5372로 입력한다.
		{
			pin.stBcdPin.ucPin[0]= 5;
			pin.stBcdPin.ucPin[1]= 3;
			pin.stBcdPin.ucPin[2]= 7;
			pin.stBcdPin.ucPin[3]= 2;
			pin.stBcdPin.ucPinLength = 4;

			pin.ucPinIndex = 128;
			pin.ucReason = 1; // ippv 
			pin.ucServiceIdx = 0;
		}
		else if(strcmp(aucStr, "pc") == 0) // channel : SAT.1 - 강제로 pincode를 5372로 입력한다.
		{
			pin.stBcdPin.ucPin[0]= 5;
			pin.stBcdPin.ucPin[1]= 3;
			pin.stBcdPin.ucPin[2]= 7;
			pin.stBcdPin.ucPin[3]= 2;
			pin.stBcdPin.ucPinLength = 4;

			pin.ucPinIndex = 128;
			pin.ucReason = 3; // pc for non-smartcard
			pin.ucServiceIdx = 0;
		}
		vmx_vr_bc_ui_SetPinVerificationToBC(&pin);
	}
	else
	{
		BcCmdError(DBG_ASSERT, ("- user command is not valid\n"));
	}
	return CMD_OK;
}

void xsvc_cas_vmx_bc_SetUserCmd(void)
{
	BcCmdPrint(DBG_PRINT, ("[Enter]\n"));

	CMD_RegisterWord(VmxCmd,
						"vmx",
						"verimatrix user command",
						"- help \n \
        - cmd- information \n \
        - init- osm buf init \n \
        - ver - get version info \n \
        - osd- osd test \n \
        - trigger- trigger test  num ber ( 1 ~ ) \n \
        - show- osd display \n \ 
        - pinorder- IPPV test \n \
        - pincheck- Pin Test \n \
        - emm- multi emm test");
}

void CMD_Register_UserCmd_Vmx(void)
{
	xsvc_cas_vmx_bc_SetUserCmd();
}

