/**************************************************************************************/
/**
 * @file oplwebappupgrade.h
 *
 * WebApp Upgrade Interfaces
 *
 **************************************************************************************
 **/

#ifndef __OPL_WEBAPPUPGRADE_H__
#define __OPL_WEBAPPUPGRADE_H__

#include "bpltype.h"

typedef void *OPL_WebAppUpgradeHandle;
typedef void (*OplWebAppUpgradeCallback)(int event, int value);


class OplWebAppUpgrade {
private:
	OPL_WebAppUpgradeHandle m_hOplWebAppUpgradeHandle;			// handle to use communicating with AppKit. delete it if no needed!!!
	OplWebAppUpgradeCallback m_pfOplWebAppUpgradeResultListener;	// listener registered from Browser

	void init();

public:
	OplWebAppUpgrade() {
		m_hOplWebAppUpgradeHandle = NULL;
		m_pfOplWebAppUpgradeResultListener = NULL;
		this->init();
	}

	OplWebAppUpgrade(OplWebAppUpgradeCallback pfOplWebAppUpgradeResultListener) {
		m_hOplWebAppUpgradeHandle = NULL;
		m_pfOplWebAppUpgradeResultListener = pfOplWebAppUpgradeResultListener;
		this->init();
	}

	~OplWebAppUpgrade() {
		if ( m_pfOplWebAppUpgradeResultListener ) {
			m_pfOplWebAppUpgradeResultListener = NULL;
		}
	}

	BPLStatus checkNewApplication();
	BPLStatus stopCheckNewApplication();
	BPLStatus registWebAppUpgradeResultCallback(OplWebAppUpgradeCallback pfOplWebAppUpgradeResultListener);
	void unregistWebAppUpgradeResultCallback();
	void onUpdateResultCallback(int event, int value);
};

#endif

