/**
	@file     	pmgr_transcoder.c
	@brief    	transcode recording control application.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 						 	\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <octo_common.h>

#include <svc_pipe.h>
#include <svc_transcoder.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_transcoder.h>
//#include <xmgr_transcoder.h>

#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>

#include "./local_include/_xmgr_transcoder.h"

#define ___LOCAL_DEFINITIONS___

#define ___LOCAL_DATATYPES___

#define ___LOCAL_VARIABLES___

#define ___LOCAL_FUNCTION_PROTOTYPES___

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___

#define ___PLUGIN_FUNCTIONS___

HERROR pmgr_transcoder_Start (HUINT32 ulActionId, MgrTrcd_Start_t *pstStartArg)
{
#if defined(CONFIG_TRANSCODING_STREAMING)
	return xmgr_transcoder_Start (ulActionId, pstStartArg);
#else
	return ERR_FAIL;
#endif
}

HERROR pmgr_transcoder_Stop (HUINT32 ulActionId)
{
#if defined(CONFIG_TRANSCODING_STREAMING)
	return xmgr_transcoder_Stop (ulActionId);
#else
	return ERR_FAIL;
#endif
}

// end of file
