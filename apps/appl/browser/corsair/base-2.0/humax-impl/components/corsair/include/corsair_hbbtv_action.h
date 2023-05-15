/** **********************************************************************************************************
	@file 		corsair_hbbtv_action.h

	@date		2014/12/04
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

#ifndef __CORSAIR_HBBTV_ACTION_H__
#define __CORSAIR_HBBTV_ACTION_H__

#include "hbbtv_api.h"
#include "gogi_opera_api.h"

#include "corsair_hbbtv_application.h"

#include <vector>



class CorsairHbbTVManager;
class CorsairHbbTVApp;
/**************************************************************************************
 * class CorsairHbbTVAction
 **************************************************************************************/
class CorsairHbbTVAction {
public:
	CorsairHbbTVAction();
	virtual ~CorsairHbbTVAction();
	virtual void execute() {}

protected:
	static bool s_is_started_mheg;
public:
	static unsigned int sizeofapps() { return s_hbbtv_active_apps.size() > 0; }
	static void showActiveApps();
	static CorsairHbbTVApp *getRunningApp(CorsairHbbTVApp::eHbbTVApplicationType type);

protected:
	static std::vector<CorsairHbbTVApp *>	s_hbbtv_active_apps;
};

class CorsairHbbTVStartAction : public CorsairHbbTVAction {
public:
	CorsairHbbTVStartAction(CorsairHbbTVApp::eHbbTVApplicationType type, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVStartAction();
};

class CorsairHbbTVStopAction : public CorsairHbbTVAction {
public:
	CorsairHbbTVStopAction(CorsairHbbTVApp::eHbbTVApplicationType type, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVStopAction();
};

class CorsairHbbTVStartedAction : public CorsairHbbTVAction {
public:
	CorsairHbbTVStartedAction(HBBTV_NotificationApplicationData *data, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVStartedAction();
};

class CorsairHbbTVStoppedAction : public CorsairHbbTVAction {
public:
	CorsairHbbTVStoppedAction(HBBTV_NotificationApplicationData *data, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVStoppedAction();
};

class CorsairHbbTVUpdateAction : public CorsairHbbTVAction {
public:
	CorsairHbbTVUpdateAction(HBBTV_NotificationApplicationData *data, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVUpdateAction();
};

class CorsairHbbTVNewAITAction : public CorsairHbbTVAction {
public:
	CorsairHbbTVNewAITAction(HBBTV_AITUpdateNotification *data, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVNewAITAction();
};

class CorsairHbbTVShowAction : public CorsairHbbTVAction {
public:
	CorsairHbbTVShowAction();
	CorsairHbbTVShowAction(GogiOperaWindow *gogi_window, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVShowAction();
};

class CorsairHbbTVHideAction : public CorsairHbbTVAction {
public:
	CorsairHbbTVHideAction();
	CorsairHbbTVHideAction(GogiOperaWindow *gogi_window, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVHideAction();
    virtual void execute();
};

class CorsairHbbTVExitAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVExitAction(CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVExitAction();
};

class CorsairHbbTVBRExitAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVBRExitAction();
	virtual ~CorsairHbbTVBRExitAction();
};


class CorsairHbbTVBIExitAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVBIExitAction();
	virtual ~CorsairHbbTVBIExitAction();
};

class CorsairHbbTVChannelChangedAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVChannelChangedAction(CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVChannelChangedAction();
};

class CorsairHbbTVClearAppListAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVClearAppListAction(CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVClearAppListAction();
};

class CorsairHbbTVNetworkUpdateAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVNetworkUpdateAction(CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVNetworkUpdateAction();
};

class CorsairHbbTVTeletextAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVTeletextAction(CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVTeletextAction();
};

class CorsairHbbTVErrorAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVErrorAction(GogiOperaWindow *gogi_window, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVErrorAction();
};

class CorsairHbbTVBIAppStartAction: public CorsairHbbTVAction {
public:
	typedef enum {
		HBBTV_BIAPP_NONE = 0,
		HBBTV_BIAPP_START_NOTOFFLIVE
	} eHbbTVBIAppFlag;
public:
	CorsairHbbTVBIAppStartAction(const char *url, CorsairHbbTVManager *hm);
	CorsairHbbTVBIAppStartAction(const char *url, CorsairHbbTVManager *hm, eHbbTVBIAppFlag flag);
	virtual ~CorsairHbbTVBIAppStartAction();

private:
	eHbbTVBIAppFlag m_flag;
};

#if defined(CONFIG_PROD_FVP4000T)
class CorsairHbbTVBIMhegAppStatus: public CorsairHbbTVAction {
public:
	typedef enum {
		HBBTV_BIAPP_NOT_RUNNING = 0,
		HBBTV_BIAPP_RUNNING,
		HBBTV_BIAPP_STOPPING,
	} eHbbTVBIAppStatus;	
public:
	CorsairHbbTVBIMhegAppStatus(int status, CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVBIMhegAppStatus();

private:
	eHbbTVBIAppStatus m_status;
};
#endif
class CorsairHbbTVDoNotReactivateAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVDoNotReactivateAction();
	virtual ~CorsairHbbTVDoNotReactivateAction();
};

class CorsairHbbTVDoReactivateAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVDoReactivateAction(CorsairHbbTVManager *hm);
	virtual ~CorsairHbbTVDoReactivateAction();
};

class CorsairHbbTVVodStopAction: public CorsairHbbTVAction {
public:
	CorsairHbbTVVodStopAction();
	virtual ~CorsairHbbTVVodStopAction();
	virtual void execute();
};
    
#endif
