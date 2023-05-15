/**************************************************************************************/
/**
 * @file oplwebappupgrade.cpp
 *
 * WebApp Upgrade Interfaces
 *
 **************************************************************************************
 **/

#include "oplwebappupgrade.h"

#include <apk.h>

void OplWebAppUpgrade::init()
{
	/** not implemted yet - delete this comments after implement or not needed */
	// @ToDo initialize if needed
	// for example, APK_SOTUP_Create (&m_hOplWebAppUpgradeHandle) to create handle
	// for example, APK_SOTUP_RegistEventCallback(onUpdateResultCallback) to regist event callback to AppKit
}

BPLStatus OplWebAppUpgrade::checkNewApplication()
{
	/** not implemted yet - delete this comments after implement */
	// @ToDo call corresponding function of AppKit
	// for example, APK_SOTUP_CheckNewApplication(m_hOplWebAppUpgradeHandle)

	return BPL_STATUS_OK;
}

BPLStatus OplWebAppUpgrade::stopCheckNewApplication()
{
	/** not implemted yet - delete this comments after implement */
	// @ToDo call corresponding function of AppKit
	// for example, APK_SOTUP_StopCheckNewApplication(m_hOplWebAppUpgradeHandle)

	return BPL_STATUS_OK;
}

BPLStatus OplWebAppUpgrade::registWebAppUpgradeResultCallback(OplWebAppUpgradeCallback pfOplWebAppUpgradeResultListener)
{
	if ( pfOplWebAppUpgradeResultListener == NULL )
	{
		return BPL_STATUS_ERROR;
	}

	m_pfOplWebAppUpgradeResultListener = pfOplWebAppUpgradeResultListener;

	return BPL_STATUS_OK;
}

void OplWebAppUpgrade::unregistWebAppUpgradeResultCallback()
{
	m_pfOplWebAppUpgradeResultListener = NULL;
}

void OplWebAppUpgrade::onUpdateResultCallback(int event, int value)
{
	if ( m_pfOplWebAppUpgradeResultListener )
	{
		m_pfOplWebAppUpgradeResultListener(event, value);
	}
}

