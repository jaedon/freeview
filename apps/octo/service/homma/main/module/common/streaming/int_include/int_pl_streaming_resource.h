
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


#ifndef	__INT_PL_STREAMING_RESOURCE__
#define	__INT_PL_STREAMING_RESOURCE__



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define PL_STREAMRES_INST_NUM					16


/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct	__plStreamResInst_t
{
    Handle_t			hResHandle;
    Handle_t			hRingbuf;
    Handle_t			hStreamingId;
    HBOOL				bExecuting;
    HUINT32				ulSamaSlot;

    UNIXTIME			utStartTime;
    UNIXTIME			utEndTime;
    UNIXTIME			utMakeTime;

    PLStreaming_SetupStatus_e eSetupStatus;

    HBOOL               bUseRingBuf;
} _plStreamResInst_t;

typedef struct 	__plStreamResMgr_t
{
    unsigned long			ulTaskId;
    unsigned long			ulMsgId;
    cbPLConflictSchedule	pConflictCallback;
    _plStreamResInst_t		astInstance[PL_STREAMRES_INST_NUM];
} _plStreamResMgr_t;


/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
extern	HOM_Result_e 	PL_Streaming_Resource_Init(void);
extern	void			PL_Streaming_Resource_DeInit(void);

extern	Handle_t		PL_Streaming_Resource_New(Handle_t hStreamingId);
extern	HUINT32		 	PL_Streaming_Resource_Delete(Handle_t hResHandle, HBOOL bWaiting);

extern	HINT32 			PL_Streaming_Resource_GetRemaining (void);
extern	void 			PL_Streaming_Resource_UpdateTime(const Handle_t hResHandle, int SchDuration, int PeriodSec);
extern	void 			PL_Streaming_Resource_SetConflictCallback (cbPLConflictSchedule cbCallback);
extern 	HOM_Result_e 	PL_Streaming_Resource_GetStatus(const Handle_t hResHandle, PLStreaming_SetupStatus_e *peSetupStatus);
extern	HOM_Result_e 	PL_Streaming_Resource_Start(const Handle_t hResHandle, PLStreaming_ModeInst_t *pModeInst, HUINT32 PlayPosition);
extern	HOM_Result_e 	PL_Streaming_Resource_UpdateSchedulePids(PLStreaming_ModeInst_t* pstInstMode, HINT32 nPIDsCount, HUINT16 usPIDs[], Handle_t hMsgId);
extern	HOM_Result_e 	PL_Streaming_Resource_UpdateSchedulePosition(const Handle_t hResHandle, HUINT32 ulPosition, Handle_t hMsgId);
extern  HOM_Result_e    PL_Streaming_Resource_WaitForCancelSchedule(HUINT32 SamaSlot);
extern _plStreamResInst_t *PL_Streaming_Resource_GetInstanceByRingBuf (Handle_t hRingBuf);



/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */
extern	void 			PL_Streaming_Resource_PrintScheduleInfo(DxSchedule_t	*pstSchedule);


#endif /* __INT_PL_STREAMING_RESOURCE__ */

