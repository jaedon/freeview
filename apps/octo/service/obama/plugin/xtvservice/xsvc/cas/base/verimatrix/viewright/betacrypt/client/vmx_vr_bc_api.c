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
	@file     vmx_vr_bc_api.c
	@brief    vmx_vr_bc_api.c (Verimatrix BC API)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#include <stdlib.h>

#include "bc_consts.h"
#include "bc_types.h"
#include "bc_main.h"

#include "mw_nvram_mgr.h"
#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_os.h"
#include "vmx_ui.h"
#include "_xsvc_cas_vmx_copycontrol.h"
#include "_xsvc_cas_vmx_psi.h"
#include "vmx_vr_bc_api.h"
#include "_xsvc_cas_vmx_bc_sc.h"
#include "vmx_vr_bc_mgr.h"

void (*BC_SC_Notify)(enBcScNotify_t value) = NULL;	// callback function between MW and VMX lib.
enBcScNotify_t 	BcScNotify = BC_SC_NONE;

static HLONG				s_ulRefSysTime;


HUINT8 *enBcScNotify_str[] =
{
    "BC_SC_NONE",
    "BC_SC_RW_COMPLETED",
    "BC_SC_SETPARAM_COMPLETED",
    "BC_SC_POWERDOWN_COMPLETED",
    "BC_SC_INSERTED",
    "BC_SC_REMOVED",
    "BC_SC_ERROR",
    "BC_SC_RESET",
    "BC_SC_MUTE"
};

HUINT8 *enCmd_str[] =
{
    "k_ConnectSc",
    "k_DisconnectSc",
    "k_ResetSc",
    "k_GetATRSc",
    "k_CardDetectSc"
};

HUINT8 *enScState_str[] =
{
    "k_SC_Ok",
    "k_SC_NotPresent",
    "k_SC_HwError",
    "k_SC_Rejected",
    "k_SC_UpdateRequired",
    "k_SC_NSC"
};

HUINT8 *enFilterMode_str[] =
{
	"k_PageSearch",
	"k_PageLocked",
	"k_DisableFilter"
};

unsigned char *enDescState_str[] =
{
    "k_DS_Ok",
    "k_DS_Error",
    "k_DS_NoECMs",
    "k_DS_ClearOrForeignEs",
    "k_DS_Preview",
    "k_DS_Pairing",
    "k_DS_MS",
    "k_DS_NoCWs",
    "k_Init"
};

HUINT8 *vmx_bc_client_enBcNotifyStr(enBcNotify_t state)
{
    switch (state)
    {
	    case k_BcPinVerified:
	        return "k_BcPinVerified";
	    case k_BcPinChanged:
	        return "k_BcPinChanged";
	    case k_BcPinFailure:
	        return "k_BcPinBlocked";
	    case k_BcPinBlocked:
	        return "k_BcPinBlocked";
	    case k_BcPinMayNotBeChanged:
	        return "k_BcPinMayNotBeChanged";
	    case k_BcPinMayNotBeBypassed:
	        return "k_BcPinMayNotBeBypassed";
	    case k_BcPinBadIndex:
	        return "k_BcPinBadIndex";
	    case k_BcPinBadLength:
	        return "k_BcPinBadLength";
	    case k_BcPinNotEnoughPurse:
	        return "k_BcPinNotEnoughPurse";
	    case k_BcPinGeneralError:
	        return "k_BcPinGeneralError";
	    default:
	        return "?";
    }
}

VmxBc_Cmd_e VmxBc_ConvertToUserSCCmd(enCmd_t cmd);
VmxBc_EcmFilterMode_t VmxBc_ConvertToUserEcmFilterMode(enFilterMode_t filterMode);

VmxBc_Cmd_e VmxBc_ConvertToUserSCCmd(enCmd_t cmd)
{
	VmxBc_Cmd_e eConvertedCmd = eVmxBc_Cmd_ConnectSc;

	switch(cmd)
	{
		case k_ConnectSc:
			eConvertedCmd = eVmxBc_Cmd_ConnectSc;
			break;
		case k_DisconnectSc:
			eConvertedCmd = eVmxBc_Cmd_DisconnectSc;
			break;
		case k_ResetSc:
			eConvertedCmd = eVmxBc_Cmd_ResetSc;
			break;
		case k_GetATRSc:
			eConvertedCmd = eVmxBc_Cmd_GetATRSc;
			break;
		case k_CardDetectSc:
			eConvertedCmd = eVmxBc_Cmd_CardDetectSc;
			break;
		default:
			break;
	}

	return eConvertedCmd;
}

VmxBc_EcmFilterMode_t VmxBc_ConvertToUserEcmFilterMode(enFilterMode_t filterMode)
{
	VmxBc_EcmFilterMode_t eConvertedEcmFilterMode = eVmxBc_EcmFilterMode_PageSearch;

	switch(filterMode)
	{
		case k_PageSearch:
			eConvertedEcmFilterMode = eVmxBc_EcmFilterMode_PageSearch;
			break;
		case k_PageLocked:
			eConvertedEcmFilterMode = eVmxBc_EcmFilterModePageLocked;
			break;
		case k_DisableFilter:
			eConvertedEcmFilterMode = eVmxBc_EcmFilterModeDisableFilter;
			break;
		default:
			break;
	}

	return eConvertedEcmFilterMode;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// Onput library API //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @brief  Initialises the betacrypt ® Subsystem
 *  @return  k_BcSuccess, k_BcError, k_BcNSc
 */
vint16_t vmx_vr_bc_api_BcInit(void)
{
	vint16_t rst;
	
	HxLOG_Print(DBG_PRINT, ("OUTPUT	: BC_Init() [Enter]\n");

	s_ulRefSysTime = VK_TIMER_GetSystemTick();
	rst = BC_Init();

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_Init() [Exit]\n");

	return rst;
}

/**
 *  @brief  checks the existence of a NSc implementation and if NSc is already initialized
 *  @return k_BcSuccess, k_BcError, k_BcNotSupported
 */
vint16_t vmx_vr_bc_api_CheckNSc( void )
{
	vint16_t rst;

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_CheckNSc() [Enter]\n");

	rst = BC_CheckNSc();

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_CheckNSc() [Exit]\n");

	return rst;
}

#ifdef BC_NSC_INCLUDED
/**
 *  @brief  IInitialises the betacrypt ® Non-Smart Card implementation, if a correct configuration is given
 *  @param[in]  pabPtr
 *                pointer to memory where the personalization data is available
 *  @param[in]  pwLen
 *                length of this memory part used for personalization
 *  @return  k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_InitNSc(vuint8_t* pabPtr, vuint16_t* pwLen )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_InitNSc() [Enter] * pwLen(%d)\n", * pwLen);

	if(BC_InitNSc(pabPtr, pwLen) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_InitNSc() [Exit] lenth = %d \n", *pwLen);
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_InitNSc() [Exit]\n");

	return ERR_OK;
}

/**
 *  @brief  Initialises the betacrypt ® Non-Smart Card individualization, by generating a message that is to be sent to the indvidualization tool, 
  	  	  which responses by sending the individualization data that is handed over to the library by the BC_InitNsc call.
 *  @param[output]  pabPtr
 *                pointer to memory where the message is to be put
 *  @param[output]  wLen
 *                length of this memory part that can be written (as input parameter) and the length of the data used, as output data
 *  @param[in]  pabBoxSerNo
 *			pointer to the set top box serial number the data must at least be 8 bytes binary
 *  @param[in]  pabManuId 
 *			hands over the manufacturer id given for the individualization
 *  @param[in]  pabManuData 
 *			hands over the manufacturer data given for the individualization
 *  @param[in]  pabProvId 
 *  			hands over the provider id given for the individualization
 *  @param[in]  pabProvData 
 *			hands over the provider data given for the individualization
 *  @return  k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_InitNScComm( vuint8_t* pabPtr, vuint16_t* pwLen, vuint8_t* pabBoxSerNo, vuint8_t* pabManuId, vuint8_t* pabManuData, vuint8_t* pabProvId, vuint8_t* pabProvData )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_InitNScComm() [Enter]\n");

	if(BC_InitNScComm(pabPtr, pwLen, pabBoxSerNo, pabManuId, pabManuData, pabProvId, pabProvData) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_InitNScComm() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_InitNScComm() [Exit]\n");

	return ERR_OK;
}
#endif

void vmx_vr_bc_api_Task(HUINT32 argc, void *argv)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);
 
	while (1)
	{		
		BC_Task();

		if(BcScNotify != BC_SC_NONE)
		{
			HxLOG_Print(DBG_PRINT, ("OUTPUT	: BC_SC_Notify() [Enter]  %s\n", enBcScNotify_str[BcScNotify]);
			BC_SC_Notify(BcScNotify);
			HxLOG_Print(DBG_PRINT, ("OUTPUT	: BC_SC_Notify() [Exit] \n");
			BcScNotify = BC_SC_NONE;
		}
		VK_TASK_Sleep(10);
	}

	HxLOG_Error(DBG_ASSERT, ("[Exit]\n");
}

/**
 *  @brief  Returns the version, date and build time of the library
 *  @param[output]  pacVersion
 *                pointer to memory for Date String; max 20 bytes  but max 32bytes for advanced security freatures
 *  @param[output]  pacDate
 *               pointer to memory for Time String, max 20 bytes
 *  @param[output]  pacTime
 *               pointer to memory for Time String, max 20 bytes
 * 
 *  Changed Functions
 * 		    This function has a change in the size of the first parameter.
 * 		    While in normal version max 20 bytes are used for the version
 * 		    string, for advanced security a max of 32 bytes is needed.
 *  @return  k_BcSuccess, k_BcError
 */
void vmx_vr_bc_api_GetVersion( vuint8_t* pacVersion, vuint8_t* pacDate, vuint8_t* pacTime )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_GetVersion() [Enter]\n");

	BC_GetVersion(pacVersion, pacDate, pacTime);

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_GetVersion() [Exit]\n");
}

/**
 *  @brief  Returns the CA system ID if available
 *  @return  CA system ID, k_CaIdNotAvailable = 0xFFFF
 */
HERROR vmx_vr_bc_api_GetCASystemID (vuint16_t *usCaSystemId)
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_Get_CASystemID() [Enter]\n");

	* usCaSystemId = BC_Get_CASystemID();

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_Get_CASystemID() > usCaSystemId(0x%x)[Exit]\n", * usCaSystemId);

	return ERR_OK;
}

