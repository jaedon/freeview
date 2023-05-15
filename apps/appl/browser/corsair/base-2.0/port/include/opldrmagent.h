/**************************************************************************************/
/**
 * @file opldrmagent.h
 *
 * drmAgent
 *
 * @author  Sung-hoon Lee(shlee3@humaxdigital.com)
 * @date    2012/08/22
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************/

#ifndef __OPLDRMAGENT_H__
#define __OPLDRMAGENT_H__

#include "opltype.h"
#include "bpltype.h"

#ifdef __cplusplus
#include <list>
#endif
#include <pthread.h>


/******************************************************************
	Define
******************************************************************/


/******************************************************************
	Enum
 ******************************************************************/

 typedef enum{
	 DRM_SYSTEM_STATUS_READY,
	 DRM_SYSTEM_STATUS_UNKNOWN,
	 DRM_SYSTEM_STATUS_INITIALISING,
	 DRM_SYSTEM_STATUS_ERROR,
 }eOplDrmAgentDrmSystemStatus_t;

/******************************************************************
	Typedef
******************************************************************/
typedef void (*OplDASendDRMtListener_t)(void *hDrmAgent, unsigned short pzMsgId, const char *pzResultMsg, 	unsigned int ulResultCode);

typedef void (*OplDASystemstatustListener_t)(void *hDrmAgent,const char *pzDrmSystemId);

/******************************************************************
	class OplDrmAgentManager
******************************************************************/
class OplDrmAgent;

using namespace std;

class OplDrmAgentManager
{

public:
	static OplDrmAgentManager & getInstance();
	static void releaseInstance();

	OplDrmAgent* CreateDRMAgent();
	BPLStatus AddDRMAgent(OplDrmAgent *);
	BPLStatus RemoveDRMAgent(OplDrmAgent *);

	OplDrmAgent * GetDRMAgent(const unsigned int MsgID);
	OplDrmAgent * GetDRMAgent(const char *pzDrmSystemId);

	void lock();
	void unlock();

private:

	OplDrmAgentManager();
	~OplDrmAgentManager();

	static void onDRMAgentDRMMessage(const char *pzResultMsg, unsigned short usMsgId,	unsigned int ulResultCode);
	static void onDRMAgentDRMStatusMessage(const char *pzDrmSystemId);


private:

	std::list<OplDrmAgent *>	m_lDrmAgent;
	static OplDrmAgentManager *	s_pThis;
	pthread_mutex_t 	m_mtx;

};


/******************************************************************
	class OplDrmAgent
******************************************************************/

class OplDrmAgent
{

public:
	OplDrmAgent();
	OplDrmAgent(OplDASendDRMtListener_t SDcallback , OplDASystemstatustListener_t SScallback);

	virtual ~OplDrmAgent();

	BPLStatus sendDrmMessage(const char *msgType, const char *msg, const char *DRMSystemID, unsigned short &msgId);

	BPLStatus getDrmSystemStatus(const char *DRMSystemID, eOplDrmAgentDrmSystemStatus_t &peStatus);

	void setDrmAgentResultEventListener(OplDASendDRMtListener_t SDlistener, OplDASystemstatustListener_t SSlistener);

	bool getMsgID(unsigned short a_msgid);
	bool getDRMSystemID(const char * pzDrmSystemId);

	OplDASendDRMtListener_t m_pSendDRMCallback;
	OplDASystemstatustListener_t m_pSystemStatusCallback;

private:
	std::list<unsigned short> m_nMsgID;

	std::list<const char *> m_DRMSystemID;

//	eOplDrmAgentDrmSystemStatus_t m_status;

};



#endif

