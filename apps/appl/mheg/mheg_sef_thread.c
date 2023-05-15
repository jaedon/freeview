/**
	@file     mheg_sef_thread.c
	@brief    si filter thread

	Description: si table, section ������ filtering�� �����ϸ�, driver���� ���� ������ section filtering�� �Ѵ�. \n
	Module: PAL-SEF \n
	Remarks : octo base (this module is from LIME/LIME+) \n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

// last revision of pal_sef_thread.c : 108371

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<stdlib.h>
#include	<string.h>

#include	<vkernel.h>

#include 	<mheg_sef.h>
#include 	"_mheg_sef.h"
#include 	"mheg_si.h"
#include	"mheg_int.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#if defined(CONFIG_DEBUG)
//#define _SITHD_SEM_DEBUG_
//#define _SITHD_CHECK_PARSE_CB_
//#define _SITHD_TIMER_ERR_DEBUG_
#endif
#ifdef _SITHD_SEM_DEBUG_
#define	mhegSef_ENTER_SITHD_CRITICAL	{ HxLOG_Print(C_CODE_F_PURPLE), HxLOG_Print("++++ mhegSef_ENTER_SITHD_CRITICAL(%s:%d)\n", __FUNCTION__, __LINE__), HxLOG_Print(C_CODE_RESET); VK_SEM_Get(s_ulSithdSemId);}
#define	mhegSef_LEAVE_SITHD_CRITICAL	{ VK_SEM_Release(s_ulSithdSemId); HxLOG_Print(C_CODE_F_PURPLE), HxLOG_Print("---- mhegSef_LEAVE_SITHD_CRITICAL(%d)"C_CODE_RESET"\n\n", __LINE__); }
#else
#define mhegSef_ENTER_SITHD_CRITICAL	VK_SEM_Get(s_ulSithdSemId)
#define mhegSef_LEAVE_SITHD_CRITICAL	VK_SEM_Release(s_ulSithdSemId)
#endif
#ifdef _SITHD_CHECK_PARSE_CB_
#define	mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, tid, fid, uniqid)	HxLOG_Print(C_CODE_F_YELLOW), 									\
								HxLOG_Print("\t\t++ mhegSef_ENTER_PARSE_CB(%d) %s tid=%02X, fid=%04X, uniqid=%04X\n", __LINE__, 	\
								(ulTblIdAndResult&0xffff == eSiFilterResult_Complete) ?	"complete": 				\
								(ulTblIdAndResult&0xffff == eSiFilterResult_Timeout) ? "timeout":"error" 			\
								, tid, fid, uniqid), HxLOG_Print(C_CODE_RESET)
#define	mhegSef_LEAVE_PARSE_CB			HxLOG_Print(C_CODE_F_YELLOW), HxLOG_Print("\t\t-- mhegSef_LEAVE_PARSE_CB(%d)"C_CODE_RESET"\n", __LINE__)
#else
#define mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, tid, fid, uniqid)
#define mhegSef_LEAVE_PARSE_CB
#endif

#define mhegSef_SET_BIT(p, nPos)		(p[(nPos>>3)]  |=(0x80 >>(nPos&7)))
#define mhegSef_CLEAR_BIT(p, nPos)	(p[(nPos>>3)] &=(~(0x80 >>(nPos&7))))
#define mhegSef_TEST_BIT(p, nPos)	(p[(nPos>>3)] &(0x80 >>(nPos&7)))

#define mhegSEF_CHANGE_TABLE_WAITING(a)					    \
			{													\
				if(a & eFilterTable_Running)					\
					a = (a << 4);								\
			}
#define mhegSef_CHANGE_TABLE_RUNNING(a)					    \
			{													\
				if(a & eFilterTable_Waiting)					\
					a = (a >> 4);								\
			}
#ifdef _SITHD_TIMER_ERR_DEBUG_
#define mhegSef_CANCEL_TIMER(tid)							\
			{													\
				if(tid)											\
				{ 												\
					HINT32	res;								\
					res = VK_TIMER_Cancel(tid); 				\
					if(res)										\
						HxLOG_Print("\t[%s:%d] timer error\n", __FUNCTION__, __LINE__);	\
					tid = 0; 									\
				}												\
			}
#else
#define mhegSef_CANCEL_TIMER(tid)							\
			{													\
				if(tid) { VK_TIMER_Cancel(tid); tid = 0; }		\
			}
#endif
#define mhegSef_SET_TIMER_ONCE(ulMsTimeout, fn, pvArgs, ulArgSize, ulTimerId)		\
{																						\
	mhegSef_CANCEL_TIMER(ulTimerId);												\
	VK_TIMER_EventAfter(ulMsTimeout, fn, (void *)pvArgs, ulArgSize, &ulTimerId);		\
}
#define mhegSef_SET_TIMER_REPEAT(ulMsTimeout, fn, pvArgs, ulArgSize, ulTimerId)	    \
{																						\
	mhegSef_CANCEL_TIMER(ulTimerId);												\
	VK_TIMER_EventEvery(ulMsTimeout, fn, (void *)pvArgs, ulArgSize, &ulTimerId);		\
}

#define mhegSef_SI_GetFilterMode(ucResult, ucSrc)               \
			{                                                       \
				if(ucSrc == eSiFilterMode_Once)                    \
					ucResult = eFilterTable_Once;                 \
				else if(ucSrc == eSiFilterMode_Monitor)            \
					ucResult = eFilterTable_MonitorByVerNum;      \
				else if(ucSrc == eSiFilterMode_Continue)           \
					ucResult = eFilterTable_Continue;             \
				else                                                \
					ucResult = eFilterTable_MonitorByRepeat;      \
			}

#define mhegSef_MAKE_TABLEID_AND_RESULT(ulTableIdAndResult, ucTableId, eSiFilterResult)	\
{																							\
	ulTableIdAndResult = ((0xff & ucTableId) << 16) | (0xffff & eSiFilterResult);			\
}


#define mhegSef_MAX_SITABLE_NUM					100

#define mhegSef_COMPLETE_COLLECT_SECTION			0
#define mhegSef_INSUFFIECIENT_SECTION			1

#define mhegSef_FILTERFAIL_RETRY_TIMEOUT         500
#define mhegSef_CHANGE_RETRY_TIMEOUT             2000
#define mhegSef_QFULL_RETRY_TIMEOUT              4000

#define mhegSef_INVALID_LOADID                   0xffff

#define MHEG_DMX_SND_MSGQ_SIZE					512
#define MHEG_DMX_RCV_MSGQ_SIZE					512
#define MHEG_DMX_SND_TASK_PRIORITY				(VK_TASK_PRIORITY_MW_REF+1)
#define MHEG_DMX_RCV_TASK_PRIORITY				(VK_TASK_PRIORITY_MW_REF+1)
#define MHEG_DMX_SND_TASK_STACK_SIZE			0x00010000	//64K
#define MHEG_DMX_RCV_TASK_STACK_SIZE			0x00010000	//64k

/*====================================================================================
	demux err�� ��� sithd ��ü������ ó���ϰ��ϴ� ���
====================================================================================*/
#if 0
#define S_SEF_RETRY_ON_DI_ERR
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/**< si filter table�� state */
typedef enum tagSiFilterTableMode
{
	eFilterTable_Unused					= 0x00,

	/* filter �ɷ� �ִ� ��� */
	eFilterTable_Once					= 0x01,
	eFilterTable_MonitorByRepeat		= 0x02,
	eFilterTable_MonitorByVerNum		= 0x04,	/* SITABLE_MONITOR_BY_REPEAT�� �ֱ������� ���͸� �Ŵ� ����̶��, SITABLE_MONITOR_BY_VERNUM�� section�� version number�� Ȱ���ϴ� ���� */
	eFilterTable_Continue				= 0x08,
	eFilterTable_Running				= 0x0f,

	/* timer ��Ѹ��� ��� */
	eFilterTable_Wait_Once				= 0x10,
	eFilterTable_Wait_MonitorByRepeat	= 0x20,
	eFilterTable_Wait_MonitorByVerNum	= 0x40,
	eFilterTable_Wait_Continue			= 0x80,
	eFilterTable_Waiting				= 0xf0
} mhegSef_FilterTableMode_t;

/**< �ߺ����͸� �����ϱ� ���� ����ü */
typedef	struct
{
	HUINT8				ucSlot;
	HUINT8				ucFilterMode;
	HUINT16				usFilterId;
	HUINT32				ulTimeOut;		// in msec.
	HUINT32				ulUserParam;
	HUINT32				ulSended;
	MhegSef_Callback_t	pfnParseFunc;
} mhegSef_TagData_t;

/**< demux�� �ɷ� �ִ� section filter ������ ���� ����ü. �Ʒ� filter table�� ����������� table mode�� eFilterTable_Unused�̳� �ƴϳ��� ������ */
typedef	struct
{
	mhegSef_FilterTableMode_t	eCurFilterTableMode;	/**< filter mode & state - eFilterTable_Unused,eFilterTable_Once,eFilterTable_MonitorByRepeat,SITABLE_SEARCH,eFilterTable_Running,eFilterTable_Wait_Once,eFilterTable_MonitorByRepeat,eFilterTable_Wait_Continue,SITABLE_WAIT_SEARCH,eFilterTable_Waiting */
	MhegSef_TableCompleteMode_e	eSiTableCompleteMode;	/**< table ������ ���� ���� section ������ ���� ��� �� ������ �ִ� */

	HUINT8						ucDemuxId;				/**< filter start �� �Ѱܹ��� demux id */
	HUINT16						usUniqueId;				/**< tsinfo�� unique_id�� ����. tp������ si�� ������ ��찡 �����Ƿ�, parse callback�� parameter�� Ȱ��ȴ�. */
	HUINT16						usPid;					/**< pid of section */
	HUINT16						usExtId;				/**< table id extension - svc id, network id, bouquet id, ... */
	HBOOL						bCheckCrc;				/**< section handler (driver)���� crc üũ�� �ϰ� ������ TRUE�� �Ǿ�� ��, filter param���� �Ѿ�� */
	HUINT32						ulCrc;					/**< section number 0�� section�� crc32 �ʵ� ��, monitor mode���� ���ο� section�� ���Դ��� üũ�ϱ� ���� */

	HUINT32						ulLoadId;				/**< filter start �� �Ҵ�޴� request id, ����ϰ� �ִٰ� filter stop �� ��� */
	HULONG						ulRetryTimerId;			/**< fiter fail �� retry timer ����� ���� timer id */
	HULONG						ulTimeoutTimerId;		/**< timeout ���� ó���� ���� timer id */
	MhegSef_TimeOutMode_e		eTimeoutMode;			/**< timeout ó�� ���� once�� repeat�� �ִ�. continue mode���� Ȱ��� */

	MhegSef_Callback_t			pfnFreeSectionFunc;		/**< free callback, filter stop�� �ҷ�����. */
	HUINT8						aucSectionRecv[32];		/**< table complete check�� ���� array, 256 bits */
	HUINT8						aucSectionTotal[32];	/**< table complete check�� ���� array, 256 bits */
	MhegSef_FilterLength_e		eFilterLen;				/**< section filter length�� 8, 16 ����Ʈ�� �����Ѵ�. ���� ����̹��� 8, 16����Ʈ ������ */
	HUINT8						aucFilterValue[MHEG_SEF_MAX_FILTER_LEN];	/**< section filter value */
	HUINT8						aucFilterMask[MHEG_SEF_MAX_FILTER_LEN];	/**< section filter mask */

	HINT32						nSecCntInPackage;		/**< ���� section�� �ϳ��� �޽����� ������ ���ؼ� ���. 2�̻��� ���� �ǹ� ����. continue mode�� section filter�� ����ϴ� ���, demux q_full ���� ���ɹ����� ���� ��� ��� */
	HUINT32						nDurationInPackage;
	MhegSef_RawData_t			stRawData;				/**< section�� ������ ����ü */

	mhegSef_TagData_t			stTagData;
} mhegSef_FilterTable_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG				 s_ulSiSendMsgQId, s_ulSiRcvMsgQId;
STATIC HULONG				 s_ulSithdSemId, s_ulFilterIdSemId;
STATIC HULONG				 s_ulSiSendTaskId, s_ulSiRcvTaskId;
STATIC HUINT8				 s_aucFilterMode[MHEG_SEF_MAX_FILTER_LEN];
STATIC HUINT8				 s_ucDemuxNum;
STATIC mhegSef_FilterTable_t	*s_pstFilterTable;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


