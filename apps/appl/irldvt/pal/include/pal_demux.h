/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_demux.h.
	@brief	  pal layer¿« demux module header file.

	Description: 									\n
	Module: PAL/DEMUX							 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef	__PAL_DEMUX_H__
#define	__PAL_DEMUX_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>

#include <hlib.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	ePAL_DMX_TSSTATE_UNKNOWN = 0,
	ePAL_DMX_TSSTATE_CLEAN,
	ePAL_DMX_TSSTATE_SCRAMBLED
}PalDmx_TsState_e;

typedef struct
{
	HUINT16					usPcrPid;
	HUINT16					usVideoPid;
	HUINT16					usMainAudioPid;
	HUINT16					usSubAudioPid;
}PalDmx_PidInfo_t;

typedef struct
{
	HBOOL					bMainPcrFilter;
	HBOOL					bSubPcrFilter;
	HBOOL					bMainVideoFilter;
	HBOOL					bSubVideoFilter;
	HBOOL					bMainAudioFilter;
	HBOOL					bSubAudioFilter;
}PalDmx_FilterStatus_t;

typedef struct
{
	PalDmx_PidInfo_t			stPid;
	PalDmx_FilterStatus_t		stStatus;
	HBOOL					bSupportPcsb;
	HUINT16					stPcsb[2];							/* PID for checking scrambled bit */
	HUINT32					stPcsbHandle[2];
}PalDmx_Status_t;


/**
@brief Pal Demux's Input device types
*/
typedef enum tagePAL_DMX_INPUT_TYPE
{
	ePAL_DMX_INPUT_NONE = 0,    			/**< Input device type : Not connected */
	ePAL_DMX_INPUT_CURRENT ,    			/**< Input device type : Use previsous setting*/
	ePAL_DMX_INPUT_TUNER,					/**< Input device type : tuner */
	ePAL_DMX_INPUT_RECORDER,				/**< Input device type : recorder: play file that is currently recording. */
	ePAL_DMX_INPUT_SCRAMBLED_FILE_RECORDER, /**< Input device type : play scrambled file that is currently recording. */
	ePAL_DMX_INPUT_PLAYER,					/**< Input device type : player: play file in storage media. */
	ePAL_DMX_INPUT_SCRAMBLED_FILE_PLAYER,    /**< Input device type : play scrambled file */
	ePAL_DMX_INPUT_MEDIA_PLAYER,				/**< Input device type : used to set demux path for media module.>*/
	ePAL_DMX_INPUT_EXTERNAL,					/**< Input device type : external input */
	ePAL_DMX_INPUT_MAX						/**< Input device type : wrong Input type*/
} PalDmx_Input_e;


/**
@brief Pal Demux's Output device types
*/
typedef enum tagePAL_DMX_OUTPUT_TYPE
{
	ePAL_DMX_OUTPUT_NONE = 0,				/**< Output device type : Not specefic - initial value */
	ePAL_DMX_OUTPUT_CURRENT ,    			/**< Output device type : Use previsous setting*/
	ePAL_DMX_OUTPUT_RECORDER,				/**< Output device type : recorder */
	ePAL_DMX_OUTPUT_DECODER,				/**< Output device type : decoder */
	ePAL_DMX_OUTPUT_EXTERNAL,				/**< Output device type : output stream without modification.*/
	ePAL_DMX_OUTPUT_MAX						/**< Output device type : Wrong Output type */
} PalDmx_Output_e;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR PAL_DMX_Init(void);
HERROR PAL_DMX_SetPath(HUINT32 ulDemuxId, PalDmx_Input_e eInType, HUINT32 ulInDvcId, PalDmx_Output_e eOutType, HUINT32 ulGroupId);
HERROR PAL_DMX_ReleasePath(HUINT32 ulDemuxId);
HERROR PAL_DMX_StartAVPid(HUINT32 ulDemuxId, PalDmx_PidInfo_t *pstPidInfo, HBOOL bMainAV);
HERROR PAL_DMX_ChangeAVPid(HUINT32 ulDemuxId, PalDmx_PidInfo_t *pstPidInfo);
HERROR PAL_DMX_StopAllAVPid(HUINT32 ulDemuxId);
PalDmx_PidInfo_t *PAL_DMX_GetAVPidStatus(HUINT32 ulDemuxId);

HERROR PAL_DMX_RegisterPidForScrambleBit(HUINT32 ulDemuxId, HUINT16 usVideoPid, HUINT32 usAudioPid);
HERROR PAL_DMX_GetScrambleBitStatus(HUINT32 ulDemuxId, HUINT16 *pusVideoPid, HUINT16 *pusAudioPid);
HERROR PAL_DMX_UnRegisterPidForScrambleBit(HUINT32 ulDemuxId);
HERROR PAL_DMX_CheckScrambleBitByPid(HUINT32 ulDemuxId, PalDmx_TsState_e *peState);

HBOOL PAL_DMX_IsSupportScrambleBitChecking(HUINT32 ulDemuxId);

HERROR PAL_DMX_GetPlayBackId(HUINT32 unDmxId, HUINT32 *punPlayBackId);

#if defined(CONFIG_SUPPORT_FCC)
HERROR PAL_DMX_StartAVPrimerPid(HUINT32 ulDemuxId, PalDmx_PidInfo_t *pstPidInfo, HBOOL bMainAV, HUINT32 ulPrimerIdx);
HERROR PAL_DMX_StopAllAVPrimerPid(HUINT32 ulDemuxId, HUINT32 ulPrimerIdx);
HERROR PAL_DMX_ChangePrimerAVPid(HUINT32 ulDemuxId, PalDmx_PidInfo_t *pstPidInfo, HUINT32 ulPrimerIdx);
#endif


#endif	//__PAL_DEMUX_H__

