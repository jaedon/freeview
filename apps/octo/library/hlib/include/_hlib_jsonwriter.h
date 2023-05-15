
#define	____HLIB_JSONWRITER_README___________________

/******************************************************************************
 [Example Code]

 void	 test_jsonwriter(void)
 {
	 HINT32  jw;
	 HUINT8  binary[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
	 const HCHAR *text;
	 HCHAR	 errmsg[256];

	 jw = HLIB_JSONWRITER_Open();
	 if (jw)
	 {
		 HxJSONWRITER_ObjectBegin(jw);
		 {
			 HxJSONWRITER_String(jw, "foo", "bar");
			 HxJSONWRITER_Array(jw, "array");
			 {
				 HxJSONWRITER_StringValue(jw, "string");
				 HxJSONWRITER_IntegerValue(jw, 1234);
				 HxJSONWRITER_FloatValue(jw, 3.14159);
				 HxJSONWRITER_BooleanValue(jw, TRUE);
				 HxJSONWRITER_NullValue(jw);
				 HxJSONWRITER_BinaryValue(jw, binary, 10);
				 HxJSONWRITER_UserJSONValue(jw, "{\"userjson\":null}");

				 HxJSONWRITER_ObjectBegin(jw);
				 {
					 HxJSONWRITER_Array(jw, "nested_array");
					 {
						 HxJSONWRITER_NullValue(jw);
					 }
					 HxJSONWRITER_ArrayEnd(jw);
				 }
				 HxJSONWRITER_ObjectEnd(jw);
				 HxJSONWRITER_Object(jw, "nested_null_object");
				 HxJSONWRITER_ObjectEnd(jw);
			 }
			 HxJSONWRITER_ArrayEnd(jw);

			 HxJSONWRITER_Integer(jw, "integer", 1818);
			 HxJSONWRITER_Float(jw, "float", 0.0001);
			 HxJSONWRITER_False(jw, "false");
			 HxJSONWRITER_True(jw, "true");
			 HxJSONWRITER_Null(jw, "null");
			 HxJSONWRITER_String(jw, "URL", "ABCDE가나다라 1234!@#$%^&*()_+-=");
			 HxJSONWRITER_Binary(jw, "binary", binary, 10);
			 HxJSONWRITER_UserJSON(jw, "userdata", "{\"userjson\":false}");
		 }
		 HxJSONWRITER_ObjectEnd(jw);

		 text = HLIB_JSONWRITER_Error(jw);
		 if (text)
		 {
			 printf("JSONWRITER: ERROR(%s)\n", text);
		 }
		 else
		 {
			 text = HLIB_JSONWRITER_GetJSON(jw);
			 if (text)
				 printf("JSONWRITER:\n%s\n", text);
		 }
		 HLIB_JSONWRITER_Close(jw);
	 }
 }


 [Expected Result]
 {
    "foo": "bar",
    "array": ["string", 1234, 3.141590, true, null, "QUJDREVGR0hJSg==", {
        "userjson": null
    }, "nested_object", {
        "nested_array": [null]
    }, "nested_null_object", {}],
    "integer": 1818,
    "float": 0.000100,
    "false": false,
    "true": true,
    "null": null,
    "URL": "ABCDEF%EA%B0%80%EB%82%98%EB%8B%A4%EB%9D%BC%201234!%40%23%24%25%5E%26*()_%2B-%3D",
    "binary": "QUJDREVGR0hJSg==",
    "userdata": {
        "userjson": false
    }
}
 ************************************************************************************/

#ifndef	HLIB_JSONWRITER_H__
#define HLIB_JSONWRITER_H__

#include "htype.h"


#ifdef	__cplusplus

