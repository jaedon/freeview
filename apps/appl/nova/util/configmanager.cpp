#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "configmanager.h"
#include "configmanager_c.h"
#include "nova_utils.h"

#define		CONFIG_ROOT_NODE		"Config"
#define		CONFIG_ACCESS_TIME		"datetime"
#define		CONFIG_CONFIG_VERSION	"version"
#define		CONFIG_DEFAULT_FILE		"config.xml"


typedef vector<string>			NOVA_FILES;
typedef NOVA_FILES::iterator	NOVA_FILES_IT;


// ========================================== IMPLEMENTATION ==============================================
ConfigManager::ConfigManager(string ConfVersion)
{
	_configPath		= CONFIG_DEFAULT_FILE;
	_configNewVersion	= ConfVersion;
}

ConfigManager::~ConfigManager()
{
	_configs.clear();
}

string ConfigManager::getNewConfVersion()
{
	return _configNewVersion;
}

string ConfigManager::getConfVersion()
{
	return _configVersion;
}

bool ConfigManager::readConfig(char* path)
{
	xmlDoc* doc;
	xmlNode* nodeConfig = NULL;
	xmlXPathContextPtr context = NULL;
	xmlXPathObjectPtr result = NULL;
	xmlNodeSetPtr nodeset = NULL;
	int i;
	char* datetime;
	char* confVersion;
	char xpath_query[1024];

	if (path != NULL)
	{
		_configPath = path;
	}

	_configTime = "";

	_configs.clear();

	doc = xmlParseFile(_configPath.c_str());
	if (doc == NULL)
	{
		printf("[%s:%d] xmlParseFile(%s) failed\n", __FUNCTION__, __LINE__, _configPath.c_str());
		goto ErrBlock;
	}

	nodeConfig = xmlDocGetRootElement(doc);
	if (nodeConfig == NULL)
	{
		printf("[%s:%d] xmlDocGetRootElement failed\n", __FUNCTION__, __LINE__);
		goto ErrBlock;
	}

	// @datetime attr
	datetime = (char *)xmlGetProp(nodeConfig, (xmlChar*)CONFIG_ACCESS_TIME);
	if (datetime != NULL)
	{
		_configTime = datetime;
	}

	// @configVersion attr
	confVersion = (char *)xmlGetProp(nodeConfig, (xmlChar*)CONFIG_CONFIG_VERSION);
	if (confVersion == NULL)
	{
		// Default Config must be loaded.
		goto ErrBlock;
	}

	_configVersion = confVersion;

	if (_configNewVersion.compare(confVersion) != 0)
	{
		// New Default Config must be loaded.
		goto ErrBlock;
	}

	context = xmlXPathNewContext(doc);
	if(context == NULL)
	{
		printf("[%s:%d] xmlXPathNewContext failed\n", __FUNCTION__, __LINE__);
		goto ErrBlock;
	}

	sprintf(xpath_query,"/%s/*", CONFIG_ROOT_NODE);
	result = xmlXPathEvalExpression((xmlChar*)xpath_query, context);
	if (result == NULL)
	{
		printf("[%s:%d] xmlXPathEvalExpression failed\n", __FUNCTION__, __LINE__);
		goto ErrBlock;
	}

	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		printf("[%s:%d] no config item!\n", __FUNCTION__, __LINE__);
		goto ErrBlock;
	}

	nodeset = result->nodesetval;
	for (i = 0 ; i < nodeset->nodeNr ; i++)
	{
		_configs.insert(make_pair((char*)(nodeset->nodeTab[i]->name), (char*)(xmlNodeGetContent(nodeset->nodeTab[i]))));
	}

	if (result != NULL)
	{
		xmlXPathFreeObject(result);
	}

	if (context != NULL)
	{
	   xmlXPathFreeContext(context);
	}

	if (doc != NULL)
	{
		xmlFreeDoc(doc);
	}

	xmlCleanupParser();

	return true;

ErrBlock:
	if (result != NULL)
	{
		xmlXPathFreeObject(result);
	}

	if (context != NULL)
	{
	   xmlXPathFreeContext(context);
	}

	if (doc != NULL)
	{
		xmlFreeDoc(doc);
	}

	xmlCleanupParser();

	return false;
}

bool ConfigManager::saveConfig()
{
	xmlDocPtr doc;
	xmlNodePtr nodeConfig;
	xmlNodePtr node;
	CONFIG_MAP_IT it;
	struct timeval tv;
	struct tm* ptm;
	char datetime[1024];

	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);
	sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d",
			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	_configTime = datetime;

	doc = xmlNewDoc((xmlChar*)"1.0");
	doc->encoding = (xmlChar*)strdup("UTF-8");

	nodeConfig = xmlNewNode(NULL, (xmlChar*)CONFIG_ROOT_NODE);

	// @datetime attr
	xmlSetProp(nodeConfig, (xmlChar*)CONFIG_ACCESS_TIME, (xmlChar*)datetime);

	// @confVersion
	xmlSetProp(nodeConfig, (xmlChar*)CONFIG_CONFIG_VERSION, (xmlChar*)_configVersion.c_str());

	xmlDocSetRootElement(doc, nodeConfig);

	for ( it = _configs.begin() ; it != _configs.end() ; it++)
	{
		printf("name:%s value:%s\n", it->first.c_str(), it->second.c_str());

		node = xmlNewNode(NULL, (xmlChar*)it->first.c_str());
		if (node != NULL)
		{
			xmlNodeSetContent(node, (xmlChar*)it->second.c_str());
			xmlAddChild(nodeConfig,node);
		}
	}

	if (xmlSaveFormatFile(_configPath.c_str(), doc, 1) == -1)
	{
		printf("[%s:%d] xmlSaveFormatFile(%s) failed\n", __FUNCTION__, __LINE__, _configPath.c_str());
	}

	xmlFreeDoc(doc);

	return true;
}

