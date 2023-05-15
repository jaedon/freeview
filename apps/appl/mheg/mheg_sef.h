/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mheg_sef.h
	@brief	  mheg section filtering module

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: MHEG - Demux \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MHEG_SEF_H__
#define	__MHEG_SEF_H__

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
#define MHEG_SEF_MAX_FILTER_LEN 				16
#define MHEG_SEF_MAX_SI_RAW_SECTION_NUM      0x200

#ifdef CONFIG_DEBUG
//#define _MHEG_SEF_ALLOC_FREE_DEBUG_
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
} MhegSef_DemuxType_e;

typedef struct
{
	HUINT8 ucNumLiveRecDemux;/**< live, record �� demux ���� */
	HUINT8 ucNumPbDemux;     /**< playback ���� demux ���� */
} MhegSef_DemuxInfo_e;

#define _____si_filter_thread__________________________________________________

/*
	@param ulType Table id(16bit) and Result(16bit) result is MhegSef_FilterResult_e.
	@param ulParam1 FilterId(16bit) and Ts Unique Id(16bit)
	@param ulParam2 SiRawData_t�� ������ ������ section raw data
	@param ulParam3 ��κ� hAction���� ���.
	@remark Callback �Լ��� Free�Լ� �ΰ����� ���ȴ�.
*/
typedef void (*MhegSef_Callback_t)(HUINT32 ulType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

typedef enum
{
	eSefFilterLen_4Bytes = 4,
	eSefFilterLen_8Bytes = 8,
	eSefFilterLen_10Bytes = 10,
	eSefFilterLen_16Bytes = 16
} MhegSef_FilterLength_e;

typedef enum
{
	eSiFilterResult_Complete = 1,
	eSiFilterResult_Timeout,
	eSiFilterResult_Error
} MhegSef_FilterResult_e;

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
} MhegSef_FilterMsg_e;

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
} MhegSef_FilterMode_e;

/**< si table gathering �� table ������ complete�� üũ�� �� (eit ����), one section�� ó���� ���� �˸��� flag */
typedef enum
{
	eSiCompleteMode_Table,
	eSiCompleteMode_Section
} MhegSef_TableCompleteMode_e;

/**< filter timeout ó�� ��� */
typedef enum
{
	eContinueTimeout_Once = 0,	/* ���� �ѹ� �� �߻� */
	eContinueTimeout_Repeat	/* ���� �߻� �� timeout �ֱ⿡ ���� ��� �߻� */
} MhegSef_TimeOutMode_e;

typedef enum
{
	eSI_TABLE_SKIPPED = 0,
	eSI_TABLE_WAITING,
	eSI_TABLE_RECEIVED,
	eSI_TABLE_CHANGED,
	eSI_TABLE_TIMEOUT,
	eSI_TABLE_RUNING,
	eSI_TABLE_FAIL
} eFilterTableStatus_t;


/** @brief si section filter parameter */
typedef	struct
{
	HUINT8        		ucDemuxId;
	HUINT16       		usUniqueId;
	HUINT16       		usPid;
	HUINT16       		usFilterId;
	MhegSef_TableCompleteMode_e	eSiTableCompleteMode;
	MhegSef_TimeOutMode_e			eTimeoutMode;
	HUINT8        		ucMode;        		/**< SiFilterMode_t�� �����Ͽ� ���*/
	HINT32				nSecCntInPackage;	/**< section filter�� continue mode ��� �� �ѹ��� �޾� ���� section count */
	HUINT32				nDurationInPackage;	/**< section filter�� continue mode ��� �� timeout duration */
	HUINT32       		ulTimeOut;
	HUINT32       		ulUserParam;
	HBOOL         		bCheckCrc;
	MhegSef_Callback_t  		pfnParseFunc;
	MhegSef_Callback_t  		pfnFreeFunc;
	MhegSef_FilterLength_e 	eFilterLen;
	HUINT8        		pFilterValue[MHEG_SEF_MAX_FILTER_LEN];
	HUINT8        		pFilterMask[MHEG_SEF_MAX_FILTER_LEN];
} MhegSef_FilterParam_t;

typedef	struct tagSI_RETRIEVE_PARAM
{
	HUINT32		ulViewId;
	HUINT16		usTsUniqId;
	HUINT8		ucDemuxId;
	HUINT16		usPid;
	HUINT8		ucMode;
	HUINT8		ucHead;
	HUINT8		ucTimeoutMode;
	HBOOL		bTableFilter;
	HBOOL		bUseExtId;
	HBOOL		bUseVerNum;
	HBOOL		bCrc;
	HUINT16		usExtId;
	HUINT8		ucVerNum;
	HINT32		nSecCntInPackage;
	HUINT32		nDurationInPackage;
}SI_RETRIEVE_PARAM;

