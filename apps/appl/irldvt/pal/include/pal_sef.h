/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_sef.h
	@brief	  pal section filtering module

	Description: File�� ���� ������ �����ϴ�.		\n
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
	HUINT8 ucNumLiveRecDemux;/**< live, record �� demux ���� */
	HUINT8 ucNumPbDemux;     /**< playback ���� demux ���� */
} PalSef_DemuxInfo_e;

#define _____si_filter_thread__________________________________________________

/*
	@param ulType Table id(16bit) and Result(16bit) result is PalSef_FilterResult_e.
	@param ulParam1 FilterId(16bit) and Ts Unique Id(16bit)
	@param ulParam2 SiRawData_t�� ������ ������ section raw data
	@param ulParam3 ��κ� hAction���� ���.
	@remark Callback �Լ��� Free�Լ� �ΰ����� ���ȴ�.
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
	eSiFilterMsg_Retry,						/**< ���� filter�� ��� retry �ϰų� (filter fail�� ���), monitoring�� ���� retry */
	eSiFilterMsg_Timeout,
	eSiFilterMsg_Start,
	eSiFilterMsg_Stop,
	eSiFilterMsg_StopAll,					/**< �ش� demux ���� ��� filter�� stop�Ѵ� */
	eSiFilterMsg_StopByUserParam, 			/**< ���� user param�� ���� ��� ���͸� stop, �ַ� action handle */
	eSiFilterMsg_EmergencyStop = 0x8282		/**< receie task�� msg q full���� ���, retry�ϴ� �뵵�� ��� */
} PalSef_FilterMsg_e;

/**                                                                                                                                         \n
  @brief si section filter mode                                                                                                             \n
         (1) once mode                                                                                                                      \n
             - table complete �� parse callback �� �� ���� �� filter table�� �����.                                                        \n
             - ���� filter�� stop�� �ʿ�� ����.                                                                                            \n
         (2) monitor mode                                                                                                                   \n
             - table complete �� filter table�� ��� ����������, section�� crc32�� ����� ��쿡 parse callback�� �����Ѵ�.                 \n
             - �� �̻� filter�� ������� �ʴ� ��� start filter �� �Ҵ� ���� filter id�� stop����ߵ�.                                      \n
             - monitor by repeat�� monitor by vernum�� ���� ���� ����� ������.                                                             \n
               ���� ���� ������ �ø��� ������, monitor by repeat�� ���,                                                                    \n
               sithd�� �δ��� ���� �ַ���, monitor by vernum�� ���ٶ�.                                                                    \n
               ���� �� ������ ���̴� ���� �������ٵ�, Ȥ�ö� Ʃ���� �ʿ��� ��찡 �ִٸ� (chip dependant �ϴٵ���.. ) �����ؼ� ���ٶ�   \n
         (3) continue mode                                                                                                                  \n
             - section ���� ������ parse callback ����                                                                                      \n
             - filter id�� stop �ʿ�                                                                                                        \n
*/
typedef enum
{
	eSiFilterMode_Once,
	eSiFilterMode_Monitor,	/* monitor by repeat */
	eSiFilterMode_Continue
} PalSef_FilterMode_e;

/**< si table gathering �� table ������ complete�� üũ�� �� (eit ����), one section�� ó���� ���� �˸��� flag */
typedef enum
{
	eSiCompleteMode_Table,
	eSiCompleteMode_Section
} PalSef_TableCompleteMode_e;

