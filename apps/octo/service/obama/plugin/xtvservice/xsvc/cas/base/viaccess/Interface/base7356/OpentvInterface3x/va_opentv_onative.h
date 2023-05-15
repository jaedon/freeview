/*
   Copyright

    This source code is VIACCESS intellecutal property an is strictly confidential.
    The receiver undertakes to use it within the strict and sole framework of the
    licence agreement entered into with VIACCESS pertaining to the use of such
    source code. No part of VIACCESS source code may be reproduced or transmitted 
    in any form or by any means, electronic or mechanical, including photocopying, 
    recording, or by any information storage and retrieval system, without permission 
    in writing from VIACCESS. The information in this code is subject to change 
    without notice. VIACCESS does not warrant that this code is error free. If you 
    find any problems with this code or wish to make comments, please report them 
    to VIACCESS.

    Trademarks 

    Viaccess is a registered trademark of Viaccess ® in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
*/

/*
    date        : 30/06/2006
    file        : va_opentv_onative.h
    Description : It is the Viaccess'API of CAS functionalities
*/

#ifndef _VA_OPENTV_O_NATIVE_H_
#define _VA_OPENTV_O_NATIVE_H_

#include "va_types.h"
#include "va_opentv_Otype.h"

/***************************************************************************/
/************************** constantes definition **************************/
/***************************************************************************/
/* Number of pipe */
#define kO_VA_SETUP_NBMAX_PIPE			kVA_SETUP_NBMAX_ACS

/* Number of smart card reader */
#define kO_VA_SETUP_NBMAX_SC			kVA_SETUP_NBMAX_SC

/* File directory and name for NVM driver */
#define kO_VA_NVM_FILE_DIR              NVM_ROOT_PATH "/va"
#define kO_VA_NVM_FILE_NAME             kO_VA_NVM_FILE_DIR "/va-file"

/* To be used as parameter prodid for o_fs_register_client */
#define kO_VA_NVM_PRODUCER_ID			3 /* MANUFACTURER_ID */

/* To be used as parameter applid for o_fs_register_client */
#define kO_VA_NVM_APPLICATION_ID        1000

/***************************************************************************/
/***************************** types definition ****************************/
/***************************************************************************/
/* Descrambler handle */
typedef DWORD tO_VA_DESC_Descrambler;

/* Serial number */
typedef struct
{
    UINT8  uiSerialNumberSize;
    BYTE   *pSerialNumber;

}   tO_VA_INIT_StbSerialNb;

/* PipeTag mode */
typedef enum
{
	eO_VA_MODE_LIVE ,
	eO_VA_MODE_PERSISTENT_RECORD ,
	eO_VA_MODE_TIMESHIFT_RECORD ,
	eO_VA_MODE_PERSISTENT_PLAYBACK,
	eO_VA_MODE_TIMESHIFT_PLAYBACK,
	eO_VA_MODE_LAST
} tO_VA_INIT_PipeTagMode;

/* PVR attributes range that Viaccess can use: 1 + 1 per ecm stream are necessary */
typedef struct
{
    UINT32	 									uiAttributeIdMin;
	UINT32	 									uiAttributeIdMax;
} tO_VA_INIT_PvrParameters;

/* Callback to be provided to return the pipeTag mode: mandatory */
typedef tO_VA_INIT_PipeTagMode (*tO_VA_PVR_GetPipeTagModeCallback)( DWORD pipeTag);

typedef struct
{
    UINT32								uiNbPipe; 
    UINT32								uiNbSmartCard;
    DWORD								aPipeTag[kO_VA_SETUP_NBMAX_PIPE]; /* Deprecated */
    tO_VA_INIT_StbSerialNb				stStbSerialNumber;
    tO_VA_PVR_GetPipeTagModeCallback 	pfGetPipeTagMode;	
    tO_VA_INIT_PvrParameters			pvrParameters;
}   tO_VA_INIT_ConfigurationParameters;

/***************************************************************************/
/*************************** functions definition **************************/
/***************************************************************************/
/* User API */
extern INT   o_VA_UI_SetDefaultFocus(void);
extern INT   o_VA_UI_ResetFocus(void);

/* ACS Init API */
extern INT   o_VA_INIT_InitializeAcs(const tO_VA_INIT_ConfigurationParameters  *pParameters);
extern INT   o_VA_INIT_StopAcs(void);
extern void  o_VA_INIT_StartAcs(void);

/* Ca Driver API */
extern INT o_VA_INIT_InitializeCaDriver (UINT32 caDriverElemId);

/* Descrambler API */  
extern tO_VA_DESC_Descrambler o_VA_DESC_Open (DWORD dwPipeTag);
extern INT o_VA_DESC_Close (tO_VA_DESC_Descrambler dwDescramblerId);
extern INT o_VA_DESC_SetPid (tO_VA_DESC_Descrambler dwDescramblerId, UINT16 uiPid);
extern INT o_VA_DESC_SetKeys (tO_VA_DESC_Descrambler   dwDescramblerId,
								UINT32                  uiOddKeyLength,
								const BYTE              *pOddKey,
								UINT32                  uiEvenKeyLength,
								const BYTE              *pEvenKey);

/* Standby API */
extern void o_VA_STDBY_EnterStandby(void);
extern void o_VA_STDBY_ExitStandby (void);
extern void o_VA_STDBY_StandbyProcessDone(void); 

#endif /* _VA_OPENTV_O_NATIVE_H_ */
