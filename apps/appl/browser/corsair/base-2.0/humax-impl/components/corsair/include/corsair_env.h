/*************************************************************************************************************
	File 		: corsair_env.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/09/27
	attention 	: 

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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
*************************************************************************************************************/

#ifndef __CORSAIR_ENV_H__
#define __CORSAIR_ENV_H__


/**********************************************************************************
 * CorsairEnv
 **********************************************************************************/
class CorsairEnv {
public:
	CorsairEnv() {}
	virtual ~CorsairEnv() {}

	static bool initenv();
	static bool renew();

private:
	static bool hasNewVersion();
	static bool getInstalledVersion(char *buf, int buf_len);
	static bool getNewVersion(char *buf, int buf_len);
	static bool getVersion(const char *path, char *buf, int buf_len);
	static bool copy();
	static bool copydir( const char *source_dir, const char *target_dir);
	static bool copyfile( const char *source_file, const char *target_file);
	static bool isGreaterThan(const char *src, const char *dst);

private:
	static char *m_opera_home_target;
	static char *m_opera_home_source;
	static char *m_opera_dir;
	static char *m_opera_font;
	static char *m_opera_version;
	static char *m_opera_ini;
	static char *m_opera_widget_install_dir;
	
};


#endif	/* __CORSAIR_ENV_H__ */
