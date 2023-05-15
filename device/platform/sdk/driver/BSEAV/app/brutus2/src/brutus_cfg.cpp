/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_cfg.cpp $
* $brcm_Revision: 2 $
* $brcm_Date: 2/28/12 5:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus_cfg.cpp $
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "brutus_cfg.h"

// include it a second time in order to define the structure
#undef BRUTUS_CFG_H__
#undef BRUTUS_DECLARE
#define BRUTUS_DECLARE_CONFIGSETTING_VALUES
#include "brutus_cfg.h"

#include "bstd.h"

BDBG_MODULE(brutus_cfg);


CConfiguration::CConfiguration(const char *filename)
{
    int i = 0;

	_pRegistry = new MStringHash();

    /* add default name/value pairs */
    for (i = 0; i < total(); i++)
    {
        set(_predefines[i]._name, _predefines[i]._default);
    }

	if (filename)
		read(filename);
}

CConfiguration::CConfiguration(const CConfiguration & cfg)
{
	_pRegistry = new MStringHash(*cfg._pRegistry);
}


CConfiguration::~CConfiguration()
{
	delete _pRegistry;
}

#undef TOTAL_SETTINGS
#define TOTAL_SETTINGS() (sizeof(_predefines)/sizeof(_predefines[0]))
int CConfiguration::total()
{
	return TOTAL_SETTINGS();
}

const char *CConfiguration::getName(int index)
{
	return _predefines[index]._name;
}

const char *CConfiguration::getDescription(int index)
{
	return _predefines[index]._description;
}

const char *CConfiguration::get(const char *name, const char *defaultvalue) const {
	MString &s = _pRegistry->get(name);
	if (!s)
		return defaultvalue;
	else
		return s;
}

int CConfiguration::getInt(const char *name, int defaultvalue) const {
	MString &s = _pRegistry->get(name);
	if (!s)
		return defaultvalue;
	else
		return s.toInt();
}

bool CConfiguration::getBool(const char *name, bool defaultvalue) const {
	MString &s = _pRegistry->get(name);
	if (!s)
		return defaultvalue;
	else
		return s[0] == 't' || s[0] == 'T' ||
			s[0] == 'y' || s[0] == 'Y' || s.toInt();
}

double CConfiguration::getDouble(const char *name, double defaultvalue) const {
	MString &s = _pRegistry->get(name);
	if (!s)
		return defaultvalue;
	else
		return s.toDouble();
}

void CConfiguration::set(const char *name, double value) {
	char buf[25];
	sprintf(buf, "%f", value);
	// I don't care for the %e implementation of MString
	set(name, buf);
}

void CConfiguration::set(const char *name, const char *value) {
	//printf("CConfiguration::set(%s,%s)\n", name, value);
	_pRegistry->add(name, value);
}

void CConfiguration::set(const char *name, int value) {
	set(name, MString(value));
}

int CConfiguration::read(const char *filename) {
	return _pRegistry->read(filename, true)?0:1;
}

void CConfiguration::printHelp() const
{
	for (unsigned i=0;i<TOTAL_SETTINGS();i++)
		printf("%s=%s (type %s)\n  %s\n",
			_predefines[i]._name,
			get(_predefines[i]._name),
			_predefines[i]._type,
			_predefines[i]._description);
}



