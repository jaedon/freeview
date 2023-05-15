
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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


#ifndef	__INT_PL_STREAMING_CONTEXT__
#define	__INT_PL_STREAMING_CONTEXT__



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

typedef enum  _PLStreamingClientType_e
{
	ePLStreamingClientType_Ios = 0,
	ePLStreamingClientType_Android,
	ePLStreamingClientType_WindowsPhone,
	ePLStreamingClientType_Unknown,
} PLStreamingClientType_e;

typedef struct _PLStreaming_SetupInfo_t
{
	HINT32					nChunkMultiply;
	HUID					uUID;
	HBOOL					bIsIncludeVideo;
	HCHAR					*pszDisplayString;
	HCHAR					*pszDeviceID;
	HCHAR					*pszIPAddress;
	HCHAR					*pszStreamingPath;
	const HCHAR				*pszPrefix; /* 삭제하면 절대 안됨, define된 값 */
	PL_MediaProfile_t		*pstMediaProfile;
	DxRsvStreamType_e		eStreamType;
	PLStreaming_OutType_e	eOutType;
	DxTuneParam_t			stTuningParam;

	// satip에서 m3u8을 이용시 사용, resource를 사용하지 않고, streaming id만 받음 .
	HBOOL					bNotUseResource;
	PLStreamingClientType_e etClientType;
	HCHAR					*pszDeviceName;
	int                     serverfd; // 2015/3/25 mhkang: http://svn.humaxdigital.com:3000/issues/101413
	int                     clientfd;
} PLStreaming_SetupInfo_t;

typedef enum  _PLStreaming_SetupStatus_e
{
	ePLStreaming_SetupStatus_None 		= 0,

	ePLStreaming_SetupStatus_Start,
	ePLStreaming_SetupStatus_Running,
	ePLStreaming_SetupStatus_Loaded,
	ePLStreaming_SetupStatus_Conflict,

	ePLStreaming_SetupStatus_HlsPausing,
	ePLStreaming_SetupStatus_HlsPaused,
	ePLStreaming_SetupStatus_Error,

	ePLStreaming_SetupStatus_End
} PLStreaming_SetupStatus_e;

typedef enum _PLStreaming_InfoType_e
{
	PLStreaming_InfoType_Unknown = 0,
	PLStreaming_InfoType_Describe,
	PLStreaming_InfoType_Options
} PLStreaming_InfoType_e;

typedef enum _PLStreaming_DelayedCmd_e
{
	PLStreaming_DelayedCmd_None = 0,
	PLStreaming_DelayedCmd_Play

} PLStreaming_DelayedCmd_e;

typedef struct _PLStreaming_ModeHLS_t
{
	PL_StreamingKey_t		*pstBitrateStreamingKey;
	Handle_t				hProfileHandle;
	HUINT32                 RequestTS;      // 2015/2/4 mhkang: Request TS Index
	HUINT32                 StartTsIndex;   // 2015/1/26 mhkang: Support PVR HLS SEEK(http://svn.humaxdigital.com:3000/issues/100200)
} PLStreaming_ModeHLS_t;

typedef struct _PLStreaming_ModeSatIp_t
{
	DxService_t 	*pstSvc;
	DxTransponder_t	*pstTS;
	HxFILE_t 		*pDumpFile;
	HBOOL			bFirstGet;
	HUINT32			ulFirstGetTick;
#ifdef CONFIG_DEBUG
	HUINT32			ulStartReadTick;
	HUINT32			ulLastReadTick;
#endif
	HBOOL			bSrcConverted;

	HBOOL			bIgnoreNext;
	HINT32          nSrc;   // 2014/6/11 mhkang: Use nSrc when 'src' parameter does not exist in RTSP Message.
	HBOOL           bIsOwner;   // 2014/6/12 mhkang: SAT>IP owner or no-owner?
	HINT32			nStreamId;
	void*           pUdsObj;    // 2015/3/25 mhkang: http://svn.humaxdigital.com:3000/issues/101413
} PLStreaming_ModeSatIp_t;

