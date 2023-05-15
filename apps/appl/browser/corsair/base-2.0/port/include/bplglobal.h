/**************************************************************************************/
/**
 * @file bplglobal.h
 *
 * 
 *
 * @author  
 * @date    2012/02/02
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __BPL_GLOBAL_CONFIG_H__
#define __BPL_GLOBAL_CONFIG_H__

#include "bpltype.h"
#include <string>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};

/*------------------------------------------------------------------------------
 *	class BplGlobalConfig
 *------------------------------------------------------------------------------*/
class BplGlobalConfig {
public:
	BplGlobalConfig() {}
	virtual ~BplGlobalConfig() {}

public:
	static void *getBrowserHandle();
	static void setBrowserHandle(void *handle);
	static void setReactivateAit(bool reactivate);
	static bool getReactivateAit(void) { return s_reactivate_ait; };
	static void setIsRunningHbbTVApp(bool is_running);
	static bool getIsRunningHbbTVApp(void) { return s_is_running_hbbtv_app; };

	/**
	 *@note Below interfaced are needed for hbbtv security in humax side. 
	 */
	typedef enum {
		HBBTV_APPLICATION_BI    = 0,
		HBBTV_APPLICATION_BR
	} hbbtv_applcation_type;

	static bool getIsBroadcastRelated();
	static void setStartedHbbTVAppType(hbbtv_applcation_type type);
	static void setChangedHbbTVAppType(hbbtv_applcation_type type);
	static void setStoppedApp();
	
public:
	static void 		*s_browserHandle;
	static bool 		s_reactivate_ait;
	static bool			s_mheg_status;
	static bool 		s_ait_do_not_send;
	static bool			s_object_debuger_enable;
	static bool 		s_is_running_hbbtv_app;

	static unsigned int s_hbbtvAppCnt;
	static hbbtv_applcation_type s_hbbtvAppType;
};

/*------------------------------------------------------------------------------
 *	class BplOperaGlobalConfig
 *------------------------------------------------------------------------------*/
class BplOperaGlobalConfig : public BplGlobalConfig {
public:
	BplOperaGlobalConfig();
	virtual ~BplOperaGlobalConfig();

	const char *getOperaHomePath();
	const char *getOperaDirPath();
	const char *getOperaFontPath();

	void updateOperaHome(void);

	bool setOperaUserAgentData(char *ua_data);

private:
	void setOperaEnv(void);
	string getOperaHomeVersion(const char *path);
	bool copyOperaHome();

private:
	string		m_operaHomePath;
	string		m_operaDirPath;
	string		m_operaFontPath;
};

#endif

#endif