/**
 *  @brief  Set the EMM PID to the betacrypt®library
 *  @param[in]  wEmmPid
 *               EMM PID, k_EmmPidNotAvailable = 0xFFFF
 *  @return  k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_SetEmmPid(vuint16_t wEmmPid)
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_SetEMM_Pid() [Enter] - u16EmmPid(%d)(0x%x)\n\n", wEmmPid, wEmmPid);

	if(BC_SetEMM_Pid(wEmmPid) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_SetEMM_Pid() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_SetEMM_Pid() [Exit]\n");

	return ERR_OK;
}

/**
 *  @brief  Returns the Macrovision Control of the selected service
 *  @param[in]  wServiceId
 *               Service ID of the service to return the control
 *  @param[in]  bServiceIdx
 * 		     Service index used to identify the service
 *  @return MacroVisionControl byte, k_MacroVisionControlNotAvailable = 0xFF
 */
vuint8_t vmx_vr_bc_api_GetMacroVisionCtrl( vuint16_t wServiceId, vuint8_t bServiceIdx )
{
	vuint8_t rst;

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_GetMacroVisionCtrl() [Enter]\n");

	rst = BC_GetMacroVisionCtrl(wServiceId, bServiceIdx );

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_GetMacroVisionCtrl() [Exit]\n");

	return rst;
}

/**
 *  @brief  Performs a verification of the PIN given by bPinIndex
 *  @param[in]  bPinLength
 *               Length of the transmitted PIN in digits
 *  @param[in]  pabPin
 * 		    PIN to be checked
 *  @param[in]  bPinIndex
 * 		    PIN to be checked
 *  @param[in]  bReason
 * 		    Reason, why pin check is performed: 0xFF: internal STB for Smart Card 0xFE: internal STB for Non-Smart Card
 *  @param[in]  bServiceIdx
 * 		    the index of the service for which the check should be done
 *  @return k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_CheckPin( vuint8_t bPinLength, vuint8_t* pabPin, vuint8_t bPinIndex, vuint8_t bReason, vuint8_t bServiceIdx )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_CheckPin() [Enter]\n");

	if(BC_CheckPin(bPinLength, pabPin, bPinIndex, bReason, bServiceIdx) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_CheckPin() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_CheckPin() [Exit]\n");

	return ERR_OK;
}

/**
 *  @brief  Performs a change of the PIN given by bPinIndex
 *  @param[in]  bOldPinLength
 *               Length of the old PIN in digits
 *  @param[in]  pabOldPin
 * 		    Pointer to old PIN
 *  @param[in]  bNewPinLength
 * 		    Length of the new PIN in digits
 *  @param[in]  pabNewPin
 * 		    Pointer to new PIN
 *  @param[in]  bPinIndex
 * 		    PIN to be changed
 *  @return k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_ChangePin( vuint8_t bOldPinLength, vuint8_t* pabOldPin, vuint8_t bNewPinLength, vuint8_t* pabNewPin, vuint8_t bPinIndex )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_ChangePin() [Enter]\n");

	if(BC_ChangePin(bOldPinLength, pabOldPin, bNewPinLength, pabNewPin, bPinIndex) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_ChangePin() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_ChangePin() [Exit]\n");

	return ERR_OK;
}

void vmx_vr_bc_api_NotifyFct(enBcNotify_t state)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: [Notify] (%s)\n", vmx_bc_client_enBcNotifyStr(state));
	// TODO_VMX
}

/**
 *  @brief  I/O Control function
 *  @param[in]  cmd
 *               Command argument the routine interprets as the operation to be performed
 *  @param[in]  pvParams
 * 		   Passes parameters between the host SW and the betacrypt ® library pwLen: reserved for future use
 *  @return k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_SetIoctlNotify(void)
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_Ioctl() [Enter]\n");

	if(BC_Ioctl(k_ConnectBc, vmx_vr_bc_api_NotifyFct, NULL) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_Ioctl() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_Ioctl() [Exit]\n");

	return ERR_OK;
}

void vmx_vr_bc_api_ConnectScNotify(vvoid_t* pvParams)
{
	HxLOG_Print(DBG_PRINT, ("STBMW	: [Enter] vmx_vr_bc_api_ConnectScNotify\n");

	if(pvParams == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("STBMW	: pvParams is NULL\n");
	}

	BC_SC_Notify = pvParams;

	HxLOG_Print(DBG_PRINT, ("STBMW	: [Exit] vmx_vr_bc_api_ConnectScNotify\n");
}

void vmx_vr_bc_api_ScNotify(enBcScNotify_t eNodify)
{
	if (BC_SC_Notify != NULL)
	{
#if 1
		if(eNodify==BC_SC_INSERTED)
		{
			HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_SC_Notify() [Ener] (%s)\n", enBcScNotify_str[eNodify]);
			BC_SC_Notify(eNodify);
			HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_SC_Notify() [Exit] \n");
		}
		else
		{
			HxLOG_Print(DBG_PRINT, ("STBMW	 Ready BC_SC_Notify(%s) \n", enBcScNotify_str[eNodify]);
			BcScNotify = eNodify;
		}
#else
		HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_SC_Notify() [Enter] (%s) \n", enBcScNotify_str[eNodify]);
		BC_SC_Notify(eNodify);
		HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_SC_Notify() [Exit] \n");
#endif
	}
	else
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
	}
}

/**
 *  @brief  Triggers the betacrypt ® library to descramble the selected service
 *  @param[in]  wServiceId
 *               Service ID of the service to open
 *  @param[in]  wIndex
 * 		    Number of PIDs to descramble
 *  @param[in]  pawEcmPid
 * 		    pointer to an array of ECM PIDs
 *  @param[in]  pawStreamPid
 * 		    Pointer to new PIN
 *  @param[in]  bServiceIdx
 * 		    index of the service which should be started
 *  @return k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_StartDescrambling( vuint16_t wServiceId, vuint16_t wIndex, vuint16_t * pawEcmPid, vuint16_t *pawStreamPid, vuint8_t bServiceIdx )
{
	int i;
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_StartDescrambling() [Enter] - wServiceId(0x%x), wIndex(%d), bServiceIdx(%d)\n", wServiceId, wIndex, bServiceIdx);

#ifdef CONFIG_DEBUG
	// wIndex 갯수만큼 pawEcmPid와 pawStreamPid에 등록되어 있어야한다.
	 if(VMX_DBG_VR_BC_API == 1)
	{
		for(i=0;i<wIndex;i++)
		{
			HxLOG_Print ("wIndex[%d] : Pid(0x%x), EcmPid(0x%x)\n",i, pawStreamPid[i], pawEcmPid[i]);
		}
	}
#endif

	if(BC_StartDescrambling(wServiceId, wIndex, pawEcmPid, pawStreamPid, bServiceIdx) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_StartDescrambling() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_StartDescrambling() [Exit]\n");

	return ERR_OK;
}

/**
 *  @brief  Triggers the betacrypt®library to stop descrambling the requested service
 *  @param[in]  wServiceId
 *               Service ID of the service to stop
 *  @param[in]  wIndex
 * 		    service index used to identify the service
 *  @return k_BcSuccess, k_BcError
 */