/********************************************************************
*	static functions Prototypes
********************************************************************/
STATIC void     mheg_sef_ThreadSendTask(void *pvParam);
STATIC void     mheg_sef_ThreadReceiveTask(void *pvParam);
STATIC HERROR 	mheg_sef_ThreadProcessSectionPackage(mhegSef_FilterTable_t *pstFilterTable, MhegSef_RawData_t *pstRawData);
STATIC HERROR 	mheg_sef_ThreadProcessOneSection(mhegSef_FilterTable_t *pstFilterTable, HUINT8 *pucRawSec);
STATIC void     mheg_sef_ThreadStopFilterForcingly(HINT32 nTableNum);
STATIC void     mheg_sef_ThreadFreeSiRawData(MhegSef_RawData_t *pstRawData);
STATIC HUINT32  mheg_sef_ThreadAddTagToSiTableData(mhegSef_FilterTable_t *psmhegSef_FilterTable_t, MhegSef_FilterParam_t *pstSiParam, HUINT32 ulSended);
STATIC HUINT32  mheg_sef_ThreadFindEmptyTable(void);
STATIC HUINT32  mheg_sef_ThreadFindTableByLoadId(HUINT32 ulLoadId, HINT32 *pnIndex);
STATIC HUINT32  mheg_sef_ThreadFindTableByRetryTimerId(HUINT32 ulTimerId, HINT32 *pnIndex);
STATIC HUINT32  mheg_sef_ThreadFindSiTableByFilterId(HUINT16 usFilterId, HINT32 *pnIndex);
STATIC void     mheg_sef_ThreadMakeUnusedTable(mhegSef_FilterTable_t *pstFilterTable);
STATIC HUINT32  mheg_sef_ThreadMakeTable(mhegSef_FilterTable_t *pstFilterTable, HUINT8 *pucRawData);
STATIC HUINT16  mheg_sef_ThreadSiGetUniqueFilterId(void);
STATIC HUINT32  mheg_sef_ThreadStartFilter(mhegSef_FilterTable_t *pstFilterTable, HUINT8 ucDemuxId, HUINT16 usPid, HUINT8 ucRecvMode, HUINT32 ulTimeOut, HBOOL bCrc);
STATIC HUINT32  mheg_sef_ThreadRestartFilter(mhegSef_FilterTable_t *pstFilterTable);
STATIC HUINT32  mheg_sef_ThreadStopFilter(HUINT16 usFilterId);
STATIC void     mheg_sef_ThreadSendFilterFail(mhegSef_FilterTable_t *pstFilterTable);
STATIC void     mheg_sef_ThreadFilterFailNotifier(HULONG ulTimerId, void *pvLocalData);
STATIC void		mheg_sef_ThreadTimeoutNotifier(HULONG ulTimerId, void *pvLocalData);
STATIC void 	mheg_sef_ThreadFilterMsgStopAll(MhegSef_SendMessage_t *pstInMsg);

STATIC void     mheg_sef_ThreadPrintFilterTable(HINT32 nPrintLevel, HUINT8 ucTableIndex, mhegSef_FilterTable_t *pstFilterTable);

#define ____________________________________________________

void MHEG_SEF_SiMakeDefaultFilterParam(MhegSef_FilterParam_t		*pstSiFilterParam,
											HUINT8				 ucTableId,
											HUINT8				 ucDemuxId,
											HUINT16				 usUniqueId,
											HUINT16				 usPid,
											HBOOL				 bIsTableFilter,
											HBOOL				 bUseExtId,
											HBOOL				 bUseVerNum,
											HBOOL				 bCheckCrc,
											HUINT16				 usExtId,
											HUINT8				 ucVerNum,
											MhegSef_FilterMode_e	 eFilterMode,
											HUINT32				 ulTimeout,
											HUINT32				 ulUserParam,
											MhegSef_Callback_t	 pfnParse,
											MhegSef_Callback_t	 pfnFree)
{
	HxSTD_memset(pstSiFilterParam, 0, sizeof(MhegSef_FilterParam_t));

	pstSiFilterParam->ucDemuxId     = ucDemuxId;
	pstSiFilterParam->usUniqueId    = usUniqueId;
	pstSiFilterParam->usPid         = usPid;
	mhegSef_SI_GetFilterMode(pstSiFilterParam->ucMode, eFilterMode);
	pstSiFilterParam->eSiTableCompleteMode = (bIsTableFilter == TRUE) ? eSiCompleteMode_Table : eSiCompleteMode_Section;
	pstSiFilterParam->eTimeoutMode  = eContinueTimeout_Once;
	pstSiFilterParam->pfnParseFunc  = pfnParse;
	pstSiFilterParam->pfnFreeFunc   = pfnFree;
	pstSiFilterParam->ulTimeOut     = ulTimeout;
	pstSiFilterParam->bCheckCrc     = bCheckCrc;
	pstSiFilterParam->ulUserParam   = ulUserParam;

	pstSiFilterParam->eFilterLen    = eSefFilterLen_4Bytes;
	/* pstSiFilterParam->nSecCntInPackage �� ���� ����ϱ� ���ϴ� ���, ���� ������ �ʿ��� */

	/* if you want to change a filter mask value, you have to set the value manually */
	pstSiFilterParam->pFilterValue[0] = ucTableId;
	pstSiFilterParam->pFilterMask[0]  = 0xff;
	if(bUseExtId == TRUE)
	{
		pstSiFilterParam->pFilterValue[1] = (HUINT8)((usExtId >> 8) & 0xff);
		pstSiFilterParam->pFilterValue[2] = (HUINT8)(usExtId & 0xff);
		pstSiFilterParam->pFilterMask[1] = 0xff;
		pstSiFilterParam->pFilterMask[2] = 0xff;
	}
	else
	{
		pstSiFilterParam->pFilterMask[1] = 0;
		pstSiFilterParam->pFilterMask[2] = 0;
	}

	//pstSiFilterParam->ucVerNum = ucVerNum;
	if(bUseVerNum == TRUE)
	{
		pstSiFilterParam->pFilterMask[3] = 0x3E;
		pstSiFilterParam->pFilterValue[3] = (ucVerNum & 0x1f) << 1;
	}
	else
	{
		pstSiFilterParam->pFilterMask[3] = 0;
	}
}

/**************************************************************************
	si section ���� ����
	 - PAT, PMT, NIT, SDT, BAT, TDT, TOT, ...
	input  : filter parameter structure (which is already memory allocated)
	         filter parameter is mem-free on si send task.
	output : filter id
**************************************************************************/
HUINT16 MHEG_SEF_StartSecFilter(MhegSef_FilterParam_t *pstSiFilterParam)
{
	MhegSef_SendMessage_t	 stMsg;
	MhegSef_FilterParam_t	*pstNewSiFilterParam;

	pstSiFilterParam->usFilterId = mheg_sef_ThreadSiGetUniqueFilterId();
	pstNewSiFilterParam = (MhegSef_FilterParam_t*)HLIB_STD_MemAlloc(sizeof(MhegSef_FilterParam_t));
	HxSTD_memcpy(pstNewSiFilterParam, pstSiFilterParam, sizeof(MhegSef_FilterParam_t));

	stMsg.eMsgClass = eSiFilterMsg_Start;
	stMsg.ucDemuxId = pstNewSiFilterParam->ucDemuxId;
	stMsg.msg.stStart.pFilterParam = pstNewSiFilterParam;

	VK_MSG_Send(s_ulSiSendMsgQId,(void *)&stMsg, sizeof(MhegSef_SendMessage_t));

    return pstSiFilterParam->usFilterId;
}

HUINT32 MHEG_SEF_GetSiSndMsgQId(void)
{
	return s_ulSiSendMsgQId;
}

void MHEG_SEF_StopAllSecFilterSync (HUINT8 ucDmxId)
{
	MhegSef_SendMessage_t	stMsg;

	HxSTD_memset(&stMsg, 0, sizeof(MhegSef_SendMessage_t));

	stMsg.eMsgClass = eSiFilterMsg_StopAll;
	stMsg.ucDemuxId = ucDmxId;

	mhegSef_ENTER_SITHD_CRITICAL;
	mheg_sef_ThreadFilterMsgStopAll (&stMsg);
	mhegSef_LEAVE_SITHD_CRITICAL;
}

void MHEG_SEF_StopSecFilter(HUINT16 usFilterId)
{
	MhegSef_SendMessage_t	stMsg;

	HxSTD_memset(&stMsg, 0, sizeof(MhegSef_SendMessage_t));
	stMsg.eMsgClass = eSiFilterMsg_Stop;
	stMsg.msg.stStop.usFilterid = usFilterId;
	VK_MSG_Send(s_ulSiSendMsgQId, (void*)&stMsg, sizeof(MhegSef_SendMessage_t));
}

/* ulUserParam�� action handle�� �� ���� �ִ� */
void MHEG_SEF_StopSecFilterByUserParam(HUINT8 ucDemuxId, HUINT32 ulUserParam)
{
	MhegSef_SendMessage_t	stOutMsg;

	HxSTD_memset(&stOutMsg, 0, sizeof(MhegSef_SendMessage_t));
	stOutMsg.eMsgClass = eSiFilterMsg_StopByUserParam;
	stOutMsg.ucDemuxId = ucDemuxId;
	stOutMsg.msg.stStopByUserParam.ulUserParam = ulUserParam;

	VK_MSG_Send(s_ulSiSendMsgQId, (void*)&stOutMsg, sizeof(MhegSef_SendMessage_t));
}

#define _____mheg_sef_internal_functions_____________________________________

