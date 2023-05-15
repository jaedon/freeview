// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "NativeDisc.h"

#include "opldiscinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OOIFReturnCode native_DiscInfoOnCreate()
{
	return OOIF_RETURN_OK;
}


#ifdef JLABS
// All return in megabytes
OOIFReturnCode native_HDiscInfoGetFree(HDiscInfoHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OplDiscInfo *item = (OplDiscInfo *)identifier;
	*retval = (OOIFNumber)item->FreeSize;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HDiscInfoGetTotal(HDiscInfoHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OplDiscInfo *item = (OplDiscInfo *)identifier;
	*retval = (OOIFNumber)item->TotalSize;

	return OOIF_RETURN_OK;
}

// The space reserved for scheduled or ongoing recordings or downloads
OOIFReturnCode native_HDiscInfoGetReserved(HDiscInfoHandle identifier, OOIFNumber *retval)
{
	ENTRY;

	OplDiscInfo *item = (OplDiscInfo *)identifier;
	*retval = (OOIFNumber)item->ReservedSize;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HDiscInfoGetIsFormatted(HDiscInfoHandle identifier, OOIFBoolean *retval)
{
	ENTRY;

	OplDiscInfo *item = (OplDiscInfo *)identifier;
	*retval = (OOIFBoolean)item->IsFormatted;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HDiscInfoGetDiscId(HDiscInfoHandle identifier, const char **retval)
{
	ENTRY;

	OplDiscInfo *item = (OplDiscInfo *)identifier;

	if (item->DiscId == NULL)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	*retval = item->DiscId;
	return OOIF_RETURN_OK;

}

OOIFReturnCode native_HDiscInfoGetName(HDiscInfoHandle identifier, const char **retval)
{
	ENTRY;

	OplDiscInfo *item = (OplDiscInfo *)identifier;

	if (item->Name == NULL)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}
	*retval = item->Name;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HDiscInfoSetName(HDiscInfoHandle identifier,  const char *value)
{
	ENTRY;

	OplDiscInfo *item = (OplDiscInfo *)identifier;

	if (item->DiscId == NULL)
	{
		return OOIF_RETURN_VALUE_UNDEFINED;
	}

	strncpy(item->Name, value, strlen(item->Name));

	OPL_DiscInfo_SetStorageName(item->DiscId, value);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_HDiscInfoReleaseHandle(HDiscInfoHandle identifier)
{
	OplDiscInfo *item = (OplDiscInfo *)identifier;

	if (NULL != item)
	{
		if (NULL != item->DiscId)		{ HLIB_STD_MemFree(item->DiscId); 	}
		if (NULL != item->Name)			{ HLIB_STD_MemFree(item->Name); 	}
		HLIB_STD_MemFree(item);
	}

	return OOIF_RETURN_OK;
}
#endif
// OIPF

// All return in megabytes
OOIFReturnCode native_DiscInfoGetFree(OOIFNumber *retval)
{
#ifndef JLABS
	unsigned int size = OplDiscInfo::getFreeSize();
	*retval = static_cast<OOIFNumber>(size);
#endif
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DiscInfoGetTotal(OOIFNumber *retval)
{
#ifndef JLABS
	unsigned int size = OplDiscInfo::getTotalSize();
	*retval = static_cast<OOIFNumber>(size);
#endif
	return OOIF_RETURN_OK;
}

// The space reserved for scheduled or ongoing recordings or downloads
OOIFReturnCode native_DiscInfoGetReserved(OOIFNumber *retval)
{
#ifndef JLABS
	unsigned int size = OplDiscInfo::getReservedSize();
	*retval = static_cast<OOIFNumber>(size);
#endif
	return OOIF_RETURN_OK;
}

#endif // OIPF