HERROR  vmx_vr_bc_api_StopDescrambling( vuint16_t wServiceId, vuint8_t bServiceIdx )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_StopDescrambling() [Enter]- wServiceId(0x%x), bServiceIdx(%d)\n", wServiceId, bServiceIdx);

	if(BC_StopDescrambling(wServiceId, bServiceIdx) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_StopDescrambling() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_StopDescrambling() [Exit]\n");

	return ERR_OK;
}

/**
 *  @brief  Sets the filter address assigned to the STB for EMMfiltering in the betacrypt ®library 
 *  @param[in]  bAddressLen
 *               length of the address in the second parameter
 *  @param[in]  pabAddress
 * 		    address to be used for filtering MSB first
 *  @return k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_SetSTBFilterAddress( vuint8_t bAddressLen, vuint8_t* pabAddress)
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_SetSTBFilterAddress() [Enter]\n");

	if(BC_SetSTBFilterAddress(bAddressLen, pabAddress) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_SetSTBFilterAddress() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_SetSTBFilterAddress() [Exit]\n");

	return ERR_OK;
}

/**
 *  @brief  Initializes the On Screen Message functionality to display messages out of the betacrypt®library
 *  @param[in]  wMaxWidth
 *               defines the maximum width of the positioning system for On Screen Messages
 *  @param[in]  wMaxHeight
 * 		    defines the maximum height of the positioning system for On Screen Messages
 *  @param[in]  pabUnblockString
 * 		    defines the text to be added to ask the user for manual removal of the On Screen Message
 *  @return none
 */
void vmx_vr_bc_api_InitWindow( vuint16_t wMaxWidth, vuint16_t wMaxHeight, vuint8_t* pabUnblockString )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_InitWindow() [Enter] - width(%d), height(%d)\n", wMaxWidth, wMaxHeight);

	BC_InitWindow(wMaxWidth, wMaxHeight, pabUnblockString);

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_InitWindow() [Exit]\n");
}


/**
 *  @brief  fills a string with the actual smartcard number.
 *  @param[in]  pabSerNo
 *               buffer to hold the string, This buffer must be at least 35 bytes large to hold the 16
 *		     byte serial number and the trailing 0 byte. for the real smart card as well as for the non-smart-card system.
 *  @param[output]  wMaxHeight
 * 		    defines the maximum height of the positioning system for On Screen Messages
 *  @param[in]  wLen
 * 		    lengthg of buffer given
 *  @return k_BcSuccess, k_BcError
 */
HERROR vmx_vr_bc_api_GetSCNo( vuint8_t* pabSerNo, vuint16_t wLen )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_GetSCNo() [Enter]\n");

	if(BC_GetSCNo(pabSerNo, wLen) != k_BcSuccess)
	{
		HxLOG_Error(DBG_ASSERT, ("OUTPUT : BC_GetSCNo() [Exit]\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_GetSCNo() [Exit]\n");

	return ERR_OK;
}

/**
 *  @brief  results in a call of the given parameter function to display the actual purse value(s)
 *  @param[in]  pvParams
 *               a function that is called after the pursevalues have been read from the smartcard and/or smartcard-free.
 *  @return nothing
 */
void vmx_vr_bc_api_GetPurse( vvoid_t * pvParams )
{
	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_GetPurse() [Enter]\n");

	BC_GetPurse(pvParams);

	HxLOG_Print(DBG_PRINT, ("OUTPUT : BC_GetPurse() [Exit]\n");
}

#ifdef BC_CHAT_INCLUDED
vuint8_t* BC_ChatName( vuint8_t bChatId );
vvoid_t BC_StartChat( vuint8_t bChatId );
vvoid_t BC_ClearChat( vvoid_t );
#endif

#ifdef DOWNLOAD_CHECK
vint16_t  BC_CheckCertificate( vuint8_t* pabImage, vuint32_t wLength, vuint8_t* pabCert, vuint32_t wCertLength );
#endif

#ifdef MULTI_CA_SUPPORT
vvoid_t BC_EnableCA( vvoid_t );
vvoid_t BC_DisableCA( vvoid_t );
#endif

#ifdef BC_RC_INCLUDED
vint16_t BC_EncryptMSG( vuint8_t* pabMsg, vuint16_t wPos, vuint16_t wLength, vuint8_t bKeyToUse );
vint16_t BC_DecryptMSG( vuint8_t* pabMsg, vuint16_t wPos, vuint16_t wLength, vuint8_t bKeyToUse );
#endif

#ifdef BC_IP_INCLUDED
vvoid_t BC_OpenConnection( vuint32_t wIpId );
vvoid_t BC_CloseConnection( vuint32_t wIpId );
#endif

#ifdef BC_PVR_INCLUDED
vint16_t BC_PVRReplay( vuint32_t lEventId, vuint8_t bLen, vuint8_t* pabData, vuint8_t bServiceIdx );
#endif

vint16_t BC_CheckPairingState( vvoid_t );

#ifdef BC_DVR_INCLUDED
vint16_t BC_DVRRecord( vuint8_t bServiceIdx,
                      vuint8_t bChannelId,
                      vuint8_t* pabInfo,
                      vuint16_t wInfoLen,
                      vuint8_t * pabActualTime );
vint16_t BC_DVRReplay( vuint8_t bChannelId,
                    vuint8_t* pabInfo,
                    vuint16_t wInfoLen,
                    vuint8_t* pabStoreInfo,
                    vuint16_t wStoreInfoLen,
                    vuint8_t * pabActualTime );
vint16_t BC_DVRStop( vuint8_t bChannelId );
vint16_t BC_DVREncrypt( vuint8_t bChannelId,
                       vuint8_t* pabDest,
                       vuint8_t* pabSource,
                       vuint32_t lSize,
                       vuint8_t* pabStoreInfo,
                       vuint16_t* pwStoreInfoLen );
vint16_t BC_DVRDecrypt( vuint8_t bChannelId,
                       vuint8_t* pabDest,
                       vuint8_t* pabSource,
                       vuint32_t lSize );

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// Input library API //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  @brief
 *    Controls the ECM filter
 *  @param[in] FilterId
 *               Index of the ECM Filter
 *  @param[in] enFilterMode
 *               Mode the filter is used
 *  @param[in]  wEcmPid
 *                PID the filter should work on
 *  @param[in]  bTableId
 *                Table ID of the requested ECM
 *  @param[in]  bVersion
 *                Version of the requested ECM
 *  @param[in]  bPage
 *                Page of the requested ECM
 *  @return  return k_BcSuccess, k_BcError
 *  @remarks
 *    With this function call the betacrypt library sets up and stops a specific ECM filter with the behaviour defined in the parameter enFilterMode.
 */
vint16_t  FS_SetECMFilter(vuint8_t bFilterId, enFilterMode_t enFilterMode, vuint16_t wEcmPid, vuint8_t bTableId, vuint8_t bVersion, vuint8_t bPage)
{
	VmxBc_EcmFilterMode_t eFilterMode;

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]  bFilterId(%d), enFilterMode(%s), enFilterMode(0x%x), wEcmPid(0x%x), bTableId(0x%x), bVersion(0x%x), bPage(%d)\n", __FUNCTION__, bFilterId, enFilterMode_str[enFilterMode], enFilterMode, wEcmPid, bTableId, bVersion, bPage);

	eFilterMode = VmxBc_ConvertToUserEcmFilterMode(enFilterMode);
	if(vmx_vr_bc_ecm_SetEcmFilter(bFilterId, eFilterMode, wEcmPid, bTableId, bVersion, bPage) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

	return k_BcSuccess;
}

/**
 *  @brief  Reads one ECM from the appropriate ECM buffer
 *  @param[in]  bFilterId
 *                Filter ID to read data from
 *  @param[out]  pabBuffer
 *                Pointer to memory where to put the ECM,
 *  @param[in][out]  pwLen
 *                     pointer to an HUINT16, where to put in the length of the ECM (=number of bytes)
 *  @return  k_BcSuccess, k_BcError
 */

vint16_t  FS_ReadECM(vuint8_t bFilterId, vuint8_t* pabBuffer, vuint16_t* pwLen)
{
//	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);

	VMX_memset(pabBuffer, 0, * pwLen);
	if(vmx_vr_bc_ecm_ReadEcmBuf(bFilterId, pabBuffer, pwLen) != ERR_OK)
	{
//		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

//	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

	return k_BcSuccess;
}

/**
 *  @brief  Flushes the desired ECM buffer
 *  @param[in]  bFilterId
 *                The index of the ECM buffer to flush
 *  @return  k_BcSuccess
 */
vint16_t  FS_FlushECM_Buffer(vuint8_t bFilterId)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] bFilterId(%d)\n", __FUNCTION__, bFilterId);

	vmx_vr_bc_ecm_FlushEcmBuf(bFilterId);

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

	return k_BcSuccess;
}

/**
 *  @brief  Controls the EMM filter parameters
 *  @param[in]  FilterIndex
 *                Index to EMM filter to setup
 *  @param[in]  bAddressLength
 *                Length of the address the filter should work on
 *  @param[in]  pabAddress
 *                pointer to the address information
 *  @return  k_BcSuccess, k_BcFilterInUseError
 */
vint16_t  FS_SetEMMFilter(vuint8_t bFilterIndex, vuint8_t bAddressLength, vuint8_t* pabAddress)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [[Enter] bFilterIndex(%d), bAddressLength(%d), address(0x%x)\n", __FUNCTION__,  bFilterIndex, bAddressLength, pabAddress[0]);

	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_API, "FS_SetEMMFilter", pabAddress, bAddressLength);
	
	if(vmx_vr_bc_emm_SetEmmFilter(bFilterIndex, bAddressLength, pabAddress) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcFilterInUseError;
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

    	return k_BcSuccess;
}

/**
 *  @brief  Sets the current EMM-PID
 *  @param[in]  wEmmPid
 *  PID the EMM filter should filter on
 *  @return  k_BcSuccess, k_BcError
 */
vint16_t  FS_SetEMM_Pid(vuint16_t wEmmPid)
{
    	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [[Enter] wEmmPid(0x%x)\n",__FUNCTION__, wEmmPid);

	if(vmx_vr_bc_emm_SetEmmPid(wEmmPid) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

    	return k_BcSuccess;
}

/**
 *  @brief  Reads one EMM from the EMM buffer
 *  @param[out]  pabBuffer
 *            Pointer to memory where to put the EMM
 *  @param[in][out]  pwLen
 *                     pointer to an HUINT16, where to put in the length of the EMM (=number of bytes)
 *                     The input value defines the maximum allowed length. If the message is larger than this maximum size,
 *                     k_BcError is returned and *pwLen is set to 0 -no data copying shall take place in this case
 *  @return  k_BcSuccess, k_BcError
 */
vint16_t  FS_ReadEMM(vuint8_t* pabBuffer, vuint16_t* pwLen)
{
	HBOOL bError;

//	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] pwLen = %d\n", __FUNCTION__, * pwLen);

	VMX_memset(pabBuffer, 0, * pwLen);
	/*If no EMMs are available, FS_ReadEMM() should return k_BcSuccess with pwLen set to zero. */
	if(vmx_vr_bc_emm_ReadEmmBuf(pabBuffer, pwLen, &bError) != ERR_OK)
	{
		*pwLen = 0;
		// If the message is larger than this maximum size, k_BcError is returned and *pwLen is set to 0 .no data copying shall take place in this case.
		if(bError == TRUE)
		{
			*pwLen = 0;
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcError;
		}
		HxLOG_Print(DBG_PRINT, ("STB No Emm are available\n");
	}

//	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

    	return k_BcSuccess;
}

/**
 *  @brief  Flushes the EMM buffer
 *  @param  none
 *  @return  k_BcSuccess
 *  @remarks  All EMMs in the buffer are discarded
 */
vint16_t  FS_FlushEMM_Buffer(vvoid_t)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);

	vmx_vr_bc_emm_FlushEmmBuf();

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

    	return k_BcSuccess;
}

/**
 *  @brief  Starts descrambling of a service
 *  @param[in]  wIndex
 *                Number of stream PIDs in array
 *  @param[in]  pawStreamPid
 *                pointer to an array of stream PIDs
 *  @param[in]  bServiceIdx
 *                index of the service that is signalled
 *  @return  k_BcSuccess
 *  @remarks
 *    This function is called each time the control words change and is followed by a call of MMI_SetDescrambling_State() call with k_DS_Ok
 */
vint16_t  FS_StartDescrambling(vuint16_t wIndex, vuint16_t *pawStreamPid, vuint8_t bServiceIdx)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] wIndex(%d), bServiceIdx(%d)\n", __FUNCTION__, wIndex, bServiceIdx);

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

   	return k_BcSuccess;
}

