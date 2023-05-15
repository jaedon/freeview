/*****************************************************************************
 ******************************************************************************
 *
 *         File : bc_main.h
 *
 * Description : MAIN module header file for CAMLIB.000.2-IGL2.15S-E00
 *
 *    Copyright : BETARESEARCH 2003 (c)
 *    Copyright : COMVENIENT  2008 (c)
 *    Copyright : Verimatrix  2011 (c)
 *
 ******************************************************************************
 *****************************************************************************/

/**************************** CVS Infos ****************************************
 *
 *  $Source: /home/boxop/cvsroot/bc2_cam_src/bc_main.h,v $
 *  $Date: 2014/03/14 11:08:15 $
 *  $Revision: 1.15 $
 *
 ***************************** CVS Infos ***************************************/

#define _VERSION_2_00_
#ifndef _BCMAIN_H_DEF_
#define _BCMAIN_H_DEF_

#ifndef _BC_TYPES_H_
/*****************************************************************************
******************************************************************************
*
*         File : bc_types.h
*
*  Description : type definitions used by the betacrypt library
*
*    Copyright : COMVENIENT  2008 (c)
*    Copyright : Verimatrix  2011 (c)
*
******************************************************************************
*****************************************************************************/

/**************************** CVS Infos ****************************************
*
*  $Source: /home/boxop/cvsroot/bc2_cam_src/bc_types.h,v $
*  $Date: 2012/12/17 12:27:13 $
*  $Revision: 1.2.1.1 $
*
***************************** CVS Infos ***************************************/
#define _BC_TYPES_H_

// Types used by the betacrypt library
typedef  signed char    int8_t;     /*  range :  -128 to 127               */
typedef  unsigned char  uint8_t;    /*  range :  0 to 255                  */
typedef  signed short   int16_t;    /*  range :  -32768 to 32767           */
typedef  unsigned short uint16_t;   /*  range :  0 to 65535                */
//typedef  signed long    int32_t;    /*  range :  -2147483648 to 2147483647 */
typedef  unsigned long  uint32_t;   /*  range :  0 to 4294967295           */
typedef  signed long long    int64_t;    /*  range :  -2^63 to 2^63-1      */
typedef  unsigned long  long uint64_t;   /*  range :  0 to 2^64-1          */
typedef  unsigned short bool_t;     /*  range :  0 to 1 (false, true)      */
typedef  float          float32_t;  /*  +-1.175494E-38 to +-3.402823E+38   */
typedef  void           void_t;     /*  range :  n.a.                      */

#ifndef true
#define true  (1 == 1)
#endif
#ifndef false
#define false (1 == 0)
#endif

#endif
 //_BC_TYPES_H_


#ifndef _BC_CONSTS_H_DEF_
/*****************************************************************************
 ******************************************************************************
 *
 *         File : bc_consts.h
 *
 *  Description : File to be included by the host software with all
 *                definitions of constants
 *
 *    Copyright : BETARESEARCH 2003 (c)
 *    Copyright : COMVENIENT  2008 (c)
 *    Copyright : Verimatrix 2011 (c)
 *
 ******************************************************************************
 *****************************************************************************/

/**************************** CVS Infos ****************************************
 *
 *  $Source: /home/boxop/cvsroot/bc2_cam_src/bc_consts.h,v $
 *  $Date: 2012/12/17 12:25:52 $
 *  $Revision: 1.4.1.1 $
 *
 ***************************** CVS Infos ***************************************/

#define _BC_CONSTS_H_DEF_

typedef enum
{
	k_PageSearch,
	k_PageLocked,
	k_DisableFilter
}  enFilterMode_t;

typedef enum
{
	k_SC_Ok = 0,
	k_SC_NotPresent,
	k_SC_HwError,
	k_SC_Rejected,
	k_SC_UpdateRequired,
	k_SC_NSC
} enScState_t;

