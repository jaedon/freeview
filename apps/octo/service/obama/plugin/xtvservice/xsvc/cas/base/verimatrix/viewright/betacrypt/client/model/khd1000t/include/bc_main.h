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
#include "bc_types.h"
#endif
#ifndef _BC_CONSTS_H_DEF_
#include "bc_consts.h"
#endif

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
#define BC_COPYCONTROL

// ***************************************************************************
// ***************** Output functions of the host system *********************

extern vint16_t BC_Init(vvoid_t);
extern vint16_t BC_CheckNSc( vvoid_t );

#ifdef BC_NSC_INCLUDED
extern vint16_t  BC_InitNSc(vuint8_t* pabPtr, vuint16_t* pwLen );
extern vint16_t BC_InitNScComm ( vuint8_t* pabPtr, vuint16_t* pwLen,
		vuint8_t* pabBoxSerNo,
		vuint8_t* pabManuId,
		vuint8_t* pabManuData,
		vuint8_t* pabProvId,
		vuint8_t* pabProvData );
#endif
extern vvoid_t   BC_Task(vvoid_t);
extern vvoid_t   BC_GetVersion(vuint8_t* pacVersion, vuint8_t* pacDate, vuint8_t* pacTime);
extern vuint16_t BC_Get_CASystemID(vvoid_t);
extern vint16_t  BC_SetEMM_Pid(vuint16_t wEmmPid);
extern vuint8_t  BC_GetMacroVisionCtrl(vuint16_t wServiceId, vuint8_t bServiceIdx);
extern vint16_t  BC_CheckPin(vuint8_t bPinLength, vuint8_t* pabPin, vuint8_t bPinIndex, vuint8_t bReason, vuint8_t bServiceIdx);
extern vint16_t  BC_ChangePin(vuint8_t bOldPinLength, vuint8_t* pabOldPin,
		vuint8_t bNewPinLength, vuint8_t* pabNewPin,
		vuint8_t bPinIndex);
extern vint16_t  BC_Ioctl(enBcCmd_t cmd, vvoid_t* pvParams, vuint16_t* pwLen);
extern vint16_t  BC_StartDescrambling(vuint16_t wServiceId, vuint16_t wIndex,
		vuint16_t * pawEcmPid, vuint16_t *pawStreamPid,
		vuint8_t bServiceIdx);
extern vint16_t  BC_StopDescrambling(vuint16_t wServiceId, vuint8_t bServiceIdx);
extern vint16_t  BC_SetSTBFilterAddress( vuint8_t bAddressLen, vuint8_t* pabAddress );

// initialize the OSM window system. wMaxWidth and wMaxHeight define 
// the real pixel size of the screen that can be used for displaying 
// text messages. The values for wX and wY will always be handed over 
// within the boundaries
//     0 <= wX < wMaxWidth
//     0 <= wY < wMaxHeight
// bUnblockString is appended to the message, if the window can be 
// removed by the customer. E.g. if the customer must press the 
// OK-button, the string could look like "OK?". This text is 
// appended on a separate line.
extern vvoid_t   BC_InitWindow( vuint16_t wMaxWidth, vuint16_t wMaxHeight,
		vuint8_t* pabUnblockString );
extern vint16_t  BC_GetSCNo( vuint8_t* pabSerNo, vuint16_t wLength );

// BC_GetPurse Param is a function pointer of type:
// void function( vuint8_t bNumber, vuint32_t* lPurse,
//   vuint16_t wMult, vuint16_t wDiv,
//   vuint8_t bLocation, vuint8_t bSign0,
//   vuint8_t bSign1000, vuint8_t bCount, vuint8_t bLen,
//   vuint8_t *abText);
// bNumber gives the number of entries in lPurse-Array
// lPurse is the array of purse-values
// all other parameters see ISC_OrderPin
extern vvoid_t BC_GetPurse( vvoid_t* pvParams );
#ifdef BC_CHAT_INCLUDED
extern vuint8_t* BC_ChatName( vuint8_t bChatId );
extern vvoid_t BC_StartChat( vuint8_t bChatId );
extern vvoid_t BC_ClearChat( vvoid_t );
#endif

