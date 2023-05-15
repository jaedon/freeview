// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __CHANNELCONFIG
#define __CHANNELCONFIG

#include "jsplugin.h"

#include "ChannelList.h"
#ifdef OIPF
#include "EventTarget.h"
#include "EmptyEvent.h"
#endif // OIPF

#include "humax/HDataTypes.h"
#include "humax/ChannelScanParameters.h"

namespace Ooif
{
typedef enum
{
    DEFAULT,
    RECORDING
} ChannelConfigSelection;
#ifdef OIPF
class ChannelConfig : public EventTarget
{
private:
	static list<ChannelConfig*> instances;
#else
class ChannelConfig : public ObjectInstance
{
#endif // OIPF
protected:
	ChannelList *channels;
	ChannelConfigSelection selection;
	humax::SearchedInfo_Data	*searchedInfos;

private:
	static	int	onChannelConfigEventOPL(void *pvData);
	static	int	convertOPLParams(jsplugin_value *argv, int *pnCount, void **pvData);

#if defined(OIPF) && defined(HMX_WEBUI)
	static	int countListener (const char *name);
#endif

public:
	ChannelConfig();
	ChannelConfig(ChannelConfigSelection selection);
	~ChannelConfig();
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
#ifdef OIPF
	// Specific functions for this interface
	static int createFilteredList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelScanParametersObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelScanOptionsObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelSearchedInfoObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

#ifdef	HMX_WEBUI
	static int removeChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int removeChannelList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setSelectedConflictLcn(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif
	static void onChannelConfigEvent(ChannelConfigEvent event, ChannelConfigEventInfo *info);
	/*
		OIPF 2.1 Spec������ Stop Scan�ÿ� Abort�ǰ� ä�θ���Ʈ ������ �źεǾ�� �ϳ�,
		HUMAX�ʿ信 ���� stopScan���� ������ Scan���Ŀ� commitScan(bSave) flag�� ���� �����ϴ� �κ��� ���� ��Ų��.
		���� ������ startScan�ÿ� �߰ߵɶ� ���� service�߰� �ϰ� updateChannelList event�� �����־�� ��.
		��, stopScan�� �Ϻ��� scan instance�� destroy�ϹǷ� stopScan���� commitScan�� ȣ��Ǿ�� ����ȴ�. (--> 2.1 spec)
		startScan ---> wait complete ---> commitScan ---> finish
		startScan ---> stopScan ---> finish
	*/
	static void onChannelConfigEvent(humax::ChannelConfigEvent event, humax::ChannelConfigEventInfo *info);
	static const ClassType class_name = CLASS_CHANNEL_CONFIG;

#if defined(HMX_WEBUI)
	static int controlScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void	convertScanParameters(ChannelScanParameters *scanParameters, void *pvParams);
#endif
#endif // OIPF
#ifdef JLABS
	static int createChannelListCollection(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // JLABS

#ifdef JLABS_JCOM
	static int createChannelMask(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void onResultCreateMaskEvent(int result);
#endif //JLABS_JCOM
};

class CSearchedInfo : public ObjectInstance
{
	private:
		int					infoCount;
		humax::SearchedInfo_Data	*searchedInfos;

	public:
		CSearchedInfo(int _infoCount, humax::SearchedInfo_Data *_searchedInfo);
		~CSearchedInfo();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static int	getInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	getStringInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	setInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int	setStringInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		humax::SearchedInfo_Data	getInfo(int index);
		static const ClassType class_name = CLASS_SEARCHEDINFO;
};

} /* namespace Ooif */

#endif // __CHANNELCONFIG