typedef enum
{
	k_DS_Ok = 0,
	k_DS_Error,
	k_DS_NoECMs,
	k_DS_ClearOrForeignEs,
	k_DS_Preview,
	k_DS_Pairing,
	k_DS_MS,
	k_DS_NoCWs,
	k_Init   // only for internal use
} enDescState_t;

typedef enum
{
	k_ConnectBc = 0,
	k_DisconnectBc
} enBcCmd_t;

typedef enum
{
	k_BcPinVerified            = 0x00,
	k_BcPinChanged             = 0x01,
	k_BcPinFailure             = 0x21,
	k_BcPinBlocked             = 0x22,
	k_BcPinMayNotBeChanged     = 0x23,
	k_BcPinMayNotBeBypassed    = 0x24,
	k_BcPinBadIndex            = 0x25,
	k_BcPinBadLength           = 0x26,
	k_BcPinNotEnoughPurse      = 0x30,
	k_BcPinGeneralError        = 0xFF
} enBcNotify_t;

typedef enum
{
	k_ConnectSc = 0,
	k_DisconnectSc,
	k_ResetSc,
	k_GetATRSc,
	k_CardDetectSc
} enCmd_t;

typedef enum
{
	k_Success    = 0,
	k_Error      = 1,
	k_UnknownCmd = 2
} enReturn_t;

typedef enum
{
	BC_SC_NONE = 0,					/* Reserved for future use. */
	BC_SC_RW_COMPLETED,			/* The parameter BC_SC_RW_COMPLETED shall be signalled by means of BC_SC_Notify() as soon as the response message from SC is available for the betacrypt ¢ç library to be read. */
	BC_SC_SETPARAM_COMPLETED,		/* Reserved for future use */
	BC_SC_POWERDOWN_COMPLETED, 	/* This parameter shall be set if the SC interface is not powered or no longer available. This shouldn¡¯t happen. */
	BC_SC_INSERTED,					/* This parameter shall be set if a SC has been inserted */
	BC_SC_REMOVED,					/* This parameter shall be set if the SC has been removed */
	BC_SC_ERROR,						/* This parameter shall be set if a SC error occurred. Possible errors could be if the SC is plugged in the wrong way, if the SC doesn¡¯t send an ATR, etc. */
	BC_SC_RESET,						/* This parameter shall be set after a reset of the SC when the ATR is available. */
	BC_SC_MUTE						/* Reserved for future use. */
} enBcScNotify_t;

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef EOF
#define EOF (-1)
#endif


#define CARD_SLOTS                          1   // number of card slots

// block ids for non volatile memory
#define NVM_BLOCK_0                         0
#define NVM_BLOCK_1                         1
#define NVM_BLOCK_2                         2
#define NVM_BLOCK_3                         3
#define NVM_BLOCK_4                         4
#define NVM_BLOCK_5                         5
#define NVM_BLOCK_6                         6
#define NVM_BLOCK_7                         7
#define NVM_BLOCK_8                         8
#define NVM_BLOCK_9                         9

#define k_BcNSc                             1
#define k_BcSuccess                         0
#define k_BcError                          -1
#define k_BcGeneralError                   -2
#define k_BcEmmQueueOverflowError          -3
#define k_BcEcmQueueOverflowError          -4
#define k_BcFilterInUseError               -5
#define k_BcNoRespAvailable                -6
#define k_BcScBusy                         -7
#define k_BcNothingDone                    -8
#define k_BcNotSupported                   -9
#define k_BcTimedOut                       -10

#define k_BcMacroVisionControlNotAvailable  0xFF
#define k_CaIdNotAvailable                  0xFFFF

#define k_BcNoPairing                         0
#define k_BcPairingInProgress                 1
#define k_BcPairingOk                         2
#define k_BcPairingBad                        3

#define MAX_EMM_INDEX                       32
#define MAX_ADDR_LEN                        4


#define BC_PIN_PC                           0x00
#define BC_PIN_IPPV_PC                      0x01
#define BC_PIN_STB_LOCK                     0x02
#define BC_PIN_SHOPPING                     0x03