#ifdef DOWNLOAD_CHECK
extern vint16_t  BC_CheckCertificate( vuint8_t* pabImage, vuint32_t wLength, vuint8_t* pabCert, vuint32_t wCertLength );
#endif

#ifdef MULTI_CA_SUPPORT
extern vvoid_t BC_EnableCA( vvoid_t );
extern vvoid_t BC_DisableCA( vvoid_t );
#endif

#ifdef BC_RC_INCLUDED
extern vint16_t BC_EncryptMSG( vuint8_t* pabMsg, vuint16_t wPos, vuint16_t wLength, vuint8_t bKeyToUse );
extern vint16_t BC_DecryptMSG( vuint8_t* pabMsg, vuint16_t wPos, vuint16_t wLength, vuint8_t bKeyToUse );
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

// ***************************************************************************
// ***************** INPUT function to the host software *********************

//// ECM
extern vint16_t  FS_SetECMFilter(vuint8_t bFilterId, enFilterMode_t enFilterMode, vuint16_t wEcmPid,
		vuint8_t bTableId, vuint8_t bVersion, vuint8_t bPage);
extern vint16_t  FS_ReadECM(vuint8_t bFilterId, vuint8_t* pabBuffer, vuint16_t* pwLen);
extern vint16_t  FS_FlushECM_Buffer(vuint8_t bFilterId);

//// EMM
extern vint16_t  FS_SetEMMFilter(vuint8_t bFilterIndex, vuint8_t bAddressLength,
		vuint8_t* pabAddress);
extern vint16_t  FS_SetEMM_Pid(vuint16_t wEmmPid);
extern vint16_t  FS_ReadEMM(vuint8_t* pabBuffer, vuint16_t* pwLen);
extern vint16_t  FS_FlushEMM_Buffer(vvoid_t);

//// DESCR
extern vint16_t  FS_StartDescrambling(vuint16_t wIndex, vuint16_t *pawStreamPid, vuint8_t bServiceIdx);
extern vint16_t  FS_StopDescrambling(vuint8_t bServiceIdx);

// --- MMI---
extern vint16_t  MMI_SetDescrambling_State(vuint16_t wIndex,
		vuint16_t* pawStreamPid,
		enDescState_t * paenDescState,
		vuint8_t bServiceIdx);
extern vint16_t  MMI_SetSmartcard_State(enScState_t);

// --- System calls ---
extern vint32_t  SYS_GetTickCount(vvoid_t);
extern vint16_t  SYS_ReadNvmBlock(vuint8_t* pabDest, vuint16_t wLength);
extern vint16_t  SYS_WriteNvmBlock(vuint8_t* pabSrc, vuint16_t wLength);
#ifdef BC_NSC_INCLUDED
extern vint16_t  SYS_ReadNvmData(vuint8_t bBlockId, vuint8_t* pabDest, vuint16_t wLength);
extern vint16_t  SYS_WriteNvmData(vuint8_t bBlockId, vuint8_t* pabSrc, vuint16_t wLength);
#endif
extern vint32_t  SYS_Random(vvoid_t);
extern vint16_t  SYS_SetDialogue(vuint16_t wDialogLength, vuint8_t* pabDialogue);

extern vvoid_t SYS_GetBoxId( vuint8_t* pabBoxId );
// --- SC ---
extern vint16_t  SC_Write(vuint8_t* pabBuffer, vuint16_t* pwLen, vuint16_t wTimeout);
extern vint16_t  SC_Read(vuint8_t* pabBuffer, vuint16_t* pwLen);
extern vint16_t  SC_IOCTL(enCmd_t cmd, vvoid_t* pvParams, vuint16_t* pwLen);

