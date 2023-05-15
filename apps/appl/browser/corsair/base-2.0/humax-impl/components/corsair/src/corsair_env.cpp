/*************************************************************************************************************
	File 		: corsair_env.cpp
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

#include "corsair_log.h"
#include "corsair_env.h"
#include "corsair_product_env.h"
#include "corsair_tokenizer.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

#define OPERA_VERSION	"version.txt"
#define OPERA_INI		"opera.ini"
#define	VEGA_DFB		"YES"
#define LANG			"en_US.UTF-8"

/**********************************************************************************
 * static variables
 **********************************************************************************/
char *CorsairEnv::m_opera_home_target;
char *CorsairEnv::m_opera_home_source;
char *CorsairEnv::m_opera_dir;
char *CorsairEnv::m_opera_font;
char *CorsairEnv::m_opera_widget_install_dir;
char *CorsairEnv::m_opera_version;
char *CorsairEnv::m_opera_ini;

#if 0
const char *__opera_home_target = "/var/lib/humaxtv/browser/";
const char *__opera_home_source = "/usr/browser/opera_home/";
const char *__opera_dir = "/usr/browser/opera_dir/";
const char *__opera_font = "/usr/fonts/";
const char *__opera_version = "version.txt";
const char *__opera_ini = "opera.ini";
#if defined(CONFIG_OP_JCOM) || defined(CONFIG_PROD_IR4000HD)
const char *__opera_widget_install_dir = "/var/lib/humaxtv_user/widgets/";
#else
const char *__opera_widget_install_dir = "/mnt/hd4/widgets/";
#endif
#endif

/**********************************************************************************
 * CorsairEnv
 **********************************************************************************/
bool CorsairEnv::initenv()
{
	Directory_t *dir = CorsairProductEnv::getDirectory();
	
	m_opera_home_target = strdup(dir->opera_home_target_dir.c_str());
	m_opera_home_source = strdup(dir->opera_home_source_dir.c_str());
	m_opera_dir = strdup(dir->opera_dir.c_str());
	m_opera_font = strdup(dir->opera_font_dir.c_str());
	m_opera_widget_install_dir = strdup(dir->opera_widget_install_dir.c_str());
	m_opera_version = strdup(OPERA_VERSION);
	m_opera_ini = strdup(OPERA_INI);

	if ( setenv("OPERA_HOME", m_opera_home_target, 0) != 0
		|| setenv("OPERA_DIR", m_opera_dir, 0) != 0
		|| setenv("OPERA_FONTS", m_opera_font, 0) != 0
		|| setenv("OPERA_OIPF_WIDGET_INSTALL_DIR", m_opera_widget_install_dir, 0) != 0
		|| setenv("USE_VEGA_DFB", VEGA_DFB, 0) != 0
		|| setenv("LANG", LANG, 0) != 0 )
	{
		LOG_DEBUG("initializing Corsair environment variables is failed.\n");
	}

	LOG_DEBUG("initialized Corsair environment variables successfully.\n");

	system("cp -LF /usr/lib/libopera* /var/lib/humaxtv/browser/");
	return true;
}

bool CorsairEnv::renew()
{
	LOG_DEBUG("Started.\n");
	if ( access(m_opera_home_target, R_OK|W_OK) != 0 )
	{
		LOG_DEBUG("There is no Opera HOME directory. create it.\n");
		if ( mkdir(m_opera_home_target, 0700) != 0 ) return false;
		return copy();
	}
	
	char opera_ini[512] = {0, };
	snprintf(opera_ini, sizeof(opera_ini), "%s/%s", m_opera_home_target, m_opera_ini);
	if ( access(opera_ini, R_OK|W_OK) != 0 )
	{
		LOG_DEBUG("There is no Opera ini file.\n");
		return copy();
	}

	if ( hasNewVersion() )
	{
		LOG_DEBUG("There is new Opera HOME version.\n");
		return copy();
	}

	return true;
}

bool CorsairEnv::hasNewVersion()
{
	char installed_version[16] = {0,};
	if ( !getInstalledVersion(installed_version, sizeof(installed_version)) ) return false;

	char new_version[16] = {0,};
	if ( !getNewVersion(new_version, sizeof(new_version)) ) return false;

	return isGreaterThan(new_version, installed_version);
}

bool CorsairEnv::getInstalledVersion(char *buf, int buf_len)
{
	return getVersion(m_opera_home_target, buf, buf_len);
}

bool CorsairEnv::getNewVersion(char *buf, int buf_len)
{
	return getVersion(m_opera_home_source, buf, buf_len);
}

