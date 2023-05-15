/*
 * (c) 2011-2015 Humax Co., Ltd.
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
/********************************************************************
 * $Workfile:   vmx_vr_bc_ui_log.h  $
 * Project: 	 Verimatrix CAS
 * Author:		ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef __INT_VMXBCUILOG_H__
#define __INT_VMXBCUILOG_H__

HUINT8 *VmxBc_OsmType_Str[] =
{
	"eVmxBc_OsmType_Osd",
	"eVmxBc_OsmType_Trigger"
};

HUINT8 *VmxBc_OsmStauts_Str[] =
{
	"eVmxBc_OsmStatus_None",	
	"eVmxBc_OsmStatus_BuildWindow",
	"eVmxBc_OsmStatus_DisplayWindow"
};

HUINT8 *VmxBc_TriggerType_Str[] =
{
  	"eVmxBc_TriggerType_Upgrade",
	"eVmxBc_TriggerType_Rescan",
	"eVmxBc_TriggerType_Switch",	
	"eVmxBc_TriggerType_Watermarking",
	"eVmxBc_TriggerType_PinNumberReset",
	"eVmxBc_TriggerType_Eas",
	"eVmxBc_TriggerType_Max"
};

HUINT8 *VmxBc_UpgradeType_Str[] =
{
  	"eVmxBc_UpgradeType_Forced",
  	"eVmxBc_UpgradeType_Now",
  	"eVmxBc_UpgradeType_NowAfterUserConfirmation",
  	"eVmxBc_UpgradeType_PowerDown",
  	"eVmxBc_UpgradeType_PowerDownAfterUserConfirmation",
  	"eVmxBc_UpgradeType_PowerUp",
  	"eVmxBc_UpgradeType_PowerUpAfterUserConfirmation",
  	"eVmxBc_UpgradeType_OnNextServiceChange",
  	"eVmxBc_UpgradeType_OnNextNonDescramblingPeriod", 
  	"eVmxBc_UpgradeType_Max"
};

HUINT8 *VmxBc_RescanType_Str[] =
{
  	"eVmxBc_RescanType_Now",
  	"eVmxBc_RescanType_NowAfterUserConfirmation",
  	"eVmxBc_RescanType_PowerDown",
  	"eVmxBc_RescanType_PowerDownAfterUserConfirmation",
  	"eVmxBc_RescanType_PowerUp",
  	"eVmxBc_RescanType_PowerUpAfterUserConfirmation",
  	"eVmxBc_RescanType_OnNextServiceChange",
  	"eVmxBc_RescanType_OnNextNonDescramblingPeriod",
  	"eVmxBc_RescanType_Max"
};

HUINT8 *VmxBc_SwitchType_Str[] =
{
  	"eVmxBc_SwitchType_ServiceNow",
  	"eVmxBc_SwitchType_AtTime",
  	"eVmxBc_SwitchType_ForTimePeriod",
  	"eVmxBc_SwitchType_AtTimeForTimePeriod",
  	"eVmxBc_SwitchType_IfServiceCannotBeDescrambled",
  	"eVmxBc_SwitchType_UntilTimeReached",
  	"eVmxBc_SwitchType_BackNow",
  	"eVmxBc_SwitchType_BackInXSeconds",
  	"eVmxBc_SwitchType_BackAtTime",
  	"eVmxBc_SwitchType_Max"
};

HUINT8 *VmxBc_WindowMode_Str[] =
{
	"eVmxBc_DisPlayMode_CenterOfHeightAsReference",
	"eVmxBc_DisPlayMode_UpperSideAsReference",
	"eVmxBc_DisPlayMode_CenterOfWidthAsReference",
	"eVmxBc_DisPlayMode_RightSideAsReference",
	"eVmxBc_DisPlayMode_LsbOfTextAlignment",
	"eVmxBc_DisPlayMode_MsbOfTextAlignment",
	"eVmxBc_DisPlayMode_UseGivenWidthAndHeight"
};

HUINT8 *VmxBc_DisplayWindowMode_Str[] =
{
	"eVmxBc_DisplayWindow_RemoveByUser",
	"eVmxBc_DisplayWindow_NoRemoveByUser"
};

HUINT8 *VmxBc_DisplayWindowType_Str[] =
{
	"FX_NEXTSTB",
	"RM_NEXTSTB",
	"FX_NEXTCA",
	"RM_NEXTCA",
	"FX_STILLCA",
	"RM_STILLCA",
	"FX_IMMED",
	"RM_IMMED"
};

HUINT8 *VmxBc_Osm_X_Pos_Str[] =
{
	"eVmxBc_Osm_X_Pos_Left",
	"eVmxBc_Osm_X_Pos_Center",
	"eVmxBc_Osm_X_Pos_Right"
};

HUINT8 *VmxBc_Osm_Y_Pos_Str[] =
{
	"eVmxBc_Osm_Y_Pos_Upper",
	"eVmxBc_Osm_Y_Pos_Center",
	"eVmxBc_Osm_Y_Pos_Bottom"
};

HUINT8 *VmxBc_Osm_Text_Algi_Str[] =
{
	"eVmxBc_Osm_Text_Alignment_LeftAligned",
	"eVmxBc_Osm_Text_Alignment_RightAligned",
	"eVmxBc_Osm_Text_Alignment_Centered",
	"eVmxBc_Osm_Text_Alignment_Justified"
};

HUINT8 *VmxBc_PinReason_Type_Str[] =
{
	"eVmxBc_PinReason_PC",
	"eVmxBc_PinReason_IPPV",
	"This string is not exist",	
	"eVmxBc_PinReason_PC_UsingNonSmartcardPin",
	"eVmxBc_PinReason_ResumingOfEvent",
	"eVmxBc_PinReason_SelectiingOfEvent",
	"eVmxBc_PinReason_InternalSTBForSC",
	"eVmxBc_PinReason_InternalSTBForNSC"	
};

HUINT8 *VmxBc_PinIndex_Str[] =
{
	"eVmxBc_PinIndex_PC_PIN2",							/* PIN Index 0: Parental control PIN 2 */
	"eVmxBc_PinIndex_IPPC_PC_PIN1",					/* PIN Index 1: IPPV/parental control PIN 1 */
	"eVmxBc_PinIndex_STB_LOCK_PIN",					/* PIN Index 2: STB Lock PIN */
	"eVmxBc_PinIndex_HOME_SHOPPING_PIN"	
};

HUINT8 *VmxBc_DescState_str[] =
{
    "eVmxBc_DS_Ok",
    "eVmxBc_DS_Error",
    "eVmxBc_DS_NoECMs",
    "eVmxBc_DS_ClearOrForeignEs",
    "eVmxBc_DS_Preview",
    "eVmxBc_DS_Pairing",
    "eVmxBc_DS_MS",
    "eVmxBc_DS_NoCWs",
    "eVmxBc_Init"
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Betacrypt Library ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif		//#ifndef __INT_VMXBCUILOG_H__

