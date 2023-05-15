// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
#ifndef __DRMCONTROLINFO
#define __DRMCONTROLINFO

#include "NativeDrmAgent.h"

#include "ObjectInstance.h"
#include "WrapperObject.h"

namespace Ooif
{
class DRMControlInformationDataWrapper
{
public:
	DRMControlInformationData data;
	void init(void *id);
	~DRMControlInformationDataWrapper();
};

class DRMControlInformation : public ObjectInstance,
	public DataObject<DRMControlInformation, DRMControlInformationDataWrapper>
{
private:
	DRMControlInformationData data;
	void init();

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	DRMControlInformation(DRMControlInformationHandle i);
	DRMControlInformation(const DRMControlInformation &original);
	~DRMControlInformation();

	static void releaseNative(DRMControlInformationHandle id);
};
} /* namespace Ooif */

#endif // __DRMCONTROLINFO
#endif // HBBTV_DOWNLOAD || OIPF
