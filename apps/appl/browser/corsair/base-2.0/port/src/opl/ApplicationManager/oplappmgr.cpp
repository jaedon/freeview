/**************************************************************************************/
/**
 * @file oplappmgr.cpp
 *
 * Management of OIPF Application
 *
 **************************************************************************************
 **/
#include "oplappmgr.h"

#include "hapi.h"

#include <hlib.h>
#include <apk.h>
#include <string>
#include <iostream>

using namespace std;


/**********************************************************************************
 * class OplApplicationManager
 ***********************************************************************************/
eOplApplicationWindow	OplApplicationManager::s_app_window;

void OplApplicationManager::debug(const char *level, const char *log)
{
	(void)level;
	{
		static int	s_setLevel;
		if (!s_setLevel)
		{
			s_setLevel = 1;
		}
	}
	HxLOG_Print("WEBAPP : %s", log);
}

/**********************************************************************************
 * class OplOIPFApplicationManager
 ***********************************************************************************/
list<string> OplOIPFApplicationManager::s_trusted_apps;

bool OplOIPFApplicationManager::addTrustedApplication(const char *url)
{
	if ( url == NULL ) return false;
	if ( findApplication(url) ) return false;	/* the url has already been registered. */

	s_trusted_apps.push_back(url);

//	printAll();

	return true;
}

bool OplOIPFApplicationManager::removeTrustedApplication(const char *url)
{
	if ( url == NULL ) return false;

	list<string>::iterator it;
	for ( it = s_trusted_apps.begin(); it!=s_trusted_apps.end(); it++ )
	{
		if ( (*it).find(url) != string::npos )
		{
			s_trusted_apps.remove(*it);
			return true;
		}
	}

	return false;
}

void OplOIPFApplicationManager::removeAllApplications(void)
{
	s_trusted_apps.clear();
}

bool OplOIPFApplicationManager::findApplication(const char *url)
{
	if ( url == NULL ) return false;

	showList();

	list<string>::iterator it;
	for ( it = s_trusted_apps.begin(); it!=s_trusted_apps.end(); it++ )
	{
		string str(url);
		if ( (*it).find(str) != string::npos ) return true;
		if ( str.find(*it) != string::npos ) return true;
	}

	return false;
}

void OplOIPFApplicationManager::showList()
{
	HxLOG_Print("OplApplicationManager:: All applications\n");
	HxLOG_Print("------------------------------------------------------\n");
	list<string>::iterator it;
	for ( it = s_trusted_apps.begin(); it!=s_trusted_apps.end(); it++ )
	{
		HxLOG_Print("Application = %s\n", (*it).c_str());
	}
	HxLOG_Print("------------------------------------------------------\n");
}

bool OplOIPFApplicationManager::doTeletext(void)
{
	HAPI_BroadcastSignal("signal:onTeletextRun", 0, "i", 1);
	return false;
}

/* workaround for TV apps.

	when user start the OTVS/OTVB/HbbTV service on TV Apps. asteriks hook up the url, then start each application on matching window.
	and it stored to remomber which application is created via asteriks's createApplicationListener.
	So when the OTVS/OTVB/HbbTV is terminated, this value shall be set to 0.
	search with this function name,  to know the flow.
*/

typedef struct{
	const char* 	app_name;
	bool			loaded;
}HumaxAppState;

#define APP_NAME_HBBTV "HbbTV Application"
#define APP_NAME_OTVS "Opera TV Store"
#define APP_NAME_OTVB "Opera TV Browser"
#define APP_NAME_MHEG "Mheg Application"


static HumaxAppState s_HumaxApplication[OPLHUMAXAPPLICATION_ALL] = {
	{APP_NAME_OTVS, 0},
	{APP_NAME_OTVB, 0},
	{APP_NAME_HBBTV, 0},
	{APP_NAME_MHEG, 0}
};
static HumaxApplicationListener *humax_app_listerner = NULL;

void OplOIPFApplicationManager::setHumaxApplicationListener(HumaxApplicationListener *listener )
{
	humax_app_listerner = listener;
}

eOplHumaxApplicationType OplOIPFApplicationManager::getHumaxApplicationType(const char* app_name)
{
	if( app_name == NULL )
		return OPLHUMAXAPPLICATION_ALL;

	if(strcmp( app_name, "otvs" ) == 0 )
		return 	OPLHUMAXAPPLICATION_OTVS;
	if(strcmp( app_name, "otvb" ) == 0 )
		return 	OPLHUMAXAPPLICATION_OTVB;
	if(strcmp( app_name, "hbbtv" ) == 0 )
		return 	OPLHUMAXAPPLICATION_HBBTV;
	if(strcmp( app_name, "mheg" ) == 0 )
			return	OPLHUMAXAPPLICATION_MHEG;

	return OPLHUMAXAPPLICATION_ALL;
}

void OplOIPFApplicationManager::getHumaxApplications(int *count, char ***app_name )
{
	int i, j;
		*count = 0;
	*app_name = NULL;

	for( i = 0; i < OPLHUMAXAPPLICATION_ALL; i++ )
	{
		if ( s_HumaxApplication[i].app_name != NULL && s_HumaxApplication[i].loaded )
			(*count)++;
	}

	if ( *count > 0 )
	{
		*app_name = (char **)malloc(sizeof(char *) * (*count));
		if(*app_name != NULL)
		{
			for( i = 0, j = 0; i < OPLHUMAXAPPLICATION_ALL; i++ )
			{
				if ( s_HumaxApplication[i].app_name != NULL && s_HumaxApplication[i].loaded )
					*app_name[j++] = strdup(s_HumaxApplication[i].app_name);
			}
		}
	}
	else
	{
		*app_name = NULL;
	}
}

