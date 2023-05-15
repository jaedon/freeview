/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MHEG_MGR_COMMON_H__
#define	__MHEG_MGR_COMMON_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include "mheg_rawmgr.h"


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
	CMD_MHEG_SI_START_SEARCH = 1,
	CMD_MHEG_SI_STOP_SEARCH,
	CMD_MHEG_SI_PAT,
	CMD_MHEG_SI_PMT,
	CMD_MHEG_SI_REQUEST_MONITOR_PMT,
	CMD_MHEG_CURRENT_TABLE_ADD,
	CMD_MHEG_SI_START_LIVE_SECTION,
	CMD_MHEG_STREAMING_SI_START,
	CMD_MHEG_STREAMING_SI_STOP,
	CMD_MHEG_STREAMING_SI_PAT,
	CMD_MHEG_STREAMING_SI_PMT,
	CMD_MHEG_MAX
} MHEG_Command_t;

typedef struct tagPMT_ELEMENT_INFO
{
	HUINT8	ucStreamType;
	HUINT16	usElemPid;
} PmtElementInfo_t, *pPmtElementInfo_t;

typedef struct tagPMT_PROGRAM_INFO
{
	HUINT16	usProgNum;
	HUINT8	ucVer;
	HBOOL	bCurNextInd;
	HUINT16	usPCRPid;
	HUINT32 ulCrc32;
} PmtProgramInfo_t;

typedef struct tagPAT_PROGINFO
{
	HUINT16		usProgNum;
	HUINT16		usProgMapPid;
} PatProgInfo_t;

typedef struct tagMHEG_639_LAN_INFO
{
	AUDIO_Type_t	eAudType;
	HUINT8			szIso639LangCode[4];
} MhegIso639LangInfo_t;

#define	MAX_SECTION_NUM			256

#define	MAX_VIDEO_ES_NUM		2
#define	MAX_AUDIO_ES_NUM		16		/* 개수 변경 되면 MAX_AUDIO_NUM도 필 변경 바람. */
#define	MAX_SUBTTL_ES_NUM		12		/* 개수 변경 되면 MAX_SUBTTL_NUM도 필 변경 바람. */
#define	MAX_TELTXT_ES_NUM		8		/* 개수 변경 되면 MAX_TTX_INITAIL_PAGE도 필 변경 바람. */

#define	TAG_ALL_DESCRIPTORS			0xFFFFFFFF


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HULONG		MHEG_SI_GetQid(void);
void 		MHEG_SI_Init(void);
HERROR		MHEG_SI_SendSectionMessage (MHEG_Command_t eMsgType, HUINT32 ulViewId, HUINT32 ulTableStatus, HUINT32 ulFilterId,HUINT16 usCheckId);
HERROR		MHEG_SI_GetPmtProgramInfoFromRaw (HUINT8 *pucRaw, PmtProgramInfo_t *pstInfo);
HERROR		MHEG_SI_CountPmtElementFromRaw (HUINT8 *pucRaw, HUINT32 *pulElemCnt);
HERROR		MHEG_SI_GetPmtElementInfoFromRaw (HUINT8 *pucRaw, HUINT32 ulElemIdx, PmtElementInfo_t *pstInfo);
HERROR		MHEG_SI_CountPmtElementDescriptorFromRaw (HUINT8 *pucRaw, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 *pulDesCnt);
HERROR		MHEG_SI_GetPmtElementDescriptorFromRaw (HUINT8 *pucRaw, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes);
HERROR		MHEG_SI_GetPmtElementDescriptor (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nSubIdx, HUINT32 ulTag, HUINT32 nDesIdx, void **ppvDes);
HERROR		MHEG_SI_CountPatSection (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 *pulCnt);
HERROR		MHEG_SI_CountPmtSection (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 *pulSecCnt);
HERROR		MHEG_SI_GetPmtElementInfo (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nSubIdx, PmtElementInfo_t *pstInfo);
HERROR		MHEG_SI_CountPmtElementDescriptor (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nSubIdx, HUINT32 ulTag, HUINT32 *pnDesCnt);
HERROR		MHEG_SI_GetPatProgramInfo (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulSubIdx, PatProgInfo_t *pstInfo);
HERROR		MHEG_SI_CountPatProgramFromRaw (HUINT8 *pucRaw, HUINT32 *pulCnt);
HERROR		MHEG_SI_CountPatProgram (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pulCnt);
HERROR		MHEG_SI_GetPatProgramInfoFromRaw (HUINT8 *pucRaw, HUINT32 ulInfoIdx, PatProgInfo_t *pstInfo);
HERROR		MHEG_SI_CountPmtElement (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pnCnt);
HERROR 		MHEG_SI_GetRawSection (MhegRawPool *pstPool, SiTableType_t eTableType, HUINT16 usTsUniqId, HUINT32 ulSecNum, HBOOL bUseExtId, HUINT16 usExtId, HUINT8 **ppucRaw);



HERROR		MHEG_SI_FindAudioCodecFromPSI (HUINT32 ulStreamType, DxAudioCodec_e *peCodec);
HERROR		MHEG_SI_FindVideoCodecFromPSI(HUINT32 ulStreamType, DxVideoCodec_e *ucCodec);


#ifdef CONFIG_DEBUG
void	MHEG_SI_DEBUG_SetupMediaPatFilter(void);
void	MHEG_SI_DEBUG_PrintMHEGIPStreamingStatus(void);
#endif

/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/


#endif /* !__MHEG_MGR_COMMON_H__ */

