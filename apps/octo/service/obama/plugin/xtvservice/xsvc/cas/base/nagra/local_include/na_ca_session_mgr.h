/*
@file				na_ca_session_mgr.h
@brief				na_ca_session_mgr.h (Nagra CAK Service)

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef _CA_SESSION_MGR_H_
#define _CA_SESSION_MGR_H_

// �׽�Ʈ�� �Ϸ�Ǹ� MULTI_DIRECT_TSID�� enable�� ���·� �ҽ��������� 2015.11.30
// Multiscreen�� �����ϱ����� �ϳ��� stream���� decryption -> encryption �� �ΰ� ��� ó���ؾ�������
// decryption�� no direct�� encryption�� direct�� transportSessionId�� �������־���Ѵ�.
#if defined(CONFIG_SUPPORT_NOVA_APP)
#define MULTI_DIRECT_TSID
#endif

/********************************************************************
  Structure and Type Definition
 ********************************************************************/
typedef TUnsignedInt32 TSessionManagerStatus;
typedef TUnsignedInt32 NEXUS_KeySlotHandle;
typedef TUnsignedInt32 NEXUS_ParserBand;
typedef TUnsignedInt32 NEXUS_PlaypumpHandle;

typedef TSessionManagerStatus (*BrcmCaStreamSessionKeyHandleCallback)(TTransportSessionId xTSid, NEXUS_KeySlotHandle m2mKeyHandle);
typedef TSessionManagerStatus (*BrcmCaCipherSessionKeyLoadedCallback)(TTransportSessionId xTSid, NEXUS_KeySlotHandle m2mKeyHandle);


/********************************************************************
  Extern Function Declaration
 ********************************************************************/
#if defined (CONFIG_MW_CAS_NAGRA_PRM_3_1)
HERROR	caSessionMgr_AddTransportSessionIdDirect(TTransportSessionId xTsId);
HERROR	caSessionMgr_RemoveTransportSessionIdDirect(TTransportSessionId xTsId);
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR	 caSessionMgr_SendActionHdlToNova(Handle_t hAction, Handle_t hSvcHdl);
HERROR	 caSessionMgr_SendDrmHandleToNova(Handle_t hAction, Handle_t hSvcHdl);
#endif

HERROR	caSessionMgr_AddTransportSessionId(Handle_t hAction);
HERROR	caSessionMgr_RemoveTransportSessionId(Handle_t hAction);
HERROR	caSessionMgr_AssociatePids(Handle_t hAction, HUINT32 ulNumOfPid, HUINT16 *pusPids);
HERROR	caSessionMgr_AssociateIv(Handle_t hAction, const TUnsignedInt8 *pxIv, size_t xIvSize);
HERROR	caSessionMgr_GetKeyslotHandle(Handle_t hAction, Handle_t *phKeySlot);
#if defined(MULTI_DIRECT_TSID)
HBOOL	CA_NA_CaSessionMgrIsTsIdSetDirect(TTransportSessionId xTransportSessionId);
#else
HBOOL	CA_NA_CaSessionMgrIsTsIdSetDirect(void);
#endif
HERROR	CA_NA_CaSessionMgrInitialize(void);

#endif /* _CA_SESSION_MGR_H_ */