void OplOIPFApplicationManager::setHumaxApplicationLoad(eOplHumaxApplicationType app)
{
	if(!humax_app_listerner)
		return;

	s_HumaxApplication[app].loaded = true;
	humax_app_listerner(1/*MANAGER_EVENT_APPLICATION_LOADED*/, s_HumaxApplication[app].app_name );
}

void OplOIPFApplicationManager::setHumaxApplicationUnload(eOplHumaxApplicationType app)
{
	if(!humax_app_listerner)
		return;

	s_HumaxApplication[app].loaded = false;
	humax_app_listerner(2/*MANAGER_EVENT_APPLICATION_UNLOADED*/, s_HumaxApplication[app].app_name );
}

void OplOIPFApplicationManager::createHumaxApplication(const char* app_name, const char *url )
{
	const char* app = NULL;

	if ( strcmp(APP_NAME_HBBTV, app_name) == 0 )
	{
		app = "hbbtv";
	}
	else if ( strcmp(APP_NAME_OTVS, app_name) == 0 )
	{
		app = "otvs";
	}
	else if ( strcmp(APP_NAME_OTVB, app_name) == 0 )
	{
		app = "otvb";
	}
	else if ( strcmp(APP_NAME_MHEG, app_name) == 0 )
	{
		app = "mheg";
	}

	if ( app != NULL )
		HAPI_BroadcastSignal("signal:onHumaxApplicationCreate", 0, "ss", app, url);

	return;
}

void OplOIPFApplicationManager::destroyHumaxApplication(const char* app_name )
{
	const char* app = NULL;

	if ( strcmp(APP_NAME_HBBTV, app_name) == 0 )
	{
		app = "hbbtv";
	}
	else if ( strcmp(APP_NAME_OTVB, app_name) == 0 )
	{
		app = "otvb";
	}
	else if ( strcmp(APP_NAME_OTVS, app_name) == 0 )
	{
		app = "otvs";
	}
	else if ( strcmp(APP_NAME_MHEG, app_name) == 0 )
	{
		app = "mheg";
	}

	if ( app != NULL )
	{
		HAPI_BroadcastSignal("signal:onHumaxApplicationDestroy", 0, "s", app);
	}

	return;
}

void OplOIPFApplicationManager::setNagraPrmStarted()
{
	if(!humax_app_listerner)
		return;

	humax_app_listerner(15/*MANAGER_EVENT_APPLICATION_NAGRA_PRM_START*/, "NagraPrmStarted");
}
void OplOIPFApplicationManager::setNagraPrmStopped()
{
	if(!humax_app_listerner)
		return;

	humax_app_listerner(16/*MANAGER_EVENT_APPLICATION_NAGRA_PRM_STOP*/, "NagraPrmStopped");
}

/* end */

/**********************************************************************************
 * class OplHbbTVApplicationManager
 ***********************************************************************************/
list<string> OplHbbTVApplicationManager::s_trusted_apps;
bool OplHbbTVApplicationManager::s_has_activ_hbbtv = false;
bool OplHbbTVApplicationManager::s_has_digital_teletext = false;

bool OplHbbTVApplicationManager::addTrustedApplication(const char *url)
{
	if ( url == NULL ) return false;
	if ( findApplication(url) ) return false;	/* the url has already been registered. */

	s_trusted_apps.push_back(url);

	//printAll();

	return true;
}

bool OplHbbTVApplicationManager::removeTrustedApplication(const char *url)
{
	if ( url == NULL ) return false;

	list<string>::iterator it;
	for ( it = s_trusted_apps.begin(); it!=s_trusted_apps.end(); it++ )
	{
		if ( (*it).find(url) != string::npos )
		{
			s_trusted_apps.remove(*it);
			return true;
		}
	}

	return false;
}

void OplHbbTVApplicationManager::removeAllApplications(void)
{
	s_trusted_apps.clear();
}

bool OplHbbTVApplicationManager::findApplication(const char *url)
{
	if ( url == NULL ) return false;

	list<string>::iterator it;

	for ( it = s_trusted_apps.begin(); it!=s_trusted_apps.end(); it++ )
	{
		string str(url);
		if ( (*it).find(str) != string::npos ) return true;
		if ( str.find(*it) != string::npos ) return true;
	}

	return false;
}

bool OplHbbTVApplicationManager::createHbbTVApplication( eOplHbbTVApplicationType type )
{
	switch( type )
	{
		case OPLHBBTVAPPLICATIONTYPE_AUTOSTART:
		case OPLHBBTVAPPLICATIONTYPE_INDEPENDENT:
			HAPI_BroadcastSignal("signal:onHbbTVAppStart", 0, "i", type);
			break;

		default:
			OplHbbTVApplicationManager::destroyHbbTVApplication();
			break;
	}
	return false;
}

bool OplHbbTVApplicationManager::destroyHbbTVApplication()
{
	HAPI_BroadcastSignal("signal:onHbbTVAppStart", 0, "i", OPLHBBTVAPPLICATIONTYPE_UNDEFINED);

	return false;
}

void OplHbbTVApplicationManager::setHasDigitalTeletext(bool value)
{
	s_has_digital_teletext = value;
}

bool OplHbbTVApplicationManager::getHasDigitalTeletext(void)
{
	return s_has_digital_teletext;
}

