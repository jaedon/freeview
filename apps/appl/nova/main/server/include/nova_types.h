/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/



#ifndef __NOVA_TYPES_H__
#define __NOVA_TYPES_H__

#include <NovaspreadTuner.h>
#include "sapi.h"

//#define CONSOLE_PRINT	// for debugging
#define USE_TRANSCODING

#ifdef  CONSOLE_PRINT
#define HxLOG_Info		printf
#define HxLOG_Warning	printf
#define HxLOG_Error		printf
#define HxLOG_Print		printf
#define HxLOG_Debug		printf
#define HxLOG_Message	printf
#endif


#define     MASK_PID_ALL        -1
#define     MASK_PID_NONE       0
#define     MASK_PID_SET		1

#define		TO_INVALID_SLOTID	0xffffffff
#define		TO_INVALID_TUNERID	0xffffffff	// prefix 'TO' means TunerObject


typedef enum
{
	TT_Unknown = -1,
	TT_SAT,
	TT_TER,
	TT_CAB,
	TT_MAX
} TUNER_TYPE_e;


typedef struct
{
	TUNER_TYPE_e					eTunerType;

	int								Src;
	unsigned int					Freq;
	unsigned int					SymbolRate;
	DxSat_CodeRate_e				eFec;
	DxSat_Polarization_e			ePol;
	DxSat_TransportSpec_e			eTransSpec;
	DxSat_PskModulation_e			ePskMod;
	DxSat_Pilot_e					ePilot;
	DxSat_RollOff_e					eRollOff;

	NovaspreadTTunerAllocationMode	Mode;
	unsigned short					Priority;

	NovaspreadTDvbId				DvbId;
	bool							UseTranscoder;
	bool							UseCrypto;

	NovaspreadTTranscoding			Transcoding;
} TUNER_PARAM_t;

#endif
