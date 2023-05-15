/** **********************************************************************************************************
	@file 		pal_mic.c

	@date		2014/03/05
	@author		jykim2@humaxdigital.com
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <pal_mic.h>
#include <di_mic.h>
#include <di_audio.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR	PAL_MIC_Start(void)
{
	HxLOG_Warning("\n");

	DI_AUDIO_SetMute(DI_AUDIO_OUTPUT_ALL ,FALSE);
	DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);
	DI_AUDIO_SetHdmiOutput(DI_AUDOUT_SPDIF_DOWNMIX);

	return  DI_MIC_Start();
}

HERROR	PAL_MIC_Stop(void)
{
	HxLOG_Warning("\n");

	return  DI_MIC_Stop();
}

HERROR	PAL_MIC_SetVolume(HUINT32 ulVolume)
{
	HUINT32	ulDecoderV = ((100-ulVolume) == 0) ? 20 : (100-ulVolume);

	HxLOG_Warning("ulVolume : %d, ulDecoderV : %d\n", ulVolume, ulDecoderV);
	DI_AUDIO_SetDecoderVolume(0, ulDecoderV);

	return DI_MIC_SetVolume(ulVolume);
}