/**
 *  @brief  Stops descrambling of a service
 *  @param[in]  bServiceIdx
 *                index of the service that is signalled
 *  @return  k_BcSuccess
 */
vint16_t  FS_StopDescrambling(vuint8_t bServiceIdx)
{
	// TODO_VMX : comment : BC_StartDescrambling()을 호출하면 lib에서 FS_StopDescrambling()을 호출함. 그럼 해당 serviceidx에 대해서 descrambler, ecm filter를 free 해주어야하는데
	// BC_StartDescrambling()를 호출한후 descrambler, ecm filter를 바로 진행하기 때문에 여기서 해주지 않음.
	// 추후 정상적으로 동작하는지 확인해보자.
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);

#ifdef VMX_SVC_TEST
	vmx_psi_SendMsg(eVmx_PsiMsg_StopSvc, bServiceIdx, NULL, NULL);
#endif

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

    	return k_BcSuccess;
}

/**
 *  @brief  Sets the state of descrambling for the current service
 *  @param[in]  wIndex
 *                Number of stream states returned
 *  @param[in]  pawStreamPid
 *                pointer to an array of stream PIDs the stream descrambling states corresponds to
 *  @param[in]  penDescState
 *                pointer to an array of descrambling state as defined in bc_consts.h
 *                In the mode signalled k_DS_Preview the video can already be descrambled but a buying dialogue might be started.
 *                This can be used to deactivate video display while no descrambling can happen.
 *                If mode k_DS_NoCWs is signalled, access to the service is in principle possible, but the service is either still under parental control,
 *                meaning no correct pin has yet been entered, or the service is with IPPV and has not yet been ordered.
 *  @param[in]  bServiceIdx
 *                index of the service that is signalled
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    It is called from the betacrypt library every time the descrambling state of one ore more streams of the selected service changes
 *    If MMI_SetDescrambling_State is called with k_DS_Pairing the ECM could be handled but the control words cannot be used, as the pairing is not OK.
 *    If MMI_SetDescrambling_State is called with k_DS_MS the ECM could be handled but the control words cannot be used, as the slave has no actual data from the master set top box.
 */
vint16_t  MMI_SetDescrambling_State(vuint16_t wIndex, vuint16_t* pawStreamPid, enDescState_t * paenDescState, vuint8_t bServiceIdx)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] wIndex(%d), bServiceIdx(%d)\n", __FUNCTION__, wIndex, bServiceIdx);

#if CONFIG_DEBUG
	if(VMX_DBG_VR_BC_API == 1)
	{
		int i;
		for (i = 0; i < wIndex; i++)
		{
			HxLOG_Print ("	index=%d, StreamPID=0x%x, DescState=%d(%s)\n", i, pawStreamPid[i], paenDescState[i], enDescState_str[paenDescState[i]]);
		}
	}
#endif
	
	if(vmx_vr_bc_ui_SetDescramblingState(wIndex, pawStreamPid, (void *)paenDescState, bServiceIdx) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
	
    	return k_BcSuccess;
}

/**
 *  @brief  Sets the state of the SC
 *  @param[in]  enScState
 *                SC state as defined in bc_consts.h
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    This MMI function informs the host SW of the current SC state.
 *    Furthermore it informs whether a betacrypt2 SC is inserted and a library update is required.
 *    In the latter the enScState equals k_SC_UpdateRequired. The host SW shall open an info dialogue in which the user is asked to confirm the SW update
 */
vint16_t  MMI_SetSmartcard_State(enScState_t enScState)
{
	vuint16_t usCaSystemId = 0;

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);

	if(enScState == k_SC_Ok)
	{
		vmx_vr_bc_api_GetCASystemID(&usCaSystemId);
		vmx_vr_bc_mgr_SetCaSystemId(usCaSystemId);
	}
	else if(enScState == k_SC_UpdateRequired)
	{

	}
	else
	{
//		usCaSystemId = 0;
		vmx_vr_bc_api_GetCASystemID(&usCaSystemId);
		vmx_vr_bc_mgr_SetCaSystemId(usCaSystemId);
	}
	
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s  [Exit]enScState(%d)(%s), usCaSystemId(0x%x)\n", __FUNCTION__, enScState, enScState_str[enScState], usCaSystemId);

    	return k_BcSuccess;
}

