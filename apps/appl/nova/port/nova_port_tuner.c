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

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nova_port.h>
#include "tuner_api.h"
/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/

PUBLIC NovaspreadTUInt32 NovaspreadTunerGetTransportSessionId ( NovaspreadTTuner This )
{
	return TUNERAPI_GetTransportSessionId(This);
}

PUBLIC NovaspreadTBoolean NovaspreadTunerSetTranscoding(NovaspreadTTuner This, NovaspreadTTranscoding* aTranscoding)
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_SetTranscoding(This, aTranscoding);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_SetTranscoding failed(Err:%d)\n", ErrCode);
		return NOVASPREAD_FALSE;
	}

	return NOVASPREAD_TRUE;
}

PUBLIC NovaspreadTBoolean NovaspreadTunerSetStateChangeListener(NovaspreadTTuner This, NovaspreadTTunerStateChangeListener aListener, void* aContext )
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_SetStateChangeListner(This, aListener, aContext);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_SetStateChangeListner failed(Err:%d)\n", ErrCode);
		return NOVASPREAD_FALSE;
	}

	return NOVASPREAD_TRUE;
}

PUBLIC NovaspreadTTunerError NovaspreadTunerGetError ( NovaspreadTTuner This )
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_GetErrorCode(This);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_GetErrorCode failed(Err:%d)\n", ErrCode);
		// convert ErrorCode into Nova ErrCode;
		return -1;  // not defined by Novaspread.
	}


	return NOVASPREAD_TUNER_ERROR_NONE;
}

PUBLIC NovaspreadTBoolean NovaspreadTunerSetPids(NovaspreadTTuner  This, NovaspreadTUInt16* aPids, NovaspreadTUInt32  aNoOfPids)
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_SetPids(This, aPids, aNoOfPids);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_SetPids failed(Err:%d)\n", ErrCode);
		return NOVASPREAD_FALSE;
	}

	return NOVASPREAD_TRUE;
}

PUBLIC NovaspreadTBoolean NovaspreadTunerSetAllPids ( NovaspreadTTuner This )
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_SetPids(This, NULL, MASK_PID_ALL);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_SetPids failed(Err:%d)\n", ErrCode);
		return NOVASPREAD_FALSE;
	}

	return NOVASPREAD_TRUE;
}

PUBLIC NovaspreadTBoolean NovaspreadTunerStart ( NovaspreadTTuner This )
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_Start(This);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_Start failed(Err:%d)\n", ErrCode);
		return NOVASPREAD_FALSE;
	}

	return NOVASPREAD_TRUE;
}

PUBLIC void NovaspreadTunerStop ( NovaspreadTTuner This )
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_Stop(This);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_Stop failed(Err:%d)\n", ErrCode);
		return;
	}

	return;
}

PUBLIC NovaspreadTBoolean NovaspreadTunerIsLocked (NovaspreadTTuner This )
{
	NOVA_ErrCode_e ErrCode;
	bool isLocked = false;

	ErrCode = TUNERAPI_IsLocked(This, &isLocked);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_IsLocked failed(Err:%d)\n", ErrCode);
		return NOVASPREAD_FALSE;
	}

	return isLocked;
}

PUBLIC NovaspreadTTunerSignalInfo NovaspreadTunerGetSignalInfo( NovaspreadTTuner This )
{
	NovaspreadTTunerSignalInfo info = {0, 0};
	NOVA_ErrCode_e ErrCode;
	int level, quality;

	ErrCode = TUNERAPI_GetSignalLevel(This, &level);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_GetSignalLevel failed(Err:%d)\n", ErrCode);
		return info;
	}

	ErrCode = TUNERAPI_GetSignalQuality(This, &quality);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_GetSignalQuality failed(Err:%d)\n", ErrCode);
		return info;
	}

	info.Level	= (unsigned char)level;
	info.Quality= (unsigned char)quality;

	return info;
}

PUBLIC NovaspreadTBoolean NovaspreadTunerSetDataAvailableListener(NovaspreadTTuner This, NovaspreadTTunerDataAvailableListener aListener, void * aContext )
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_SetDataAvailableListner(This, aListener, aContext);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_SetDataAvailableListner failed(Err:%d)\n", ErrCode);
		return NOVASPREAD_FALSE;
	}

	return NOVASPREAD_TRUE;
}

PUBLIC NovaspreadTUInt32 NovaspreadTunerReadData(NovaspreadTTuner This, NovaspreadTUInt8* aBuffer, NovaspreadTUInt32 aBufferSize)
{
	int readSize;

	readSize =  TUNERAPI_ReadPackets(This, aBuffer, aBufferSize);
	if (readSize < 0)
	{
		HxLOG_Error("TUNERAPI_ReadPackets failed(Err:%d)\n", readSize);
		return 0;
	}

	return readSize;
}

/**
@param aPriority:
	0000-0999 : reserved for future use
	1000-1999 : low priority host features (PIP, Download, etc.)
	2000-2999 : NovaspreadClient features
	3000-3999 : high priority host features (HDMI=3500)
	4000-65535 : reserved for future use
*/
PUBLIC NovaspreadTBoolean NovaspreadTunerSetPriority(NovaspreadTTuner This, NovaspreadTUInt16 aPriority)
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_SetPriority(This, aPriority);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_SetPriority failed(Err:%d)\n", ErrCode);
		return NOVASPREAD_FALSE;
	}

	return NOVASPREAD_TRUE;
}

/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/

