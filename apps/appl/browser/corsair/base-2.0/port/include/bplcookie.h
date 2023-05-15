/**************************************************************************************/
/**
 * @file bplcookie.h
 *
 * 
 *
 * @author  
 * @date    2012/07/12
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __BPL_COOKIE_H__
#define __BPL_COOKIE_H__

#include "bpltype.h"
#include <vector>
#include <string>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};

/*------------------------------------------------------------------------------
 *	class BplCookie
 *------------------------------------------------------------------------------*/
struct BplCookie {
	/** Name of the cookie */
	char* name;
	/** The cookie value will only be sent to this domain */
	char* domain;
	/** The cookie value will only be sent to pages starting with this path */
	char* path;
	/** Cookie value */
	char* value;
	/** Time and date when the cookie expires, treat this as a time_t value */
	long expires;
	/** Flag which indicates if the cookie was stored when visiting a server via HTTPS */
	int secure;
};

class BplCookieManager {
public:
	BplCookieManager() {}
	virtual ~BplCookieManager() {}

public:
	typedef std::vector<BplCookie *> CookieVector;

	static void add(BplCookie *cookie);
	static void del(const char *name);
	static void delAll(void);

	static void setUrl(const char *url);
	static std::string getUrl();
	static bool getCookieByDomain(const char *domain, std::string &cookie);
	static bool getCookieByName(const char *name, std::string &cookie);

	/* for debug */
	static void showCookie();
	static void showCurrentUrl();

private:
	static void lock();
	static void unlock();

public:
	static std::vector<BplCookie *>	m_cookies;
	static pthread_mutex_t			m_mtx;
	static std::string				m_url;
};

#endif

#endif