#if 0
HLONG getTimeDifference( HLONG tickCount1, HLONG tickCount2 )
{
	HLONG t1, t2;

	t1 = tickCount1 & 0x7fffffff;
	t2 = tickCount2 & 0x7fffffff;

	if( t1 > t2 ) /* wrap around */
	{
		t1 -= 0x40000000;
		t2 += 0x40000000;
	}

	return t2-t1;
}
#endif

/**
 *  @brief  Gets the timer tick variable represented in ms
 *  @param  none
 *  @return  timer tick value in ms
 *  @remarks
 *    The accuracy shall be at least 10 ms
 *    The wrap around can either be handled by keeping increasing the value and therefore staring from -2^31 running towards 0 or
 *    by clipping the highest bit and therefore restarting by 0.
 *    The time-difference must be positive.
 */
vint32_t  SYS_GetTickCount(vvoid_t)
{
	static HLONG	ulCurSysTime = 0;
	HLONG 		ulTime = 0;
	ulCurSysTime = VK_TIMER_GetSystemTick();

	if (ulCurSysTime >= s_ulRefSysTime)
	{
		ulTime = ulCurSysTime - s_ulRefSysTime;
//		HxLOG_Print(DBG_PRINT, ("INPUT	: %s  (%lu ms) .\n", __FUNCTION__,  ulTime/*, ulCurSysTime, s_ulRefSysTime*/);
	}
	else
	{
		ulTime = 0x7fffffff - s_ulRefSysTime + ulCurSysTime + 1;
//		HxLOG_Print(DBG_PRINT, ("INPUT	: %s  (%lu ms) ..\n", __FUNCTION__, ulTime/*, ulCurSysTime, s_ulRefSysTime*/);
	}

	if(ulTime < 0)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
	}

	return ulTime;
}

/**
 *  @brief  Reads Data from non volatile memory
 *  @param[out]  pabDest
 *                 pointer to destination
 *  @param[in]  wLength
 *                number of bytes to read
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    The host system must grant access to non-volatile memory (e.g. EEPROM or Flash) of size not smaller than 2048 byte.
 */
vint16_t  SYS_ReadNvmBlock(vuint8_t* pabDest, vuint16_t wLength)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] wLength(%d)\n", __FUNCTION__, wLength);

	// wLength must be have 2048's size.
	if(MW_NVRAM_ReadData("VMX_BC_BLOCK", 0, wLength, pabDest) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

//	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_API, "SYS_ReadNvmBlock", pabDest, wLength);
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

   	return k_BcSuccess;
}

/**
 *  @brief  Writes Data to non volatile memory
 *  @param[in]  pabDest
 *                pointer to data which should be written
 *  @param[in]  wLength
 *                number of bytes to write
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    The host system must grant access to non-volatile memory (e.g. EEPROM or Flash) of size not smaller than 2048 byte.
 *    The frequency of write accesses is very low and is therefore not critical at all with respect to guaranteed write cycles of flash memory.
 *    As a rule of thumb read/write operations may occur each time a Smart Card is inserted or the library is initialised.
 */
vint16_t  SYS_WriteNvmBlock(vuint8_t* pabSrc, vuint16_t wLength)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s  [Enter] wLength(%d)\n", __FUNCTION__, wLength);

	// wLength must be have 2048's size.
	if(MW_NVRAM_WriteData("VMX_BC_BLOCK", 0, wLength, pabSrc) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_API, "SYS_WriteNvmBlock", pabSrc, wLength);
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);	

	return k_BcSuccess;
}

#ifdef BC_NSC_INCLUDED
/**
 *  @brief  Reads Data from non volatile memory
 *  @param[in]  bFileId
 *                identifier for the two different memory locations (only 0 and 1 values are used for the multiple times read and write area,
 *                and 2 is used for the one time area of 16 kBytes)
 *  @param[in]  pabDest
 *                pointer to destination
 *  @param[in]  wLength
 *                number of bytes to read
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    The host system must grant access to non-volatile memory (e.g. EEPROM or Flash) of size not smaller than 2048 byte, for advanced security 4096 bytes,
 *    for bFileId being 0 or 1.
 *    This memory block differs from the memory block used for SYS_ReadNvmBlock/ SYS_WriteNvmBlock.
 *    This flash area is written every time a change in the smartcard free system configuration happens.
 * 
 *	
 *	<advanced secrutiy인경우 아래의 내용으로 진행햐야함 >
 * 	Changed Functions
 *		This function has a changed semantics. For bFileId = 0 and
 *		bFileId = 1, the size of the memory array is not 2048 bytes,
 *		but 4096 bytes. If DVR is activated this size increases to 8192
 *		bytes for each block.
 *		Normally this function is only needed if smartcard free is
 *		activated. In case of advanced security, this function is also
 *		needed for smartcard only operation.
 */
vint16_t  SYS_ReadNvmData(vuint8_t bBlockId, vuint8_t* pabDest, vuint16_t wLength)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s BlockId(%d), wLength(%d)\n", __FUNCTION__, bBlockId, wLength);

	if (bBlockId == 0)
	{
		// wLength must be have 4096's size for advanced security
		if(MW_NVRAM_ReadData("VMX_BC_NSC_FIELD0", 0, wLength, pabDest) != ERR_OK)
		{
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcError;
		}
	}
	else if (bBlockId == 1)
	{
		// wLength must be have 4096's size for advanced security
		if(MW_NVRAM_ReadData("VMX_BC_NSC_FIELD1", 0, wLength, pabDest) != ERR_OK)
		{
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcError;
		}

	}
	else if (bBlockId == 2)
	{
		// wLength must be have 32708's size for advanced security
		if(MW_NVRAM_ReadData("VMX_BC_NSC_DATA", 0, wLength, pabDest) != ERR_OK)
		{
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcError;
		}
		
	}
	else
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

	return k_BcSuccess;
}
#endif

#ifdef BC_NSC_INCLUDED
/**
 *  @brief  Writes Data to non volatile memory
 *  @param[in]  bFileId
 *                identifier for the two different memory locations (only 0 and 1 values are used for the multiple times read and write area,
 *                and 2 is used for the one time area of 16 kBytes)
 *  @param[in]  pabSrc
 *                pointer to data which should be written
 *  @param[in]  wLength
 *                number of bytes to write
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    The host system must grant access to non-volatile memory (e.g. EEPROM or Flash) of size not smaller than 2048 byte, for advanced security 4096 bytes.
 *    The frequency of write accesses is very low and is therefore not critical at all with respect to guaranteed write cycles of flash memory.
 *    Two memory blocks in different EEPROM/Flash segments must be provided to ensure, that a write failure due to power off guarantees the previously written memory block is valid.
 *    If the data in these segments get lost,the set top can no longer be used with the smartcard free system.
 *    The set top box cannot be reindividualized to the same id in case of such a loss.
 *	
 *	
 *	<advanced secrutiy인경우 아래의 내용으로 진행햐야함 >
 * 	Changed Functions
 *		This function has a changed semantics. For bFileId = 0 and
 *		bFileId = 1, the size of the memory array is not 2048 bytes,
 *		but 4096 bytes. If DVR is activated this size increases to 8192
 *		bytes for each block.
 *		Normally this function is only needed if smartcard free is
 *		activated. In case of advanced security, this function is also
 *		needed for smartcard only operation.
 */
vint16_t  SYS_WriteNvmData(vuint8_t bBlockId, vuint8_t* pabSrc, vuint16_t wLength)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] BlockId(%d) wLength = %d\n", __FUNCTION__, bBlockId, wLength);

	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_API, "SYS_WriteNvmData", pabSrc, wLength);

	if (bBlockId == 0)
	{
		// wLength must be have 4096's size for advanced security
		if(MW_NVRAM_WriteData("VMX_BC_NSC_FIELD0", 0, wLength, pabSrc) != ERR_OK)
		{
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcError;
		}		
	}
	else if (bBlockId == 1)
	{
		// wLength must be have 4096's size for advanced security
		if(MW_NVRAM_WriteData("VMX_BC_NSC_FIELD1", 0, wLength, pabSrc) != ERR_OK)
		{
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcError;
		}		
	}
	else if (bBlockId == 2)
	{
		// wLength must be have 32708's size for advanced security
		// NSc data는 factorySW에서 write됨으로 여기서 사용할 필요 없음.
		if(MW_NVRAM_WriteData("VMX_BC_NSC_DATA", 0, wLength, pabSrc) != ERR_OK)
		{
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcError;
		}		
	}
	else
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

    	return k_BcSuccess;
}
#endif

/**
 *  @brief  Returns a random number
 *  @param  none
 *  @return  32-bit random number
 */
vint32_t  SYS_Random(vvoid_t)
{
	vint32_t x = 0;

	// TODO_VMX
	x = rand() & 0xff;
	x |= (rand() & 0xff) << 8;
	x |= (rand() & 0xff) << 16;
	x |= (rand() & 0xff) << 24;

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] %lu\n", __FUNCTION__, x);

	return x;
}

