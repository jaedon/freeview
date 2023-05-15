// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NJSE_UTILS_H
#define __NJSE_UTILS_H
/**
   Class containing static utility methods for NJSE
*/

#include "jsplugin.h"
#include "Collection.h"
#include "NativeNJSETypes.h"

#include <map>

namespace Lovefilm
{
class NJSEUtils
{
private:
	/** A global temporary string buffer */
	static char *tmp_buff;
	/** Indicates the currently allocated size of str_buff */
	static size_t tmp_buff_size;
	/** Map of the window - global context correspondence */
	typedef std::map<long int, jsplugin_obj*> WindowGlobalContextMap;
	static WindowGlobalContextMap global_contexts;

protected:
	static void onGlobalContextUnload(jsplugin_obj *c) { unregisterGlobalContext(c); };

public:
	/** Gets the host URL (FQDN) of the context. */
	static char *getHost(jsplugin_obj *context);
	/** Gets the window the context belongs to */
	static long int getWindow(jsplugin_obj *context);

	/** Registers the given context as a global context for current window */
	static void registerGlobalContext(jsplugin_obj* context);
	/** Unregisters the given context from being the global context for current window */
	static void unregisterGlobalContext(jsplugin_obj* context);
	/** Returns the global jsplugin context for the given window.*/
	static jsplugin_obj* getGlobalContext(long int window);

	/** Get the global context object, unique per HTML document,
	 * for specified JS context
	 *
	 * Returns NULL in case of error
	 */
	static jsplugin_obj* getGlobalContext(jsplugin_obj *context);

	/** Creates a new string collection from the char* in strings. Frees strings after use.*/
	static StringCollection *CreateStringCollection(int count, char **strings);

	/** Returns a pointer to a temporary buffer that may be used to store
	 * strings that NJSE needs to retain ownership of when they need to be
	 * returned to core. Any buffer previously obtained through
	 * getTmpBuffer is invalidated by the subsequent call to getTmpBuffer.
	 *
	 * Do not free this buffer directly. You may use freeTmpBuffer to free it,
	 * but it isn't necessary to call it at all, excecpt when NJSE is unloaded.
	 *
	 * Returns NULL when a large enough buffer cannot be allocated.
	 */
	static char *getTmpBuffer(size_t size);

	/** Frees the temporary buffer managed by getTmpBuffer This function
	 * does not need to be called by each user of getTmpBuffer, and it is
	 * sufficient to call it when NJSE is unloaded.
	 */
	static void freeTmpBuffer();

	/** Calculates the buffer size needed to wrap a sequence of binary data
	 * in UTF-8. Does not include a null terminator.
	 */
	static int getLengthOfBinaryDataInUTF8(const unsigned char *data, int data_len);

	/** Wrap a sequence of binary data in UTF-8 by treating each byte as a
	 * unicode code point (effectively "UCS-1"). To determine how big a
	 * buffer is required, call getLengthOfBinaryDataInUTF8.
	 */
	static void wrapBinaryDataInUTF8(char *utf8, int &utf8_len, const unsigned char *data, int data_len);

	/** Unwrap a sequence of binary data from a UTF-8 string previously
	 * created by wrapBinaryDataInUTF8. Aborts and returns false if the
	 * UTF-8 string contains any code point greater than 255. The size
	 * of the output data is guaranteed to be less than or equal to the
	 * size of the input data, so a buffer of the same size can be used.
	 */
	static bool unwrapBinaryDataFromUTF8(unsigned char *data, int &data_len, const char *utf8, int utf8_len);

	/** Deserializes doubles array from string. The string is expected to be
	 * a comma separated list. The caller of the function is responsible for
	 * free()'ing the array returned in values.
	 */
	static int deserializeIntegerArray(const char *array, int *count, NJSENumber **values);

	/** Checks if value of type double is 'Not a Number'
	 */
	static bool isNaN(double value);
};
} /* namespace Lovefilm */

#endif // __NJSE_UTILS_H
