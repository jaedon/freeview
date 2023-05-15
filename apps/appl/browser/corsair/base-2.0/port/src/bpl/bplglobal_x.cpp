/**************************************************************************************/
/**
 * @file bplglobal.cpp
 *
 *
 *
 * @author
 * @date    2012/03/03
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "bplglobal.h"
#include "apk.h"
#include <hlib.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

void *BplGlobalConfig::s_browserHandle = NULL;
bool BplGlobalConfig::s_reactivate_ait = false;
bool BplGlobalConfig::s_mheg_status = false;
bool BplGlobalConfig::s_ait_do_not_send = false;
bool BplGlobalConfig::s_object_debuger_enable = false;
bool BplGlobalConfig::s_is_running_hbbtv_app = false;
unsigned int BplGlobalConfig::s_hbbtvAppCnt = 0;
BplGlobalConfig::hbbtv_applcation_type BplGlobalConfig::s_hbbtvAppType = BplGlobalConfig::HBBTV_APPLICATION_BI;


/*------------------------------------------------------------------------------
 *	class BplGlobalConfig
 *------------------------------------------------------------------------------*/
void *BplGlobalConfig::getBrowserHandle()
{
	return s_browserHandle;
}

void BplGlobalConfig::setBrowserHandle(void *handle)
{
	if ( handle == NULL ) return;

	s_browserHandle = handle;
}

bool BplGlobalConfig::getIsBroadcastRelated()
{
	if ( s_hbbtvAppCnt <= 0 )
	{
		return false;
	}

	if ( s_hbbtvAppType == HBBTV_APPLICATION_BI )
	{
		return false;
	}

	return true;
}

void BplGlobalConfig::setStartedHbbTVAppType(hbbtv_applcation_type type)
{

	s_hbbtvAppType = type;
	
	s_hbbtvAppCnt++;
}

void BplGlobalConfig::setChangedHbbTVAppType(hbbtv_applcation_type type)
{
	s_hbbtvAppType = type;
}

void BplGlobalConfig::setStoppedApp()
{
    s_hbbtvAppCnt --;
    if ( s_hbbtvAppCnt <= 0) 
	{
		s_hbbtvAppCnt = 0;
		s_hbbtvAppType = HBBTV_APPLICATION_BI;
	}
}


/*------------------------------------------------------------------------------
 *	class BplOperaGlobalConfig
 *------------------------------------------------------------------------------*/
const char *OPERA_HOME_PATH	= "/var/lib/humaxtv/browser/";
const char *OPERA_DIR_PATH	= "/usr/browser/opera_dir/";
const char *OPERA_FONT_PATH	= "/usr/fonts/";

const char *OPERA_HOME_ORG_PATH	= "/usr/browser/opera_home/";
const char *OPERA_HOME_VERSION_FILE	= "version.txt";

BplOperaGlobalConfig::BplOperaGlobalConfig()
	: m_operaHomePath(OPERA_HOME_PATH),
	m_operaDirPath(OPERA_DIR_PATH),
	m_operaFontPath(OPERA_FONT_PATH)
{
}

BplOperaGlobalConfig::~BplOperaGlobalConfig()
{
}

const char *BplOperaGlobalConfig::getOperaHomePath()
{
	return m_operaHomePath.c_str();
}

const char *BplOperaGlobalConfig::getOperaDirPath()
{
	return m_operaDirPath.c_str();
}

const char *BplOperaGlobalConfig::getOperaFontPath()
{
	return m_operaFontPath.c_str();
}

void BplOperaGlobalConfig::updateOperaHome()
{
	if ( access(OPERA_HOME_PATH, R_OK|W_OK) != 0 )
	{
		if ( mkdir(OPERA_HOME_PATH, 0755) == 0 )
		{
			copyOperaHome();
			setOperaEnv();
			cout << "---------------------------------------------------------" << endl;
			cout << "OPERA HOME is created as new one." << endl;
			cout << "---------------------------------------------------------" << endl;
		}
		return;
	}

	string currentVersion = getOperaHomeVersion(OPERA_HOME_PATH);
	if ( currentVersion == "" )
	{
		copyOperaHome();
		setOperaEnv();
		cout << "---------------------------------------------------------" << endl;
		cout << "OPERA HOME is updated becuase there is no version info." << endl;
		cout << "---------------------------------------------------------" << endl;
		return;
	}

	string newVersion = getOperaHomeVersion(OPERA_HOME_ORG_PATH);
	if ( newVersion == "" )
	{
		setOperaEnv();
		return;
	}

	cout << "Checking OperaHOME: current=" << currentVersion << " new=" << newVersion << endl;
	if ( strncmp(newVersion.c_str(), currentVersion.c_str(), 6)  > 0 )
	{
		copyOperaHome();
		setOperaEnv();
		cout << "---------------------------------------------------------" << endl;
		cout << "OPERA HOME is updated to latest version." << endl;
		cout << "---------------------------------------------------------" << endl;
		return;
	}

	setOperaEnv();
	cout << "Setting the Opera environment is finished." << endl;
}

void BplOperaGlobalConfig::setOperaEnv(void)
{
	setenv("OPERA_HOME", m_operaHomePath.c_str(), 0);
	setenv("OPERA_DIR", m_operaDirPath.c_str(), 0);
	setenv("OPERA_FONTS", m_operaFontPath.c_str(), 0);
	setenv("USE_VEGA_DFB", "YES", 0);
}

string BplOperaGlobalConfig::getOperaHomeVersion(const char *path)
{
	string versionPath = path;
	versionPath += "/";
	versionPath += OPERA_HOME_VERSION_FILE;

	std::fstream fin(versionPath.c_str(), std::ios_base::in);
	if ( !fin.is_open() ) return string("");

	char version[1024] = {0,};
	if ( !fin.getline(version, sizeof(version)) ) return string("");

	return string(version);
}

bool BplOperaGlobalConfig::copyOperaHome()
{
	/*
	 * TODO::
	 * This routine will be modified after implementing HLIB_DIR_Copy in hxdir.
	 */

	char copyCommand[1024] = {0,};
	snprintf(copyCommand, 1024, "cp -arf %s/* %s", OPERA_HOME_ORG_PATH, OPERA_HOME_PATH);

	int pid = vfork();
	if ( pid == -1 )
	{
		return false;
	}

	if ( pid == 0 )
	{
		char *argv[4];
		argv[0] = (char *)"sh";
		argv[1] = (char *)"-c";
		argv[2] = (char *)copyCommand;
		argv[3] = NULL;
		execve("/bin/sh", argv, environ);
		exit(127);
	}

	do
	{
		int status = 0;
		if (waitpid(pid, &status, 0) == -1)
		{
			if (errno != EINTR)
				return false;
		}
		else
		{
			return false;
		}
	} while(1);

	return false;
}

bool BplOperaGlobalConfig::setOperaUserAgentData(char *ua_data)
{
	HUINT32		ulViewId;

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_SetBrowserUserAgentData(ulViewId, ua_data);
	return true;
}

void BplGlobalConfig::setReactivateAit(bool reactivate)
{
	//cout << "=====> BplGlobalConfig::setReactivateAit=" << reactivate << endl;
	BplGlobalConfig::s_reactivate_ait = reactivate;
}

void BplGlobalConfig::setIsRunningHbbTVApp(bool is_running)
{
	//cout << "=====> BplGlobalConfig::setIsRunningHbbTVApp=" << is_running << endl;
	BplGlobalConfig::s_is_running_hbbtv_app = is_running;
}