/**
 *  @brief  Requests the host SW to process dialogue messages
 *  @param[in]  wDialogueLength
 *                Length of dialogue
 *  @param[in]  pabDialogue
 *                pointer to dialogue
 *  @return  k_BcSuccess, k_BcError
 */
vint16_t  SYS_SetDialogue(vuint16_t wDialogLength, vuint8_t* pabDialogue)
{
	// NOT USED	
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);
    	return k_BcSuccess;
}

/**
 *  @brief  returns the individual box id
 *  @param[out]  pabBoxId
 *                 this 8-byte-array is used to return the individual box id
 *  @return  none
 *  @remarks
 *    In case of an advanced security implementation, where the chip-set-id is available in the chip-set OTP-area,
 *    this call is used to inform the set top box application about the chip-set id, a 64bit-value.
 *    In this case the value must be taken by the set top box as the individual box id.
 * 
 *    Changed Functions
 *  		 This function has a changed semantics. While in the normal,
 *  		non advanced security mode, this function hands over the box
 *  		id from the set top box application to the library, the direction
 *  		is changed for advanced security. For advanced security the
 *  		betacrypt library reports the box-id, which is read from the chip
 *  		set, to the set top box application.
 */
vvoid_t SYS_GetBoxId( vuint8_t* pabBoxId )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_API, "SYS_GetBoxId", pabBoxId, 8);

	vmx_vr_bc_mgr_SetBoxId(pabBoxId);

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);	
}

/**
 *  @brief  Writes data to SC
 *  @param[in]  pabBuffer
 *                Pointer to message buffer (with error correction codes not included)
 *  @param[in]  pwLen
 *                Pointer to number of bytes to write to SC
 *  @param[out]  pwLen
 *                Pointer to number of bytes actually written to SC
 *  @param[in]  wTimeout
 *                Timeout in ms
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    The host software must implement the transport protocol description unit (TPDU) required to communicate with betacrypt compatible Smart Cards.
 */
vint16_t  SC_Write(vuint8_t* pabBuffer, vuint16_t* pwLen, vuint16_t wTimeout)
{
	HxLOG_Print(DBG_PRINT, ("[Enter] * pwLen(%d), wTimeout(%d)\n", * pwLen, wTimeout);
	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_API, "SC_Write", pabBuffer, * pwLen);
	if(vmx_vr_bc_sc_WriteToSC(pabBuffer, pwLen, wTimeout) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

    	return k_BcSuccess;
}

/**
 *  @brief  Reads data from SC
 *  @param[in]  pabBuffer
 *                Pointer to message buffer
 *  @param[in]  pwLen
 *                Pointer to length of message in byte
 *  @param[out]  pwLen
 *                Pointer to number of bytes actually read from SC
 *  @param[in]  wTimeout
 *                Timeout in ms
 *  @return  k_BcSuccess, k_BcError, k_BcTimedOut
 *  @remarks
 *    The host system must implement the transport protocol description unit (TPDU) required to communicate with the betacrypt SC.
 */
vint16_t  SC_Read(vuint8_t* pabBuffer, vuint16_t* pwLen)
{	
	HBOOL bTimeout = FALSE;

//	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);

	VMX_memset(pabBuffer, 0, * pwLen);
	if(vmx_vr_bc_sc_ReadFromSC(pabBuffer, pwLen, &bTimeout) != ERR_OK)
	{
		if(bTimeout == TRUE)
		{
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcTimedOut;
		}
		else
		{
			HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
			return k_BcError;
		}
	}

	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_API, "SC_Read", pabBuffer, *pwLen);
	
//	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
    	return k_BcSuccess;
}

/**
 *  @brief  I/O Controls to the SC device driver
 *  @param[in]  cmd
 *                Command argument the routine interprets as the operation to be performed. Defined in bc_consts.h
 *  @param[in]  pvParams
 *                Passes parameters between host SW and betacrypt library
 *  @param[in]  pwLen
 *                Length of message
 *  @param[out]  pwLen
*                  Number of bytes returned
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    The host system must implement the transport protocol description unit (TPDU) required to communicate with the betacrypt SC.
 */
vint16_t  SC_IOCTL(enCmd_t cmd, vvoid_t* pvParams, vuint16_t* pwLen)
{
	VmxBc_Cmd_e eCmd = VmxBc_ConvertToUserSCCmd(cmd);

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] cmd=%d(%s)\n",__FUNCTION__, cmd, enCmd_str[cmd]);
	
	if(vmx_vr_bc_sc_IoCtrlWithSC(eCmd, pvParams, pwLen) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return k_BcError;
	}

	if(cmd == k_GetATRSc)
	{
		xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_API, "ATR", pvParams, * pwLen);
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);

	return k_BcSuccess;
}

/**
 *  @brief  prepares an On Screen Message
 *  @param[in]  pabMsg
 *                the text message itself, a 0-terminated string
 *  @param[in]  wMode
 *                Passes parameters between host SW and betacrypt library
 *                Bit15 - Not used
 *                Bit14 - Not used
 *                Bit13 - Not used
 *                Bit12 - Not used
 *                Bit11 - Not used
 *                Bit10 - Not used
 *                Bit9  - Not used
 *                Bit8  - Use given width and height
 *                Bit7  - Not used
 *                Bit6  - Not used
 *                Bit5  - Msb of text alignment
 *                Bit4  - Lsb of text alignment
 *                Bit3  - Take the right side as reference
 *                Bit2  - Take the center of the width as reference
 *                Bit1  - Take the upper side as reference
 *                Bit0  - Take the center of the height as reference
 *  @param[in]  wX
 *                X-coordinate of anchor point
 *  @param[in]  wY
 *                Y-coordinate of anchor point
 *  @param[in]  wW
 *                width of the text box, if (wMode&0x100)==0x100 else ignored
 *  @param[in]  wH
 *                height of the text box, if (wMode&0x100)==0x100 else ignored
 *  @param[in]  bBackground
 *                color-index for the background color
 *  @param[in]  bAlpha
 *                alpha-blending index to be used
 *  @param[in]  bForeGround
 *                color-index for the foreground color
 *  @return  none
 *  @remarks
 *    The coordinates given in wX and wY are relative to the defined sizes of the BC_InitWindow function.
 *    The upper left corner of the screen is the position 0,0
 */
vvoid_t   OSD_BuildWindow( vuint8_t* pabMsg, vint16_t wMode, vint16_t wX, vint16_t wY, vint16_t wW, vint16_t wH, vint8_t bBackground, vint8_t bAlpha, vint8_t bForeground )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] wMode(%d), x(%d), y(%d), w(%d), h(%d), bg(0x%x), alpha(0x%x), fg(0x%x)\n", __FUNCTION__, wMode, wX, wY, wW, wH, bBackground, bAlpha, bForeground);

	if(vmx_vr_bc_ui_SetOsdBuildWindow(pabMsg, wMode, wX, wY, wW, wH, bBackground, bAlpha, bForeground) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);	
}

/**
 *  @brief  displays the prepare window for the given time
 *  @param[in]  bDisplayMode
 *                The LSB of DisplayMode tells, whether the window can be removed or not.
 *                If (wDisplayMode&1)==0 the window can be removed by the user, else not.
 *  @param[in]  wDuration
 *                defines the seconds the window is displayed for.
 *                If 0 is given, the window will displayed until a new On Screen Message
 *  @return
 *    0, 1
 *    The result is 1 if the window was displayed and the message can therefore be marked as displayed.
 *    If the result is 0, the message was not displayed (e.g. due to EMMfiltering when STB is turned off) and must therefore be rescheduled again.
 */
vuint16_t  OSD_DisplayWindow( vuint8_t bDisplayMode, vuint16_t wDuration )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] DisplayMode(%d), wDuration(%d)\n", __FUNCTION__,  bDisplayMode, wDuration);

	if(vmx_vr_bc_ui_SetOsdDisplayWindow(bDisplayMode, wDuration) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return 0;
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
    	return 1;
}

/**
 *  @brief  requests a PIN input from the user for the given index and with the selected textblock
 *  @param[in]  bPinIndex
 *                gives the Pin type which is requested
 *                The index defined is used for BC_CheckPin command after the Pin has been inserted
 *  @param[in]  bTextSelector
 *                defines the text message to be displayed for the user input
 *                0 - check for parental control
 *                1 - check for Impulse Pay Per View
 *                3 - check for parental control using non-smartcard pin
 *                4 - check for resuming of an event
 *                5 - check for selecting an event
 *  @param[in]  bServiceIdx
 *                gives the service index for which the pin request is targeted
 *  @return  none
 */