/********************************************************************
	SI���� �ʱ�ȭ.
********************************************************************/
HERROR mheg_sef_SiCreateFilterThread(void)
{
	HINT32		ret;

	mheg_sef_GetDemuxNum(&s_ucDemuxNum);

	s_pstFilterTable = (mhegSef_FilterTable_t*)HLIB_STD_MemAlloc(sizeof(mhegSef_FilterTable_t) * mhegSef_MAX_SITABLE_NUM);
	HxSTD_memset(s_pstFilterTable, 0, sizeof(mhegSef_FilterTable_t) * mhegSef_MAX_SITABLE_NUM);
	HxSTD_memset(s_aucFilterMode, 0, MHEG_SEF_MAX_FILTER_LEN);

	ret = VK_MSG_Create(MHEG_DMX_SND_MSGQ_SIZE, sizeof(MhegSef_SendMessage_t), "SiSS", &s_ulSiSendMsgQId, VK_SUSPENDTYPE_FIFO);
	if(ret != ERR_OK)
		goto err_blk;
	ret = VK_MSG_Create(MHEG_DMX_RCV_MSGQ_SIZE, sizeof(MhegSef_ReceiveMessage_t), "SiRS", &s_ulSiRcvMsgQId, VK_SUSPENDTYPE_FIFO);
	if(ret != ERR_OK)
		goto err_blk;
	ret = VK_SEM_Create(&s_ulSithdSemId, "SiTh", VK_SUSPENDTYPE_PRIORITY);
	if(ret != ERR_OK)
		goto err_blk;

	ret = VK_SEM_Create(&s_ulFilterIdSemId, "fidS", VK_SUSPENDTYPE_PRIORITY);
	if(ret != ERR_OK)
		goto err_blk;

	ret = VK_TASK_Create(mheg_sef_ThreadSendTask, MHEG_DMX_SND_TASK_PRIORITY, MHEG_DMX_SND_TASK_STACK_SIZE, "SiST", NULL, &s_ulSiSendTaskId, 0);
	if(ret != ERR_OK)
		goto err_blk;
	ret = VK_TASK_Create(mheg_sef_ThreadReceiveTask, MHEG_DMX_RCV_TASK_PRIORITY, MHEG_DMX_RCV_TASK_STACK_SIZE, "SiRT", NULL, &s_ulSiRcvTaskId, 0);
	if(ret != ERR_OK)
		goto err_blk;

	VK_TASK_Start(s_ulSiSendTaskId);
	VK_TASK_Start(s_ulSiRcvTaskId);

	return ERR_OK;

err_blk:
	HxLOG_Critical("\n\n");
	HxLOG_Error("Critical error....(%d)\n", ret);
	return ERR_FAIL;
}

/********************************************************************
	�ش� Demux Id�� �ɷ� �ִ� ��� filter�� ������.
********************************************************************/
HUINT32	mheg_sef_SiRequestFilterStopByDemux(HUINT8 ucDemuxId)
{
	MhegSef_SendMessage_t	stOutMsg;

	HxSTD_memset(&stOutMsg, 0, sizeof(MhegSef_SendMessage_t));
	stOutMsg.eMsgClass = eSiFilterMsg_StopAll;
	stOutMsg.ucDemuxId = ucDemuxId;

	VK_MSG_Send(s_ulSiSendMsgQId, (void*)&stOutMsg, sizeof(MhegSef_SendMessage_t));

	return ERR_OK;
}

#define _____local_functions_________________________________________________

#define _____send_msg_proc_functions_________________________

/**< eSiFilterMsg_Retry */
STATIC void mheg_sef_ThreadFilterMsgRetry(MhegSef_SendMessage_t *pstInMsg)
{
	mhegSef_FilterTable_t		*pstFilterTable;
	HINT32				nFilterIndex = pstInMsg->msg.stRetry.nFilterIndex;

	if(nFilterIndex >= mhegSef_MAX_SITABLE_NUM)
	{
		HxLOG_Error("\t[eSiFilterMsg_Retry] invalid filter table index(%d)\n", nFilterIndex);
		return;
	}

	pstFilterTable = &s_pstFilterTable[nFilterIndex];
	HxLOG_Info("\t[eSiFilterMsg_Retry] table_idx:%d, cur_mode:0x%02x, pid:0x%x)\n", nFilterIndex, pstFilterTable->eCurFilterTableMode, pstFilterTable->usPid);
	HxLOG_Print("\tFILTER [%02X %02X %02X %02X %02X %02X %02X %02X]\n", pstFilterTable->aucFilterValue[0], pstFilterTable->aucFilterValue[1], pstFilterTable->aucFilterValue[2], pstFilterTable->aucFilterValue[3], pstFilterTable->aucFilterValue[4], pstFilterTable->aucFilterValue[5], pstFilterTable->aucFilterValue[6], pstFilterTable->aucFilterValue[7]);
	HxLOG_Print("\tMASK   [%02X %02X %02X %02X %02X %02X %02X %02X]\n", pstFilterTable->aucFilterMask[0], pstFilterTable->aucFilterMask[1], pstFilterTable->aucFilterMask[2], pstFilterTable->aucFilterMask[3], pstFilterTable->aucFilterMask[4], pstFilterTable->aucFilterMask[5], pstFilterTable->aucFilterMask[6], pstFilterTable->aucFilterMask[7]);

	if((pstFilterTable->eCurFilterTableMode & eFilterTable_Waiting) && (PID_NULL != pstFilterTable->usPid))
	{
		if(mheg_sef_ThreadRestartFilter(pstFilterTable) == ERR_FAIL)
		{
#if defined(S_SEF_RETRY_ON_DI_ERR)
			mhegSEF_CHANGE_TABLE_WAITING(pstFilterTable->eCurFilterTableMode);
			mhegSef_SET_TIMER_ONCE(mhegSef_FILTERFAIL_RETRY_TIMEOUT, mheg_sef_ThreadFilterFailNotifier, NULL, 0, pstFilterTable->ulRetryTimerId);
#else
			/* ������ dmx err�� ��� sithd ��ü������ retry�� ������ octo������ error�� �ְ�, ����ó���� �����(���� �ǳ�)���� �ѱ�� - parse callback�� error ó�� �߰��ʿ� */
			mheg_sef_ThreadSendFilterFail(pstFilterTable);
			mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);
			HxSTD_memset(pstFilterTable, 0, sizeof(mhegSef_FilterTable_t));
			mheg_sef_ThreadMakeUnusedTable(pstFilterTable);
#endif
		}
		else
		{
			mhegSef_CHANGE_TABLE_RUNNING(pstFilterTable->eCurFilterTableMode);
		}
	}
}

/**< eSiFilterMsg_Timeout */
STATIC void mheg_sef_ThreadFilterMsgTimeout(MhegSef_SendMessage_t *pstInMsg)
{
	HINT32				i;
	HUINT32				ulTimerId, ulTblIdAndResult = 0;
	mhegSef_FilterTable_t		*pstFilterTable;

	ulTimerId = pstInMsg->msg.stTimeout.ulTimerId;

	for(i = 0; i < mhegSef_MAX_SITABLE_NUM; i++)
	{
		pstFilterTable = &s_pstFilterTable[i];
		if(0 == (pstFilterTable->eCurFilterTableMode & eFilterTable_Running))
			continue;

		if(ulTimerId != pstFilterTable->ulTimeoutTimerId)
			continue;

		/* timer once�� �����ϱ�, timer id�� 0���� ������ش�. */
		pstFilterTable->ulTimeoutTimerId = 0;

		if(pstFilterTable->stTagData.pfnParseFunc)
		{
			if(pstFilterTable->eCurFilterTableMode == eFilterTable_MonitorByVerNum)
			{
				/* ����� version�� section�� �ö� ���� �ʴ� ���, timeout action�� �������� �ʴ´�. */
				if(pstFilterTable->aucFilterMask[3] == 0)
				{ /* NO_VER_CHANGE TIMEOUT */
					mhegSef_MAKE_TABLEID_AND_RESULT(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], eSiFilterResult_Timeout);
					mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], pstFilterTable->stTagData.usFilterId, pstFilterTable->usUniqueId);
					pstFilterTable->stTagData.pfnParseFunc(ulTblIdAndResult, (HUINT32)((pstFilterTable->stTagData.usFilterId << 16) | pstFilterTable->usUniqueId), 0, pstFilterTable->stTagData.ulUserParam);
					mhegSef_LEAVE_PARSE_CB;
					pstFilterTable->stTagData.ulSended = 0;		// TimeOut���Ŀ� ���� �ٽ� �÷��ֽ� ���ؼ�......
				}
				else
				{ /* VER_CHANGE TIMEOUT */
				}
			}
			else
			{
				mhegSef_MAKE_TABLEID_AND_RESULT(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], eSiFilterResult_Timeout);
				mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], pstFilterTable->stTagData.usFilterId, pstFilterTable->usUniqueId);
				pstFilterTable->stTagData.pfnParseFunc(ulTblIdAndResult, (HUINT32)((pstFilterTable->stTagData.usFilterId << 16) | pstFilterTable->usUniqueId), 0, pstFilterTable->stTagData.ulUserParam);
				mhegSef_LEAVE_PARSE_CB;
				pstFilterTable->stTagData.ulSended = 0;		// TimeOut���Ŀ� ���� �ٽ� �÷��ֽ� ���ؼ�......
			}

			switch(pstFilterTable->eCurFilterTableMode)
			{
				case eFilterTable_Once:
					mheg_sef_ThreadStopFilter(pstFilterTable->stTagData.usFilterId);
					break;

				case eFilterTable_MonitorByRepeat:
					pstFilterTable->ulCrc = 0;
					MHEG_SEF_DisableSectionFilter(pstFilterTable->ucDemuxId, pstFilterTable->ulLoadId);
					pstFilterTable->ulLoadId = mhegSef_INVALID_LOADID;
					mhegSEF_CHANGE_TABLE_WAITING(pstFilterTable->eCurFilterTableMode);
					mhegSef_SET_TIMER_ONCE(mhegSef_CHANGE_RETRY_TIMEOUT, mheg_sef_ThreadFilterFailNotifier, NULL, 0, pstFilterTable->ulRetryTimerId);
					break;

				case eFilterTable_MonitorByVerNum:
					/* version_number�� ����� ��� ����� �ٽ� �Ǵ�. */
					if(pstFilterTable->aucFilterMask[3] == 0x3E)
					{
						MHEG_SEF_DisableSectionFilter(pstFilterTable->ucDemuxId, pstFilterTable->ulLoadId);
						pstFilterTable->ulLoadId = mhegSef_INVALID_LOADID;

						pstFilterTable->aucFilterValue[3] = 0;
						pstFilterTable->aucFilterMask[3] = 0;
						if(mheg_sef_ThreadRestartFilter(pstFilterTable) == ERR_FAIL)
						{
#if defined(S_SEF_RETRY_ON_DI_ERR)
							mhegSEF_CHANGE_TABLE_WAITING(pstFilterTable->eCurFilterTableMode);
							mhegSef_SET_TIMER_ONCE(mhegSef_FILTERFAIL_RETRY_TIMEOUT, mheg_sef_ThreadFilterFailNotifier, NULL, 0, pstFilterTable->ulRetryTimerId);
#else
							/* ������ dmx err�� ��� sithd ��ü������ retry�� ������ octo������ error�� �ְ�, ����ó���� �����(���� �ǳ�)���� �ѱ�� - parse callback�� error ó�� �߰��ʿ� */
							mhegSef_MAKE_TABLEID_AND_RESULT(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], eSiFilterResult_Error);
							mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], pstFilterTable->stTagData.usFilterId, pstFilterTable->usUniqueId);
							pstFilterTable->stTagData.pfnParseFunc(ulTblIdAndResult, (HUINT32)((pstFilterTable->stTagData.usFilterId << 16) | pstFilterTable->usUniqueId), 0, pstFilterTable->stTagData.ulUserParam);
							mhegSef_LEAVE_PARSE_CB;
							mheg_sef_ThreadStopFilter(pstFilterTable->stTagData.usFilterId);
