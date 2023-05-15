/**************************************************************************************/
/**
 * @file oplappmgr.h
 *
 * Management of OIPF Application
 *
 **************************************************************************************
 **/

#ifndef __OPL_APPMGR_H__
#define __OPL_APPMGR_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
};

#include <list>
#include <string>

typedef enum {
	OPLHBBTVAPPLICATIONTYPE_UNDEFINED = 0,
	OPLHBBTVAPPLICATIONTYPE_AUTOSTART,
	OPLHBBTVAPPLICATIONTYPE_INDEPENDENT,
} eOplHbbTVApplicationType;

typedef enum {
	OPLWINDOW_OIPF		= 0,
	OPLWINDOW_HBBTV,
	OPLWINDOW_TOTAL
} eOplApplicationWindow;

typedef enum {
	OPLHUMAXAPPLICATION_OTVS = 0,
	OPLHUMAXAPPLICATION_OTVB,
	OPLHUMAXAPPLICATION_HBBTV,
	OPLHUMAXAPPLICATION_MHEG,
	OPLHUMAXAPPLICATION_ALL
} eOplHumaxApplicationType;

typedef void (HumaxApplicationListener)(int type, const char* app_name);

class OplApplicationManager {
public:
	OplApplicationManager() {}
	virtual ~OplApplicationManager() {}

	static void debug(const char *level, const char *log);
	static eOplApplicationWindow getApplicationWindow() { return s_app_window; }
	static void setApplicationWindow(eOplApplicationWindow window)
	{ s_app_window = window; }

protected:
	static eOplApplicationWindow	s_app_window;
};

class OplOIPFApplicationManager {
public:
	OplOIPFApplicationManager() {}
	virtual ~OplOIPFApplicationManager() {}

	static bool addTrustedApplication(const char *url);
	static bool removeTrustedApplication(const char *url);
	static void removeAllApplications(void);
	static bool findApplication(const char *url);
	static bool doTeletext(void);
	static void setHumaxApplicationListener(HumaxApplicationListener *listener );
	static void getHumaxApplications(int *count, char ***app_name );
	static void setHumaxApplicationLoad(eOplHumaxApplicationType app);
	static void setHumaxApplicationUnload(eOplHumaxApplicationType app);
	static void createHumaxApplication(const char* app_name, const char *url );
	static void destroyHumaxApplication(const char *app_name );
	static eOplHumaxApplicationType getHumaxApplicationType(const char *app_name );
	static void setNagraPrmStarted(void);
	static void setNagraPrmStopped(void);

private:
	static void showList();

private:
	static std::list<std::string>	s_trusted_apps;
};

class OplHbbTVApplicationManager {
public:
	OplHbbTVApplicationManager() {}
	virtual ~OplHbbTVApplicationManager() {}

	static bool addTrustedApplication(const char *url);
	static bool removeTrustedApplication(const char *url);
	static void removeAllApplications(void);
	static bool findApplication(const char *url);
	static bool createHbbTVApplication(eOplHbbTVApplicationType type);
	static bool destroyHbbTVApplication(void);
	static void setHasDigitalTeletext(bool value);
	static bool getHasDigitalTeletext(void);

private:
	static std::list<std::string>	s_trusted_apps;
	static bool s_has_digital_teletext;
	static bool s_has_activ_hbbtv;
};

#endif

#endif
