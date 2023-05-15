// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "OOIFUtils.h"
#include "macros.h"

using namespace Ooif;

OOIFUtils::WindowGlobalContextMap OOIFUtils::global_contexts;

//static
char *OOIFUtils::getHost(jsplugin_obj *context)
{
	char *host;
	int r = g_opera_callbacks->get_object_host(context, &host);
	if (r != JSP_CALLBACK_OK)
	{
		OOIF_LOG_WARNING("Error when getting context host");
		return NULL;
	}

	return host;
}

//static
long int OOIFUtils::getWindow(jsplugin_obj *context)
{
	long int window;
	int r = g_opera_callbacks->get_window_identifier(context, &window);

	if (r != JSP_CALLBACK_OK)
	{
		OOIF_LOG_WARNING("Error when getting context window");
		return -1;
	}
	return window;
}

void OOIFUtils::registerGlobalContext(jsplugin_obj* context)
{
	global_contexts.insert(std::make_pair(getWindow(context), context));
	int r = g_opera_callbacks->add_unload_listener(context, OOIFUtils::onGlobalContextUnload);
	if (r != JSP_CALLBACK_OK)
		OOIF_LOG_WARNING("Failed to add unload listener for global context (%p)", context);
}

void OOIFUtils::unregisterGlobalContext(jsplugin_obj* context)
{
	// It needs to be done this way because context is already invalid
	// at this point...
	for (WindowGlobalContextMap::iterator it = global_contexts.begin(); it != global_contexts.end(); ++it)
	{
		if (it->second == context)
		{
			global_contexts.erase(it);
			return;
		}
	}
}

jsplugin_obj* OOIFUtils::getGlobalContext(long int window)
{
	WindowGlobalContextMap::iterator it = global_contexts.find(window);
	if (it != global_contexts.end())
		return it->second;
	else
		return NULL;
}

jsplugin_obj* OOIFUtils::getGlobalContext(jsplugin_obj *context)
{
	return OOIFUtils::getGlobalContext(OOIFUtils::getWindow(context));
}

StringCollection *OOIFUtils::CreateStringCollection(int count, char **strings)
{
	StringCollection *collection = new StringCollection;
    if (NULL != strings)
    {
	for (int i=0; i<count; i++)
	{
		collection->addItem(strings[i]);
	}
	HLIB_STD_MemFree(strings);
    }
	return collection;
}

StringWithLengthCollection* OOIFUtils::CreateStringWithLengthCollection(int count, char **strings, int *string_lenghts)
{
	StringWithLengthCollection *collection = new StringWithLengthCollection;
	if(!string_lenghts)
	{
		for (int i=0; i<count; i++)
		{
			collection->addItem(new StringWithLength(strings[i]));
		}
	}
	else
	{
		for (int i=0; i<count; i++)
		{
			collection->addItem(new StringWithLength(strings[i], string_lenghts[i]));
		}
		HLIB_STD_MemFree(string_lenghts);
	}
	HLIB_STD_MemFree(strings);
	return collection;
}

char *OOIFUtils::tmp_buff = NULL;
size_t OOIFUtils::tmp_buff_size = 0;
char *OOIFUtils::getTmpBuffer(size_t size)
{
	if (tmp_buff)
	{
		if (tmp_buff_size >= size && tmp_buff_size < size * 2)
			return tmp_buff;
		else
			freeTmpBuffer();
	}

	tmp_buff = (char*)HLIB_STD_MemAlloc(size);
	if (tmp_buff)
		tmp_buff_size = size;
	return tmp_buff;
}

void OOIFUtils::freeTmpBuffer()
{
	if(tmp_buff != NULL)
	{
		HLIB_STD_MemFree(tmp_buff);
	}
	tmp_buff=NULL;
	tmp_buff_size=0;
}

int OOIFUtils::getLengthOfBinaryDataInUTF8(const unsigned char *data, int data_len)
{
	int utf8_len = data_len;
	for (int i = 0; i < data_len; i++)
	{
		if (data[i] >= 0x80u)
			utf8_len++;
	}
	return utf8_len;
}

void OOIFUtils::wrapBinaryDataInUTF8(char *utf8, int &utf8_len, const unsigned char *data, int data_len)
{
	utf8_len = 0;
	for (int i = 0; i < data_len; i++)
	{
		if (data[i] >= 0x80u)
		{
			utf8[utf8_len++] = 0xc0u | (data[i] >> 6);
			utf8[utf8_len++] = 0x80u | (data[i] & 0x3fu);
		}
		else
			utf8[utf8_len++] = data[i];
	}
}

bool OOIFUtils::unwrapBinaryDataFromUTF8(unsigned char *data, int &data_len, const char *utf8, int utf8_len)
{
	data_len = 0;
	for (int i = 0; i < utf8_len; )
	{
		unsigned char c = ((unsigned char*)utf8)[i++];
		if (c >= 0x80u)
		{
			if (c - 0xc0u >= 0x04u) // codepoint >= 256, or invalid continuation byte
				return false;
			else
				data[data_len++] = ((c & 0x3u) << 6) | (utf8[i++] & 0x3fu);
		}
		else
		{
			data[data_len++] = c;
		}
	}
	return true;
}

int OOIFUtils::deserializeIntegerArray(const char *array, int *count, OOIFNumber **values)
{
	const char *delim = ",";
	int token_count = 1;
	char *array_ptr = NULL;
	int len_array = strlen(array);
	char *array_buf = (char *)HLIB_STD_MemAlloc((len_array + 1) * sizeof(char));

	if (array_buf == NULL)
	{
		return -1;
	}

	strncpy(array_buf, array, len_array);
	array_ptr = array_buf;

	while (*array_ptr != 0)
	{
		if (*array_ptr == *delim)
			token_count++;
		array_ptr++;
	}

	*values = (OOIFNumber *)HLIB_STD_MemAlloc(sizeof(OOIFNumber) * token_count);
	if (*values == NULL)
	{
		HLIB_STD_MemFree(array_buf);
		return -1;
	}

	char * arg1_tmp_buf, * pch;
	pch = strtok_r(array_buf, delim, &arg1_tmp_buf);
	int i = 0;
	while (pch != NULL)
	{
		(*values)[i] = atof(pch);
		i++;
		pch = strtok_r(NULL, delim, &arg1_tmp_buf);
	}
	*count = i;

	HLIB_STD_MemFree(array_buf);
	return 0;
}

bool OOIFUtils::isNaN(double value)
{
	volatile double _value = value;
	return (_value != _value);
}