#endif
						}
					}
					else
					{
						pstFilterTable->ulCrc = 0;
						MHEG_SEF_DisableSectionFilter(pstFilterTable->ucDemuxId, pstFilterTable->ulLoadId);
						pstFilterTable->ulLoadId = mhegSef_INVALID_LOADID;
						mhegSEF_CHANGE_TABLE_WAITING(pstFilterTable->eCurFilterTableMode);
						mhegSef_SET_TIMER_ONCE(mhegSef_CHANGE_RETRY_TIMEOUT, mheg_sef_ThreadFilterFailNotifier, NULL, 0, pstFilterTable->ulRetryTimerId);
					}
					break;

				case eFilterTable_Continue:
					if(pstFilterTable->eTimeoutMode == eContinueTimeout_Repeat)
					{
						mhegSef_SET_TIMER_ONCE(pstFilterTable->stTagData.ulTimeOut, mheg_sef_ThreadTimeoutNotifier, NULL, 0, pstFilterTable->ulTimeoutTimerId);
					}
					break;

				default:
					break;
			}
		}
	}
}

/**< eSiFilterMsg_Start */
STATIC void mheg_sef_ThreadFilterMsgStart(MhegSef_SendMessage_t *pstInMsg)
{
	HERROR				err;
	HUINT8				ucDemuxId, ucReceiveMode;
	HINT32				nSiTableIndex;
	HUINT16				usExtId;
	mhegSef_FilterTable_t		*pstFilterTable;
	MhegSef_FilterParam_t		*pstFilterParam = NULL;

	HxLOG_Info("\t[eSiFilterMsg_Start] dmx:%d, pid:0x%04x, tid:0x%02x, mode:0x%02x\n", pstInMsg->ucDemuxId, pstInMsg->msg.stStart.pFilterParam->usPid,
																pstInMsg->msg.stStart.pFilterParam->pFilterValue[0], pstInMsg->msg.stStart.pFilterParam->ucMode);
	ucDemuxId = pstInMsg->ucDemuxId;
	pstFilterParam = pstInMsg->msg.stStart.pFilterParam;

	/* find empty filter table */
	nSiTableIndex = mheg_sef_ThreadFindEmptyTable();
	if(nSiTableIndex >= mhegSef_MAX_SITABLE_NUM)
	{
		HxLOG_Error("Critical error : insuffient si table filter!\n");
		HLIB_STD_MemFree(pstFilterParam);
		return;
	}
	pstFilterTable = &s_pstFilterTable[nSiTableIndex];
	pstFilterTable->eFilterLen = pstFilterParam->eFilterLen;
	pstFilterTable->eTimeoutMode = pstFilterParam->eTimeoutMode;
	HxSTD_memcpy(pstFilterTable->aucFilterValue, pstFilterParam->pFilterValue, pstFilterParam->eFilterLen);
	HxSTD_memcpy(pstFilterTable->aucFilterMask, pstFilterParam->pFilterMask, pstFilterParam->eFilterLen);
	pstFilterTable->bCheckCrc = pstFilterParam->bCheckCrc;
	ucReceiveMode = pstFilterParam->ucMode;

	pstFilterTable->pfnFreeSectionFunc = pstFilterParam->pfnFreeFunc;
	if((pstFilterParam->ucMode == eFilterTable_Continue) && (pstFilterParam->nSecCntInPackage > 1))
	{
		pstFilterTable->nSecCntInPackage = pstFilterParam->nSecCntInPackage;
		pstFilterTable->nDurationInPackage = pstFilterParam->nDurationInPackage;
	}
	else
	{
		pstFilterTable->nSecCntInPackage = 0;
		pstFilterTable->nDurationInPackage = 0;
	}

	/* �� filter table�� �߰��ϴ� ������, ���⼭ fail���� ���� ���� */
	err = mheg_sef_ThreadAddTagToSiTableData(pstFilterTable, pstFilterParam, 0);
	if(ERR_OK!=err)			HxLOG_Critical("Critical Eror!!!\n");
	err = mheg_sef_ThreadStartFilter(pstFilterTable, ucDemuxId, pstFilterParam->usPid, ucReceiveMode, pstFilterParam->ulTimeOut, pstFilterTable->bCheckCrc);
	if(err == ERR_FAIL)
	{ /* filter err��, retry filter by timer */
		mhegSEF_CHANGE_TABLE_WAITING(ucReceiveMode);

#if defined(S_SEF_RETRY_ON_DI_ERR)
		mhegSef_SET_TIMER_ONCE(mhegSef_FILTERFAIL_RETRY_TIMEOUT, mheg_sef_ThreadFilterFailNotifier, NULL, 0, pstFilterTable->ulRetryTimerId);
#else
		/* ������ dmx err�� ��� sithd ��ü������ retry�� ������ octo������ error�� �ְ�, ����ó���� �����(���� �ǳ�)���� �ѱ�� - parse callback�� error ó�� �߰��ʿ� */
		mheg_sef_ThreadSendFilterFail(pstFilterTable);
		mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);
		HxSTD_memset(pstFilterTable, 0, sizeof(mhegSef_FilterTable_t));
		mheg_sef_ThreadMakeUnusedTable(pstFilterTable);
		HLIB_STD_MemFree(pstFilterParam);
		return;
#endif
	}
	else
	{
		pstFilterTable->eSiTableCompleteMode = pstFilterParam->eSiTableCompleteMode;
		pstFilterTable->eCurFilterTableMode = ucReceiveMode;
		pstFilterTable->usUniqueId = pstFilterParam->usUniqueId;
		if(pstFilterParam->pFilterMask[1] | pstFilterParam->pFilterMask[2])
		{
			usExtId = (HUINT16)((pstFilterParam->pFilterValue[1]) << 8 | pstFilterParam->pFilterValue[2]);
			if(usExtId)
				pstFilterTable->usExtId = usExtId;
		}
	}

	HLIB_STD_MemFree(pstFilterParam);
}

/**< eSiFilterMsg_Stop */
STATIC void mheg_sef_ThreadFilterMsgStop(MhegSef_SendMessage_t *pstInMsg)
{
	HxLOG_Info("\t[eSiFilterMsg_Stop:%d] f_id:0x%04x\n", pstInMsg->ucDemuxId, pstInMsg->msg.stStop.usFilterid);
	mheg_sef_ThreadStopFilter(pstInMsg->msg.stStop.usFilterid);
}

/**< eSiFilterMsg_StopAll */
STATIC void mheg_sef_ThreadFilterMsgStopAll(MhegSef_SendMessage_t *pstInMsg)
{
	HINT32		i;

	HxLOG_Info("\t[eSiFilterMsg_StopAll:%d]\n", pstInMsg->ucDemuxId);
	for(i = 0; i < mhegSef_MAX_SITABLE_NUM; i++)
	{
		if((s_pstFilterTable[i].eCurFilterTableMode != eFilterTable_Unused) && (s_pstFilterTable[i].ucDemuxId == pstInMsg->ucDemuxId))
			mheg_sef_ThreadStopFilterForcingly(i);
	}
}

/**< eSiFilterMsg_StopByUserParam */
STATIC void mheg_sef_ThreadFilterMsgStopByUserParam(MhegSef_SendMessage_t *pstInMsg)
{
	HINT32		i;

	for(i = 0; i < mhegSef_MAX_SITABLE_NUM; i++)
	{
		if((s_pstFilterTable[i].eCurFilterTableMode != eFilterTable_Unused) && (s_pstFilterTable[i].ucDemuxId == pstInMsg->ucDemuxId))
		{
			if(s_pstFilterTable[i].stTagData.ulUserParam == pstInMsg->msg.stStopByUserParam.ulUserParam)
			{
				mheg_sef_ThreadStopFilter(s_pstFilterTable[i].stTagData.usFilterId);
			}
		}
	}
}

/**< eSiFilterMsg_EmergencyStop */
/********************************************************************

********************************************************************/

STATIC void mheg_sef_ThreadFilterMsgEmergencyStop(MhegSef_SendMessage_t *pstInMsg)
{
	HUINT32					ulRequestId;
	HINT32					nSiTable;
	mhegSef_FilterTable_t	*pstFilterTable;

	HxLOG_Error("\t[eSiFilterMsg_EmergencyStop] [WARN] retry filter after for a while...\n");

	ulRequestId = pstInMsg->msg.stEmergencyStop.ulRequestId;
	if(mheg_sef_ThreadFindTableByLoadId(ulRequestId, &nSiTable) == ERR_OK)
	{
		pstFilterTable = &s_pstFilterTable[nSiTable];
		MHEG_SEF_DisableSectionFilter(pstFilterTable->ucDemuxId, pstFilterTable->ulLoadId);
		mhegSEF_CHANGE_TABLE_WAITING(pstFilterTable->eCurFilterTableMode);
		mhegSef_SET_TIMER_ONCE(mhegSef_QFULL_RETRY_TIMEOUT, mheg_sef_ThreadFilterFailNotifier, NULL, 0, pstFilterTable->ulRetryTimerId);
	}
}
#define _______________________________________________________________________

/********************************************************************
	SI Send Task.
********************************************************************/
STATIC void mheg_sef_ThreadSendTask(void *pvParam)
{
	MhegSef_SendMessage_t	stInMsg;

	while (1)
	{
		if(VK_OK != VK_MSG_Receive(s_ulSiSendMsgQId, (void*)&stInMsg, sizeof(MhegSef_SendMessage_t)))
			continue;

		mhegSef_ENTER_SITHD_CRITICAL;

		HxLOG_Print("mheg_sef_ThreadSendTask : [%d]\n", stInMsg.eMsgClass);

		switch (stInMsg.eMsgClass)
		{
			case eSiFilterMsg_Retry:
				mheg_sef_ThreadFilterMsgRetry(&stInMsg);
				break;

			case eSiFilterMsg_Timeout:
				mheg_sef_ThreadFilterMsgTimeout(&stInMsg);
				break;

			case eSiFilterMsg_Start:
				mheg_sef_ThreadFilterMsgStart(&stInMsg);
				break;

			case eSiFilterMsg_Stop:
				mheg_sef_ThreadFilterMsgStop(&stInMsg);
				break;

			case eSiFilterMsg_StopAll:
				mheg_sef_ThreadFilterMsgStopAll(&stInMsg);
				break;

			case eSiFilterMsg_StopByUserParam:
				mheg_sef_ThreadFilterMsgStopByUserParam(&stInMsg);
				break;

			case eSiFilterMsg_EmergencyStop:
				mheg_sef_ThreadFilterMsgEmergencyStop(&stInMsg);
				break;
		}

		mhegSef_LEAVE_SITHD_CRITICAL;
	}

	HxLOG_Error("End Si Send Task...\n");
}