#ifdef BC_DVR_INCLUDED
#define k_BcDVRBadInfo -2
#define k_BcDVRServiceNotActive -3
#define k_BcDVRChannelAlreadyInUse -4
#define k_BcDVRChannelNotInUse -5
#define k_BcDVRNoTime -6
#define k_BcDVRBadStoreData -7
#define k_BcDVRDataError  -8
#define k_BcDVRNoRecording  -9
#define k_BcDVRNoReplay  -10
#endif

#endif
// _BC_CONSTS_H_DEF_

// ***************************************************************************
//      betacrypt library externals
//
// input/output to be used by the host system
//
// ECM PID (max 8) / Stream PID (max 8)
// BC - Betacrypt
// SC - Smartcard
// FS - Filtersection / descrambler
// MMI - Man Machine Interface
// PC - Parental Control
// OSD - On Screen Display
// ISC - Interactive Smart Card
// RC - Return Channel
// LIBC - Interface to functions from libc
// ***************************************************************************

#define BC_NSC_INCLUDED                     // Non SmartCard
//#define BC_CHAT_INCLUDED
//#define DOWNLOAD_CHECK
//#define MULTI_CA_SUPPORT
//#define BC_RC_INCLUDED
//#define BC_IP_INCLUDED
//#define BC_MS_INCLUDED
//#define BC_PVR_INCLUDED                     // PVR
//#define BC_DVR_INCLUDED
//#define BC_CIPLUS
//#define BC_COPYCONTROL

// ***************************************************************************
// ***************** Output functions of the host system *********************

extern int16_t BC_Init(void_t);
extern int16_t BC_CheckNSc( void_t );

#ifdef BC_NSC_INCLUDED
extern int16_t  BC_InitNSc(uint8_t* pabPtr, uint16_t* pwLen );
extern int16_t BC_InitNScComm ( uint8_t* pabPtr, uint16_t* pwLen,
		uint8_t* pabBoxSerNo,
		uint8_t* pabManuId,
		uint8_t* pabManuData,
		uint8_t* pabProvId,
		uint8_t* pabProvData );
#endif
extern void_t   BC_Task(void_t);
extern void_t   BC_GetVersion(uint8_t* pacVersion, uint8_t* pacDate, uint8_t* pacTime);
extern uint16_t BC_Get_CASystemID(void_t);
extern int16_t  BC_SetEMM_Pid(uint16_t wEmmPid);
extern uint8_t  BC_GetMacroVisionCtrl(uint16_t wServiceId, uint8_t bServiceIdx);
extern int16_t  BC_CheckPin(uint8_t bPinLength, uint8_t* pabPin, uint8_t bPinIndex, uint8_t bReason, uint8_t bServiceIdx);
extern int16_t  BC_ChangePin(uint8_t bOldPinLength, uint8_t* pabOldPin,
		uint8_t bNewPinLength, uint8_t* pabNewPin,
		uint8_t bPinIndex);
extern int16_t  BC_Ioctl(enBcCmd_t cmd, void_t* pvParams, uint16_t* pwLen);
extern int16_t  BC_StartDescrambling(uint16_t wServiceId, uint16_t wIndex,
		uint16_t * pawEcmPid, uint16_t *pawStreamPid,
		uint8_t bServiceIdx);
extern int16_t  BC_StopDescrambling(uint16_t wServiceId, uint8_t bServiceIdx);
extern int16_t  BC_SetSTBFilterAddress( uint8_t bAddressLen, uint8_t* pabAddress );
extern void_t   BC_InitWindow( uint16_t wMaxWidth, uint16_t wMaxHeight,
		uint8_t* pabUnblockString );
extern int16_t  BC_GetSCNo( uint8_t* pabSerNo, uint16_t wLength );
extern void_t BC_GetPurse( void_t* pvParams );
int16_t BC_CheckPairingState( void_t );


// ***************************************************************************
// ***************** INPUT function to the host software *********************

