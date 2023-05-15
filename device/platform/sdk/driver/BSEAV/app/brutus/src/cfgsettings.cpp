/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cfgsettings.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/8/10 5:57p $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/cfgsettings.cpp $
 * 
 * 2   3/8/10 5:57p jtna
 * SW7420-527: add support for 'include <filename>' syntax
 * 
 * 1   2/7/05 7:57p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/8   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/7   12/20/04 4:19p erickson
 * PR10925: embedded cfg docs in code
 * 
 * Irvine_BSEAVSW_Devel/6   3/5/04 2:40p erickson
 * PR9883: call copy constructor correctly
 * 
 * Irvine_BSEAVSW_Devel/5   2/26/04 11:09a erickson
 * PR9883: added DUAL_OUTPUT and a copy constructor
 *
 * Irvine_BSEAVSW_Devel/4   9/17/03 12:32p erickson
 * converted from original debug interface to magnum debug interface
 *
 * Irvine_BSEAVSW_Devel/3   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/2   6/19/03 4:27p erickson
 * added default file on open
 *
 * Irvine_BSEAVSW_Devel/2   4/9/03 1:11p erickson
 * allow y and Y for true
 *
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:20a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#include "cfgsettings.h"

// include it a second time in order to define the structure
#undef CFG_SETTINGS
#undef DECLARE
#define DECLARE_CONFIGSETTING_VALUES
#include "cfgsettings.h"

#include "bstd.h"
BDBG_MODULE(brutus_cfgsettings);

Config::Config(const char *filename)
{
	_inifile = new MStringHash();
	if (filename)
		read(filename);
}

Config::Config(const Config &cfg)
{
	_inifile = new MStringHash(*cfg._inifile);
}


Config::~Config()
{
	delete _inifile;
}

#define TOTAL_SETTINGS() (sizeof(predefines)/sizeof(predefines[0]))
int Config::total()
{
	return TOTAL_SETTINGS();
}

const char *Config::getName(int index)
{
	return predefines[index].name;
}

const char *Config::getDescription(int index)
{
	return predefines[index].description;
}

const char *Config::get(const char *name, const char *defaultvalue) const {
	MString &s = _inifile->get(name);
	if (!s)
		return defaultvalue;
	else
		return s;
}

int Config::getInt(const char *name, int defaultvalue) const {
	MString &s = _inifile->get(name);
	if (!s)
		return defaultvalue;
	else
		return s.toInt();
}

bool Config::getBool(const char *name, bool defaultvalue) const {
	MString &s = _inifile->get(name);
	if (!s)
		return defaultvalue;
	else
		return s[0] == 't' || s[0] == 'T' ||
			s[0] == 'y' || s[0] == 'Y' || s.toInt();
}

double Config::getDouble(const char *name, double defaultvalue) const {
	MString &s = _inifile->get(name);
	if (!s)
		return defaultvalue;
	else
		return s.toDouble();
}

void Config::set(const char *name, double value) {
	char buf[25];
	sprintf(buf, "%f", value);
	// I don't care for the %e implementation of MString
	set(name, buf);
}

void Config::set(const char *name, const char *value) {
	//printf("Config::set(%s,%s)\n", name, value);
	_inifile->add(name, value);
}

void Config::set(const char *name, int value) {
	set(name, MString(value));
}

int Config::read(const char *filename) {
	return _inifile->read(filename, true)?0:1;
}

void Config::printHelp() const
{
	for (unsigned i=0;i<TOTAL_SETTINGS();i++)
		printf("%s=%s (type %s)\n  %s\n",
			predefines[i].name,
			get(predefines[i].name),
			predefines[i].type,
			predefines[i].description);

}

