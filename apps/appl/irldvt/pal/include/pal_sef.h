/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_sef.h
	@brief	  pal section filtering module

	Description: File에 대한 설명을 적습니다.		\n
	Module: PAL - Demux \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_SEF_H__
#define	__PAL_SEF_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define PAL_SEF_MAX_FILTER_LEN 				16
#define PAL_SEF_MAX_SI_RAW_SECTION_NUM      0x200

#ifdef CONFIG_DEBUG
//#define _PAL_SEF_ALLOC_FREE_DEBUG_
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#define _____regarding_to_demux_________________________
typedef enum
{
	eDMX_Type_LiveRec,
	eDMX_Type_Playback,
	eDMX_Type_Unknown
} PalSef_DemuxType_e;

typedef struct
{
	HUINT8 ucNumLiveRecDemux;/**< live, record 용 demux 개수 */
	HUINT8 ucNumPbDemux;     /**< playback 전용 demux 개수 */
} PalSef_DemuxInfo_e;

#define _____si_filter_thread__________________________________________________

/*
	@param ulType Table id(16bit) and Result(16bit) result is PalSef_FilterResult_e.
	@param ulParam1 FilterId(16bit) and Ts Unique Id(16bit)
	@param ulParam2 SiRawData_t의 구조를 가지는 section raw data
	@param ulParam3 대부분 hAction으로 사용.
	@remark Callback 함수와 Free함수 두가지로 사용된다.
*/
typedef void (*PalSef_Callback_t)(HUINT32 ulType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

typedef enum
{
	eSefFilterLen_4Bytes = 4,
	eSefFilterLen_8Bytes = 8,
	eSefFilterLen_10Bytes = 10,
	eSefFilterLen_16Bytes = 16
} PalSef_FilterLength_e;

typedef enum
{
	eSiFilterResult_Complete = 1,
	eSiFilterResult_Timeout,
	eSiFilterResult_Error
} PalSef_FilterResult_e;

/** @brief si section filter message class */
typedef enum
{
	eSiFilterMsg_Retry,						/**< 가용 filter가 없어서 retry 하거나 (filter fail의 경우), monitoring을 위한 retry */
	eSiFilterMsg_Timeout,
	eSiFilterMsg_Start,
	eSiFilterMsg_Stop,
	eSiFilterMsg_StopAll,					/**< 해당 demux 상의 모든 filter를 stop한다 */
	eSiFilterMsg_StopByUserParam, 			/**< 같은 user param을 갖는 모든 필터를 stop, 주로 action handle */
	eSiFilterMsg_EmergencyStop = 0x8282		/**< receie task가 msg q full나는 경우, retry하는 용도로 사용 */
} PalSef_FilterMsg_e;

/**                                                                                                                                         \n
  @brief si section filter mode                                                                                                             \n
         (1) once mode                                                                                                                      \n
             - table complete 후 parse callback 한 번 실행 후 filter table이 사라짐.                                                        \n
             - 따로 filter를 stop할 필요는 없음.                                                                                            \n
         (2) monitor mode                                                                                                                   \n
             - table complete 후 filter table이 계속 남아있으며, section의 crc32가 변경된 경우에 parse callback을 실행한다.                 \n
             - 더 이상 filter를 사용하지 않는 경우 start filter 시 할당 받은 filter id로 stop해줘야됨.                                      \n
             - monitor by repeat과 monitor by vernum은 내부 구동 방식의 차이임.                                                             \n
               가용 필터 개수를 늘리고 싶으면, monitor by repeat을 사용,                                                                    \n
               sithd에 부담을 적게 주려면, monitor by vernum을 사용바람.                                                                    \n
               실제 둘 사이의 차이는 거의 못느낄텐데, 혹시라도 튜닝이 필요한 경우가 있다면 (chip dependant 하다든지.. ) 조정해서 사용바람   \n
         (3) continue mode                                                                                                                  \n
             - section 받을 때마다 parse callback 실행                                                                                      \n
             - filter id로 stop 필요                                                                                                        \n
*/
typedef enum
{
	eSiFilterMode_Once,
	eSiFilterMode_Monitor,	/* monitor by repeat */
	eSiFilterMode_Continue
} PalSef_FilterMode_e;

/**< si table gathering 시 table 단위로 complete를 체크할 지 (eit 제외), one section만 처리할 지를 알리는 flag */
typedef enum
{
	eSiCompleteMode_Table,
	eSiCompleteMode_Section
} PalSef_TableCompleteMode_e;

/**< filter timeout 처리 모드 */
typedef enum
{
	eContinueTimeout_Once = 0,	/* 최초 한번 만 발생 */
	eContinueTimeout_Repeat	/* 최초 발생 후 timeout 주기에 따라 계속 발생 */
} PalSef_TimeOutMode_e;

/** @brief si section filter parameter */
typedef	struct
{
	HUINT8        		ucDemuxId;
	HUINT16       		usUniqueId;
	HUINT16       		usPid;
	HUINT16       		usFilterId;
	PalSef_TableCompleteMode_e	eSiTableCompleteMode;
	PalSef_TimeOutMode_e			eTimeoutMode;
	HUINT8        		ucMode;        		/**< SiFilterMode_t를 가공하여 사용*/
	HINT32				nSecCntInPackage;	/**< section filter로 continue mode 사용 시 한번에 달아 보낼 section count */
	HUINT32				nDurationInPackage;	/**< section filter로 continue mode 사용 시 timeout duration */
	HUINT32       		ulTimeOut;
	HUINT32       		ulUserParam;
	HBOOL         		bCheckCrc;
	PalSef_Callback_t  		pfnParseFunc;
	PalSef_Callback_t  		pfnFreeFunc;
	PalSef_FilterLength_e 	eFilterLen;
	HUINT8        		pFilterValue[PAL_SEF_MAX_FILTER_LEN];
	HUINT8        		pFilterMask[PAL_SEF_MAX_FILTER_LEN];
} PalSef_FilterParam_t;

/** @brief si raw section structure to make a complete PSI/SI table. It could be used in si callbacks */
typedef	struct
{
	HUINT16	flag;
	HUINT16	num_sec;
	HUINT8	*buf[PAL_SEF_MAX_SI_RAW_SECTION_NUM];
} PalSef_RawData_t;

/**< section package flag, 한 메시지에 여러 section을 달아 보낼 경우 사용되는 것임 */
typedef enum
{
	eSectionPackage_Off = 0,
	eSectionPackage_On
} PalSef_SectionPackageFlag_e;

/**<---------------------------------------------------------
	si send thread message structure
-----------------------------------------------------------*/
/**< for eSiFilterMsg_Retry */
typedef struct
{
	HINT32				nFilterIndex;
} PalSef_RetryMsg_t;

/**< for eSiFilterMsg_Timeout */
typedef struct
{
	HUINT32				ulTimerId;
} PalSef_TimeOutMsg_t;

/**< for eSiFilterMsg_Start */
typedef struct
{
	PalSef_FilterParam_t		*pFilterParam;
} PalSef_StartMsg_t;

/**< for eSiFilterMsg_Stop */
typedef struct
{
	HUINT16				usFilterid;
	HUINT16				usReserved;
} PalSef_StoptMsg_t;

/**< for eSiFilterMsg_StopAll */
typedef struct
{
	HUINT32				ulReserved;
} PalSef_StoptAllMsg_t;

/**< for eSiFilterMsg_StopByUserParam */
typedef struct
{
	HUINT32				ulUserParam;	/**< 대개는 action handle이 지정됨 */
} PalSef_StoptByUserMsg_t;

/**< for eSiFilterMsg_EmergencyStop */
typedef struct
{
	HUINT32				ulRequestId;
} PalSef_EmergentStopMsg_t;

typedef struct tagSefSendMessage
{
	PalSef_FilterMsg_e				eMsgClass;
	HUINT8						ucDemuxId;

	/* 각 msg별로 사용하는 필드를 포함하는 구조체 */
	union
	{
		PalSef_RetryMsg_t			stRetry;
		PalSef_TimeOutMsg_t			stTimeout;
		PalSef_StartMsg_t			stStart;
		PalSef_StoptMsg_t				stStop;
		PalSef_StoptAllMsg_t			stStopAll;
		PalSef_StoptByUserMsg_t	stStopByUserParam;
		PalSef_EmergentStopMsg_t	stEmergencyStop;
	} msg;
} PalSef_SendMessage_t;

/**< section receive message structure */
typedef struct tagSefRcvMessage
{
	/* receive thread가 하는 일이 한 가지로 정형화 되어 있으므로, msg class가 없어도 됨. */
	HERROR						err;
	PalSef_SectionPackageFlag_e	eSecPackageFlag;
	HUINT8						ucDemuxId;
	HUINT8						ucTableId;
	HUINT32						ulRequestId;
	HUINT32						ulDataAddr;
} PalSef_ReceiveMessage_t;

#define __________________________________________________________________

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR PAL_SEF_Init(void);
extern HERROR PAL_SEF_GetInfo(PalSef_DemuxInfo_e *pstDemuxInfo);
extern HERROR PAL_SEF_GetType(HUINT8 ucDemuxId, PalSef_DemuxType_e *pDemuxType);

/**
  @brief
  Function Name        : PAL_SEF_GetNumOfSectionFilter
  Function Description : demux에서 가용한 section filter의 개수를 demux별로 읽어온다
  type                 : inline
  Return Value         : ERR_OK - 성공
  						 ERR_FAIL - 실패
*/
extern HERROR PAL_SEF_GetNumOfSectionFilter(HUINT8 ucDemuxId, HINT32 *pnNumOfSecFilter);

#define _____si_filter_thread_api_______________________________________

/**
  @brief
  Function Name        : PAL_SEF_StartSecFilter
  Function Description : SI, CAS등이 사용하는 section filter 거는 함수
  Parameter            : PalSef_FilterParam_t
  type                 : Async
  Return Value         : filter id - 함수를 채 빠져나오기 전에 si receive task에서 parse callback이 실행되는 경우가 있다. 이런 문제를 피하기 위해서, callback은 메시지만 보내는 형식이 되어야 한다.
*/
extern HUINT16 PAL_SEF_StartSecFilter(PalSef_FilterParam_t *pSiFilterParam);

/**
  Function Name        : PAL_SEF_StopAllSecFilterSync
  Function Description : 할당받은 demux id를 이용하여 filter를 모두 stop한다
  Parameter            : demux id
  type                 : sync
  Return Value         : nothing
*/
extern void    PAL_SEF_StopAllSecFilterSync (HUINT8 ucDmxId);

/**
  Function Name        : PAL_SEF_StopSecFilter
  Function Description : 할당받은 filter id를 이용하다 filter를 stop한다
  Parameter            : filter id
  type                 : Async
  Return Value         : nothing
*/
extern void    PAL_SEF_StopSecFilter(HUINT16 usFilterId);		/**< usFilterId를 필터 걸 때 할당받음 */
/**
  Function Name        : PAL_SEF_StopSecFilterByUserParam
  Function Description : demux_id와 action handle (userParam)에 해당하는 모든 filter를 stop한다
  Parameter            : ucDemuxId, filter id
  type                 : Async
  Return Value         : nothing
*/
extern void    PAL_SEF_StopSecFilterByUserParam(HUINT8 ucDemuxId, HUINT32 ulUserParam);

/**
  Function Name        : PAL_SEF_SiMakeDefaultFilterParam
  Function Description : PAL_SEF_StartSecFilter()에 넘겨주는 filter parameter의 기본형을 설정하는 함수
  Parameter            : si section filter 시 사용되는 항목들
  type                 : inline
  Return Value         : nothing
*/
extern void    PAL_SEF_SiMakeDefaultFilterParam(PalSef_FilterParam_t *pSiFilterParam,
                                         HUINT8          ucTableId,
                                         HUINT8          ucDemuxId,
                                         HUINT16         usUniqueId,
                                         HUINT16         usPid,
                                         HBOOL           bIsTableFilter,
                                         HBOOL           bUseExtId,
                                         HBOOL           bUseVerNum,
                                         HBOOL           bCheckCrc,
                                         HUINT16         usExtId,
                                         HUINT8          usVerNum,
                                         PalSef_FilterMode_e  eFilterMode,
                                         HUINT32         ulTimeout,
                                         HUINT32         ulUserParam,
                                         PalSef_Callback_t    pfnParse,
                                         PalSef_Callback_t    pfnFree);

#define _____di_demux_direct_filtering__________________________________________________

/**
  @remark   PAL_SEF_StartSecFilter의 내부 함수로 pal_demux 모듈 내에서만 유효한 API이다. 실질적으로 driver interface를 호출하는 부분임. \n
            PAL_SEF_StartSecFilter보다는 이름에서 주는 의미가 애매하지만, API명을 중복을 피하기 위해 만듬
  @remark   type : Inline

  @return   ERR_OK \n
            ERR_FAIL : 필터 설정 실패
*/
extern HERROR 		PAL_SEF_EnableSectionFilter(HUINT8 	ucDemuxId,
												HUINT16	usPid,
												HUINT32 ulReceiveMsgQid,
												HUINT32 ulEmergencyStopMsgQid,
												HINT32	nSecCntInPackage,
												HUINT32	nDurationInPackage,
												HUINT8	ucFilterLen,
												HUINT8	*aucFilterValue,
												HUINT8	*aucFilterMask,
												HUINT8	*aucFilterMode,
												HUINT32	*pulRequestId,
												HBOOL	bCrcCheck);
/**
  @remark   PAL_SEF_StopSecFilter의 내부 함수로 pal_demux 모듈 내에서만 유효한 API이다. 실질적으로 driver interface를 호출하는 부분임. \n
            PAL_SEF_StopSecFilter보다는 이름에서 주는 의미가 애매하지만, API명을 중복을 피하기 위해 만듬
  @remark   type : Inline

  @return   ERR_OK \n
            ERR_FAIL : 필터 stop 실패
*/
extern HERROR 		PAL_SEF_DisableSectionFilter(HUINT8 ucDemuxId, HUINT32 ulRequestId);
// This function temporary, we have to discuss emerency stop message.
extern HUINT32		PAL_SEF_GetSiSndMsgQId(void);

#define ________________________________________________________________

#endif /* !__PAL_SEF_H__ */