/********************************************************************
	SI Receive Task
********************************************************************/
#ifdef _MHEG_SEF_ALLOC_FREE_DEBUG_
STATIC void mheg_sef_ThreadCheckReceiveMsg(MhegSef_ReceiveMessage_t *pstInMsg)
{
	HINT16		sLen, i;
	HUINT8		*pucRawSec;
	MhegSef_RawData_t	*pRawPackage;

	if(eSectionPackage_On == pstInMsg->eSecPackageFlag)
	{
		pRawPackage = (MhegSef_RawData_t *)pstInMsg->ulDataAddr;
		if(pRawPackage == NULL)
		{
			HxLOG_Print("\n\n\n[%s:%d] CRITICAL ERROR - pRawPackage is NULL\n\n\n", __FUNCTION__, __LINE__);
		}
		else
		{
			for(i=0; i<pRawPackage->num_sec; i++)
			{
				pucRawSec = pRawPackage->buf[i];
				if(pucRawSec == NULL)
				{
					HxLOG_Print("\n\n\n[%s:%d] CRITICAL ERROR - pucRawSec(%d) is NULL\n\n\n", __FUNCTION__, __LINE__, i);
				}
				else
				{
					sLen = (HINT16)get12bit(&pucRawSec[1]);
					HxLOG_Print("= R %d\n", (HUINT32)((pucRawSec[sLen+3] << 24) | (pucRawSec[sLen+4] << 16) | (pucRawSec[sLen+5] << 8) | (pucRawSec[sLen+6])));
				}
			}
		}
	}
	else
	{
		pucRawSec = (HUINT8 *)pstInMsg->ulDataAddr;
		if(pucRawSec == NULL)
			HxLOG_Print("\n\n\n[%s:%d] CRITICAL ERROR - pucRawSec is NULL\n\n\n", __FUNCTION__, __LINE__);

		sLen = (HINT16)get12bit(&pucRawSec[1]);
		HxLOG_Print("= R %d\n", (HUINT32)((pucRawSec[sLen+3] << 24) | (pucRawSec[sLen+4] << 16) | (pucRawSec[sLen+5] << 8) | (pucRawSec[sLen+6])));
	}
}
#endif

STATIC	void	mheg_sef_ThreadReceiveTask(void *pvParam)
{
	MhegSef_ReceiveMessage_t	stInMsg;
	HINT32				nSiTable;
	mhegSef_FilterTable_t		*pstFilterTable;

	while (1)
	{
		if(VK_OK != VK_MSG_Receive(s_ulSiRcvMsgQId, (void*)&stInMsg, sizeof(MhegSef_ReceiveMessage_t)))
			continue;

		if(stInMsg.err != ERR_OK)
			continue;

		if(stInMsg.ucDemuxId >= s_ucDemuxNum)
		{
			HxLOG_Error("CRITICAL ERROR - invalid demux (%d) / (MAX = %d)\n", stInMsg.ucDemuxId, s_ucDemuxNum);
			continue;
		}

#ifdef _MHEG_SEF_ALLOC_FREE_DEBUG_
		mheg_sef_ThreadCheckReceiveMsg(&stInMsg);
#endif
		HxLOG_Print("RECV TASK - demuxid = %d, loadid = 0x%04x\n", stInMsg.ucDemuxId, stInMsg.ulRequestId);
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		mhegSef_ENTER_SITHD_CRITICAL;
		if(mheg_sef_ThreadFindTableByLoadId(stInMsg.ulRequestId, &nSiTable) == ERR_OK)
		{
			pstFilterTable = &s_pstFilterTable[nSiTable];
			HxLOG_Print("\t(t_id = 0x%02x, load_id = %d, nSiTable = %d, pid = 0x%04x)\n", pstFilterTable->aucFilterValue[0], stInMsg.ulRequestId, nSiTable, pstFilterTable->usPid);

			if(eSectionPackage_On == stInMsg.eSecPackageFlag)
			{
				mheg_sef_ThreadProcessSectionPackage(pstFilterTable, (MhegSef_RawData_t *)stInMsg.ulDataAddr);
			}
			else
			{
				mheg_sef_ThreadProcessOneSection(pstFilterTable, (HUINT8 *)stInMsg.ulDataAddr);
			}
		}
		else
		{
			if(eSectionPackage_On == stInMsg.eSecPackageFlag)
			{
				mheg_sef_FreePackageData((MhegSef_RawData_t *)stInMsg.ulDataAddr);
			}
			else
			{
				mheg_sef_FreeDemuxSection((HUINT8 *)stInMsg.ulDataAddr);
			}
		}
		//------------------------------------------------------------------------------------------------------------
		mhegSef_LEAVE_SITHD_CRITICAL;
	}
}

/* pstRawData�� ��� �� ���⼭ free�� */
STATIC HERROR mheg_sef_ThreadProcessSectionPackage(mhegSef_FilterTable_t *pstFilterTable, MhegSef_RawData_t *pstRawData)
{
	HUINT32		ulTblIdAndResult = 0;

	if((pstFilterTable == NULL) || (pstRawData == NULL))
	{
		HxLOG_Error("Section Receive OK, But, %s is NULL\n", (pstRawData == NULL) ? "pstRawData" : "pstFilterTable");
		return ERR_FAIL;
	}

	if(pstFilterTable->eCurFilterTableMode != eFilterTable_Continue)
	{
		HxLOG_Error("not supporting filter mode (0x%x)\n", pstFilterTable->eCurFilterTableMode);
		mheg_sef_FreePackageData(pstRawData);
		return ERR_FAIL;
	}

	if(pstFilterTable->stTagData.ucSlot && pstFilterTable->stTagData.pfnParseFunc)
	{
		mhegSef_MAKE_TABLEID_AND_RESULT(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], eSiFilterResult_Complete);
		mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], pstFilterTable->stTagData.usFilterId, pstFilterTable->usUniqueId);
		pstFilterTable->stTagData.pfnParseFunc(ulTblIdAndResult,
													(HUINT32)((pstFilterTable->stTagData.usFilterId << 16) | pstFilterTable->usUniqueId),
													(HUINT32)pstRawData,
													pstFilterTable->stTagData.ulUserParam);
		mhegSef_LEAVE_PARSE_CB;
		pstFilterTable->stTagData.ulSended = 1;
	}

	mheg_sef_FreePackageData(pstRawData);

	return ERR_OK;
}

/* pucRawSec�� ��� �� ���⼭ free �Ǵ� ó���� */
STATIC HERROR mheg_sef_ThreadProcessOneSection(mhegSef_FilterTable_t *pstFilterTable, HUINT8 *pucRawSec)
{
	HBOOL		bForcedParse;
	HUINT32		ulCrc, ulPos;
	HUINT32		ulTblIdAndResult;

	if((pstFilterTable == NULL) || (pucRawSec == NULL))
	{
		HxLOG_Error("Section Receive OK, But, %s is NULL\n", (pucRawSec == NULL) ? "Section" : "pstFilterTable");
		return ERR_FAIL;
	}

	if(mheg_sef_ThreadMakeTable(pstFilterTable, pucRawSec) == mhegSef_COMPLETE_COLLECT_SECTION)		//	���̺� �� ��Ҵ�.
	{
		bForcedParse = TRUE;
		//	Refer�� ���� �ʰ� Stop�Ѵ�. Refer���ʿ����. change�� �ɾ �ٽ� �ɸ����. ���� ���ڸ� ������...
		if(pstFilterTable->eCurFilterTableMode != eFilterTable_Continue)
		{
			HxLOG_Print("StopFilter : [%02x : %02x]\n", pstFilterTable->ucDemuxId, pstFilterTable->ulLoadId);
			MHEG_SEF_DisableSectionFilter(pstFilterTable->ucDemuxId, pstFilterTable->ulLoadId);
			pstFilterTable->ulLoadId = mhegSef_INVALID_LOADID;
		}

		if((pstFilterTable->eCurFilterTableMode == eFilterTable_MonitorByRepeat)
			|| (pstFilterTable->eCurFilterTableMode == eFilterTable_MonitorByVerNum))
		{
			if((pstFilterTable->aucFilterValue[0] != TDT_TABLE_ID)
				&& (pstFilterTable->aucFilterValue[0] != RST_TABLE_ID))
			{
				pucRawSec = pstFilterTable->stRawData.buf[0];
				ulPos = ((pucRawSec[1] << 8) | (pucRawSec[2])) & 0x0fff;
				ulCrc = (HUINT32)HxMACRO_Get32Bit(&(pucRawSec[3]) + ulPos - 4);
				if(pstFilterTable->ulCrc != ulCrc)
					pstFilterTable->ulCrc = ulCrc;
				else
					bForcedParse = FALSE;

				/* SITABLE_MONITOR_BY_VERNUM������ version_number + 1�� ���͸� ���� �Ǵ� */
				if(pstFilterTable->eCurFilterTableMode == eFilterTable_MonitorByVerNum)
				{
					pstFilterTable->aucFilterValue[3] = ((((pucRawSec[5] & 0x3e) >> 1) + 1) % 0x20) << 1;
					pstFilterTable->aucFilterMask[3] = 0x3E;
				}
			}
		}

		//	TODO: �ѹ��̶� �޾����� �޾Ҵٰ� �����ؾ���.

		if(bForcedParse)
		{
			if(pstFilterTable->stTagData.ucSlot && pstFilterTable->stTagData.pfnParseFunc)
			{
				mhegSef_MAKE_TABLEID_AND_RESULT(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], eSiFilterResult_Complete);
				mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], pstFilterTable->stTagData.usFilterId, pstFilterTable->usUniqueId);
				pstFilterTable->stTagData.pfnParseFunc(ulTblIdAndResult, (HUINT32)((pstFilterTable->stTagData.usFilterId << 16) | pstFilterTable->usUniqueId), (HUINT32)&pstFilterTable->stRawData, pstFilterTable->stTagData.ulUserParam);
				mhegSef_LEAVE_PARSE_CB;
				pstFilterTable->stTagData.ulSended = 1;
			}
		}
		else
		{
			if(pstFilterTable->stTagData.ucSlot && pstFilterTable->stTagData.pfnParseFunc &&
				(!pstFilterTable->stTagData.ulSended))
			{
				mhegSef_MAKE_TABLEID_AND_RESULT(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], eSiFilterResult_Complete);
				mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], pstFilterTable->stTagData.usFilterId, pstFilterTable->usUniqueId);
				pstFilterTable->stTagData.pfnParseFunc(ulTblIdAndResult, (HUINT32)((pstFilterTable->stTagData.usFilterId << 16) | pstFilterTable->usUniqueId), (HUINT32)&pstFilterTable->stRawData, pstFilterTable->stTagData.ulUserParam);
				mhegSef_LEAVE_PARSE_CB;
				pstFilterTable->stTagData.ulSended = 1;
			}
		}

		pstFilterTable->stRawData.flag = 1;
		if(pstFilterTable->eCurFilterTableMode == eFilterTable_MonitorByRepeat)
		{
			HxLOG_Print("CHANGE MODE : Retry...\n");
			mhegSEF_CHANGE_TABLE_WAITING(pstFilterTable->eCurFilterTableMode);
			mhegSef_SET_TIMER_ONCE(mhegSef_CHANGE_RETRY_TIMEOUT, mheg_sef_ThreadFilterFailNotifier, NULL, 0, pstFilterTable->ulRetryTimerId);
		}
		else if(pstFilterTable->eCurFilterTableMode == eFilterTable_MonitorByVerNum)
		{
			HxLOG_Print("CHANGE MODE2 : Retry...\n");
			if(mheg_sef_ThreadRestartFilter(pstFilterTable) == ERR_FAIL)
			{
#if defined(S_SEF_RETRY_ON_DI_ERR)
				mhegSEF_CHANGE_TABLE_WAITING(pstFilterTable->eCurFilterTableMode);
				mhegSef_SET_TIMER_ONCE(mhegSef_FILTERFAIL_RETRY_TIMEOUT, mheg_sef_ThreadFilterFailNotifier, NULL, 0, pstFilterTable->ulRetryTimerId);
#else
/* ������ dmx err�� ��� sithd ��ü������ retry�� ������ octo������ error�� �ְ�, ����ó���� �����(���� �ǳ�)���� �ѱ�� - parse callback�� error ó�� �߰��ʿ� */
				mheg_sef_ThreadSendFilterFail(pstFilterTable);
				mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);
				HxSTD_memset(pstFilterTable, 0, sizeof(mhegSef_FilterTable_t));
				mheg_sef_ThreadMakeUnusedTable(pstFilterTable);
#endif
			}
		}
		else if(pstFilterTable->eCurFilterTableMode == eFilterTable_Continue)
		{
			if (pstFilterTable->stTagData.ulTimeOut > 0)
			{
				mhegSef_SET_TIMER_ONCE(pstFilterTable->stTagData.ulTimeOut, mheg_sef_ThreadTimeoutNotifier, NULL, 0, pstFilterTable->ulTimeoutTimerId);
			}
			mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);
			/* continue mode���� crc�� ������� �ʴ´� */
			HxSTD_memset(pstFilterTable->aucSectionRecv, 0, 32);
			HxSTD_memset(pstFilterTable->aucSectionTotal, 0, 32);
		}
		else
		{
			mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);			//	clear raw section if table mode is onetime.
			mheg_sef_ThreadMakeUnusedTable(pstFilterTable);
		}
	}
	else		//	���� �ȵ��� ������ �ִ�.
	{
		HxLOG_Print("Waiting for rest section ... [%02x]\n", pstFilterTable->aucFilterValue[0]);
	}

	return ERR_OK;
}

