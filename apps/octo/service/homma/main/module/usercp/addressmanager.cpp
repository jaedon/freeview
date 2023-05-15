#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <hlib.h>
#include "addressmanager.h"
#include "addressmanager_c.h"


#define ADDRESS_MANAGER_VER		"1.0"


AddressManager::AddressManager()
{
}

AddressManager::~AddressManager()
{
	clear();
}

void AddressManager::setPath(string path)
{
	this->path = path;
}

string AddressManager::getPath(void)
{
	return path;
}

void AddressManager::resetVector(void)
{
	clear();
}

bool AddressManager::addAddress(string mac)
{
	ADDRESS_IT it;

	for(it = _addresses.begin(); it != _addresses.end(); it++)
	{
		if((*it) == mac)
		{
			return false;
		}
	}

	_addresses.push_back(mac);

	return true;
}

bool AddressManager::saveAsFile(void)
{
	ADDRESS_IT it;
	string path = getPath();
	string mac;
	FILE* fp;

	fp = fopen(path.c_str(), "wt");
	if (fp == NULL)
	{
		HxLOG_Error("Cannot open file(%s) : errno(%d)\n", path.c_str(), errno);
		return false;
	}

	fprintf(fp, "<allowedlist ver='%s'>\n", ADDRESS_MANAGER_VER);

	for (it = _addresses.begin() ; it != _addresses.end() ; it++)
	{
		fprintf(fp, "  <deviceid>%s</deviceid>\n", (*it).c_str());
	}

	fprintf(fp, "</allowedlist>");

	fclose(fp);

	return true;
}

int AddressManager::getItemCount(void)
{
	return _addresses.size();
}

void AddressManager::clear()
{
	_addresses.clear();
}

#define	GOTO_IF(expr, pos)	do{if(expr){HxLOG_Debug(HxANSI_COLOR_RED(" %s failed!!\n"), #expr);goto pos;}}while(0)

bool AddressManager::parseXmlToList(void)
{
	HERROR			hErr = ERR_FAIL;
	HERROR			hResult = ERR_FAIL;
	HxXMLDOC		hXmlDocHandle = NULL;
	HxXMLELEMENT	hRootElement = NULL;

	HxList_t		*pstDeviceIdHead = NULL;
	HxList_t		*pstDeviceIdIter = NULL;
	HxXMLELEMENT	hDeviceId = NULL;

	HUINT8			*pszPath = (HUINT8*)getPath().c_str();
	HUINT8			*pszContent = NULL;

	hErr = HLIB_XML_InitParser();
	GOTO_IF(hErr != ERR_OK, func_end);

	hErr = HLIB_XML_ParseFile(pszPath, &hXmlDocHandle);
	GOTO_IF(hErr != ERR_OK, func_end);

	hErr = HLIB_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	GOTO_IF(hErr != ERR_OK, func_end);

	hErr = HLIB_XML_FindChildElementByName(hRootElement, (HUINT8*)"deviceid", &pstDeviceIdHead);
	GOTO_IF(hErr != ERR_OK, func_end);

	for (pstDeviceIdIter = HLIB_LIST_First(pstDeviceIdHead); pstDeviceIdIter != NULL; pstDeviceIdIter = HLIB_LIST_Next(pstDeviceIdIter))
	{
		pszContent = NULL;

		hDeviceId = (HxXMLELEMENT)HLIB_LIST_Data(pstDeviceIdIter);
		if (hDeviceId == NULL)
		{
			HxLOG_Warning("null data \n");
			continue;
		}

		hErr = HLIB_XML_GetElementContents(hXmlDocHandle, hDeviceId, &pszContent);
		if (hErr != ERR_OK)
		{
			HxLOG_Warning("HLIB_XML_GetElementContents failed \n");
			continue;
		}

		if(pszContent != NULL)
		{
			HxLOG_Debug("deviceid[%s]\n", pszContent);

			_addresses.push_back(string((const char*)pszContent));

			HLIB_XML_FreeAttrContents(pszContent);
		}
	}

	hResult = ERR_OK;

func_end:
	if(hXmlDocHandle != NULL)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	if (pstDeviceIdHead != NULL)
	{
		HLIB_LIST_RemoveAll(pstDeviceIdHead);
	}

	return ((hResult == ERR_OK) ? true : false);
}

// C Interfaces ================================================
AddressManager* g_AddressMgr = NULL;

bool ADDRMGR_Init(const char* path)
{
	g_AddressMgr = new AddressManager();
	if (g_AddressMgr == NULL)
	{
		return false;
	}

	g_AddressMgr->setPath(string(path));
	return true;
}

bool ADDRMGR_Deinit(void)
{
	if (g_AddressMgr != NULL)
	{
		delete g_AddressMgr;
		g_AddressMgr = NULL;
	}
	return true;
}

void ADDRMGR_Reset(void)
{
	if (g_AddressMgr == NULL)
	{
		HxLOG_Error("AddManager is not initialized\n");
		return;
	}

	g_AddressMgr->resetVector();
}

bool ADDRMGR_Parse(void)
{
	if (g_AddressMgr == NULL)
	{
		HxLOG_Error("AddManager is not initialized\n");
		return false;
	}

	return g_AddressMgr->parseXmlToList();
}

int ADDRMGR_GetItemCount(void)
{
	if (g_AddressMgr == NULL)
	{
		HxLOG_Error("AddManager is not initialized\n");
		return false;
	}

	return g_AddressMgr->getItemCount();
}

bool ADDRMGR_AddAddress(const char* mac)
{
	if (g_AddressMgr == NULL)
	{
		HxLOG_Error("AddManager is not initialized\n");
		return false;
	}

	return g_AddressMgr->addAddress(mac);
}

bool ADDRMGR_SaveAsFile(void)
{
	if (g_AddressMgr == NULL)
	{
		HxLOG_Error("AddManager is not initialized\n");
		return false;
	}

	return g_AddressMgr->saveAsFile();
}
