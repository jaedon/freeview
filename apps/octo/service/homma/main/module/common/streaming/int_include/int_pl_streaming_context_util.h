/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  int_pl_streaming_context_util.h.
	@brief

	Description:  									\n
	Module: PL/Streaming Context Util				\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PL_StreamingContextUtil_H___
#define	___PL_StreamingContextUtil_H___


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define PL_STREAMING_HLS_DEFAULT_CHANNEL_PREFIX			"HCHANNEL:"
#define PL_STREAMING_HLS_DEFAULT_PVRFILE_PREFIX			"/mnt/hd2/Recordings"
#define PL_STREAMING_HLS_FILE_EXTENSION					".m3u8"

#define PL_STREAMING_ByPass_DEFAULT_CHANNEL_PREFIX		"HCHANNEL:"
#define PL_STREAMING_ByPass_DEFAULT_PVRFILE_PREFIX		"/mnt/hd2/.recordings"
#define PL_STREAMING_ByPass_FILE_EXTENSION				".m3u8"


/*******************************************************************/
/********************      Enum     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Structs   *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HOM_Result_e PL_StreamingContextUtil_MakeTuningParamAndDisplayString(PLStreaming_ModeInst_t *pstInst);
extern HOM_Result_e PL_StreamingContextUtil_GetUID(const PLStreaming_StartInfo_t *pstStartInfo, HUID *ret_puUID);
extern HOM_Result_e PL_StreamingContextUtil_GetRecDuration(HUID UID, HINT32 *RecDuration);



/*******************************************************************/
/********************      Debug     *******************************/


#endif	//___PL_StreamingContextUtil_H___
