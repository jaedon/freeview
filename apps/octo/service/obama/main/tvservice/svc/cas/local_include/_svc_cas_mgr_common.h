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
* Header Files :  ��� ������ ������ .c ���� include �� �� !
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

//	FTA�� ��쵵 IRdeto Condition�� ���ؾ� �Ѵٴ� Irdeto �Ծ�
#if	defined(CONFIG_OP_ZIGGO) || defined(CONFIG_PROD_ICORDHDPLUSME) || defined(CONFIG_OP_RUSSIA_TVT)
#define	PASS_ALL_SVC_TO_IRDETO
#endif

/* �Ʒ� define���� side effect�� ���� �Ǿ� �켱 �� define���� ��� */
#if	defined (CONFIG_PROD_ICORDHDPLUSME)
#define	CONNECT_TO_AVAILABLE_INST_WHEN_DISCONNECT_ORG_INST		// ����ϴ� instance disconnect �� �̿� ������ instance ã�� ���� (descramble ���̴� instance ���� �� ���� instance�� �����Ͽ� ��� descramble �ϱ� ����)
#define CHANGE_INST_WHEN_INST_NOT_WORK_WELL						// context�� �Ҵ�� instance�� ���� ����(����� device ������ ����) ���� ���� �� ��ü�� �� �ִ� define
#define IR_SC_IN_OUT_NOTI_TO_CASMGR								// IR���� SC ����/���Ž� Cas Mgr�ʿ� �˷��־�� �ϳ� ������ �˷����� �ʾ���. �̸� �˷��ִ� define
#endif

#define CAS_MAX_NUM_OF_LIVE_PID			6	// WTV���� Descramble ��û�ϴ� PID ���� -> hmkim's comment : Irdeto CAS �ý����� ��� CI+ copy protection �� 6������ ������ ����. ���� �� ���� �մ��� ����.

/* BS, 2008.01.09 : ���� ...Giga Cam�� ��� Supported Ca System Id�� 23����. */
/*
	2009.04.16. Viaccess cam & TPS 2002 �� stream�� descriptor �� 34�� ��찡 �߻��Ͽ�, Crash ������.
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

#if 0 // test�뵵
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

/*	�Ϲ������� eCAS_State_Idle -> eCAS_State_Started -> eCAS_State_SetPid -> eCAS_State_subCasSelected -> eCAS_State_CasOK or eCAS_State_CasFail ������ ����.
*/
typedef enum
{
	eCAS_State_Idle,				// CAS mgr �ʱ� ���� �Ǵ� CMD_CAS_STOP �� ����
	eCAS_State_Started,				// CMD_CAS_START �� ����
	eCAS_State_SetPid,				// pid selected�� ����.. app���� svc start�� ���� �׻� set pid�� �ٷ� �Ѵ�.
	eCAS_State_subCasNotSelected,	// sub cas�� ���õ��� ���ߴ�.
	eCAS_State_subCasSelected,		// sub cas�� ���õ� ����
	eCAS_State_CasOK,				// sub cas�� descramble ���°� OK��� �÷� �־���.
	eCAS_State_CasFail,				// sub cas�� descramble ���°� NOT OK��� �÷� �־���.
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