vvoid_t   ISC_CheckPin( vuint8_t bPinIndex, vuint8_t bTextSelector, vuint8_t bServiceIdx )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] bPinIndex(%d), bTextSelector(%d), bServiceIdx(%d)\n", __FUNCTION__, bPinIndex, bTextSelector, bServiceIdx);

	vmx_vr_bc_ui_SetCheckPin(bPinIndex, bTextSelector, bServiceIdx);

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);	
}

/**
 *  @brief  requests a PIN input from the user for the given index to order an IPPV event with the given costs
 *  @param[in]  bPinIndex
 *                gives the Pin type which is requested
 *  @param[in]  lPurse
 *                gives the number of tokens available in the customer purse of the smart card
 *  @param[in]  lCost
 *                gives the number of tokens needed to buy the current event
 *  @param[in]  wMult
 *                gives the value to multiply the purse/costs to generate the values in the currency
 *  @param[in]  wDiv
 *                gives the value to divide the purse/cost multiplied by wMult through to generate the values in the currencies
 *  @param[in]  bLocation
 *                defines, whether the currency information shall be put in front (!=0) or at the end (=0) of the number
 *  @param[in]  bSign0
 *                defines the sign to be used to separate full currency units from parts.
 *                If no partial digits are defined (bCount=0), the sign should not be displayed
 *  @param[in]  bSign1000
 *                defines the sign to be used to separate 1000 numbers. If it is 0, no separation is done
 *  @param[in]  bCount
 *                gives the number of part digits
 *  @param[in]  bLen
 *                gives the length of the currency text string
 *  @param[in]  abText
 *                points to the currency text string
 *  @param[in]  lEventId
 *                gives the event id (CA-related) for the current event
 *  @param[in]  bServiceIdx
 *                gives service index for which the request is made
 *  @remarks
 *    The index defined is used for BC_CheckPin command after the Pin has been inserted and bReason must be set to 1 for Impulse Pay Per View.
 *  @return  none
 */
vvoid_t   ISC_OrderPin( vuint8_t bPinIndex, vuint32_t lPurse,	vuint32_t lCost, vuint16_t wMult, vuint16_t wDiv, vuint8_t bLocation, vuint8_t bSign0, vuint8_t bSign1000, vuint8_t bCount, vuint8_t bLen, vuint8_t *abText, vuint32_t lEventId, vuint8_t bServiceIdx )
{
//	HxLOG_Print(DBG_PRINT, ("[Enter] PinIndex(%d), lPurse(%d), lCost(0x%08x), wMult(%d), wDiv(%d), bLocation(%d), bSign0(%d), bSign1000(%d), bCount(%d), bLen(%d), lEventId(0x%08x), bServiceIdx(%d)\n", 
//		bPinIndex, lPurse, lCost, wMult, wDiv, bLocation, bSign0, bSign1000, bCount, bLen, lEventId, bServiceIdx);	
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);

	vmx_vr_bc_ui_SetOrderPin(bPinIndex, lPurse, lCost, wMult, wDiv, bLocation, bSign0, bSign1000, bCount, bLen, abText, lEventId, bServiceIdx);

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);	
}


#ifdef BC_CHAT_INCLUDED
/**
 *  @brief  Displays the given chat text
 *  @param[in]  pabText
 *                text to be displayed
 *  @return  none
 */
void CHT_Display(HUINT8* pabText)
{
    HxLOG_Print(DBG_PRINT, ("\n");
}
#endif

#ifdef BC_IP_INCLUDED
/**
 *  @brief  starts a connection request for the given address
 *  @param[in]  pabName
 *                name of the conditional access server
 *  @param[in]  wPort
 *                port id of the connection at the server side
 *  @return  none
 */
vvoid_t IP_OpenConnectionByName( vuint8_t* pabName, vuint16_t wPort )
{
    HxLOG_Print(DBG_PRINT, ("\n");
}

/**
 *  @brief  starts a connection request for the given address
 *  @param[in]  pabAddr
 *                either IP4 or IP6 address of the conditional access server
 *  @param[in]  wPort
 *                port id of the connection at the server side
 *  @return  none
 */
vvoid_t IP_OpenConnectionByAddress( vuint8_t* lAddress, vuint16_t wPort )
{
    HxLOG_Print(DBG_PRINT, ("\n");
}

/**
 *  @brief  Starts a connection close request
 *  @param[in]  wIpId
 *                id of the connection
 *  @return  none
 */
vvoid_t IP_CloseConnection( vuint32_t wIpId )
{
    HxLOG_Print(DBG_PRINT, ("\n");
}

/**
 *  @brief  Send data using the opened ip connection
 *  @param[in]  wIpId
 *                id of the connection
 *  @param[in]  wLen
 *                number of bytes to be transferred
 *  @param[in]  pabBuffer
 *                pointer to the data to be transfered
 *  @return
 *    returns the descrambler id that has to be used for this elementary stream
 */
vint16_t IP_Send( vuint32_t wIpId, vint16_t wLen, vuint8_t* pabBuffer )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return 0;
}

/**
 *  @brief  Receives data using the opened ip connection
 *  @param[in]  wIpId
 *                id of the connection
 *  @param[in]  wLen
 *                number of bytes to be transferred
 *  @param[in]  pabBuffer
 *                pointer to the memory where the data should be placed
 *  @return
 *    number of received data bytes or negative number for an error
 */
vint16_t IP_Receive( vuint32_t wId, vint16_t wLen, vuint8_t* pabBuffer )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return 0;
}
#else
#ifdef BC_RC_INCLUDED
/**
 *  @brief  Reads one message from the return channel buffer
 *  @param[in]  pabBuffer
 *                Pointer to memory where to put the message
 *  @param[in]  pwLen
 *                pointer to an HUINT16, where to put in the length of the message (=number of bytes)
 *  @return  k_BcSuccess, k_BcError
 */
vint16_t RC_ReadMSG( vuint8_t* pabBuf, vuint16_t* pwLen )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return k_BcSuccess;
}

/**
 *  @brief  Flushes the read message buffer
 *  @param  none
 *  @return  k_BcSuccess
 */
vint16_t RC_FlushReadMSG_Buffer( vvoid_t )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return k_BcSuccess;
}

/**
 *  @brief  Writes one message to the return channel buffer
 *  @param[in]  pabBuffer
 *                Pointer to memory with the message
 *  @param[in]  wLen
 *                giving the length of the message (=number of bytes)
 *  @return  k_BcSuccess, k_BcError
 */
vint16_t RC_WriteMSG( vuint8_t* pabBuf, vuint16_t wLen )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return k_BcSuccess;
}

/**
 *  @brief  Flushes the write message buffer
 *  @param  none
 *  @return  k_BcSuccess
 */
vint16_t RC_FlushWriteMSG_Buffer( vvoid_t )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return k_BcSuccess;
}

/**
 *  @brief  Checks for pending messages in the write message queue
 *  @param  none
 *  @return  k_BcSuccess, k_BcError, k_BcNoRespAvailable, k_BcNotSupported, k_BcTimeOut
 */
vint16_t RC_Pending( vvoid_t )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return k_BcSuccess;
}

/**
 *  @brief  Requests a connection using the return channel
 *  @param[in]  pabBuffer
 *                data that can be used for connection set up, this information is ciontext sensitive
 *                and must be define mutually between RCS-implementer and CAS
 *  @param[in]  wLen
 *                number of bytes in the buffer
 *  @param[in]  wKeepAliveTO
 *                information on the timeout value for sending keepalive messages
 *  @param[in]  wDisconnectTO
 *                information on the timeout after which the connection should be closed in case of inactivity
 *  @return  k_BcSuccess, k_BcError
 */
vint16_t RC_Connect( vuint8_t* pabBuf, vuint16_t wLen, vuint16_t wKeepAliveTO, vuint16_t wDisconnectTO )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return k_BcSuccess;
}

/**
 *  @brief  signals that the return channel can be closed
 *  @param  none
 *  @return  k_BcSuccess, k_BcError
 */
