/** **********************************************************************************************************
	@file 		corsair_postman.h
	
	@date		2013/03/21
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif		
	
	(c) 2012-2013 Humax Co., Ltd. 
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
#ifndef __CORSAIR_POSTMAN_H__
#define __CORSAIR_POSTMAN_H__

#include <queue>
#include <map>
#include <string>

class CorsairMail {
public:
	virtual ~CorsairMail() {}
	virtual void execute()=0;
};
class CorsairMailOpenUrl: public CorsairMail 
{
public :
    std::string m_url;
    
	CorsairMailOpenUrl(const char *url)
    {
		if ( url != NULL )
		    m_url = url;
    }
	virtual ~CorsairMailOpenUrl(){};
	void execute();
};

class CorsairMailConnectDragonfly: public CorsairMail 
{
public :
	CorsairMailConnectDragonfly(){};
	virtual ~CorsairMailConnectDragonfly(){};
	void execute();
};

class CorsairMailSetSystemLanguage: public CorsairMail 
{
public :
	std::string m_language_code;

	CorsairMailSetSystemLanguage(const char*);
	virtual ~CorsairMailSetSystemLanguage(){};
	void execute();
};

class CorsairMailEnableHbbTV: public CorsairMail 
{
public :
	bool data;
	
	CorsairMailEnableHbbTV(bool enable): data(enable){}
	virtual ~CorsairMailEnableHbbTV(){};
	void execute();
};

class CorsairMailCreateHumaxApplication: public CorsairMail 
{
public :
	std::string m_url;
	std::string m_name;
	
	CorsairMailCreateHumaxApplication(const char *name, const char *url): m_name(name)
	{
		if ( url != NULL )
		    m_url = url;
	}
	virtual ~CorsairMailCreateHumaxApplication(){};
	void execute();
};

class CorsairMailDestroyHumaxApplication: public CorsairMail 
{
public :
	std::string m_name;
	
	CorsairMailDestroyHumaxApplication(const char *name): m_name(name)
	{
        ;
	}
	virtual ~CorsairMailDestroyHumaxApplication(){};
	void execute();
};

class CorsairMailResendAIT: public CorsairMail
{
public:
    CorsairMailResendAIT(){};
    ~CorsairMailResendAIT(){};
    void execute();
};

class CorsairMailSetMhegStatus: public CorsairMail
{
public:
    CorsairMailSetMhegStatus(bool status);
    ~CorsairMailSetMhegStatus(){};
    void execute();
    
private:
    bool m_status;
};

/** **************************************************************************************
	@class	CorsairPostman
 	@brief 	This module delivers the message from external thread to Opera main thread.
	Because some APIs of Opera is thread-unsafe,
	external thread have to deliver the message to Opera through this.
****************************************************************************************** */
class CorsairPostman 
{
public:
	CorsairPostman();
	virtual ~CorsairPostman();

	/**
		@fn 	post
		@param	mail the data for sending to Opera main thread
		@breif 	pass the data to Opera main thread
	*/
	bool post(CorsairMail *mail);

	/**
		@fn 	run
		@breif 	This function has to be called in Opera main loop repeatly.
		In this loop, all messages from external thread will be handled.
	*/
	void run();

private:
	std::queue<CorsairMail *> m_mail_box;
};

#endif
