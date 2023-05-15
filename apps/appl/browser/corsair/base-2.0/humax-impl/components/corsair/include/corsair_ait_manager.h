/** **********************************************************************************************************
	@file 		corsair_ait_manager.h

	@date		2012/09/13
	@author		IPM-SW1T(tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#ifndef __CORSAIR_AIT_MANAGER_H__
#define __CORSAIR_AIT_MANAGER_H__

#include "bplait.h"
#include "hbbtv_api.h"
#include "corsair_config.h"
#include "oplbroadcast.h"

/** **********************************************************************************************************
	@class	CorsairAITManager
 	@brief 	this class manage the information of AIT to controll hbbtv application
*********************************************************************************************************** */
using namespace std;

class AITSection;

class CorsairAITManager {
public:
	static CorsairAITManager* getInstance();
	static void destroyInstance();

	static void init( );
	static void shutdown();
	static void suspend();
	static void resume();
	static void removeInvalidAIT();
	static void notifyCompletedToLoadAIT(unsigned char version_no);
	static void eventHandler( eBplAitEvent eEvent, unsigned int ulParam );
	static void showAITInfo();
	static void resendAIT();
    static void resetAITDoneStatus();
    static void cbNetworkConnetionTimer(unsigned long TimerId, void *params);
    static void setNetworkConnectionStatus(bool connect);

	AITSection* getCurrentAITSection(){ return m_latest_aitsection; }
	HBBTV_EventData* createEventData(HBBTV_EventType type,
											bool isNone=false,
											unsigned int section_len=0,
											unsigned char *section=NULL);
private:
	CorsairAITManager();
	virtual ~CorsairAITManager();

	void sendAITEvent(bool force=false);
	void sendAITNoneEvent(void);
	void sendAITInvalidEvent(void);
	bool checkServiceChanged(void);
	void reactivateAITEvent(bool active);
	void setActivation(bool active ) { m_active = active; }
	bool getActivation(void) { return m_active; }
     /**
        * @brief in MHEG and HBBTV coexistence case , Our Device can not manage prority to execute. \
        *   So, corsair ait module should have scenario code to manage prority to execute temporarily \
        *   That is m_aitdone flag \
        *   if you wanna know more detail infomation, please refer to the FVP - Technical Specification v1.2.1.pdf  part 5.2.1 \
        */
    void setAitDoneStatus(bool status)  { m_aitdone = status; }
    bool getAitDoneStatus(void) { return m_aitdone; }

private:
	static CorsairAITManager *m_instance;
	AITSection *m_latest_aitsection;
	int m_active;
    bool m_aitdone;
	BplAitService_t	m_latest_service;
	eBplAitEvent m_latest_event;
    unsigned long s_ulDhcpTimer;
};

#endif	// __CORSAIR_AIT_MANAGER_H__
