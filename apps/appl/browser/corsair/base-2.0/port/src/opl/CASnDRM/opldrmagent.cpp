/**************************************************************************************/
/**
 * @file opldrmagent.c
 *
 * DrmAgent
 *
 * @author  Sung-hoon Lee(shlee3@humaxdigital.com)
 * @date    2012/08/22
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 ****************************************************************************************/
#include "oapi.h"
#include "apk.h"
#include <hlib.h>

#include <stdlib.h>
#include <string.h>
//#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include "opldrmagent.h"

/******************************************************************
	Define
******************************************************************/
#define ENTRY HxLOG_Debug("[opldrmagent][%s:%d]++\n", __FUNCTION__, __LINE__);

/******************************************************************
	Typedef
******************************************************************/


/******************************************************************
	Static variables
******************************************************************/


/******************************************************************
	Static function prototypes
******************************************************************/


/******************************************************************
	Static functions
******************************************************************/
static unsigned short __urandom(void)
{
	int fd = open("/dev/urandom", O_RDONLY);
	unsigned short r = 0;
	ssize_t ret = -1;

	if (fd < 0) {
		HxLOG_Error("[OPL_DRMAGENT][%s] Error> could not open /dev/urandom\n", __FUNCTION__);
		return 0;
	}

	ret = read(fd, &r, sizeof (r));
	if (ret < 0) {
		HxLOG_Error("[OPL_DRMAGENT][%s] Error> could not read /dev/urandom\n", __FUNCTION__);
	}

	if (fd >= 0) close(fd);

	return (unsigned short)r;
}


/******************************************************************
	Interfaces
******************************************************************/
OplDrmAgentManager* OplDrmAgentManager::s_pThis = NULL;

OplDrmAgentManager & OplDrmAgentManager::getInstance()
{
	ENTRY;

	if(s_pThis == NULL)
	{
		s_pThis = new OplDrmAgentManager;
		APK_DRMAGENT_Init();
		APK_DRMAGENT_RegisterDrmMessageNotifier(onDRMAgentDRMMessage);
		APK_DRMAGENT_RegisterDrmStatusNotifier(onDRMAgentDRMStatusMessage);
	}

	return *s_pThis;
}
void OplDrmAgentManager::releaseInstance()
{
	ENTRY;

	if(s_pThis != NULL)
	{
		if(s_pThis->m_lDrmAgent.size() == 0)  // there is no itme
		{
			delete s_pThis;
			s_pThis = NULL;
		}
	}
}
OplDrmAgentManager::OplDrmAgentManager()
{
	ENTRY;

	pthread_mutex_init(&m_mtx, NULL);

}
OplDrmAgentManager::~OplDrmAgentManager()
{

	APK_DRMAGENT_RegisterDrmMessageNotifier(NULL);
	APK_DRMAGENT_RegisterDrmStatusNotifier(NULL);

	pthread_mutex_destroy(&m_mtx);
}


void OplDrmAgentManager::onDRMAgentDRMMessage(const HCHAR *pzResultMsg, HUINT16 usMsgId, HUINT32 ulResultCode)
{
	ENTRY;

	OplDrmAgentManager tmp = OplDrmAgentManager::getInstance();

	std::list<OplDrmAgent *>::iterator it;

	tmp.lock();
	for ( it=tmp.m_lDrmAgent.begin(); it!=tmp.m_lDrmAgent.end(); it++)
	{
		if((*it)->getMsgID(usMsgId)== true)
			if((*it)->m_pSendDRMCallback)
				(*it)->m_pSendDRMCallback(static_cast<void *>(*it), usMsgId, pzResultMsg, ulResultCode);
	}
	tmp.unlock();
}

void OplDrmAgentManager::onDRMAgentDRMStatusMessage(const HCHAR *pzDrmSystemId)
{
	ENTRY;

	OplDrmAgentManager tmp = OplDrmAgentManager::getInstance();

	std::list<OplDrmAgent *>::iterator it;
	tmp.lock();
	for ( it=tmp.m_lDrmAgent.begin(); it!=tmp.m_lDrmAgent.end(); it++ )
	{
		if((*it)->getDRMSystemID(pzDrmSystemId)== true)
			if((*it)->m_pSystemStatusCallback)
			{
				(*it)->m_pSystemStatusCallback((static_cast<void *>(*it)),pzDrmSystemId);
			}
	}
	tmp.unlock();
}

OplDrmAgent * OplDrmAgentManager::GetDRMAgent(const unsigned int a_MsgID)
{
	ENTRY;

	std::list<OplDrmAgent *>::iterator it;
	for ( it=m_lDrmAgent.begin(); it!=m_lDrmAgent.end(); it++)
	{
		if((*it)->getMsgID(a_MsgID))
			return (*it);
	}
	return NULL;

}