typedef struct _PLStreaming_ModeByPass_t
{
	PL_StreamingKey_t				*pstBitrateStreamingKey;
	HINT32			nLastReadChunkId;
	HxFILE_t 				*pDumpFile;
#ifdef CONFIG_DEBUG
	HUINT32					ulStartReadTick;
	HUINT32					ulLastReadTick;
#endif

} PLStreaming_ModeByPass_t;

#define     MASK_PID_ALL        0x1
#define     MASK_PID_NONE       0x2
#define     MASK_PID_NULL       0x4

typedef struct _PLStreaming_ModeInst_t
{
	Handle_t						hStreamingId;
	Handle_t						hRingbuf;
	Handle_t						hResHandle;
	HINT32							nBitrate;
	PLStreaming_SetupInfo_t			stSetupInfo;
	PLMediaProfile_Item_t			*pstMediaProfileItem;
	PL_StreamingKey_t				*pstStreamingKey;
	HINT32							nPIDsCount;
	HUINT16 						usPIDs[DxRSV_PIDTABLE_COUNT];
	PLStreaming_SetupStatus_e		eSetupStatus;

	union
	{
		PLStreaming_ModeHLS_t 		stHLS;
		PLStreaming_ModeSatIp_t		stSatIp;
		PLStreaming_ModeByPass_t	stByPass;
	} Mode;

	/* 2014/3/10 mhkang: RTSP SETUP/PLAY 명령시 TP변경 여부  */
	HBOOL                           isChangedTP;
	HCHAR*							tunerInfo;
	HUINT32							tunerInfo_length;
	HBOOL                           isLocked;           // 2014/12/22 mhkang: when homma receive ts from obama, isLocked becomes true.
	UNIXTIME                        utUpdateTunerInfo;  // 2014/12/22 mhkang: Period of updating TP Info.
	PLStreaming_DelayedCmd_e        eDelayedCmd;

	/* 2015/2/11 mhkang:
	PidFlag == 0   : pids="1,2,3,.."
	PidFlag & 0x01 : pids=all
	PidFlag & 0x02 : pids=none
	PidFlag & 0x04 : not exist
	*/
	HCHAR                           PidFlag;
	HINT32					tunerid;
} PLStreaming_ModeInst_t;


typedef struct	_plStreamingInst_t
{
	HULONG					ulSemId;
	HBOOL					bExecuting;

	Handle_t				hStreamingId;
	DxRsvStreamType_e		eStreamType;
	PLStreaming_ModeInst_t	*pstModeInst;
} plStreamingInst_t;

/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
extern 	HOM_Result_e 	PL_Streaming_Context_Init(void);
extern	void 			PL_Streaming_Context_DeInit(void);