bool CorsairEnv::getVersion(const char *path, char *buf, int buf_len)
{
    char temporary_to_read[64] = {0,};
	char version_path[256] = {0,};
	size_t	len;
	snprintf(version_path, sizeof(version_path), "%s/%s", path, m_opera_version);

	FILE *fp = fopen(version_path, "r");
	if ( fp == NULL ) return false;

    len = fread(temporary_to_read, 1, sizeof(temporary_to_read), fp);
    temporary_to_read[len]='\0';

	fclose(fp);
	if(len <= 0)
		LOG_DEBUG("can't get version.\n");

    snprintf(buf, buf_len, "%s", temporary_to_read);

	return true;
}

bool CorsairEnv::copy()
{
	LOG_DEBUG("Copy files to Opera HOME.\n");

	CorsairEnv::copydir( ( const char *)m_opera_home_source, ( const char *)m_opera_home_target);

	LOG_DEBUG("Copying files is finished.\n");
	return true;
}

bool CorsairEnv::copydir( const char *source_dir, const char *target_dir)
{
	string source, target;
	struct dirent* entry = NULL;
	struct stat stFileInfo;
	
	if ( !target_dir || !source_dir )
		return false;
	
	DIR* dir = opendir(source_dir);
	
	while ((entry = readdir(dir)) != NULL)
	{
		source = source_dir;
		source.append(entry->d_name);
		
		target = target_dir;
		target.append(entry->d_name);
		
		if (lstat(source.c_str(), &stFileInfo) < 0)
           perror ( source.c_str() );
           
		if( S_ISDIR(stFileInfo.st_mode) && ( strcmp(entry->d_name, "..") !=0 && strcmp(entry->d_name, ".") != 0) )
		{
			if ( mkdir( target.c_str(), 0700 ) != 0 )
			{
				LOG_ERR("mkdir is failed %s\n",target.c_str());
			}
			else
			{
				source.append("/");
				target.append("/");
				copydir(source.c_str(), target.c_str());
			}
		}
		else if( S_ISREG(stFileInfo.st_mode) )
		{
			if ( CorsairEnv::copyfile(source.c_str(),target.c_str()) == false )
			{
				LOG_ERR("failed to copy the file : from %s to %s \n",source.c_str(),target.c_str());
			}
		}
	}

	closedir(dir);

	return true;
}

bool CorsairEnv::copyfile( const char *source_file, const char *target_file)
{
	FILE *nSrcHandle, *nDstHandle;
	unsigned char *buf;
	size_t	len;

	LOG_DEBUG(" from %s to %s \n",source_file,target_file);
	
	if (strcmp(source_file, target_file) == 0)
	{
		return false;
	}

	if ((nSrcHandle = fopen(source_file, "rb")) == NULL)
	{
		LOG_ERR("fail to read... %s\n",source_file);
		return false;
	}

	if ((nDstHandle = fopen(target_file, "wb")) == NULL)
	{
		LOG_ERR("fail to read... %s\n",target_file);
		fclose(nSrcHandle);
		return false;
	}

	buf = (unsigned char*)malloc(16*1024);
	if (buf == NULL)
	{
		LOG_ERR("fail to malloc... %s\n",target_file);
		fclose(nSrcHandle);
		fclose(nDstHandle);
		return false;
	}

	while ((len = fread( buf, 1, 16*1024, nSrcHandle)) != 0)
	{
		LOG_DEBUG("read len... %d \n",len);
		if (fwrite( buf, 1, len,nDstHandle) == 0)
		{
			LOG_ERR("fail to write...%s\n",source_file);
			fclose(nSrcHandle);
			fclose(nDstHandle);
			free(buf);
			return false;
		}
	}

	fclose(nSrcHandle);
	fclose(nDstHandle);

	free(buf);

	return true;
}

bool CorsairEnv::isGreaterThan(const char *src, const char *dst)
{
	if ( !src || !dst ) return false;

	const unsigned int MAX_VER_NUM = 3;

	int num_src[MAX_VER_NUM] = {0,};
	CorsairTokenizer tok_src;
	string str_src(src);
	tok_src.set(str_src, ".");
	for ( int i =0; i<MAX_VER_NUM; ++i )
		num_src[i] = atoi(tok_src.next().c_str());

	int num_dst[MAX_VER_NUM] = {0,};
	CorsairTokenizer tok_dst;
	string str_dst(dst);
	tok_dst.set(str_dst, ".");
	for ( int i =0; i<MAX_VER_NUM; ++i )
		num_dst[i] = atoi(tok_dst.next().c_str());

	if ( num_src[0] > num_dst[0] ) return true;
	if ( num_src[1] > num_dst[1] ) return true;
	if ( num_src[2] > num_dst[2] ) return true;

	return false;

}
