/**************************************************************************************/
/**
 * @file corsair_tools.h
 *
 * Corsair Tools
 *
 * @author  IPM-SW1T (tipmsw1@humaxdigital.com)
 * @date    2012/09/15
 * @attention   (c) 2011-2012 Humax Co., Ltd.

 *	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
 *	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 *	non-assignable, non-transferable and non-exclusive license to use this Software.
 *	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 *	you agree to the responsibility to take all reasonable efforts to protect the any information
 *	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 *	reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 *	If you have no authorized license, discontinue using this Software immediately.

 *	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 *	IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 *	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 *	 CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 *	This License is effective until terminated. You may terminate this License at any time by destroying all copies
 *	of the Software including all documentation. This License will terminate immediately without notice from Humax
 *	to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 *	of the Software and all documentation.

 *	The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 *	All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 *	in the Republic of Korea.

 *
 **************************************************************************************
 **/


#ifndef __CORSAIR_TOOLS_H__
#define __CORSAIR_TOOLS_H__
#include <string>

using namespace std;

class CorsairTools {
public:
	CorsairTools();
	virtual ~CorsairTools();

	static unsigned long getTicks();

	static bool startLiveTV();
	static bool stopLiveTV();
	static bool recoverLiveTV();
	static bool subtitleOn();
	static bool subtitleOff();
	static bool mhegOn();
	static bool mhegOff();
	static bool mhegBlock();
	static bool mhegUnBlock();
	static bool oipfAppBlock();
	static bool oipfAppUnBlock();

	static bool setPanelString(const char* string);
	static bool clearPanelString();
	static bool setPreferenceLanguage();

	static bool getUrlfromUsb(char *buf, int bufsize);
        static bool getUrlfromUsb(const string path, string &url);

	static char* getUsbPath(char *file);
	static bool startDragonFly(const char *ipaddr, unsigned short port);
	static bool startDragonFlyWithNfsroot();
	static bool startDragonFlyWithUsb();
	static bool stopDragonFly();

	static void syncCookie();
	static void saveCookie();
	static bool changeDaeApp();
	static bool ReloadMainAppFromUSB();
	static bool doFormatWidgetInformation();
	static void initializeWidgets();
        static const char *getExternalMainAppURL();
        static bool isPlayingMedia();
        static void doAbort();

    class UrlParser
    {
        public:
            /**
             * @brief Constructor getting full address URL of target
             * @param url_s
             */
            UrlParser(const std::string& url_s) // omitted copy, ==, accessors, ...
            {
                parse(url_s);
            }

            /**
             * @brief Getting protocol like http, https, dvb
             * @return return string of protocol
             */
            string getProtocol() { return m_protocol; }

            /**
             * @brief Getting host name like formal domain without file path
             * @return return string of host name
             */
            string getHost() { return m_host; }

            /**
             * @brief Getting file path which is including root path like /index.html
             * @return return string of target file
             */
            string getPath() { return m_path; }

            /**
             * @brief Getting query which is like ?id=TEST&msg=helloworld
             * @return return string of query
             */
            string getQuery() { return m_query; }

        private:
            void parse(const std::string& url_s);
            string m_protocol;
            string m_host;
            string m_path;
            string m_query;
    };
};
#endif
