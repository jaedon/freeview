/** **********************************************************************************************************
	@file 		corsair_hbbtv_manager.h

	@date		2014/11/29
	@author
	@breif

	(c) 2014 Humax Co., Ltd.
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

#ifndef __CORSAIR_HBBTV_MANAGER_H__
#define __CORSAIR_HBBTV_MANAGER_H__

#include "corsair_hbbtv_application.h"
#include "corsair_hbbtv_action.h"

#include <vector>
#include <pthread.h>

/**************************************************************************************
 * class CorsairHbbTVManager
 **************************************************************************************/
class CorsairHbbTVManager {
public:
	friend class CorsairHbbTVChannelChangedAction;
	friend class CorsairHbbTVNetworkUpdateAction;
	friend class CorsairHbbTVTeletextAction;

public:
	typedef enum {
		TELETEXT_STATE_LIVE     = 0,
		TELETEXT_STATE_DIGITAL,
		TELETEXT_STATE_STANDARD
	} eTeletextState;

	typedef enum {
		HBBTV_APP_STATE_START		= 0,
		HBBTV_APP_STATE_STOP,
		HBBTV_APP_STATE_UPDATE
	} eHbbTVAppState;

public:
	CorsairHbbTVManager();
	virtual ~CorsairHbbTVManager();

	void addHbbTVApplication(CorsairHbbTVApp *app);
	void removeHbbTVApplication(unsigned short appid, unsigned long orgid);
	CorsairHbbTVApp *getApplication(CorsairHbbTVApp::eHbbTVApplicationType type);
	CorsairHbbTVApp *getRunningApplication(CorsairHbbTVApp::eHbbTVApplicationType type);
	bool isRunningApplication();
	void action(CorsairHbbTVAction *action);
	void showApps();
	void resetTeletext();
	void clearApps();
	void setHbbTVApplicationFromHumax(bool load);
	bool getHbbTVApplicationFromHumax();
	void setNagraPRMStatus(bool enable);
	bool getNagraPRMStatus(void);
    bool isBroadcastRelatedApplicationFromXMLAIT(HBBTV_AITUpdateNotification *data);

	/* action queue */
	bool addAction(CorsairHbbTVAction *action);
	bool executeAllActions(void);
    bool clearActions(void);

private:
	void addBIApplication();

	void setTeletext(eTeletextState state);
	void runTeletext();
	bool startDigitalTeletext();
	bool startStandardTeletext();
	void comebackLive();
	void lock(void) { pthread_mutex_lock(&m_mtx); }
	void unlock(void) { pthread_mutex_unlock(&m_mtx); }

private:
	std::vector<CorsairHbbTVApp *>	m_hbbtv_apps;
	std::vector<CorsairHbbTVAction *> m_hbbtv_actions;
	eTeletextState m_teletext_state;
	bool m_net_connected;
	bool m_prm_status;
	bool m_hbbtv_from_humax;
	pthread_mutex_t m_mtx;
};

#endif
