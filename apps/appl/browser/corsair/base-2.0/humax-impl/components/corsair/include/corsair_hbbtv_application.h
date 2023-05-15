/** **********************************************************************************************************
	@file 		corsair_hbbtv_application.h
	
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

#ifndef __CORSAIR_HBBTV_APPLICATION_H__
#define __CORSAIR_HBBTV_APPLICATION_H__

#include <string>

/**************************************************************************************
 * class CorsairHbbTVApp
 **************************************************************************************/
class CorsairHbbTVApp {
public:
	typedef enum {
		HBBTV_APPTYPE_NONE				= 0,
		HBBTV_APPTYPE_BR_AUTOSTART,
		HBBTV_APPTYPE_BR_TELETEXT,
		HBBTV_APPTYPE_BI,
		HBBTV_APPTYPE_BR_XMLAIT,
        HBBTV_APPTYPE_BR_PRESENT
	} eHbbTVApplicationType;

public:
	CorsairHbbTVApp();
	virtual ~CorsairHbbTVApp();

	virtual bool run() { return false; }
	virtual bool stop() { return false; }
	virtual bool show();
	virtual bool hide();
	virtual bool stop_by_system();
	bool isRunning() { return m_is_running; }

	unsigned short getAppID() { return m_appid; }
	void setAppID(unsigned short appid) { m_appid = appid; }
	unsigned long getOrgID() { return m_orgid; }
	void setOrgID(unsigned long orgid) { m_orgid = orgid; }
	std::string &getUrl() { return m_url; }
	void setUrl(const char *url) { if ( url ) m_url = url; }
    eHbbTVApplicationType getAppType() { return m_apptype; }
	void setAppType(eHbbTVApplicationType type){ m_apptype=type; }
	bool isBroadcastRelated() { return m_broadcast_related; }
	void setBroadcastRelated(bool broadcast_related) { m_broadcast_related = broadcast_related; }

protected:
    bool m_broadcast_related;
    unsigned short m_appid;
    unsigned long m_orgid;
    std::string m_url;
    eHbbTVApplicationType m_apptype;
    bool m_is_running;
public:
    bool m_is_started_mheg;
};

/**************************************************************************************
 * class CorsairHbbTVBRApp 
 **************************************************************************************/
class CorsairHbbTVBRApp : public CorsairHbbTVApp {
public:
    CorsairHbbTVBRApp();
    CorsairHbbTVBRApp(unsigned short appid, unsigned long orgid, const char *url);
    virtual ~CorsairHbbTVBRApp();

    virtual bool run();
    virtual bool stop();
};

/**************************************************************************************
 * class CorsairHbbTVBRAutoApp 
 **************************************************************************************/
class CorsairHbbTVBRAutoApp : public CorsairHbbTVBRApp {
public:
    CorsairHbbTVBRAutoApp();
    CorsairHbbTVBRAutoApp(unsigned short appid, unsigned long orgid, const char *url);
    virtual ~CorsairHbbTVBRAutoApp();
};

/**************************************************************************************
 * class CorsairHbbTVBRTeletextApp 
 **************************************************************************************/
class CorsairHbbTVBRTeletextApp : public CorsairHbbTVBRApp {
public:
    CorsairHbbTVBRTeletextApp();
    CorsairHbbTVBRTeletextApp(unsigned short appid, unsigned long orgid, const char *url);
    virtual ~CorsairHbbTVBRTeletextApp();
};

/**************************************************************************************
 * class CorsairHbbTVBIApp
 **************************************************************************************/
class CorsairHbbTVBIApp : public CorsairHbbTVApp {
public:
    CorsairHbbTVBIApp();
    CorsairHbbTVBIApp(const char *url);
    virtual ~CorsairHbbTVBIApp();

    virtual bool run();
    virtual bool stop();

    bool run_without_liveoff();

};

#if defined(CONFIG_PROD_FVP4000T)
/**************************************************************************************
 * class CorsairHbbTVBRAutoApp
 **************************************************************************************/
class CorsairHbbTVBRXmlAIT : public CorsairHbbTVBRApp {
public:
    CorsairHbbTVBRXmlAIT();
    CorsairHbbTVBRXmlAIT(unsigned short appid, unsigned long orgid, const char *url);
    virtual ~CorsairHbbTVBRXmlAIT();
};

/**************************************************************************************
 * class CorsairHbbTVBRPresentApp
 **************************************************************************************/
class CorsairHbbTVBRPresentApp: public CorsairHbbTVBRApp {
public:
    CorsairHbbTVBRPresentApp();
    CorsairHbbTVBRPresentApp(unsigned short appid, unsigned long orgid, const char *url);
    virtual ~CorsairHbbTVBRPresentApp();
};

#endif


#endif
