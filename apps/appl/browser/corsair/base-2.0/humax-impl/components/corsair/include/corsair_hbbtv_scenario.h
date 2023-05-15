/** **********************************************************************************************************
	@file 		corsair_hbbtv_scenario.h
	
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

#ifndef __CORSAIR_HBBTV_SCENARIO_H__
#define __CORSAIR_HBBTV_SCENARIO_H__

#include <pthread.h>
#include <vector>
#include "corsair_hbbtv_app.h"

class CorsairHbbTVContainer;
class CorsairHbbTVApplication;

typedef enum {
	CORSAIR_HBBTV_STATE_IDLE = 0,
	CORSAIR_HBBTV_STATE_BROADCASTRELATED,
	CORSAIR_HBBTV_STATE_INDEPENENT,
	CORSAIR_HBBTV_STATE_DIGITALTELETEXT,
	CORSAIR_HBBTV_STATE_REGACYTELETEXT
}eHbbTVStatus;

/**********************************************************************************************
 * class CorsairHbbTVScenario
 **********************************************************************************************/
class CorsairHbbTVScenario {

public:
	typedef std::vector<CorsairHbbTVApplication *> CorsairHbbTVApplicationVector;
		
	CorsairHbbTVScenario();
	virtual ~CorsairHbbTVScenario();

	/**
		@fn 	lock
		@breif 	this function is to lock the HbbTV application status
		@param	N/A
		@return N/A
	*/
	void lock(void);
	
	/**
		@fn 	unlock
		@breif 	this function is to unlock the HbbTV application status
		@param	N/A
		@return N/A
	*/
	void unlock(void);
	
	/**
		@fn 	setHbbTVStatus
		@breif 	this function is to set the HbbTV application status
		@param	N/A
		@return N/A
	*/
	bool setHbbTVStatus( eHbbTVStatus state );
	
	/**
		@fn 	getHbbTVStatus
		@breif 	this function is to get the HbbTV application status
		@param	N/A
		@return N/A
	*/
	eHbbTVStatus getHbbTVStatus( void );
	
	/**
		@fn 	setHbbTVStatus
		@breif 	this function is to set the network connnection status
		@param	N/A
		@return true : the valuse is set successfully.
		@return false : the valuse is not set.
	*/
	bool setNetworkStatus( bool state );
	
	/**
		@fn 	getHbbTVStatus
		@breif 	this function is to get the network connection status
		@param	N/A
		@return true : the network is available.
		@return false : the network is not available.
	*/
	bool getNetworkStatus( void );
	
	bool init(void);
	bool reset(void);
	bool addHbbTVApplication(CorsairHbbTVApplication *hbbtv_app);
	bool removeHbbTVApplication(CorsairHbbTVApplication *hbbtv_app);
	bool removeHbbTVApplicationByType(eAppType type);
	bool showErrorMessage(void);
	CorsairHbbTVApplication* getHbbTVApplicationByType(eAppType type);
	bool isTrustedApp(const char* url);
	
	bool runBroadcastIndependentApplication(void);
	bool runBroadcastIndependentApplication(const char *url);
	bool isRunningBroadcastIndependentApplication(void);
	bool stopBroadcastIndependentApplication(void);
	bool runBroadcastRelatedApplication(void);
	bool stopBroadcastRelatedApplication(void);
	bool toggleDigitalTeletextApplication(bool forced_stop);
	bool runDigitalTeletextApplication(void);
	bool stopDigitalTeletextApplication(void);
	bool runRegacyTeletextApplication(void);
	bool stopRegacyTeletextApplication(void);
	
private:
	bool addDefaultHbbTVIndependentApplication(void);
	bool runHbbTVApplication(CorsairHbbTVApplication *app);
	bool stopHbbTVAppplication();

private:
	/* all members shall be handle with mutex locking and unlocking */
	CorsairHbbTVApplicationVector m_hbbtv_applist;
	eHbbTVStatus m_hbbtv_status;
	bool m_network_status;
	pthread_mutex_t m_mtx;	/* member for the mutext to treat hbbtv application status */
};

#endif	//__CORSAIR_HBBTV_SCENARIO_H