//// ECM
extern int16_t  FS_SetECMFilter(uint8_t bFilterId, enFilterMode_t enFilterMode, uint16_t wEcmPid,
		uint8_t bTableId, uint8_t bVersion, uint8_t bPage);
extern int16_t  FS_ReadECM(uint8_t bFilterId, uint8_t* pabBuffer, uint16_t* pwLen);
extern int16_t  FS_FlushECM_Buffer(uint8_t bFilterId);

//// EMM
extern int16_t  FS_SetEMMFilter(uint8_t bFilterIndex, uint8_t bAddressLength,
		uint8_t* pabAddress);
extern int16_t  FS_SetEMM_Pid(uint16_t wEmmPid);
extern int16_t  FS_ReadEMM(uint8_t* pabBuffer, uint16_t* pwLen);
extern int16_t  FS_FlushEMM_Buffer(void_t);

//// DESCR
extern int16_t  FS_StartDescrambling(uint16_t wIndex, uint16_t *pawStreamPid, uint8_t bServiceIdx);
extern int16_t  FS_StopDescrambling(uint8_t bServiceIdx);

// --- MMI---
extern int16_t  MMI_SetDescrambling_State(uint16_t wIndex,
		uint16_t* pawStreamPid,
		enDescState_t * paenDescState,
		uint8_t bServiceIdx);
extern int16_t  MMI_SetSmartcard_State(enScState_t enScState);

// --- System calls ---
extern signed long  SYS_GetTickCount(void_t);
extern int16_t  SYS_ReadNvmBlock(uint8_t* pabDest, uint16_t wLength);
extern int16_t  SYS_WriteNvmBlock(uint8_t* pabSrc, uint16_t wLength);
#ifdef BC_NSC_INCLUDED
extern int16_t  SYS_ReadNvmData(uint8_t bBlockId, uint8_t* pabDest, uint16_t wLength);
extern int16_t  SYS_WriteNvmData(uint8_t bBlockId, uint8_t* pabSrc, uint16_t wLength);
#endif
extern signed long  SYS_Random(void_t);
extern int16_t  SYS_SetDialogue(uint16_t wDialogLength, uint8_t* pabDialogue);

extern void_t SYS_GetBoxId( uint8_t* pabBoxId );
extern int16_t  SC_Write(uint8_t* pabBuffer, uint16_t* pwLen, uint16_t wTimeout);
extern int16_t  SC_Read(uint8_t* pabBuffer, uint16_t* pwLen);
extern int16_t  SC_IOCTL(enCmd_t cmd, void_t* pvParams, uint16_t* pwLen);
extern void_t   OSD_BuildWindow( uint8_t* pabMsg, int16_t wMode,
		int16_t wX, int16_t wY, int16_t wW, int16_t wH,
		int8_t bBackground, int8_t bAlpha, int8_t bForeground );
extern uint16_t  OSD_DisplayWindow( uint8_t bDisplayMode, uint16_t wDuration );
extern void_t   ISC_CheckPin( uint8_t bPinIndex, uint8_t bTextSelector, uint8_t bServiceIdx );
extern void_t   ISC_OrderPin( uint8_t bPinIndex, uint32_t lPurse,
		uint32_t lCost, uint16_t wMult, uint16_t wDiv,
		uint8_t bLocation, uint8_t bSign0,
		uint8_t bSign1000, uint8_t bCount, uint8_t bLen,
		uint8_t *abText, uint32_t lEventId, uint8_t bServiceIdx );



void_t FDI_SYSTEM_VMX_SendBoxId( uint8_t* pabBoxId );
unsigned char FDI_SYSTEM_VMX_WriteNvmData(uint8_t bBlockId, uint8_t* pabSrc, uint16_t wLength);
unsigned char FDI_SYSTEM_VMX_ReadNvmData(uint8_t bBlockId, uint8_t* pabDest, uint16_t wLength);

#endif
 // _BCMAIN_H_DEF_