/**< filter timeout ó�� ��� */
typedef enum
{
	eContinueTimeout_Once = 0,	/* ���� �ѹ� �� �߻� */
	eContinueTimeout_Repeat	/* ���� �߻� �� timeout �ֱ⿡ ���� ��� �߻� */
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
	HUINT8        		ucMode;        		/**< SiFilterMode_t�� �����Ͽ� ���*/
	HINT32				nSecCntInPackage;	/**< section filter�� continue mode ��� �� �ѹ��� �޾� ���� section count */
	HUINT32				nDurationInPackage;	/**< section filter�� continue mode ��� �� timeout duration */
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

/**< section package flag, �� �޽����� ���� section�� �޾� ���� ��� ���Ǵ� ���� */
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
	HUINT32				ulUserParam;	/**< �밳�� action handle�� ������ */
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

	/* �� msg���� ����ϴ� �ʵ带 �����ϴ� ����ü */
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
	/* receive thread�� �ϴ� ���� �� ������ ����ȭ �Ǿ� �����Ƿ�, msg class�� ��� ��. */
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
  Function Description : demux���� ������ section filter�� ������ demux���� �о�´�
  type                 : inline
  Return Value         : ERR_OK - ����
  						 ERR_FAIL - ����
*/
extern HERROR PAL_SEF_GetNumOfSectionFilter(HUINT8 ucDemuxId, HINT32 *pnNumOfSecFilter);

#define _____si_filter_thread_api_______________________________________

/**
  @brief
  Function Name        : PAL_SEF_StartSecFilter
  Function Description : SI, CAS���� ����ϴ� section filter �Ŵ� �Լ�
  Parameter            : PalSef_FilterParam_t
  type                 : Async
  Return Value         : filter id - �Լ��� ä ���������� ���� si receive task���� parse callback�� ����Ǵ� ��찡 �ִ�. �̷� ������ ���ϱ� ���ؼ�, callback�� �޽����� ������ ������ �Ǿ�� �Ѵ�.
*/
extern HUINT16 PAL_SEF_StartSecFilter(PalSef_FilterParam_t *pSiFilterParam);

/**
  Function Name        : PAL_SEF_StopAllSecFilterSync
  Function Description : �Ҵ���� demux id�� �̿��Ͽ� filter�� ��� stop�Ѵ�
  Parameter            : demux id
  type                 : sync
  Return Value         : nothing
*/
extern void    PAL_SEF_StopAllSecFilterSync (HUINT8 ucDmxId);

/**
  Function Name        : PAL_SEF_StopSecFilter
  Function Description : �Ҵ���� filter id�� �̿��ϴ� filter�� stop�Ѵ�
  Parameter            : filter id
  type                 : Async
  Return Value         : nothing
*/
extern void    PAL_SEF_StopSecFilter(HUINT16 usFilterId);		/**< usFilterId�� ���� �� �� �Ҵ���� */
/**
  Function Name        : PAL_SEF_StopSecFilterByUserParam
  Function Description : demux_id�� action handle (userParam)�� �ش��ϴ� ��� filter�� stop�Ѵ�
  Parameter            : ucDemuxId, filter id
  type                 : Async
  Return Value         : nothing
*/
extern void    PAL_SEF_StopSecFilterByUserParam(HUINT8 ucDemuxId, HUINT32 ulUserParam);

/**
  Function Name        : PAL_SEF_SiMakeDefaultFilterParam
  Function Description : PAL_SEF_StartSecFilter()�� �Ѱ��ִ� filter parameter�� �⺻���� �����ϴ� �Լ�
  Parameter            : si section filter �� ���Ǵ� �׸��
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
  @remark   PAL_SEF_StartSecFilter�� ���� �Լ��� pal_demux ��� �������� ��ȿ�� API�̴�. ���������� driver interface�� ȣ���ϴ� �κ���. \n
            PAL_SEF_StartSecFilter���ٴ� �̸����� �ִ� �ǹ̰� �ָ�������, API���� �ߺ��� ���ϱ� ���� ����
  @remark   type : Inline

  @return   ERR_OK \n
            ERR_FAIL : ���� ���� ����
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
  @remark   PAL_SEF_StopSecFilter�� ���� �Լ��� pal_demux ��� �������� ��ȿ�� API�̴�. ���������� driver interface�� ȣ���ϴ� �κ���. \n
            PAL_SEF_StopSecFilter���ٴ� �̸����� �ִ� �ǹ̰� �ָ�������, API���� �ߺ��� ���ϱ� ���� ����
  @remark   type : Inline

  @return   ERR_OK \n
            ERR_FAIL : ���� stop ����
*/
extern HERROR 		PAL_SEF_DisableSectionFilter(HUINT8 ucDemuxId, HUINT32 ulRequestId);
// This function temporary, we have to discuss emerency stop message.
extern HUINT32		PAL_SEF_GetSiSndMsgQId(void);

#define ________________________________________________________________

#endif /* !__PAL_SEF_H__ */