/** @brief si raw section structure to make a complete PSI/SI table. It could be used in si callbacks */
typedef	struct
{
	HUINT16	flag;
	HUINT16	num_sec;
	HUINT8	*buf[MHEG_SEF_MAX_SI_RAW_SECTION_NUM];
} MhegSef_RawData_t;

/**< section package flag, �� �޽����� ���� section�� �޾� ���� ��� ���Ǵ� ���� */
typedef enum
{
	eSectionPackage_Off = 0,
	eSectionPackage_On
} MhegSef_SectionPackageFlag_e;

/**<---------------------------------------------------------
	si send thread message structure
-----------------------------------------------------------*/
/**< for eSiFilterMsg_Retry */
typedef struct
{
	HINT32				nFilterIndex;
} MhegSef_RetryMsg_t;

/**< for eSiFilterMsg_Timeout */
typedef struct
{
	HUINT32				ulTimerId;
} MhegSef_TimeOutMsg_t;

/**< for eSiFilterMsg_Start */
typedef struct
{
	MhegSef_FilterParam_t		*pFilterParam;
} MhegSef_StartMsg_t;

/**< for eSiFilterMsg_Stop */
typedef struct
{
	HUINT16				usFilterid;
	HUINT16				usReserved;
} MhegSef_StoptMsg_t;

/**< for eSiFilterMsg_StopAll */
typedef struct
{
	HUINT32				ulReserved;
} MhegSef_StoptAllMsg_t;

/**< for eSiFilterMsg_StopByUserParam */
typedef struct
{
	HUINT32				ulUserParam;	/**< �밳�� action handle�� ������ */
} MhegSef_StoptByUserMsg_t;

/**< for eSiFilterMsg_EmergencyStop */
typedef struct
{
	HUINT32				ulRequestId;
} MhegSef_EmergentStopMsg_t;

typedef struct tagSefSendMessage
{
	MhegSef_FilterMsg_e				eMsgClass;
	HUINT8						ucDemuxId;

	/* �� msg���� ����ϴ� �ʵ带 �����ϴ� ����ü */
	union
	{
		MhegSef_RetryMsg_t			stRetry;
		MhegSef_TimeOutMsg_t			stTimeout;
		MhegSef_StartMsg_t			stStart;
		MhegSef_StoptMsg_t				stStop;
		MhegSef_StoptAllMsg_t			stStopAll;
		MhegSef_StoptByUserMsg_t	stStopByUserParam;
		MhegSef_EmergentStopMsg_t	stEmergencyStop;
	} msg;
} MhegSef_SendMessage_t;

/**< section receive message structure */
typedef struct tagSefRcvMessage
{
	/* receive thread�� �ϴ� ���� �� ������ ����ȭ �Ǿ� �����Ƿ�, msg class�� ��� ��. */
	HERROR						err;
	MhegSef_SectionPackageFlag_e	eSecPackageFlag;
	HUINT8						ucDemuxId;
	HUINT8						ucTableId;
	HUINT32						ulRequestId;
	HUINT32						ulDataAddr;
} MhegSef_ReceiveMessage_t;

enum
{
	PAT_PID    = 0x0000,
	CAT_PID    = 0x0001,
	TSDT_PID   = 0x0002,
	NIT_PID    = 0x0010,
	SDT_PID    = 0x0011,
	BAT_PID    = 0x0011,
	EIT_PID    = 0x0012,
	RST_PID    = 0x0013,
	TOT_PID    = 0x0014,
	TDT_PID    = 0x0014,
	DIT_PID 	= 0x001E, 					/* Japan, ARIB */
	SIT_PID		= 0x001F, 					/* Japan, ARIB */
	LOW_PROTECTION_SDTT_PID 	= 0x0023,	/* Japan, ARIB */
	BIT_PID 	= 0x0024,					/* Japan, ARIB */
	HIGH_PROTECTION_SDTT_PID	= 0x0028,	/* Japan, ARIB */
	CDT_PID = 0x0029,						/* Japan, ARIB */

//	PID_NULL   = 0x2000,
//	PID_CURRENT = 0x2001
};


#define __________________________________________________________________

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR MHEG_SEF_Init(void);
extern HERROR MHEG_SEF_GetInfo(MhegSef_DemuxInfo_e *pstDemuxInfo);
extern HERROR MHEG_SEF_GetType(HUINT8 ucDemuxId, MhegSef_DemuxType_e *pDemuxType);

/**
  @brief
  Function Name        : MHEG_SEF_GetNumOfSectionFilter
  Function Description : demux���� ������ section filter�� ������ demux���� �о�´�
  type                 : inline
  Return Value         : ERR_OK - ����
  						 ERR_FAIL - ����
*/
extern HERROR MHEG_SEF_GetNumOfSectionFilter(HUINT8 ucDemuxId, HINT32 *pnNumOfSecFilter);

