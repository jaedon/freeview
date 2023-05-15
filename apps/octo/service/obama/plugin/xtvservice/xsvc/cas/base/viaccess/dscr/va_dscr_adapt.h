/*
 * (c) 2011-2012 Humax Co., Ltd. 
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
 * $Workfile:   va_dscr_adapt.h  $
 * Project    :
 * Author     :
 * Description:
 *
 *                              Copyright (c) 2002 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef	_VA_DSCR_ADAPT_H_
#define	_VA_DSCR_ADAPT_H_

/********************************************************************
 Header Files
 ********************************************************************/
//#include "vporting.h"	// NULL, word, byte, ...
#include "va_types.h"

/********************************************************************
 Constant/Macro Definition
 ********************************************************************/
//efine 	PID_NULL					0x2000
//efine		DSC_NR_RESOURCES			8	// st5518은 8개 지원 -> linkpti.h : PTI_NUMBER_OF_KEYS

// temporary, to be removed
//efine 	ERR_OK 		0

/********************************************************************
 Structure and Type Definition
 ********************************************************************/


/********************************************************************
 Global Function Declaration
 ********************************************************************/
//tern unsigned long	pairHandle[DSC_NR_RESOURCES];
//tern UINT16				pairPidValue[DSC_NR_RESOURCES];

HERROR VA_DSCR_AdaptInit(void);
HERROR VA_DSCR_AdaptOpen(Handle_t hVaAction, HUINT32 *pulDscrHandle, HUINT16 usPid);
HERROR VA_DSCR_AdaptSetPid(HUINT32 ulDscrHandle, HUINT16 usPid);
HERROR VA_DSCR_AdaptResetPid(HUINT32 ulDscrHandle, HUINT16 usPid, HBOOL bClearKey);
HERROR VA_DSCR_AdaptClose(HUINT32 ulDscrHandle, HUINT16 usPid);
HERROR VA_DSCR_AdaptSetKey(HUINT32 ulDscrHandle, HUINT8 ucType, HUINT8 *pucCW);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
HBOOL VA_DSCR_Adapt_IsPlayback(Handle_t hVaAction);
#endif
#endif