#include <cstdio>
#include <cstdarg>
extern	"C" {

#else

#include <stdio.h>
#include <stdarg.h>

#endif

#define	HxJSONWRITER_DEBUG

/**
 * open JSON writer
 *
 * @return	descriptor of JSON writer
 */
HINT32	HLIB_JSONWRITER_Open (void);

/**
 * open JSON writer with custom malloc & fre
 *
 * @param	memalloc	malloc
 * @param	memfree		free
 * @return	descriptor of JSON writer
 */
HINT32	HLIB_JSONWRITER_OpenEx (void *(* memalloc)(size_t), void(* memfree)(void *));

/**
 * close JSON writer
 *
 * @param	writer	descriptor of JSON writer
 */
void	HLIB_JSONWRITER_Close (HINT32 writer);

/**
 * close JSON writer without JSON data
 *
 * @param	writer	descriptor of JSON writer
 * @return	JSON (if NULL, error)
 */
HCHAR *	HLIB_JSONWRITER_CloseWithoutJSON (HINT32 writer);

/**
 * clear JSON writer
 *
 * @param	writer	descriptor of JSON writer
 */
void	HLIB_JSONWRITER_Clear (HINT32 writer);


void	HLIB_JSONWRITER_SetEncodeExceptionToken (HINT32 writer, HCHAR *szToken);

void	HLIB_JSONWRITER_ResetEncodeExceptionToken (HINT32 writer);


/**
 * check JSON writer error
 *
 * @param	writer	descriptor of JSON writer
 * @return	if (error) return error text
 */
const HCHAR *HLIB_JSONWRITER_Error (HINT32 writer);

/**
 * get JSON
 *
 * @param	writer	descriptor of JSON writer
 * @return	JSON (if NULL, check HLIB_JSONWRITER_Error)
 */
const HCHAR	*HLIB_JSONWRITER_GetJSON (HINT32 writer);

/**
 * get length with null
 *
 * @param	writer	descriptor of JSON writer
 * @return	current offset
 */
HINT32	HLIB_JSONWRITER_GetLength(HINT32 writer);

#ifndef	HxJSONWRITER_DEBUG
/**
 * write json data JSON writer
 *
 * @param	writer	descriptor of JSON writer
 * @param	type	see below table
 * @param	...		see below table
 *
 *  +---------------------+--------------------------+
 *  | type | description  | param (separated with ,) |
 *  +------+--------------+--------------------------+
 *  |   {  | object-begin | void                     |
 *  |   }  | object-end   | void                     |
 *  |   [  | array-begin  | void                     |
 *  |   ]  | array-end    | void                     |
 *  |   s  | string       | const HCHAR * string     |
 *  |   d  | integer      | HINT32 integer           |
 *  |   f  | float        | HFLOAT64 float           |
 *  |   x  | binary       | void *data, HUINT32 size |
 *  |   b  | boolean      | HBOOL boolean            |
 *  |   n  | null         | void                     |
 *  |   u  | userjson     | const HCHAR * userjson   |
 *  +------+--------------+--------------------------+
 * @return	0: success, < 0: error
 */
HINT32	HLIB_JSONWRITER_Write (HINT32 writer, HCHAR type, ...);

/**
 * batch write
 *
 * @param	writer	descriptor of JSON writer
 * @param	types	number of write command
 * @param	...		(param, [param])
 * @return	0: success, < 0: error
 */
HINT32	HLIB_JSONWRITER_Batch (HINT32 writer, const HCHAR *types, ...);

#else
HINT32	HLIB_JSONWRITER_DebugWrite (const HCHAR *file, HINT32 line, HINT32 writer, HCHAR type, ...);
HINT32	HLIB_JSONWRITER_DebugBatch (const HCHAR *file, HINT32 line, HINT32 writer, const HCHAR *types, ...);
#define	HLIB_JSONWRITER_Write(w,t,...)		HLIB_JSONWRITER_DebugWrite(__FILE__,__LINE__,w,t,##__VA_ARGS__)
#define	HLIB_JSONWRITER_Batch(w,t,...)		HLIB_JSONWRITER_DebugBatch(__FILE__,__LINE__,w,t,##__VA_ARGS__)
#endif

#define	HxJSONWRITER_Object(writer, key)				HLIB_JSONWRITER_Batch(writer, "s{", key)
#define	HxJSONWRITER_ObjectBegin(writer)				HLIB_JSONWRITER_Write(writer, '{')
#define	HxJSONWRITER_ObjectEnd(writer)					HLIB_JSONWRITER_Write(writer, '}')

#define	HxJSONWRITER_Array(writer, key)					HLIB_JSONWRITER_Batch(writer, "s[", key)
#define	HxJSONWRITER_ArrayBegin(writer)					HLIB_JSONWRITER_Write(writer, '[')
#define	HxJSONWRITER_ArrayEnd(writer)					HLIB_JSONWRITER_Write(writer, ']')

#define	HxJSONWRITER_String(writer, key, str)			HLIB_JSONWRITER_Batch(writer, "ss", key, str)
#define	HxJSONWRITER_StringValue(writer, str)			HLIB_JSONWRITER_Write(writer, 's', str)

#define	HxJSONWRITER_Integer(writer, key, num)			HLIB_JSONWRITER_Batch(writer, "sd", key, num)
#define	HxJSONWRITER_IntegerValue(writer, num)			HLIB_JSONWRITER_Write(writer, 'd', num)

#define	HxJSONWRITER_Unsigned(writer, key, num)			HLIB_JSONWRITER_Batch(writer, "si", key, num)
#define	HxJSONWRITER_UnsignedValue(writer, num)			HLIB_JSONWRITER_Write(writer, 'i', num)

#define	HxJSONWRITER_Float(writer, key, num)			HLIB_JSONWRITER_Batch(writer, "sf", key, num)
#define	HxJSONWRITER_FloatValue(writer, num)			HLIB_JSONWRITER_Write(writer, 'f', num)

#define	HxJSONWRITER_Boolean(writer, key, arg)			HLIB_JSONWRITER_Batch(writer, "sb", key, arg)
#define	HxJSONWRITER_BooleanValue(writer, arg)			HLIB_JSONWRITER_Write(writer, 'b', arg)

#define	HxJSONWRITER_True(writer, key)					HxJSONWRITER_Boolean(writer, key, TRUE)
#define	HxJSONWRITER_False(writer, key)					HxJSONWRITER_Boolean(writer, key, FALSE)
#define	HxJSONWRITER_TrueValue(writer)					HxJSONWRITER_BooleanValue(writer, TRUE)
#define	HxJSONWRITER_FalseValue(writer)					HxJSONWRITER_BooleanValue(writer, FALSE)

#define	HxJSONWRITER_Null(writer, key)					HLIB_JSONWRITER_Batch(writer, "sn", key)
#define	HxJSONWRITER_NullValue(writer)					HLIB_JSONWRITER_Write(writer, 'n')

#define	HxJSONWRITER_Binary(writer, key, data, size)	HLIB_JSONWRITER_Batch(writer, "sx", key, data, size)
#define	HxJSONWRITER_BinaryValue(writer, data, size)	HLIB_JSONWRITER_Write(writer, 'x', data, size)

#define	HxJSONWRITER_UserJSON(writer, key, arg)			HLIB_JSONWRITER_Batch(writer, "su", key, arg)
#define	HxJSONWRITER_UserJSONValue(writer, arg)			HLIB_JSONWRITER_Write(writer, 'u', arg)

#ifdef	__cplusplus
}
#endif

#endif /* HLIB_JSONWRITER_H__ */

