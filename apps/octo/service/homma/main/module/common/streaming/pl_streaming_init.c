/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "int_streaming.h"

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
static PLStreaimng_Spec_e _plStreaming_GetSpec(void)
{
	if(HLIB_STD_StrCaseStr(CONFIG_PRODUCT_NAME, "ir4000hd"))
	{
		return ePLStreaming_Spec_MinSize;
	}
	else if(HLIB_STD_StrCaseStr(CONFIG_PRODUCT_NAME, "ir4100hd"))
	{
		return ePLStreaming_Spec_MinSize;
	}
	else if(HxSTD_StrCmp(CONFIG_PRODUCT_NAME, "icordpro"))
	{
		return ePLStreaming_Spec_MaxSize;
	}
	else if(HxSTD_StrCmp(CONFIG_PRODUCT_NAME, "hdr3000t"))
	{
		return ePLStreaming_Spec_MinSize;
	}
	else if(HxSTD_StrCmp(CONFIG_PRODUCT_NAME, "fvp4000t"))
	{
		return ePLStreaming_Spec_MinSize;
	}
	else
	{
		return ePLStreaming_Spec_Base;
	}
}



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Init(void)
{
	HERROR			hErr = ERR_OK;
	HOM_Result_e  	eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	hErr = HLIB_CP_Init();
	if( ERR_OK != hErr)
	{
		HxLOG_Error("Codepage Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}

	hErr = PL_RINGBUF_Init();
	if( ERR_OK != hErr)
	{
		HxLOG_Error("Codepage Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}

	eHomResult = PL_Streaming_Debug_Init();
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_Streaming_Debug_Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}

	eHomResult = PL_StreamingKey_Init();
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_StreamingKey_Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}

#ifdef CONFIG_TRANSCODING_STREAMING
	eHomResult = PL_MediaProfile_Init();
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_MediaProfile_Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}
#endif

	eHomResult = PL_Streaming_Spec_Init(_plStreaming_GetSpec());
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_Streaming_Spec_Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}

	eHomResult = PL_Streaming_Resource_Init();
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_Streaming_Callback_Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}

	eHomResult = PL_Streaming_Context_Init();
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_Streaming_Context_Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}

	eHomResult = PL_Streaming_Callback_Init();
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_Streaming_Callback_Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}

#ifdef CONFIG_PROD_HGS1000S
	HxLOG_Info("HGS1000S does not support NTLS streaming!\r\n");
#else
	eHomResult = PL_Streaming_Receive_Init();
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_Streaming_Callback_Init fail!\r\n");
		return eHOM_Result_Error_InitFail;
	}
#endif

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_DeInit(void)
{
	ENTER_FUNCTION;
	PL_Streaming_Context_DeInit();
	PL_Streaming_Callback_DeInit();
	PL_Streaming_Receive_DeInit();
	PL_Streaming_Resource_DeInit();
	PL_StreamingKey_DeInit();

	LEAVE_FUNCTION;

	return ;
}


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/

void	HOMMA_CMDRegister_Streaming_Init(void)
{
	HCHAR *hCmdHandle = "streaming";


#if defined(CONFIG_DEBUG)

#endif
	HOMMA_CMDRegister_Streaming(hCmdHandle);
	HOMMA_CMDRegister_StreamingCallback(hCmdHandle);
	HOMMA_CMDRegister_StreamingContext(hCmdHandle);
	HOMMA_CMDRegister_StreamingReceive(hCmdHandle);

}