// --- OSD ---
// OSD_BuildWindow prepares an OSD of a text message given in bMsg.
// wMode is giving the display-modus of the window. The window is 
// not displayed due to a call of this function.
//    wMode&3 gives the y-position of the window reference point
//    (wMode/4)&3 gives the x-position of the window reference point
//    (wMode/16)&3 gives the text-alignment inside the box
//    x- and y-position are defined in the following way:
//       (pos&1) == 1 means centered point
//       (pos&2) == 2 means take right or upper corner as reference
//       
// combined values for x- and y-position in decimal
//     2                6/14                10
//      ------------------------------------
//     |                                    |
//     |                                    |
//     |                                    |
//    1/3            5/13/7/15             9/11 
//     |                                    |
//     |                                    |
//     |                                    |
//      ------------------------------------
//     0                4/12                8
//
// The upper left corner is (0,0)
//
// text-alignment is defined to be
//    0  left aligned
//    1  right aligned
//    2  centered
//    3  justified
// wX and wY give the coordinates for the corner specified by the wMode
// if (wMode&0x100)=0x100 take width and height given in wW and wH, else
// ignore wW and wH and calculate width and height of the box according 
// to the message.
// wBackground selects one of 16 colors for the background
// bAlpha give the alpha-blending value for the textbox
// wForeground selects one of 16 colors for the text
extern vvoid_t   OSD_BuildWindow( vuint8_t* pabMsg, vint16_t wMode,
		vint16_t wX, vint16_t wY, vint16_t wW, vint16_t wH,
		vint8_t bBackground, vint8_t bAlpha, vint8_t bForeground );
//
// OSD_DisplayWindow displays the prepared window for
// wDuration seconds using the bDisplayMode to select the
// removal of the window by the customer
// A duration of 0 means the window is not automatically removed.
// It will be removed/replaced by the next message or depending 
// on the displaymode.
// If (bDisplayMode&1)==0 the window can be removed by the customer 
// pressing a selected button. Further definitions can be taken form
// the detailed description of the OSM for set-top-box document
extern vuint16_t  OSD_DisplayWindow( vuint8_t bDisplayMode, vuint16_t wDuration );

// --- interactive SC ---
//
// ask for a PIN and send the PIN check using the provided bPinIndex
// to the library using the BC_CheckPin call.
// The textselector is used to select the text displayed for customer
extern vvoid_t   ISC_CheckPin( vuint8_t bPinIndex, vuint8_t bTextSelector, vuint8_t bServiceIdx );
// Textselectors are:
//     0    ... check PIN for parental control
//     1    ... check PIN for Impulse Pay Per View buy
//     3    ... check PIN for parental control non-smartcard pin
//     4    ... check PIN for resuming event
//     5    ... check PIN for selecting event

// ask for a PIN and sent the PIN check using the provided bPinIndex
// to the library using the BC_CheckPin call.
// Use 1 as the textselector.
// Information on the currency to be used and the calculation of the currency
// values out of the given token values is described
//
//    available_credit = lPurse * wMult / wDiv;
//    cost_of_event = lCost * wMult / wDiv;
// bLocation gives to location of the currency description. If bLocation == 0
// the currency is placed at the end of the number, else at the beginning
// bSign0 gives the sign to be used to separate full currency units from parts
// bSign1000 gives the sign to be used to separate 1000 numbers
// bCount gives the number of part digits
// bLen gives the length of the abText string, which gives the currency text
// lEventId gives the event id of the provided event. If the event-id changes,
//          a new input mask should be presented, as the costs may have changed
//
// examples:
//   lPurse = 6000, lCost = 3, wMult = 1, wDiv = 2,
//   bLocation = 1, bSign0 = '.', bSign1000 = ',',
//   bCount = 2, bLen = 1, abText = "$":
//         Credit: $3,000.00  Cost: $1.50
//   lPurse = 6000, lCost = 3, wMult = 2, wDiv = 3,
//   bLocation = 0, bSign0 = ',', bSign1000 = '.',
//   bCount = 2, bLen = 4, abText = " EUR":
//         Credit: 4.000,00 EUR  Cost: 2,00 EUR
extern vvoid_t   ISC_OrderPin( vuint8_t bPinIndex, vuint32_t lPurse,
		vuint32_t lCost, vuint16_t wMult, vuint16_t wDiv,
		vuint8_t bLocation, vuint8_t bSign0,
		vuint8_t bSign1000, vuint8_t bCount, vuint8_t bLen,
		vuint8_t *abText, vuint32_t lEventId, vuint8_t bServiceIdx );