/********************************************************************
	Stop Filter with TableNum
	Filter All Stop���� ����ϴ� �Լ�. Referüũ�� �ʿ����.
********************************************************************/
STATIC	void	mheg_sef_ThreadStopFilterForcingly(HINT32 nTableNum)
{
	mhegSef_FilterTable_t	*pstFilterTable = &s_pstFilterTable[nTableNum];

	// ��������� RawSection�� Free�� �ȵ�...
	// �׸��� �� ���ﲫ��.. �� Waiting�� �����ϴ��� �𸣰���..
	// �̷��� �� ��������� üũ�ؾ���.
	mhegSef_CANCEL_TIMER(pstFilterTable->ulRetryTimerId);
	if(pstFilterTable->eCurFilterTableMode & eFilterTable_Running)
	{
		mhegSef_CANCEL_TIMER(pstFilterTable->ulTimeoutTimerId);
		MHEG_SEF_DisableSectionFilter(pstFilterTable->ucDemuxId, pstFilterTable->ulLoadId);
	}
	mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);

	if(pstFilterTable->pfnFreeSectionFunc)
		pstFilterTable->pfnFreeSectionFunc(pstFilterTable->usUniqueId, pstFilterTable->aucFilterValue[0], pstFilterTable->usExtId, pstFilterTable->ucDemuxId);
	HxSTD_memset(pstFilterTable, 0, sizeof(mhegSef_FilterTable_t));
	mheg_sef_ThreadMakeUnusedTable(pstFilterTable);
}

/********************************************************************
	Free Raw data.
********************************************************************/
STATIC	void	mheg_sef_ThreadFreeSiRawData(MhegSef_RawData_t *pstRawData)
{
	HINT32		i, cnt = 0;
	if (pstRawData == NULL)
	{
		HxLOG_Error("%s() Invalid parameter.\n", __FUNCTION__);
		goto exit;
	}

	for(i = 0; cnt < pstRawData->num_sec && i < MHEG_SEF_MAX_SI_RAW_SECTION_NUM; i++)
	{
		if(pstRawData->buf[i])
		{
			mheg_sef_FreeDemuxSection(pstRawData->buf[i]);
			cnt++;
		}
		else
		{
			HxLOG_Print("CHECK HERE - i = %d, pstRawData->num_sec = %d, pstRawData->flag = %d\n", i, pstRawData->num_sec, pstRawData->flag);
		}
	}

	HxSTD_memset(pstRawData, 0, sizeof(MhegSef_RawData_t));

exit:
	return;
}

STATIC	HUINT32	mheg_sef_ThreadAddTagToSiTableData(mhegSef_FilterTable_t *psmhegSef_FilterTable_t, MhegSef_FilterParam_t *pstSiParam, HUINT32 ulSended)
{
	mhegSef_TagData_t	*pSiTagData;

	if(psmhegSef_FilterTable_t->stTagData.ucSlot == 0)
	{
		pSiTagData = &psmhegSef_FilterTable_t->stTagData;
		pSiTagData->ucSlot = 1;
		pSiTagData->ucFilterMode = pstSiParam->ucMode;

		pSiTagData->ulTimeOut = pstSiParam->ulTimeOut;
		pSiTagData->ulUserParam = pstSiParam->ulUserParam;
		pSiTagData->usFilterId = pstSiParam->usFilterId;
		pSiTagData->pfnParseFunc = pstSiParam->pfnParseFunc;
		pSiTagData->ulSended = ulSended;

		//	TODO: change filter mode (once, change, continue...)
		//	TODO: keep max filter value.
		if(psmhegSef_FilterTable_t->eCurFilterTableMode & eFilterTable_Waiting)
		{
			mhegSef_CHANGE_TABLE_RUNNING(psmhegSef_FilterTable_t->eCurFilterTableMode);
			if(psmhegSef_FilterTable_t->eCurFilterTableMode < pSiTagData->ucFilterMode)
				psmhegSef_FilterTable_t->eCurFilterTableMode = pSiTagData->ucFilterMode;
			mhegSEF_CHANGE_TABLE_WAITING(psmhegSef_FilterTable_t->eCurFilterTableMode);
		}
		else
		{
			if(psmhegSef_FilterTable_t->eCurFilterTableMode < pSiTagData->ucFilterMode)
				psmhegSef_FilterTable_t->eCurFilterTableMode = pSiTagData->ucFilterMode;
		}
		//psmhegSef_FilterTable_t->eCurFilterTableMode = pstSiParam->ucMode;
		return ERR_OK;
	}

	HxLOG_Error("can't add tag data to filter table (f_id:0x%04x)\n", pstSiParam->usFilterId);
	HxLOG_Error("\t[%02X %02X %02X] pid:0x%04x, mode:0x%02x, userparam:0x%08x, timeout:%d, sended:%d, parse:0x%08x, free:0x%08x\n",
							pstSiParam->pFilterValue[0], pstSiParam->pFilterValue[1], pstSiParam->pFilterValue[2],
							pstSiParam->usPid,
							pstSiParam->ucMode,
							pstSiParam->ulUserParam,
							pstSiParam->ulTimeOut,
							ulSended,
							(HUINT32)pstSiParam->pfnParseFunc,
							(HUINT32)pstSiParam->pfnFreeFunc);
	return ERR_FAIL;
}

/********************************************************************
	��� �ִ� Slot�� ã�´�..
********************************************************************/
STATIC	HUINT32	mheg_sef_ThreadFindEmptyTable(void)
{
	HUINT32		i;

	for(i = 0; i < mhegSef_MAX_SITABLE_NUM; i++)
	{
		if(s_pstFilterTable[i].eCurFilterTableMode == eFilterTable_Unused)
		{
			HxSTD_memset(&s_pstFilterTable[i], 0, sizeof(mhegSef_FilterTable_t));
			return i;
		}
	}

	return mhegSef_MAX_SITABLE_NUM + 1;
}

