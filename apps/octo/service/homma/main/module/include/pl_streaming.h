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

#ifndef	___PL_STREAMING_H___
#define	___PL_STREAMING_H___


/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
// hls, satip와 구분할지에 따라 숫자를 지정해야 해서, 일단은 이대로 둔다.
// Product들을 추가할 때는 보기 좋게 #elif 를 사용하자
#if defined(CONFIG_PROD_FVP4000T)
#define PL_STREAMING_MAX_NUM	1
#elif defined(CONFIG_PROD_HGS1000S)
#define PL_STREAMING_MAX_NUM	4
#elif defined(CONFIG_PROD_HGS1000SIT)
#define PL_STREAMING_MAX_NUM	4
#else
#define PL_STREAMING_MAX_NUM	2
#endif




/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef enum _PLRequstSpecType_e
{
	ePLRequstSpecType_Unknown,
	ePLRequstSpecType_HLS,
	ePLRequstSpecType_SATIP,
	ePLRequstSpecType_AIRPLAY,
	ePLRequstSpecType_NTLS,  // Non Transcoding Live Streaming : ByPass Mode

	ePLRequstSpecType_MAX
} PLRequstSpecType_e;


typedef enum _PLRequstMediaType_e
{
	ePLRequstMediaType_Unknown,
	ePLRequstMediaType_PVRTS_A,
	ePLRequstMediaType_PVRTS_AV,
	ePLRequstMediaType_LIVE_A,
	ePLRequstMediaType_LIVE_AV,
	ePLRequstMediaType_DATA,

	ePLRequstMediaType_MAX
} PLRequstMediaType_e;

typedef enum _PLStreaming_Status
{
	/* 0 보다 큰 값은 count로 사용 */
	ePLStreaming_Status_OK          = 0,
	ePLStreaming_Status_Error       = -1,
	ePLStreaming_Status_EOF         = -2,
	ePLStreaming_Status_Cancel      = -3,
	ePLStreaming_Status_Busy        = -4,
	ePLStreaming_Status_Lock        = -5,
	ePLStreaming_Status_OutOfMem    = -6,
	ePLStreaming_Status_Destroyed   = -8,
	ePLStreaming_Status_Timeout     = -9,
	ePLStreaming_Status_Unsupported = -10,
	ePLStreaming_Status_Suspended   = -11,
	ePLStreaming_Status_IOError     = -12,
	ePLStreaming_Status_Overflow    = -13,
	ePLStreaming_Status_Underflow   = -14,
	ePLStreaming_Status_Conflict    = -18,

	  /* 추가 요망 */
} PLStreaming_Status_e;


typedef enum _PLStreaming_OutType
{
	ePLStreaming_OutType_None		= 0x00,

	/* Ringbuf 는 default로 enable */
	ePLStreaming_OutType_Ringbuf	= 0x01,
	ePLStreaming_OutType_File		= 0x02,
	ePLStreaming_OutType_DumpFile	= 0x04,
	ePLStreaming_OutType_Callback	= 0x08,

	ePLStreaming_OutType_Max
} PLStreaming_OutType_e;

typedef enum
{
	ePLStreaming_Spec_None  = 0,

	/* base를 안쓰는 경우만 정의 */

	ePLStreaming_Spec_MinSize,
	ePLStreaming_Spec_MaxSize,
	ePLStreaming_Spec_Base,


	ePLStreaming_Spec_EndOfCase
} PLStreaimng_Spec_e;


typedef struct _PLStreaming_StartInfo_t
{
	DxRsvStreamType_e		eStreamType;
	PLStreaming_OutType_e	eOutType;
	HCHAR					*pszDeviceName;
	HCHAR					*pszDeviceID;
	HCHAR					*pszUserAgent;
	HCHAR					*pszIPAddress;
	HCHAR					*pszStreamingPath;
	int                     serverfd;
	int                     clientfd;
} PLStreaming_StartInfo_t;


typedef HOM_Result_e	(*cbPLStreamingStatus)( const HCHAR *pszSessionID, PLStreaming_Status_e state);
typedef void			(*cbPLConflictSchedule)(const Handle_t hStreamingId, HINT32 eNotifyType /* SAPI_NotifyType_e */, HINT32 nRequestPid, HCHAR *fromProcessName);

/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
extern 	HOM_Result_e PL_Streaming_Init(void);
extern	void		 PL_Streaming_DeInit(void);


extern	HOM_Result_e PL_Streaming_Start(const Handle_t hSetupInfo, Handle_t *ret_phStreamingId);
extern	void 		 PL_Streaming_End(const Handle_t hStreamingId);
extern	HOM_Result_e PL_Streaming_CreateStartInfo(		const DxRsvStreamType_e	eStreamType,
														const PLStreaming_OutType_e eOutType,
													const HCHAR *pszDeviceName,
													const HCHAR *pszUserAgent,
													const HCHAR *pszDeviceID,
													const HCHAR *pszStreamingPath,
													const HCHAR *pszIPAddress,
														int socketfd[2],
													PLStreaming_StartInfo_t *ret_pstStartInfo);
extern	void	 	 PL_Streaming_DestroyStartInfo( PLStreaming_StartInfo_t *pstStartInfo );
extern	HOM_Result_e PL_Streaming_CreateSetupHandle(const PLStreaming_StartInfo_t *pstStartInfo, Handle_t *ret_phSetupInfo);
extern	void 		 PL_Streaming_DestroySetupHandle(Handle_t hSetupInfo);

extern	HOM_Result_e PL_Streaming_M3U8_Get ( const Handle_t hStreamingId, const HCHAR *pRequestPath, HINT32 *ret_pM3U8BufSize, HUINT8 **ret_ppM3U8Buf);
extern	HOM_Result_e PL_Streaming_M3U8_Release (const Handle_t hStreamingId, const HCHAR *pRequestPath, HCHAR *pM3U8Buf);
extern	HOM_Result_e PL_Streaming_TS_Get ( const Handle_t hStreamingId, const HCHAR *pRequestPath, HINT32 *ret_pTSBufSize, HUINT8 **ret_ppTSBuf);
extern	HOM_Result_e PL_Streaming_TS_Release (const Handle_t hStreamingId, const HCHAR *pRequestPath, HCHAR *pTSBuf );
extern	HOM_Result_e PL_Streaming_Info_Get ( const Handle_t hStreamingId, const HCHAR *pCommand, const HCHAR *pRequestPath, HINT32 *ret_pInfoBufSize, HCHAR **ret_ppInfoBuf );
extern	HOM_Result_e PL_Streaming_Info_Release ( const Handle_t hStreamingId, const HCHAR *pCommand, const HCHAR *pRequestPath, HCHAR *pInfoBuf );
extern	HOM_Result_e PL_Streaming_RegisterStatus( const cbPLStreamingStatus cbStatus);
extern 	HOM_Result_e PL_Streaming_FindByStartInfo(const PLStreaming_StartInfo_t *pstStartInfo, Handle_t *ret_phStreamingId);
extern 	HBOOL	 	 PL_Streaming_IsUseStreamType(const DxRsvStreamType_e	eStreamType);


/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */


extern	void	HOMMA_CMDRegister_Streaming_Init(void);

#endif	//___PL_STREAMING_H___