#ifdef BC_CHAT_INCLUDED
extern vvoid_t CHT_Display( vuint8_t* pabText );
#endif

#ifdef BC_IP_INCLUDED
vvoid_t IP_OpenConnectionByName( vuint8_t* pabName, vuint16_t wPort );
vvoid_t IP_OpenConnectionByAddress( vuint8_t* lAddress, vuint16_t wPort );
vvoid_t IP_CloseConnection( vuint32_t wIpId );
vint16_t IP_Send( vuint32_t wIpId, vint16_t wLen, vuint8_t* pabBuffer );
vint16_t IP_Receive( vuint32_t wId, vint16_t wLen, vuint8_t* pabBuffer );
#else
#ifdef BC_RC_INCLUDED
vint16_t RC_ReadMSG( vuint8_t* pabBuf, vuint16_t* pwLen );
vint16_t RC_FlushReadMSG_Buffer( vvoid_t );
vint16_t RC_WriteMSG( vuint8_t* pabBuf, vuint16_t wLen );
vint16_t RC_FlushWriteMSG_Buffer( vvoid_t );
vint16_t RC_Pending( vvoid_t );
vint16_t RC_Connect( vuint8_t* pabBuf, vuint16_t wLen, vuint16_t wKeepAliveTO, vuint16_t wDisconnectTO );
vint16_t RC_Disconnect( vvoid_t );
#endif
#endif
#ifdef BC_PVR_INCLUDED
vvoid_t PVR_Record( vuint32_t lEventId,
		vuint16_t wONID, vuint16_t wSID, vuint16_t wSIEventId,
		vuint8_t* pabStart, vuint8_t* pabStop, vuint16_t wDur,
		vuint8_t bLen, vuint8_t* pabData, vuint8_t bServiceIdx );
vvoid_t PVR_Mode( vuint32_t lEventId, vuint8_t bServiceIdx );
vvoid_t PVR_Start( vuint32_t lEventId );
vvoid_t PVR_Remove( vuint32_t lEventId );
vuint32_t PVR_Date( vvoid_t );
#endif

#ifdef BC_MS_INCLUDED
vint16_t MS_SendMasterData( vuint16_t wLen, vuint8_t* pabData );
vint16_t MS_GetMasterData( vuint16_t* wLen, vuint8_t* pabData );
#endif
#ifdef BC_DVR_INCLUDED
vvoid_t DVR_OK( vuint8_t bChannelId, vuint8_t bMode );
vvoid_t DVR_UsedKey( vuint8_t bChannelId, vuint8_t bMode );
#endif
#ifdef BC_CIPLUS
vvoid_t SYS_CIP_OutputControl( vuint8_t bServiceIdx, vuint8_t *pabCIPDesc, vuint8_t bDescLen );
#else
#ifdef BC_COPYCONTROL
vvoid_t SYS_CopyControl( vuint8_t bServiceIdx, vuint16_t wMv, vuint8_t bCgmsa, vuint8_t bHdcp, vuint8_t bHd2Sd, vuint8_t bEmiCci );
#endif
#endif

#endif
 // _BCMAIN_H_DEF_
