#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <map>

using namespace std;

typedef map<string,string>		CONFIG_MAP;
typedef CONFIG_MAP::iterator	CONFIG_MAP_IT;

#define CONFIG_INVALID_VALUE	-1
#define CONFIG_MAX_VALUE		1024

class ConfigManager
{
private:
	CONFIG_MAP	_configs;
	string		_configPath;
	string		_configTime;
	string		_configNewVersion;
	string		_configVersion;

public:
	ConfigManager(string ConfVersion);
	virtual ~ConfigManager();

	bool		readConfig(char* path);
	bool		saveConfig();

	const char*	getString(char* name);
	int			getValue(char* name);
	bool		setString(char* name, char* value);
	bool		setValue(char* name, int value);
	bool		deleteConfig(char* name);

	const char* getConfigTime();
	string		getNewConfVersion();
	string		getConfVersion();
	bool		setConfigVersion(string configVersion);

	void		printAll();
};

#endif // CONFIGMANAGER_H