/********************************************************************
	Load Id�� Si Table�� ã�´�.
********************************************************************/
STATIC	HUINT32	mheg_sef_ThreadFindTableByLoadId(HUINT32 ulLoadId, HINT32 *pnIndex)
{
	HINT32		i;

	for(i = 0; i < mhegSef_MAX_SITABLE_NUM; i++)
	{
		if(s_pstFilterTable[i].eCurFilterTableMode & eFilterTable_Running)
		{
			if(s_pstFilterTable[i].ulLoadId == ulLoadId)
			{
				if(pnIndex)
					*pnIndex = i;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

/********************************************************************
	Retry Timer Id�� Si Table�� ã�´�.
********************************************************************/
STATIC	HUINT32	mheg_sef_ThreadFindTableByRetryTimerId(HUINT32 ulTimerId, HINT32 *pnIndex)
{
	HINT32		i;

	for(i = 0; i < mhegSef_MAX_SITABLE_NUM; i++)
	{
		if(s_pstFilterTable[i].eCurFilterTableMode != eFilterTable_Unused)
		{
			if(s_pstFilterTable[i].ulRetryTimerId == ulTimerId)
			{
				if(pnIndex)
					*pnIndex = i;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

/********************************************************************
	SITYPE_ETC�� ���� Fake Unique Id�� SiTable�� ã�´�..
********************************************************************/
STATIC	HUINT32	mheg_sef_ThreadFindSiTableByFilterId(HUINT16 usFilterId, HINT32 *pnIndex)
{
	HINT32		i;

	for(i = 0; i < mhegSef_MAX_SITABLE_NUM; i++)
	{
		if(s_pstFilterTable[i].eCurFilterTableMode != eFilterTable_Unused)
		{
			if(s_pstFilterTable[i].stTagData.ucSlot)
			{
				if(s_pstFilterTable[i].stTagData.usFilterId == usFilterId)
				{
					*pnIndex = i;
					return ERR_OK;
				}
			}
		}
	}

	return ERR_FAIL;
}

/********************************************************************
	unused mode�� �ٲ۴�. loadid�� invalid�ϰ� ���� �� ����ó��...
	(loadid�� ���ϴ� ��� ����)
********************************************************************/
STATIC	void	mheg_sef_ThreadMakeUnusedTable(mhegSef_FilterTable_t *pstFilterTable)
{
	pstFilterTable->eCurFilterTableMode = eFilterTable_Unused;
	pstFilterTable->ulLoadId = mhegSef_INVALID_LOADID;
	pstFilterTable->usPid = PID_NULL;
	pstFilterTable->stTagData.ucSlot = 0;
}

/********************************************************************
	SITABLE�� rawdata�� ������.
********************************************************************/
STATIC	HUINT32	mheg_sef_ThreadMakeTable(mhegSef_FilterTable_t *pstFilterTable, HUINT8 *pucRawData)
{
	HUINT8		ucLastSecNum, ucCurSecNum, ucTableId;
	HINT32		i;

	HxLOG_Info("mheg_sef_ThreadMakeTable\n");
	ucTableId = pucRawData[0];
	if(pstFilterTable->eSiTableCompleteMode == eSiCompleteMode_Table)
	{
		ucCurSecNum = pucRawData[6];
		ucLastSecNum = pucRawData[7];
	} else
	{
		ucCurSecNum = 0;
		ucLastSecNum = 0;
	}

	i = 0;
	while ((i < 32) && (!pstFilterTable->aucSectionRecv[i]))
		i++;

	if(i == 32)
	{
		/* SITABLE_MONITOR_BY_VERNUM������ �޾Ƴ��� table�� ������ ����� �ٽ� update�Ѵ�. �̷��� �Ͽ� raw section ����ü���� �׻� section�� �޷� �ֵ��� �Ѵ�. */
		if(pstFilterTable->eCurFilterTableMode == eFilterTable_MonitorByVerNum)
			mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);

		for(i = 0; i < ucLastSecNum + 1; i++)
			mhegSef_SET_BIT(pstFilterTable->aucSectionTotal, i);
	}

	if(mhegSef_TEST_BIT(pstFilterTable->aucSectionRecv, ucCurSecNum))
	{
		HxLOG_Print("SAME Section Received. [%02x][%02x]\n", ucTableId, ucCurSecNum);
		mheg_sef_FreeDemuxSection(pucRawData);
		HxLOG_Print("Insufficient section...\n");

		return mhegSef_INSUFFIECIENT_SECTION;
	}

	mhegSef_SET_BIT(pstFilterTable->aucSectionRecv, ucCurSecNum);
	pstFilterTable->stRawData.num_sec++;
	pstFilterTable->stRawData.buf[ucCurSecNum] = pucRawData;

	if(HxSTD_memcmp(pstFilterTable->aucSectionRecv, pstFilterTable->aucSectionTotal, 32) == 0)
	{
		HxLOG_Print("Complete collect section...\n");
		return mhegSef_COMPLETE_COLLECT_SECTION;
	}

	return mhegSef_INSUFFIECIENT_SECTION;
}

/*******************************************************************
	get unique filter id for start/stop filter.
	���� �ɷ��ִ� si filter�� �ߺ����� �ʴ� filter id�� ����� �ش�.
	�� �� ���Ǵ� filter id�� filter stop�ÿ� ���ȴ�.
*******************************************************************/
STATIC HUINT16 mheg_sef_ThreadSiGetUniqueFilterId(void)
{
	static	HUINT16		_usFilterId = 0;
	HUINT16	usFilterId;

	#if 1
	VK_SEM_Get(s_ulFilterIdSemId);
	_usFilterId = (_usFilterId + 1) % 0xffff + 1;
	if (_usFilterId == 0xffff)
	{
		_usFilterId = 0x0001;
	}

	usFilterId = _usFilterId;
	VK_SEM_Release(s_ulFilterIdSemId);

	return usFilterId;
	#else
	HINT32	nIndex;
	HINT32	cnt = 0xFFFF;

	while(cnt--)
	{
		_usFilterId = (_usFilterId + 1) % 0xffff + 1;
		if(mheg_sef_ThreadFindSiTableByFilterId(_usFilterId, &nIndex) != ERR_OK)
			break;
	}

	if(cnt == 0)
	{
		HxLOG_Error("Critical error : couldn't find filter id for ETC filter!\n");
	}

	return _usFilterId;
	#endif
}

/********************************************************************
	Start Filter.....
********************************************************************/
STATIC	HUINT32	mheg_sef_ThreadStartFilter(mhegSef_FilterTable_t *pstFilterTable, HUINT8 ucDemuxId, HUINT16 usPid, HUINT8 ucRecvMode, HUINT32 ulTimeOut, HBOOL bCrc)
{
	HINT32		ret;

	HxLOG_Print("mheg_sef_ThreadStartFilter : [%04x][%02x][%01x]\n", usPid, pstFilterTable->aucFilterValue[0], ucDemuxId);
	pstFilterTable->eCurFilterTableMode = ucRecvMode;
	pstFilterTable->ucDemuxId = ucDemuxId;
	pstFilterTable->usPid = usPid;
	pstFilterTable->bCheckCrc = bCrc;
	HxSTD_memset(pstFilterTable->aucSectionRecv, 0, 32);
	HxSTD_memset(pstFilterTable->aucSectionTotal, 0, 32);

	mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);

	ret = MHEG_SEF_EnableSectionFilter(ucDemuxId, usPid,
										s_ulSiRcvMsgQId, s_ulSiSendMsgQId,
										pstFilterTable->nSecCntInPackage, pstFilterTable->nDurationInPackage,
										pstFilterTable->eFilterLen, pstFilterTable->aucFilterValue, pstFilterTable->aucFilterMask, s_aucFilterMode,
										&pstFilterTable->ulLoadId, bCrc);
	if(ret == ERR_OK)
	{
		if(ulTimeOut > 0)
		{
			mhegSef_SET_TIMER_ONCE(ulTimeOut, mheg_sef_ThreadTimeoutNotifier, NULL, 0, pstFilterTable->ulTimeoutTimerId);
		}
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("[sithd:%d]MHEG_SEF_EnableSectionFilter Error : [%04x][%02x][%01x]\n",__LINE__,
			usPid, pstFilterTable->aucFilterValue[0], ucDemuxId);
	}
	return ERR_FAIL;
}

/********************************************************************
	pSiTable�� ����ü ������� �� ���� �״�� �ٽ� Filtering��Ų��..
********************************************************************/
STATIC	HUINT32	mheg_sef_ThreadRestartFilter(mhegSef_FilterTable_t *pstFilterTable)
{
	HINT32		ret;

	HxLOG_Print("mheg_sef_ThreadRestartFilter : [%04x][%02x][%01x]\n", pstFilterTable->usPid, pstFilterTable->aucFilterValue[0], pstFilterTable->ucDemuxId);
	HxSTD_memset(pstFilterTable->aucSectionRecv, 0, 32);
	HxSTD_memset(pstFilterTable->aucSectionTotal, 0, 32);

	/* change mode 2������ filter�� ������ stop�� ������ raw section�� ����.. --> �ܺ� ��⿡�� filter �� �� �ٷιٷ� noti���ֱ� ����. */
	if(pstFilterTable->eCurFilterTableMode != eFilterTable_MonitorByVerNum)
		mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);

	ret = MHEG_SEF_EnableSectionFilter(pstFilterTable->ucDemuxId, pstFilterTable->usPid, s_ulSiRcvMsgQId, s_ulSiSendMsgQId, pstFilterTable->nSecCntInPackage, pstFilterTable->nDurationInPackage, pstFilterTable->eFilterLen, pstFilterTable->aucFilterValue, pstFilterTable->aucFilterMask, s_aucFilterMode, &pstFilterTable->ulLoadId, pstFilterTable->bCheckCrc);
	if(ret == ERR_OK)
	{
		if(pstFilterTable->stTagData.ulTimeOut > 0)
		{
			mhegSef_SET_TIMER_ONCE(pstFilterTable->stTagData.ulTimeOut, mheg_sef_ThreadTimeoutNotifier, NULL, 0, pstFilterTable->ulTimeoutTimerId);
		}
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("[sithd:%d]MHEG_SEF_EnableSectionFilter Error : [%04x][%02x][%01x]\n",__LINE__, pstFilterTable->usPid, pstFilterTable->aucFilterValue[0], pstFilterTable->ucDemuxId);
	}

	return ERR_FAIL;
}

/********************************************************************
	pid, unique id�� ���� filter�� �ߴ� ��Ų��.
********************************************************************/
STATIC	HUINT32	mheg_sef_ThreadStopFilter(HUINT16 usFilterId)
{
	HINT32				nSiTableIndex, nFilterMode;
	mhegSef_FilterTable_t		*pstFilterTable;

	if(mheg_sef_ThreadFindSiTableByFilterId(usFilterId, &nSiTableIndex) != ERR_OK)
	{
		/*
			�� ������ �ߴ� ��찡 ������,
			each mode�� ���͸� �ɾ�������, SI_StopSection() or MXC_SI_StopFilter() �� �ϴ� ���, invalid load id�� �� �ۿ� ����.
			filter�� ���� �ɷ� �ִ��� ������ Ȯ���ϸ� �� error�� ������ �� ����.
			������� ����� �� �׷���, warning ����..?

			�ַ� sn(act pmt)�� vd_demux, cai���� �θ��� stop�Լ��� ����..
		*/
		HxLOG_Print("Stop filter fail!! (Couldn't find si table!)[usFilterId:0x%X]\n", usFilterId);
		return ERR_FAIL;
	}
	nFilterMode = 0;
	pstFilterTable = &s_pstFilterTable[nSiTableIndex];
	if(pstFilterTable->stTagData.ucSlot && pstFilterTable->stTagData.usFilterId == usFilterId)
	{
		HxSTD_memset(&pstFilterTable->stTagData, 0, sizeof(mhegSef_TagData_t)); /* ucSlot & usFilterId reset */
	}
	if(pstFilterTable->stTagData.ucSlot)
	{
		if(pstFilterTable->stTagData.ucFilterMode > nFilterMode)
			nFilterMode = pstFilterTable->stTagData.ucFilterMode;
	}

	mhegSef_CANCEL_TIMER(pstFilterTable->ulRetryTimerId);
	if(pstFilterTable->eCurFilterTableMode & eFilterTable_Running)
	{
		mhegSef_CANCEL_TIMER(pstFilterTable->ulTimeoutTimerId);
		MHEG_SEF_DisableSectionFilter(pstFilterTable->ucDemuxId, pstFilterTable->ulLoadId);
	}

	if(pstFilterTable->pfnFreeSectionFunc)
		pstFilterTable->pfnFreeSectionFunc(pstFilterTable->usUniqueId, pstFilterTable->aucFilterValue[0], pstFilterTable->usExtId, pstFilterTable->ucDemuxId);

	mheg_sef_ThreadFreeSiRawData(&pstFilterTable->stRawData);
	HxSTD_memset(pstFilterTable, 0, sizeof(mhegSef_FilterTable_t));
	mheg_sef_ThreadMakeUnusedTable(pstFilterTable);

	return ERR_OK;
}

/********************************************************************
	���� ���͸� ����ϴ� ��鿡�� filter error�� �˷���.
********************************************************************/
STATIC void mheg_sef_ThreadSendFilterFail(mhegSef_FilterTable_t *pstFilterTable)
{
	HUINT32		ulTblIdAndResult;

	if(pstFilterTable->stTagData.ucSlot)
	{
		if(pstFilterTable->stTagData.pfnParseFunc)
		{
			mhegSef_MAKE_TABLEID_AND_RESULT(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], eSiFilterResult_Error);
			mhegSef_ENTER_PARSE_CB(ulTblIdAndResult, pstFilterTable->aucFilterValue[0], pstFilterTable->stTagData.usFilterId, pstFilterTable->usUniqueId);
			pstFilterTable->stTagData.pfnParseFunc(ulTblIdAndResult, (HUINT32)((pstFilterTable->stTagData.usFilterId << 16) | pstFilterTable->usUniqueId), 0, pstFilterTable->stTagData.ulUserParam);
			mhegSef_LEAVE_PARSE_CB;
		}
	}
}

/********************************************************************
	Retry Timer Callback. (not enough demux filter....)
********************************************************************/
STATIC	void	mheg_sef_ThreadFilterFailNotifier(HULONG ulTimerId, void *pvLocalData)
{
	HERROR					err;
	MhegSef_SendMessage_t	stOutMsg;
	HINT32					nIndex;

	HxLOG_Print("\t[FILTERFAIL_CALLBACK] timer_id = 0x%08X\n", (HUINT32)ulTimerId);

	if(mheg_sef_ThreadFindTableByRetryTimerId(ulTimerId, &nIndex) == ERR_OK)
	{
		s_pstFilterTable[nIndex].ulRetryTimerId = 0;	// timer error�� ���� �ʱ� ���� 0���� ����� ��
		HxSTD_memset(&stOutMsg, 0, sizeof(MhegSef_SendMessage_t));
		stOutMsg.eMsgClass = eSiFilterMsg_Retry;
		stOutMsg.msg.stRetry.nFilterIndex = nIndex;
		err = VK_MSG_SendTimeout(s_ulSiSendMsgQId, (void*)&stOutMsg, sizeof(MhegSef_SendMessage_t), 0);
		if(err != VK_OK)
		{
			HxLOG_Critical("Critical Eror!!!\n");
		}
	}
	else
	{
		HxLOG_Print("Error : SITABLE in Time FilterFail Callback is NULL\n");
	}
}

/********************************************************************
	Timeout Timer Callback
********************************************************************/
STATIC	void	mheg_sef_ThreadTimeoutNotifier(HULONG ulTimerId, void *pvLocalData)
{
	HINT32					res;
	MhegSef_SendMessage_t	stOutMsg;

	HxLOG_Print("\t[TIMEOUT_CALLBACK] timer_id:0x%08x\n", (HUINT32)ulTimerId);
	stOutMsg.eMsgClass = eSiFilterMsg_Timeout;
	stOutMsg.msg.stTimeout.ulTimerId = ulTimerId;
	res = VK_MSG_SendTimeout(s_ulSiSendMsgQId, (void*)&stOutMsg, sizeof(MhegSef_SendMessage_t), 0);
	if(res != VK_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

}

#define _____debug_cmd_____________________________________________________

STATIC	void mheg_sef_ThreadPrintFilterTable(HINT32 nPrintLevel, HUINT8 ucTableIndex, mhegSef_FilterTable_t *pstFilterTable)
{
	HINT32		i;
	HINT8		*szTableName;

	switch(pstFilterTable->aucFilterValue[0])
	{
	case PAT_TABLE_ID                    : szTableName = "PAT";			break;
	case CAT_TABLE_ID                    : szTableName = "CAT"; 		break;
	case PMT_TABLE_ID                    : szTableName = "PMT"; 		break;
	case TSDT_TABLE_ID                   : szTableName = "TSD"; 		break;
	case NIT_ACTUAL_TABLE_ID             : szTableName = "NIT_ACT"; 	break;
	case NIT_OTHER_TABLE_ID              : szTableName = "NIT_OTH"; 	break;
	case SDT_ACTUAL_TABLE_ID             : szTableName = "SDT_ACT"; 	break;
	case SDT_OTHER_TABLE_ID              : szTableName = "SDT_OTH"; 	break;
	case EIT_ACTUAL_TABLE_ID             : szTableName = "EIT_ACT_PF"; 	break;
	case EIT_OTHER_TABLE_ID              : szTableName = "EIT_OTH_PF"; 	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_0  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_1  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_2  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_3  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_4  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_5  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_6  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_7  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_8  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_9  : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_10 : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_11 : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_12 : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_13 : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_14 : szTableName = "EIT_ACT_SC";	break;
	case EIT_ACTUAL_SCHEDULE_TABLE_ID_15 : szTableName = "EIT_ACT_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_0   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_1   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_2   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_3   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_4   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_5   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_6   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_7   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_8   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_9   : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_10  : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_11  : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_12  : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_13  : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_14  : szTableName = "EIT_OTH_SC";	break;
	case EIT_OTHER_SCHEDULE_TABLE_ID_15  : szTableName = "EIT_OTH_SC";	break;
	case BAT_TABLE_ID                    : szTableName = "BAT"; 		break;
	case RST_TABLE_ID                    : szTableName = "RST"; 		break;
	case TDT_TABLE_ID                    : szTableName = "TDT"; 		break;
	case TOT_TABLE_ID                    : szTableName = "TOT"; 		break;
	case AIT_TABLE_ID                    : szTableName = "AIT"; 		break;
	case DIT_TABLE_ID                    : szTableName = "DIT"; 		break;
	case DGT_TABLE_ID                    : szTableName = "DGT"; 		break;
	case DBT_TABLE_ID                    : szTableName = "DBT"; 		break;
	case DSI_TABLE_ID                    : szTableName = "DSI/DII"; 	break;
	case DDB_TABLE_ID                    : szTableName = "DDB"; 		break;
	case UNT_TABLE_ID                    : szTableName = "UNT"; 		break;
	case CIT_TABLE_ID                    : szTableName = "CIT"; 		break;
	case CPT_TABLE_ID                    : szTableName = "CPT"; 		break;
	case SGT_TABLE_ID                    : szTableName = "SGT"; 		break;
	case RPD_TABLE_ID                    : szTableName = "RPD"; 		break;
	case DSC_TABLE_ID                    : szTableName = "DSC"; 		break;
	case ICT_TABLE_ID                    : szTableName = "ICT"; 		break;
	case SDTT_TABLE_ID                   : szTableName = "SDTT"; 		break; /* jp arib */
	case BIT_TABLE_ID                    : szTableName = "BIT"; 		break; /* jp arib */
	case FSAT_EITPF_PP_OTH_TABLE_ID                    : szTableName = "EIT_PF_++"; 		break;
	default					             : szTableName = "???"; 		break;
	}

	HLIB_CMD_Printf("[%02d] (%-10s) Tid:0x%02X, ExtId:0x%04X, Pid:0x%04X, Dmx:%d, CurMode:0x%02X <%s_FLT> UniqId:0x%04X\n",
					ucTableIndex,
					szTableName,
					pstFilterTable->aucFilterValue[0],
					pstFilterTable->usExtId,
					pstFilterTable->usPid,
					pstFilterTable->ucDemuxId,
					pstFilterTable->eCurFilterTableMode,
					(pstFilterTable->eSiTableCompleteMode == eSiCompleteMode_Table) ? "TBL":"SEC",
					pstFilterTable->usUniqueId);

	if(nPrintLevel > 1)
	{
		HLIB_CMD_Printf("\t* LoadId:0x%08X, RetryTimerId:0x%08X, TimeoutTimerId:0x%08X, TimeoutMode:%d, CrcCheck:%d, Crc32:0x%08X\n",
					pstFilterTable->ulLoadId,
					(HUINT32)pstFilterTable->ulRetryTimerId,
					(HUINT32)pstFilterTable->ulTimeoutTimerId,
					pstFilterTable->eTimeoutMode,
					pstFilterTable->bCheckCrc,
					pstFilterTable->ulCrc);
	}

	if(nPrintLevel > 2)
	{
		HLIB_CMD_Printf("\t* stRawData.flag:%d, num_sec:%d\n", pstFilterTable->stRawData.flag, pstFilterTable->stRawData.num_sec);
		HLIB_CMD_Printf("\t* stTagData\n");
		HLIB_CMD_Printf("\t  -> [f_mode:0x%02X, f_id:0x%04X, timeout:%d, sended:%d, slot:%d, UserParam:0x%08X, cb_parse:0x%08X]\n",
					pstFilterTable->stTagData.ucFilterMode,
					pstFilterTable->stTagData.usFilterId,
					pstFilterTable->stTagData.ulTimeOut,
					pstFilterTable->stTagData.ulSended,
					pstFilterTable->stTagData.ucSlot,
					pstFilterTable->stTagData.ulUserParam,
					(HUINT32)pstFilterTable->stTagData.pfnParseFunc);
	}

	if(nPrintLevel > 3)
	{
		HLIB_CMD_Printf("\t* FilterLen        :%d\n", pstFilterTable->eFilterLen);
		HLIB_CMD_Printf("\t\tFILTER_VALUE ");
		for(i=0; i<MHEG_SEF_MAX_FILTER_LEN; i++)
		{
			if(0 ==(i%8))	HxLOG_Print(" ");
			HLIB_CMD_Printf("[%02X]", pstFilterTable->aucFilterValue[i]);
		}
		HLIB_CMD_Printf("\n");

		HLIB_CMD_Printf("\t\tFILTER_MASK  ");
		for(i=0; i<MHEG_SEF_MAX_FILTER_LEN; i++)
		{
			if(0 ==(i%8))	HxLOG_Print(" ");
			HLIB_CMD_Printf("[%02X]", pstFilterTable->aucFilterMask[i]);
		}
		HLIB_CMD_Printf("\n");
	}

	if(nPrintLevel > 4)
	{
		HLIB_CMD_Printf("\t\tpfnFreeSectionFunc : 0x%08X\n", (HUINT32)pstFilterTable->pfnFreeSectionFunc);
	}

	if(nPrintLevel > 5)
	{
		HLIB_CMD_Printf("\t* SecRecv :");
		for(i=0; i<32; i++)
			HLIB_CMD_Printf(" %02X", pstFilterTable->aucSectionRecv[i]);
		HLIB_CMD_Printf("\n");
		HLIB_CMD_Printf("\t* SecTotal:");
		for(i=0; i<32; i++)
			HLIB_CMD_Printf(" %02X", pstFilterTable->aucSectionTotal[i]);
		HLIB_CMD_Printf("\n");
		HLIB_CMD_Printf("\t*         :");
		for(i=0; i<32; i++)
			HLIB_CMD_Printf(" %02d", i);
		HLIB_CMD_Printf("\n");
	}

	if(nPrintLevel > 1)
		HLIB_CMD_Printf("\n");
}

#if defined(CONFIG_DEBUG)
void		mheg_sef_PrintFilterTable(HINT32 nPrintLevel, HUINT16 usPid)
{
	HINT32	i;
	HINT32	emptyTableNum = 0;


	HLIB_CMD_Printf("\n\n##############################################################################\n");
	HLIB_CMD_Printf("		PRINT ALL SITABLE\n");
	HLIB_CMD_Printf("______________________________________________________________________________\n");
	for(i=0; i<mhegSef_MAX_SITABLE_NUM; i++)
	{
		if(s_pstFilterTable[i].eCurFilterTableMode != eFilterTable_Unused)
		{
			if((nPrintLevel == 0) || (usPid == 0xffff) || ((nPrintLevel != 0) && (usPid == s_pstFilterTable[i].usPid)))
				mheg_sef_ThreadPrintFilterTable(nPrintLevel, i, &s_pstFilterTable[i]);
		}
	}
	HLIB_CMD_Printf("\n##############################################################################\n");

	for(i=0; i<mhegSef_MAX_SITABLE_NUM; i++)
	{
		if(s_pstFilterTable[i].eCurFilterTableMode == eFilterTable_Unused)
			emptyTableNum++;
	}
	HLIB_CMD_Printf("\n########################\n");
	HLIB_CMD_Printf("Empty SI_TABLE NUM:%d\n", emptyTableNum);
	HLIB_CMD_Printf("############################\n");


	// for debugging Mheg layer's filter information.
	mheg_sef_PrintContext();
}

#endif

#if __this_function_is_not_used_at_this_time__

#endif
/*********************** End of File ******************************/
