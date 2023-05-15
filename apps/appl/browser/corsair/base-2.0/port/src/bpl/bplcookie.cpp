/**************************************************************************************/
/**
 * @file bplcookie.cpp
 *
 *
 *
 * @author
 * @date    2012/07/12
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "bplcookie.h"

#include <hlib.h>

#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace std;

std::vector<BplCookie *> BplCookieManager::m_cookies;
pthread_mutex_t BplCookieManager::m_mtx = PTHREAD_MUTEX_INITIALIZER;
std::string BplCookieManager::m_url;

/*------------------------------------------------------------------------------
 *	class BplCookieManager
 *------------------------------------------------------------------------------*/
void BplCookieManager::lock()
{
	pthread_mutex_lock(&m_mtx);
}

void BplCookieManager::unlock()
{
	pthread_mutex_unlock(&m_mtx);
}

void BplCookieManager::add(BplCookie *cookie)
{
	lock();
	m_cookies.push_back(cookie);
	unlock();
}

void BplCookieManager::del(const char *name)
{
	if ( name == NULL ) return;

	lock();
	CookieVector::iterator it;
	for (it = m_cookies.begin(); it != m_cookies.end(); ++it)
	{
		if ( (*it)->name == NULL ) continue;
		if ( strcmp(name, (*it)->name) == 0 )
		{
			if ( (*it)->name ) free((*it)->name);
			if ( (*it)->domain ) free((*it)->domain);
			if ( (*it)->path ) free((*it)->path);
			if ( (*it)->value ) free((*it)->value);
			delete (*it);
			m_cookies.erase(it);
		}
	}
	unlock();
}

void BplCookieManager::delAll(void)
{
	lock();
	CookieVector::iterator it;
	for (it = m_cookies.begin(); it != m_cookies.end(); ++it)
	{
		if ( (*it)->name ) free((*it)->name);
		if ( (*it)->domain ) free((*it)->domain);
		if ( (*it)->path ) free((*it)->path);
		if ( (*it)->value ) free((*it)->value);
		delete (*it);
	}

	m_cookies.clear();
	unlock();
}

void BplCookieManager::setUrl(const char *url)
{
	if ( url ) m_url = url;
}

string BplCookieManager::getUrl()
{
	return m_url;
}

bool BplCookieManager::getCookieByDomain(const char *domain, std::string &cookie)
{
	lock();
	CookieVector::iterator it;
	for (it = m_cookies.begin(); it != m_cookies.end(); ++it)
	{
		if ( strcmp(domain, (*it)->domain) != 0 )
			continue;

        cookie.append(std::string((*it)->name));
        cookie.append("=");
        cookie.append(std::string((*it)->value));
        cookie.append("; ");
	}

    if ( cookie.length() )
        cookie.resize(cookie.length()-2);

	unlock();

	return cookie.length() > 0 ? true : false;
}

bool BplCookieManager::getCookieByName(const char *name, std::string &cookie)
{
	lock();
	CookieVector::iterator it;
	for (it = m_cookies.begin(); it != m_cookies.end(); ++it)
	{
		if ( strcmp(name, (*it)->name) != 0 )
			continue;

        cookie.append(std::string((*it)->name));
        cookie.append("=");
        cookie.append(std::string((*it)->value));
        cookie.append("; ");
	}

    if ( cookie.length() )
        cookie.resize(cookie.length()-2);

	unlock();

	return cookie.length() > 0 ? true : false;
}

void BplCookieManager::showCookie()
{
	HxLOG_Print("---------------Cookie Information-----------------\n");
	lock();
	CookieVector::iterator it;
	for (it = m_cookies.begin(); it != m_cookies.end(); ++it)
	{
		if ( (*it)->name == NULL ) continue;
		HxLOG_Print("---------------------------------------------------\n");
		HxLOG_Print("name=%s\n", (*it)->name);
		HxLOG_Print("domain=%s\n", (*it)->domain);
		HxLOG_Print("path=%s\n", (*it)->path);
		HxLOG_Print("value=%s\n", (*it)->value);
		HxLOG_Print("---------------------------------------------------\n");
	}
	unlock();
}

void BplCookieManager::showCurrentUrl()
{
	HxLOG_Print("---------------Current Application URL-----------------\n");
	HxLOG_Print("BplCookieManager:: url = %s\n", m_url.c_str());
	HxLOG_Print("-------------------------------------------------------\n");
}