vint16_t RC_Disconnect( vvoid_t )
{
    HxLOG_Print(DBG_PRINT, ("\n");
    return k_BcSuccess;
}
#endif
#endif

#ifdef BC_PVR_INCLUDED
/**
 *  @brief  triggers the recording of an event or of data associated with the event
 *  @param[in]  lEventId
 *                id of the event to be stored
 *  @param[in]  wONID
 *                original network id of the service
 *  @param[in]  wSID
 *                service id of the service to be stored
 *  @param[in]  wSIEventId
 *                event id in the EIT to collect additional information
 *  @param[in]  pabStart
 *                UTC-time when the recording should start
 *  @param[in]  pabStop
 *                UTC-time when the recording should end
 *  @param[in]  wDur
 *                number of days the content shall be stored
 *  @param[in]  bLen
 *                length of the following data
 *  @param[in]  pabData
 *                additional information that must be stored with the content to allow the replay
 *  @param[in]  bServiceIdx
 *                service index from which the call is triggered. For an EMM-triggered call the highest index for ECMs plus one is used
 *  @return  none
 */
vvoid_t PVR_Record( vuint32_t lEventId, vint16_t wONID, vuint16_t wSID, vuint16_t wSIEventId, vuint8_t* pabStart, vuint8_t* pabStop, vuint16_t wDur, vuint8_t bLen, vuint8_t* pabData, vuint8_t bServiceIdx )
{
      HxLOG_Print(DBG_PRINT, ("\n");
}

/**
 *  @brief  informs the set top box that the actual content can or must not be stored unscrambled
 *  @param[in]  lEventId
 *                gives the eventId of the actually started event
 *  @param[in]  bServiceIdx
 *                service index from which the call is triggered. For an EMM-triggered call the highest index for ECMs plus one is used
 *  @return  none
 */
vvoid_t PVR_Mode( vuint32_t lEventId, vuint8_t bServiceIdx );
{
      HxLOG_Print(DBG_PRINT, ("\n");
}

/**
 *  @brief  start the replay of the stored content
 *  @param[in]  lEventId
 *                id of the event that is to be replayed
 *  @return  none
 */
vvoid_t PVR_Start( vuint32_t lEventId )
{
    HxLOG_Print(DBG_PRINT, ("\n");
}

/**
 *  @brief  signals the removal of a stored event
 *  @param[in]  lEventId
 *                id of the event that is to be deleted
 *  @return  none
 */
vvoid_t PVR_Remove( vuint32_t lEventId )
{
    HxLOG_Print(DBG_PRINT, ("\n");
}

/**
 *  @brief  read back the actual date in MJD-form
 *  @param  none
 *  @return
 *    date in Modified Julian Date
 */
vuint32_t PVR_Date( vvoid_t )
{
    HxLOG_Print(DBG_PRINT, ("\n");

    return k_BcSuccess;
}
#endif

#ifdef BC_MS_INCLUDED
/**
 *  @brief  hands over the information which must be sent to Slave smartcards
 *  @param[in]  wLen
 *                number of bytes the data handed over
 *  @param[in]  pabData
 *                data that must be sent to the slave smartcard
 *  @return  k_BcSuccess, k_BcError, k_BcNotSupported
 */
vint16_t MS_SendMasterData( vuint16_t wLen, vuint8_t* pabData )
{
    HxLOG_Print(DBG_PRINT, ("\n");

    return k_BcSuccess;
}

/**
 *  @brief  start the replay of the stored content
 *  @param[in]  pwLen
 *                pointer to an HUINT16, where to put in the length of the received data (=number of bytes)
 *  @param[in]  pabData
 *                data that must be sent to the slave smartcard
 *  @return  k_BcSuccess, k_BcError, k_BcNotSupported
 */
vint16_t MS_GetMasterData( vuint16_t* wLen, vuint8_t* pabData )
{
    HxLOG_Print(DBG_PRINT, ("\n");

    return k_BcSuccess;
}
#endif

#ifdef BC_DVR_INCLUDED
vvoid_t DVR_OK( vuint8_t bChannelId, vuint8_t bMode )
vvoid_t DVR_UsedKey( vuint8_t bChannelId, vuint8_t bMode )
#endif

#ifdef BC_CIPLUS
vvoid_t SYS_CIP_OutputControl( vuint8_t bServiceIdx, vuint8_t *pabCIPDesc, vuint8_t bDescLen )
#else
#ifdef BC_COPYCONTROL
/**
 *  @brief  informs about the copy control information
 *  @param[in]  bServiceIdx
 *                defines the service for which the given parameters are to be applied
 *  @param[in]  wMv
 *                this value gives the macro vision control byte to be used
 *                If wMv is greater than 255 no MacroVision must be applied
 *  @param[in]  bCgmsa
 *                this value defines the cgms-a value to apply for the content
 *                bCgmsa can take the values 0 meaning no copy restriction, 1 meaning no copy any more,
 *                2 meaning one generation of copies can be made and 3 meaning no copies are allowed at all
 *  @param[in]  bHdcp
 *                this value defines whether HDCP must be activated
 *                bHdcp has the value 0 in case no HDCP-encryption on HDMI is needed
 *                For every other value HDCP-encryption is mandatory
 *  @param[in]  bHd2Sd
 *                this value defines whether HD must be scaled to SD on non-secured output
 *                bHd2Sd has the value 0 in case HD content can be provided on analogue output as is,
 *                all other values restrict the non-secured output to SD content and therefore a rescaling to SD must be done
 *  @param[in]  bEmiCci
 *                this value defines the EMI-CCI copy protection value to be used
 *                bEmiCci can take the values 0 meaning no copy restriction, 1 meaning no copy any more,
 *                2 meaning one generation of copies can be made and 3 meaning no copies are allowed at all
 *  @return  none
 *  @remarks
 *    This function specifies which copy control actions must be done for the content which is processed for the given bServiceIdx.
 */
vvoid_t SYS_CopyControl( vuint8_t bServiceIdx, vuint16_t wMv, vuint8_t bCgmsa, vuint8_t bHdcp, vuint8_t bHd2Sd, vuint8_t bEmiCci )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter] bServiceIdx(%d), wMv(0x%x), bCgmsa(0x%x), bHdcp(0x%x), bHd2Sd(0x%x), bEmiCci(0x%x)\n", __FUNCTION__, bServiceIdx, wMv, bCgmsa, bHdcp, bHd2Sd, bEmiCci);

	if(vmx_vr_bc_copycontrol(bServiceIdx, wMv,  bCgmsa, bHdcp, bHd2Sd, bEmiCci) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
	}

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);	
}
#endif
#endif

/**
 *  @brief  This function maps the corresponding C library function
 */
vvoid_t *LIBC_memcpy( vvoid_t *s1, const vvoid_t *s2, vuint32_t n )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s \n", __FUNCTION__);
	return VMX_memcpy(s1, s2, n);
}

/**
 *  @brief  This function maps the corresponding C library function
 */
vint16_t LIBC_memcmp( const vvoid_t *s1, const vvoid_t *s2, vuint32_t n )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s \n", __FUNCTION__);
    return VMX_memcmp(s1, s2, n);
}

/**
 *  @brief  This function maps the corresponding C library function
 */
vvoid_t *LIBC_memmove( vvoid_t *s1, const vvoid_t *s2, vuint32_t n )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s \n", __FUNCTION__);
	return VMX_memmove(s1, s2, n);
}

/**
 *  @brief  This function maps the corresponding C library function
 */
vvoid_t *LIBC_memset( vvoid_t *s, vint32_t i, vuint32_t n )
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s \n", __FUNCTION__);
	return VMX_memset(s, i, n);
}

/**
 *  @brief  This function maps the corresponding C library function
 */
vint8_t *LIBC_strcpy( vint8_t *s1, const vint8_t *s2)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s \n", __FUNCTION__);
    return VMX_strcpy(s1, s2);
}

/**
 *  @brief  This function maps the corresponding C library function
 */
vvoid_t *LIBC_malloc( vint32_t size)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s  size = %lu\n", __FUNCTION__, size);
	
	return VMX_MEM_Alloc(size);
}

/**
 *  @brief  This function maps the corresponding C library function
 */
vvoid_t LIBC_free( vvoid_t * ptr)
{
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);
	if (ptr == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("INPUT	: %s [Exit]\n", __FUNCTION__);
		return;
	}
	VMX_MEM_Free(ptr);

	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Exit]\n", __FUNCTION__);	
}

/**
 *  @brief  The IO_Printf() function places output on the standard output stream stdout, typically the RS232 connector of the STB
 */
vint16_t IO_Printf( const char *format, /* args*/ ... )
{
    HINT16 n;
    va_list ap;
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);
    va_start(ap, format);
    n = (HINT16)vprintf(format, ap);
    va_end(ap);
#if 0
	if (pxString == NULL)
		return;

	DI_UART_PrintRelease(pxString);
#endif
    return n;
}

/**
 *  @brief  The IO_GetChar() function get a byte from an input buffer, typically the RS232 connector of the STB
 */
vint16_t IO_GetChar( void )
{
	#if 0
	HUINT8 ucChar;

	DI_UART_ReadByte(&ucChar);
	return (TChar)ucChar;
	#else
	HxLOG_Print(DBG_PRINT, ("INPUT	: %s [Enter]\n", __FUNCTION__);
	return (HINT16)getchar();
	#endif
}