extern	Handle_t 		PL_Streaming_Context_New (PLStreaming_SetupInfo_t *pstSetup);
extern	void 			PL_Streaming_Context_Delete (const Handle_t hStreamingId);
extern	HOM_Result_e 	PL_Streaming_Context_ResConnect (Handle_t hStreamingId);
extern	HOM_Result_e 	PL_Streaming_Context_ResConflict(const Handle_t hStreamingId, HINT32 eNotifyType/*SAPI_NotifyType_e*/, HINT32 nRequestPid, HCHAR *fromProcessName);
extern	HOM_Result_e	PL_Streaming_Context_GetM3U8 (const Handle_t hStreamingId, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData);
extern	HOM_Result_e 	PL_Streaming_Context_ReleaseM3U8 (const Handle_t hStreamingId, const HCHAR *pRequestPath, HUINT8 *pucData);
extern	HOM_Result_e 	PL_Streaming_Context_GetTS (const Handle_t hStreamingId,  const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData);
extern	HOM_Result_e 	PL_Streaming_Context_ReleaseTS (const Handle_t hStreamingId,  const HCHAR *pRequestPath, HUINT8 *pucData);
extern	HOM_Result_e 	PL_Streaming_Context_ExecCmd (const Handle_t hStreamingId,  const HCHAR *pRequestPath);
extern	HOM_Result_e	PL_Streaming_Context_GetInfo ( const Handle_t hStreamingId, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HINT32 *pulDataSize, HCHAR **ppucData );
extern	HOM_Result_e	PL_Streaming_Context_ReleaseInfo ( const Handle_t hStreamingId, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HCHAR *pucData );
extern	HCHAR			*PL_Streaming_Context_GetPrefixStringByStreamType(DxRsvStreamType_e eStreamType);
extern	HOM_Result_e	PL_Streaming_Context_CreateSetupInfo (DxRsvStreamType_e eStreamType, const PLStreaming_StartInfo_t *pstStartInfo, PLStreaming_SetupInfo_t *ret_pstSetupInfo);
extern	HOM_Result_e 	PL_Streaming_Context_DeleteSetupInfo (PLStreaming_SetupInfo_t *pstSetupInfo);
extern	Handle_t 		PL_Streaming_Context_FindByStartInfo (const PLStreaming_StartInfo_t *pstStartInfo);
extern	Handle_t 		PL_Streaming_Context_FindByStreamingKey (const PL_StreamingKey_t *pstStreamingKey);
extern	HBOOL 			PL_Streaming_Context_IsIncludeVideo(DxRsvStreamType_e eStreamType);
extern	HBOOL 			PL_Streaming_Context_IsLive(DxRsvStreamType_e eStreamType);
extern	HOM_Result_e 	PL_Streaming_Context_CreateSetupInfoByStreamingId(const Handle_t hStreamingId, PLStreaming_SetupInfo_t *ret_pstSetupInfo);
extern	HOM_Result_e 	PL_Streaming_Context_GetSetupStatus(const Handle_t hStreamingId, PLStreaming_SetupStatus_e *ret_peSetupStatus);
extern	HOM_Result_e 	PL_Streaming_Context_GetProfileItemHandle(const Handle_t hStreamingId, Handle_t *ret_phProfileItem);
extern	HOM_Result_e 	PL_Streaming_Context_DupStreamingKey(const Handle_t hStreamingId, PL_StreamingKey_t **ret_ppstStreamingKey);
extern	HOM_Result_e 	PL_Streaming_Context_GetPIDs(const Handle_t hStreamingId, HINT32 *ret_pnPIDsCount, HUINT16 *ret_arrusPIDs);
extern	HOM_Result_e 	PL_Streaming_Context_GetRingbufHandle(const Handle_t hStreamingId, Handle_t *ret_phRingbuf);
extern	HBOOL	 		PL_Streaming_Context_IsUseStreamType(const DxRsvStreamType_e	eStreamType);
extern	HBOOL	 		PL_Streaming_Context_RegisterStatus(const cbPLStreamingStatus cbStatus);
extern	HOM_Result_e	PL_Streaming_Context_StatusToDLNALib(const Handle_t hStreamingId, PLStreaming_Status_e eState);
extern  void*			PL_Streaming_Context_FindStreamingInstanceByStreamId (HBOOL isOwner, const HINT32 nStreamId);
extern	void			PL_Streaming_Context_Task(void);
extern  HCHAR*          PL_Streaming_Context_GetSetupStatusStr(PLStreaming_SetupStatus_e SetupStatus);
extern  plStreamingInst_t *PL_Streaming_Context_getInstance (const Handle_t hStreamingId);

/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

extern	void PL_Streaming_Context_Dump (Handle_t hStreaming);
extern	void PL_Streaming_Context_SetupDump (const PLStreaming_SetupInfo_t *pstSetup);


extern	void	HOMMA_CMDRegister_StreamingContext(const HCHAR *hCmdHandle);

#endif /* __INT_PL_STREAMING_CONTEXT__ */