#define _____si_filter_thread_api_______________________________________

/**
  @brief
  Function Name        : MHEG_SEF_StartSecFilter
  Function Description : SI, CAS���� ����ϴ� section filter �Ŵ� �Լ�
  Parameter            : MhegSef_FilterParam_t
  type                 : Async
  Return Value         : filter id - �Լ��� ä ���������� ���� si receive task���� parse callback�� ����Ǵ� ��찡 �ִ�. �̷� ������ ���ϱ� ���ؼ�, callback�� �޽����� ������ ������ �Ǿ�� �Ѵ�.
*/
extern HUINT16 MHEG_SEF_StartSecFilter(MhegSef_FilterParam_t *pSiFilterParam);

/**
  Function Name        : MHEG_SEF_StopAllSecFilterSync
  Function Description : �Ҵ���� demux id�� �̿��Ͽ� filter�� ��� stop�Ѵ�
  Parameter            : demux id
  type                 : sync
  Return Value         : nothing
*/
extern void    MHEG_SEF_StopAllSecFilterSync (HUINT8 ucDmxId);

/**
  Function Name        : MHEG_SEF_StopSecFilter
  Function Description : �Ҵ���� filter id�� �̿��ϴ� filter�� stop�Ѵ�
  Parameter            : filter id
  type                 : Async
  Return Value         : nothing
*/
extern void    MHEG_SEF_StopSecFilter(HUINT16 usFilterId);		/**< usFilterId�� ���� �� �� �Ҵ���� */
/**
  Function Name        : MHEG_SEF_StopSecFilterByUserParam
  Function Description : demux_id�� action handle (userParam)�� �ش��ϴ� ��� filter�� stop�Ѵ�
  Parameter            : ucDemuxId, filter id
  type                 : Async
  Return Value         : nothing
*/
extern void    MHEG_SEF_StopSecFilterByUserParam(HUINT8 ucDemuxId, HUINT32 ulUserParam);

/**
  Function Name        : MHEG_SEF_SiMakeDefaultFilterParam
  Function Description : MHEG_SEF_StartSecFilter()�� �Ѱ��ִ� filter parameter�� �⺻���� �����ϴ� �Լ�
  Parameter            : si section filter �� ���Ǵ� �׸��
  type                 : inline
  Return Value         : nothing
*/
extern void    MHEG_SEF_SiMakeDefaultFilterParam(MhegSef_FilterParam_t *pSiFilterParam,
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
                                         MhegSef_FilterMode_e  eFilterMode,
                                         HUINT32         ulTimeout,
                                         HUINT32         ulUserParam,
                                         MhegSef_Callback_t    pfnParse,
                                         MhegSef_Callback_t    pfnFree);

#define _____di_demux_direct_filtering__________________________________________________

/**
  @remark   MHEG_SEF_StartSecFilter�� ���� �Լ��� MHEG_demux ��� �������� ��ȿ�� API�̴�. ���������� driver interface�� ȣ���ϴ� �κ���. \n
            MHEG_SEF_StartSecFilter���ٴ� �̸����� �ִ� �ǹ̰� �ָ�������, API���� �ߺ��� ���ϱ� ���� ����
  @remark   type : Inline

  @return   ERR_OK \n
            ERR_FAIL : ���� ���� ����
*/
extern HERROR 		MHEG_SEF_EnableSectionFilter(HUINT8 	ucDemuxId,
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
  @remark   MHEG_SEF_StopSecFilter�� ���� �Լ��� MHEG_demux ��� �������� ��ȿ�� API�̴�. ���������� driver interface�� ȣ���ϴ� �κ���. \n
            MHEG_SEF_StopSecFilter���ٴ� �̸����� �ִ� �ǹ̰� �ָ�������, API���� �ߺ��� ���ϱ� ���� ����
  @remark   type : Inline

  @return   ERR_OK \n
            ERR_FAIL : ���� stop ����
*/
extern HERROR 		MHEG_SEF_DisableSectionFilter(HUINT8 ucDemuxId, HUINT32 ulRequestId);

extern HERROR		MHEG_SEF_SetDefaultRetrieveParam(SI_RETRIEVE_PARAM *pRetrieveParam,
														HUINT32 hAction,
														HUINT16 usTsUniqId,
														HUINT32 ucDemuxId);

extern HERROR		MHEG_SEF_RetrievePatSection (SI_RETRIEVE_PARAM *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId);

extern HERROR		MHEG_SEF_RetrievePmtSection (SI_RETRIEVE_PARAM *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId);

// This function temporary, we have to discuss emerency stop message.
extern HUINT32		MHEG_SEF_GetSiSndMsgQId(void);

#define ________________________________________________________________

#endif /* !__MHEG_SEF_H__ */
