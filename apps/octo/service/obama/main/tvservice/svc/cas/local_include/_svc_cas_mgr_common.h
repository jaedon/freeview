/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_mgr_common.h			\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CAS_MGR_COMMON_H__
#define	__SVC_CAS_MGR_COMMON_H__

/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___
#include <dlib.h>
#include <octo_common.h>

#include <svc_cas.h>
#include <_svc_cas_sub_api.h>
#include <_svc_cas_sub_svc_api.h>
#include <_svc_cas_sub_dev_api.h>


/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___


/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

#ifdef CONFIG_MW_CAS_NAGRA_OP_AMX
#define	_NA_NOCS_OVERRIDE_FUNCTION
#endif
/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___


/********************************************************************
* Global Public Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Global_Public_Definitions___

//	FTA일 경우도 IRdeto Condition을 통해야 한다는 Irdeto 규약
#if	defined(CONFIG_OP_ZIGGO) || defined(CONFIG_PROD_ICORDHDPLUSME) || defined(CONFIG_OP_RUSSIA_TVT)
#define	PASS_ALL_SVC_TO_IRDETO
#endif

/* 아래 define들은 side effect가 염려 되어 우선 모델 define으로 사용 */
#if	defined (CONFIG_PROD_ICORDHDPLUSME)
#define	CONNECT_TO_AVAILABLE_INST_WHEN_DISCONNECT_ORG_INST		// 사용하던 instance disconnect 후 이용 가능한 instance 찾아 보기 (descramble 중이던 instance 제거 시 가용 instance로 연결하여 계속 descramble 하기 위함)
#define CHANGE_INST_WHEN_INST_NOT_WORK_WELL						// context에 할당된 instance를 정상 동작(현재는 device 유무만 따짐) 하지 않을 시 교체할 수 있는 define
#define IR_SC_IN_OUT_NOTI_TO_CASMGR								// IR에서 SC 삽입/제거시 Cas Mgr쪽에 알려주어야 하나 기존에 알려주지 않았음. 이를 알려주는 define
#endif

#define CAS_MAX_NUM_OF_LIVE_PID			6	// WTV에서 Descramble 요청하는 PID 개수 -> hmkim's comment : Irdeto CAS 시스템의 경우 CI+ copy protection 은 6개까지 제한이 있음. 따라서 이 값은 합당한 것임.

/* BS, 2008.01.09 : 독일 ...Giga Cam의 경우 Supported Ca System Id가 23개다. */
/*
	2009.04.16. Viaccess cam & TPS 2002 년 stream의 descriptor 가 34인 경우가 발생하여, Crash 유발함.
*/
#define CAS_MAX_NUM_OF_CA_SYSID			64	// 20 -> 30 -> 64

#define CAS_FILTER_ID_NULL				0xFFFFFFFF
#define CAS_MAX_SECTION_LENGTH			1024
#define CAS_MAX_SECTION_NUM 			256

// Macro Tool
#define VALID_PID(x)				(x > 0 && x < 0x2000)
#define INVALID_PID(x)				!VALID_PID(x)
#define GET_ACTION_ID(c) 			((c & 0xFF000000) >> 24)
#define GET_SERVICE_ID(x)			(x & 0xffff)
#define GET_UNIQUE_ID(x) 			((x & 0xffff0000) >> 16)
#define MWCAS_UNUSED(X)

#define CAS_INITAILIZED 			1
#define CAS_NOT_INITAILIZED 		0

#define CAS_HAVE_DEVICE				TRUE
#define CAS_HAVE_NOT_DEVICE			FALSE

#define CAS_INST_ALLOCATED			TRUE
#define CAS_INST_NOT_ALLOCATED		FALSE

#define CAS_INST_USED				TRUE
#define CAS_INST_NOT_USED			FALSE

#define CAS_MAX_SECTION_NUM			256

#if defined(CONFIG_MW_CAS) && !defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#define CAS_SUPPORT_CAT
#endif

#if defined(CONFIG_MW_CI_PLUS)
#define CAS_SUPPORT_SDT
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) ||defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
#define CAS_SUPPORT_EIT
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
#define CAS_SUPPORT_TDT
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SOL) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT) \
			|| defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)

#define CAS_SUPPORT_NIT
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
#define CAS_SUPPORT_NIT_MULTI
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
#define CAS_SUPPORT_BAT
#endif

#if 0 // test용도
#define CAS_SUPPORT_CAT
#define CAS_SUPPORT_SDT
#define CAS_SUPPORT_EIT
#define CAS_SUPPORT_TDT
#define CAS_SUPPORT_NIT
#define CAS_SUPPORT_BAT
#endif

//#define CAS_SUPPORT_NIT_MULTI

typedef enum
{
	eCAS_NOTI_TYPE_NONE,
	eCAS_NOTI_TYPE_TUNER,
	eCAS_NOTI_TYPE_VIDEO,
	eCAS_NOTI_TYPE_AUDIO,
	eCAS_NOTI_TYPE_PVR,
	eCAS_NOTI_TYPE_MAX
} SvcCas_NotiType_e;

/*	일반적으로 eCAS_State_Idle -> eCAS_State_Started -> eCAS_State_SetPid -> eCAS_State_subCasSelected -> eCAS_State_CasOK or eCAS_State_CasFail 순으로 변경.
*/
typedef enum
{
	eCAS_State_Idle,				// CAS mgr 초기 상태 또는 CMD_CAS_STOP 된 상태
	eCAS_State_Started,				// CMD_CAS_START 된 상태
	eCAS_State_SetPid,				// pid selected된 상태.. app에서 svc start한 다음 항상 set pid를 바로 한다.
	eCAS_State_subCasNotSelected,	// sub cas가 선택되지 못했다.
	eCAS_State_subCasSelected,		// sub cas가 선택된 상태
	eCAS_State_CasOK,				// sub cas가 descramble 상태가 OK라고 올려 주었다.
	eCAS_State_CasFail,				// sub cas가 descramble 상태가 NOT OK라고 올려 주었다.
	eCAS_State_Max
} SvcCas_State_e;

#define _____SI________________

typedef enum
{
	eCAS_MATCH_None,
	eCAS_MATCH_Prog,
	eCAS_MATCH_Es,
	eCAS_MATCH_Max
} SvcCas_MatchLevel_e;

typedef struct tDESC_POINT_LIST
{
	unsigned char					ucTag;
	unsigned char					*pucRaw;
	struct tDESC_POINT_LIST			*pNext;
} SvcCas_DescPointList_t;

#define _____UI______________

typedef enum
{
	eCAS_EVT_TYPE_CIMmi,
	eCAS_EVT_TYPE_Nagra,
	eCAS_EVT_TYPE_IR,
	eCAS_EVT_TYPE_Max
} SvcCas_EvtType_e;


#define _____FS______________
#ifdef WIN32
#define CAS_JFFS2_PATH				""
#else
#define CAS_JFFS2_PATH				OBAMA_DIR_DATA "/cas"
#endif


/********************************************************************
* Global Public Function Prototypes
********************************************************************/
#define ___04_Global_Public_Func_Prototypes___


#endif /* !__SVC_CAS_MGR_COMMON_H__ */