OplDrmAgent * OplDrmAgentManager::GetDRMAgent(const char *pzDrmSystemId)
{
	ENTRY;

	std::list<OplDrmAgent *>::iterator it;
	for ( it=m_lDrmAgent.begin(); it!=m_lDrmAgent.end(); it++)
	{
		if((*it)->getDRMSystemID(pzDrmSystemId))
			return (*it);
	}
	return NULL;

}
OplDrmAgent* OplDrmAgentManager::CreateDRMAgent()
{
	ENTRY;
	OplDrmAgent	*obj = new OplDrmAgent;

	if(obj)
		m_lDrmAgent.push_back(obj);
	return obj;
}
BPLStatus OplDrmAgentManager::AddDRMAgent(OplDrmAgent *a_Agent)
{
	ENTRY;

	if(a_Agent)
		m_lDrmAgent.push_back(a_Agent);

	return BPL_STATUS_OK;
}
BPLStatus OplDrmAgentManager::RemoveDRMAgent(OplDrmAgent *a_Agent)
{
	ENTRY;

	if(!a_Agent) return BPL_STATUS_ERROR;
	std::list<OplDrmAgent *>::iterator it;
	for ( it=m_lDrmAgent.begin(); it!=m_lDrmAgent.end(); it++)
	{
		if(*it == a_Agent)
		{
			it= m_lDrmAgent.erase(it);
			return	BPL_STATUS_OK;
		}
	}

	return BPL_STATUS_ERROR;

}

void OplDrmAgentManager::lock()
{
	pthread_mutex_lock(&m_mtx);

}
void OplDrmAgentManager::unlock()
{
	pthread_mutex_unlock(&m_mtx);

}

OplDrmAgent::OplDrmAgent()
	:
	m_pSendDRMCallback(NULL),
	m_pSystemStatusCallback(NULL)
{
}
OplDrmAgent::OplDrmAgent(OplDASendDRMtListener_t SDcallback , OplDASystemstatustListener_t SScallback)
	:
	m_pSendDRMCallback(SDcallback),
	m_pSystemStatusCallback(SScallback)
{
}

OplDrmAgent::~OplDrmAgent()
{
	m_pSendDRMCallback=NULL;
	m_pSystemStatusCallback= NULL;
//	m_status = 0;
}

BPLStatus OplDrmAgent::sendDrmMessage(const char *msgType, const char *msg, const char *DRMSystemID, unsigned short &msgId)
{
	ENTRY;
	//struct timeval tp;

	//gettimeofday(&tp, NULL);
	//srandom(tp.tv_sec | tp.tv_usec);
	//msgId = (unsigned short)random();

	msgId = __urandom();

	m_nMsgID.push_back(msgId);
	HxLOG_Print("[%s:%d] msgType : %s \n", __FUNCTION__, __LINE__, msgType);
	HxLOG_Print("[%s:%d] msg: %s \n", __FUNCTION__, __LINE__, msg);
	HxLOG_Print("[%s:%d] msgType : %s \n", __FUNCTION__, __LINE__, DRMSystemID);
	APK_DRMAGENT_SendDrmMessage(msgType, msg, DRMSystemID,msgId);  //error need

	return BPL_STATUS_OK;
}

void OplDrmAgent::setDrmAgentResultEventListener(OplDASendDRMtListener_t SDlistener, OplDASystemstatustListener_t SSlistener)
{
	m_pSendDRMCallback = SDlistener;
	m_pSystemStatusCallback = SSlistener;
}

BPLStatus OplDrmAgent::getDrmSystemStatus(const char *DRMSystemID, eOplDrmAgentDrmSystemStatus_t &peStatus)
{
	ENTRY;

	m_DRMSystemID.push_back(DRMSystemID);
	APK_DRMAGENT_SendDrmSystemStatusRequest(DRMSystemID);
	peStatus = DRM_SYSTEM_STATUS_UNKNOWN;

	return BPL_STATUS_OK;
}


bool OplDrmAgent::getMsgID(unsigned short a_msgid)
{

	std::list<unsigned short>::iterator it;
	for ( it=m_nMsgID.begin(); it!=m_nMsgID.end(); it++ )
	{
		if((*it)== a_msgid)
			return true;
	}
	return false;
}

bool OplDrmAgent::getDRMSystemID(const HCHAR * pzDrmSystemId)
{
	std::list<const char *>::iterator it;
	for ( it=m_DRMSystemID.begin(); it!=m_DRMSystemID.end(); it++)
	{
		if(strcmp((*it), pzDrmSystemId) ==0)
			return true;
	}
	return false;
}