bool ConfigManager::setConfigVersion(string configVersion)
{
	_configVersion = configVersion;
	return true;
}

const char*	ConfigManager::getString(char* name)
{
	CONFIG_MAP_IT it = _configs.find(name);

	if (it != _configs.end())
	{
		return it->second.c_str();
	}

	return NULL;
}

int ConfigManager::getValue(char* name)
{
	const char* value;

	value = getString(name);
	if (value != NULL)
	{
		return atoi(value);
	}

	return CONFIG_INVALID_VALUE;
}

bool ConfigManager::setString(char* name, char* value)
{
	CONFIG_MAP_IT it = _configs.find(name);

	if (it != _configs.end())
	{
		it->second = string(value);
	}
	else
	{
		_configs.insert(make_pair(name, value));
	}

	return true;
}

bool ConfigManager::setValue(char* name, int value)
{
	char valueStr[CONFIG_MAX_VALUE];

	sprintf(valueStr, "%d", value);

	return setString(name, valueStr);
}

void ConfigManager::printAll()
{
	CONFIG_MAP_IT it;

	printf("=========[items:%d]============\n", _configs.size());
	for ( it = _configs.begin() ; it != _configs.end() ; it++)
	{
		printf("name:%s value:%s\n", it->first.c_str(), it->second.c_str());
	}
	printf("-------------------------------\n");
}

const char* ConfigManager::getConfigTime()
{
	return _configTime.c_str();
}

bool ConfigManager::deleteConfig(char* name)
{
	CONFIG_MAP_IT it = _configs.find(name);

	if (it != _configs.end())
	{
		_configs.erase(it);
		return true;
	}

	return false;
}

// =================================== C Interface API ===============================================

HANDLE_CONIFG CONFIG_Create(char* configVersion)
{
	return new ConfigManager(configVersion);
}

bool CONFIG_Read(HANDLE_CONIFG hConf ,char* path)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return false;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	return pConf->readConfig(path);
}

bool CONFIG_Save(HANDLE_CONIFG hConf)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return false;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	return pConf->saveConfig();
}

bool CONFIG_Close(HANDLE_CONIFG hConf)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return false;
	}

	pConf = static_cast<ConfigManager*>(hConf);
	delete pConf;

	return true;
}

bool CONFIG_SetConfigVersion(HANDLE_CONIFG hConf, char* configVersion)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return NULL;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	return pConf->setConfigVersion(configVersion);
}

const char* CONFIG_GetString(HANDLE_CONIFG hConf, char* name)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return NULL;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	return pConf->getString(name);
}

int CONFIG_GetValue(HANDLE_CONIFG hConf, char* name)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return false;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	return pConf->getValue(name);
}

bool CONFIG_SetString(HANDLE_CONIFG hConf, char* name, char* value)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return false;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	return pConf->setString(name, value);
}

bool CONFIG_SetValue(HANDLE_CONIFG hConf, char* name, int value)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return false;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	return pConf->setValue(name, value);
}

bool CONFIG_DeleteConfig(HANDLE_CONIFG hConf, char* name)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return false;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	return pConf->deleteConfig(name);
}

void  CONFIG_PrintAll(HANDLE_CONIFG hConf)
{
	ConfigManager* pConf;

	if (hConf == NULL)
	{
		return;
	}

	pConf = static_cast<ConfigManager*>(hConf);

	pConf->printAll();
}


bool CONFIG_CopyFilesToFlash(void)
{
	DIR* hDIR = NULL;
	struct dirent* info;
	NOVA_FILES		files;
	NOVA_FILES_IT	it;
	string			name;
	char			srcPath[1024];
	char			dstPath[1024];
	struct stat fileinfo = {0};
	int ret;

	hDIR = opendir(CONFIG_USER_TARA_PATH);

	if (hDIR == NULL)
	{
		HxLOG_Error("opendir(%s) failed\n", CONFIG_USER_TARA_PATH);
		return false;
	}

	do
	{
		info = readdir(hDIR);
		if (info == NULL)
		{
			break;
		}

		sprintf(srcPath, "%s/%s", CONFIG_USER_TARA_PATH, info->d_name);
		ret = stat(srcPath, &fileinfo);
		if (ret != 0)
		{
			HxLOG_Warning("stat(%s) failed(errno:%d)\n", srcPath, errno);
			continue;
		}

		if (S_ISREG(fileinfo.st_mode))
		{
			files.push_back(info->d_name);
		}
	} while(1);

	closedir(hDIR);
	hDIR = NULL;

	// copy files to destination
	for (it = files.begin() ; it != files.end() ; it++)
	{
		name = *it;

		sprintf(srcPath, "%s/%s"	 , CONFIG_USER_TARA_PATH,  name.c_str());
		sprintf(dstPath, "%s/tara/%s", CONFIG_HUMAXTV_PATH	,  name.c_str());

		HxLOG_Info("copy: %s to %s\n", srcPath, dstPath);
		if (NOVA_SendFile(srcPath, dstPath, 0777) == -1)
		{
			HxLOG_Error("Cannot copy files: from(%s) to (%s), errno:%d\n", srcPath, dstPath, errno);
			continue;
		}
	}

	return true;
}
