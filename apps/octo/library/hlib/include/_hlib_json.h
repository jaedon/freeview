/*************************************************************
 * @file		hlib_json.h
 * @date		2011/05/15
 * @author		Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header

 * http://www.humaxdigital.com
 *************************************************************/

#define	____HLIB_JSON_README___________________

/******************************************************************************
 [Example Code]

void json_example (void)
{
	HxJSON_t	 json;
	HCHAR		 buf[1024];

	HLIB_JSON_Init(eHLIB_JSON_Conf_None, NULL, NULL);

	json = HLIB_JSON_Decode("{\"signal\":{\"strength\":100, \"quality\":100, \"lock\":true}}");
	if (json && HxJSON_IsObject(json))
	{
	 HxJSON_t signal = HxJSON_Object_Get(json, "signal");
	 if (signal && HxJSON_IsObject(signal))
	 {
		 HxJSON_t value;

		 value = HxJSON_Object_Get(signal, "strength");
		 if (value)
			 printf("strength: %d\n", HxJSON_Integer_Get(value));
		 value = HxJSON_Object_Get(signal, "quality");
		 if (value)
			 printf("quality: %d\n", HxJSON_Integer_Get(value));
		 value = HxJSON_Object_Get(signal, "lock");
		 if (value)
			 printf("lock: %s\n", HxJSON_Boolean_Get(value) ? "true" : "false");
	 }
	}

	HLIB_JSON_Encode(json, buf, 1024);
	printf("1. encoded:%s\n", buf);
	HLIB_JSON_Delete(json);

	json = HxJSON_Object_New();
	if (json)
	{
	 HxJSON_t signal = HxJSON_Object_New();
	 HxJSON_t value;

	 HxJSON_Object_Set(json, "signal", signal);

	 HxJSON_Object_Set(signal, "strength", HxJSON_Integer_New(100));
	 HxJSON_Object_Set(signal, "quality",  HxJSON_Integer_New(80));
	 HxJSON_Object_Set(signal, "lock",	   HxJSON_TRUE_New());

	 HLIB_JSON_Encode(json, buf, 1024);
	 printf("2. %s\n", buf);

	 value = HxJSON_Object_Get(signal, "strength");
	 HxJSON_Integer_Set(value, 30);

	 HLIB_JSON_Encode(json, buf, 1024);
	 printf("3. %s\n", buf);

	 HLIB_JSON_Delete(json);
	}
}

 [Expected Result]

  strength: 100
  quality: 100
  lock: true
  1. encoded:{"signal": {"strength": 100, "quality": 100, "lock"
  2. {"signal": {"strength": 100, "quality": 80, "lock": true}}
  3. {"signal": {"strength": 30, "quality": 80, "lock": true}}
 ************************************************************************************/

#ifndef __HLIB_JSON_H__
#define __HLIB_JSON_H__

#include <htype.h>

#define	____HLIB_JSON_TYPE___________________

typedef enum
{
	eHLIB_JSON_Conf_None			= 0,
	eHLIB_JSON_Conf_CheckUTF8		= (1 << 0),

	eHLIB_JSON_Conf_EncodeASCII		= (1 << 1),
	eHLIB_JSON_Conf_EncodeSortKey	= (1 << 2),
	eHLIB_JSON_Conf_EncodeCompact	= (1 << 3),
	eHLIB_JSON_Conf_EncodeIndent	= (1 << 4),

	eHLIB_JSON_Conf_Full			= (eHLIB_JSON_Conf_CheckUTF8 | eHLIB_JSON_Conf_EncodeIndent),
	eHLIB_JSON_Conf_Minimum			= eHLIB_JSON_Conf_EncodeCompact,
	eHLIB_JSON_Conf_Default			= eHLIB_JSON_Conf_EncodeIndent
} HxJSON_Config_e;

typedef enum
{
	eHLIB_JSON_Object,
	eHLIB_JSON_Array,
	eHLIB_JSON_String,
	eHLIB_JSON_Integer,
	eHLIB_JSON_Real,
	eHLIB_JSON_TRUE,
	eHLIB_JSON_FALSE,
	eHLIB_JSON_NULL
} HxJSON_Type_e;

typedef void *	HxJSON_t;

#define	____HLIB_JSON_APIs___________________

/**
 * Initialize HxJSON
 *
 * @param	conf	json configuration
 * @param	_malloc	custom memory allocation
 * @param	_free	custom memory allocation
 */
void		HLIB_JSON_Init (HxJSON_Config_e conf, void *(*_malloc)(unsigned long), int (*_free)(void *));

/**
 * Create instance of 'HxJSON_t'
 *
 * @param	type	json type (@see HxJSON_Type_e)
 * @param	...		type specific aruments @see each of the Types_New()
 * @return	instance of 'HxJSON_t'
 */
HxJSON_t	HLIB_JSON_New (HxJSON_Type_e type, ...);

/**
 * Destroy instance of 'HxJSON_t'
 *
 * @param	json	instance
 */
void		HLIB_JSON_Delete (HxJSON_t json);

/**
 * Clone instance of 'HxJSON_t'
 *
 * @param	json	instance
 * @return	clone
 */
HxJSON_t	HLIB_JSON_Clone (HxJSON_t json);

/**
 * General Getter(Accessor)
 *
 * @param	json	instance
 * @param	key		(Object: const char *key, Array: HINT32 index)
 * @return	value	@see each of the Types_Get()
 */
void *		HLIB_JSON_Get (HxJSON_t json, void *key);

/**
 * General Setter(Mutator)
 *
 * @param	json	instance
 * @param	key		(Object: const char *key, Array: HINT32 index)
 * @param	value	value.... to be set into the 'json'
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_JSON_Set (HxJSON_t json, void *key, void *value);

/**
 * Remove Value (Available Object and Array)
 *
 * @param	json	instance
 * @param	key		(Object: const char *key, Array: HINT32 index)
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_JSON_Remove (HxJSON_t json, void *key);

/**
 * Detach Value (Available Object and Array)
 *
 * @param	json	instance
 * @param	key		(Object: const char *key, Array: HINT32 index)
 * @return	value (matched with key)
 */
HxJSON_t	HLIB_JSON_Detach (HxJSON_t json, void *key);

/**
 * Clear instance (Available Object, Array and String)
 *
 * @param	json	instance
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_JSON_Clear (HxJSON_t json);

/**
 * Get the number of elements in (Array or Object)
 *
 * @param	json	instance
 * @return	0 ~ size
 */
HINT32		HLIB_JSON_GetSize (HxJSON_t json);

/**
 * for_each (Object, Array)
 *
 * @param	json	instance
 * @param	doIt	callback function
 * @param	user	userdata
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_JSON_ForEach (HxJSON_t json, HBOOL (* doIt)(void *key, void *value, void *user), void *user);

/**
 * Encode 'json' as a string
 *
 * @param	json	instance
 * @param	buf		text buffer (if NULL, HxJSON malloc string buffer)
 * @param	len		buffer len
 * @return	encoded text (if 'buf' NULL, It must be 'free()'ed by caller)
 */
HCHAR *		HLIB_JSON_Encode (const HxJSON_t json, HCHAR *buf, HUINT32 len);

void		HLIB_JSON_FreeMemory (void *data);

/**
 * Decode 'jsontext' and return the Array of Object it contains
 *
 * @param	jsontext	json string
 * @return	instance of 'HxJSON_t' or NULL
 */
HxJSON_t	HLIB_JSON_Decode (const HCHAR *jsontext);
HxJSON_t	HLIB_JSON_DecodeFile (const HCHAR *path);

/**
 * Build HxJSON_t object
 *
 * +-----+----------+-------------------------+
 * | key | comments | type (argument)         |
 * +-----+----------+-------------------------+
 * |  s  | string   | const HCHAR *           |
 * |  n  | null     | no argument is consumed |
 * |  b  | boolean  | HBOOL                   |
 * |  i  | integer  | HINT32                  |
 * |  f  | real     | HFLOAT32                |
 * |  o  | instance | HxJSON_t                |
 * +-----+----------+-------------------------+
 * ! [fmt] array
 * ! {fmt} object
 *
 * example) HLIB_JSON_Build("{s:s, s:i}", "foo", "bar", "age", 28);
 *
 * @param	fmt		@see above table
 * @param	vargs
 * @return	instance of 'HxJSON_t'
 */
HxJSON_t	HLIB_JSON_Build (const HCHAR *fmt, ...);

/**
 * Scan HxJSON_t
 *
 * example) {
 *     const char *str;
 *     HxJSON_t    value;
 *
 *     HLIB_JSON_Scan(json, "{s:s, s:o}", "foo", &str, "person", &value);
 * }
 *
 * @param	json	instance
 * @param	fmt		@see MxJSON_Build() table
 * @param	vargs	(args must be passed into function with its address.., &args)
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_JSON_Scan (HxJSON_t json, const HCHAR *fmt, ...);

// These are instantly added. need review..
HxJSON_t	HLIB_JSON_Real_New_fn ( double value );
double		HLIB_JSON_Real_Get_fn (HxJSON_t json, void *key);
HERROR		HLIB_JSON_Real_Set_fn (HxJSON_t json, void *key, HFLOAT64 value);
/**
 * Get type of JSON
 *
 * @param	json	instance
 * @return	type @see HxJSON_Type_e
 */
HxJSON_Type_e	HLIB_JSON_TypeOf (HxJSON_t json);
#define		HxJSON_IsObject(json)	(HLIB_JSON_TypeOf(json) == eHLIB_JSON_Object)
#define		HxJSON_IsArray(json)	(HLIB_JSON_TypeOf(json) == eHLIB_JSON_Array)
#define		HxJSON_IsString(json)	(HLIB_JSON_TypeOf(json) == eHLIB_JSON_String)
#define		HxJSON_IsInteger(json)	(HLIB_JSON_TypeOf(json) == eHLIB_JSON_Integer)
#define		HxJSON_IsReal(json)		(HLIB_JSON_TypeOf(json) == eHLIB_JSON_Real)
#define		HxJSON_IsTRUE(json)		(HLIB_JSON_TypeOf(json) == eHLIB_JSON_TRUE)
#define		HxJSON_IsFALSE(json)	(HLIB_JSON_TypeOf(json) == eHLIB_JSON_FALSE)
#define		HxJSON_IsNULL(json)		(HLIB_JSON_TypeOf(json) == eHLIB_JSON_NULL)
#define		HxJSON_IsNumber(json)	(HxJSON_IsInteger(json) || HxJSON_IsReal(json))
#define		HxJSON_IsBoolean(json)	(HxJSON_IsTRUE(json) || HxJSON_IsFALSE(json))

//1 Object
#define		HxJSON_Object_New()					HLIB_JSON_New(eHLIB_JSON_Object)
// @param value	(HxJSON_t)
#define		HxJSON_Object_Get(json,key)			(HxJSON_t)HLIB_JSON_Get(json, (void *)key)

HxJSON_t	HLIB_JSON_Object_GetObject (HxJSON_t json, const HCHAR *key);
HxJSON_t	HLIB_JSON_Object_GetArray (HxJSON_t json, const HCHAR *key);
HINT32		HLIB_JSON_Object_GetInteger (HxJSON_t json, const HCHAR *key);
HINT32		HLIB_JSON_Object_GetIntegerWithDefaultValue (HxJSON_t json, const HCHAR *key, HINT32 defvalue);
const HCHAR	*HLIB_JSON_Object_GetString (HxJSON_t json, const HCHAR *key);
HBOOL		HLIB_JSON_Object_GetBoolean (HxJSON_t json, const HCHAR *key);
HUINT32		HLIB_JSON_Object_GetBinary (HxJSON_t json, const HCHAR *key, void *value, HUINT32 n);
void *		HLIB_JSON_Object_GetBinaryDup (HxJSON_t json, const HCHAR *key, HUINT32 *size);

void *		HLIB_JSON_Object_Iterator (HxJSON_t json);
#if	defined(CONFIG_SUPPORT_REMA)
void *		HLIB_JSON_Object_IteratorAt (HxJSON_t json,const HCHAR *key);
#endif
const HCHAR *HLIB_JSON_Object_IteratorKey (void *iter);
HxJSON_t	HLIB_JSON_Object_IteratorValue (void *iter);
void *		HLIB_JSON_Object_NextIterator (HxJSON_t json, void *iter);

// @param value	(HxJSON_t)
//1 Warning!!!! (Set이 실패한 경우, value는 자동으로 삭제됨)
#define		HxJSON_Object_Set(json,key,value)	HLIB_JSON_Set(json, (void *)key, (void *)value)
#define		HxJSON_Object_Clear(json)			HLIB_JSON_Clear(json)
#define		HxJSON_Object_Remove(json,key)		HLIB_JSON_Remove(json, (void *)key)
/**
 * Update 'json' with the key-value pairs from 'otherobj', overwriting existing keys
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_JSON_Object_Update (HxJSON_t json, HxJSON_t otherobj);

#define	HxJSON_OBJECT_EACH(json,key,value,expr)	\
	do {\
		void *	__iter = HLIB_JSON_Object_Iterator(json);\
		for ( ;__iter ; __iter = HLIB_JSON_Object_NextIterator(json, __iter)) {\
			key    = HLIB_JSON_Object_IteratorKey(__iter);\
			value  = HLIB_JSON_Object_IteratorValue(__iter);\
			expr;\
		}\
	}while(0)

//1 Array
#define		HxJSON_Array_New()						HLIB_JSON_New(eHLIB_JSON_Array)
#define		HxJSON_Array_Get(json,index)			(HxJSON_t)HLIB_JSON_Get(json, (void *)index)
// @param value	(HxJSON_t)
//1 Warning!!!! (Set, Append, Insert, Extend 가  실패한 경우, value는 자동으로 삭제됨)
#define		HxJSON_Array_Set(json,index,value)		HLIB_JSON_Set(json, (void *)index, (void *)value)
// append 'value' to the end of array
HERROR		HLIB_JSON_Array_Append (HxJSON_t json, HxJSON_t value);
// insert 'value' to at position 'index'
HERROR		HLIB_JSON_Array_Insert (HxJSON_t json, HINT32 index, HxJSON_t value);
// Appends all elements in 'other' to the end of array 'json'
HERROR		HLIB_JSON_Array_Extend (HxJSON_t json, HxJSON_t other);

#define	HxJSON_ARRAY_EACH(json,value,expr)	\
	do {\
		HINT32	__i;\
		HINT32	__n = HLIB_JSON_GetSize(json);\
		for (__i = 0 ; __i < __n ; __i++) {\
			value = HxJSON_Array_Get(json, __i);\
			expr;\
		}\
	}while(0)

//1 String
#define		HxJSON_String_New(value)			HLIB_JSON_New(eHLIB_JSON_String, (void *)value)
#define		HxJSON_String_Get(json)				(const HCHAR *)HLIB_JSON_Get(json, NULL)
#define		HxJSON_String_Set(json,value)		HLIB_JSON_Set(json, NULL, (void *)value)

//1 Integer
#define		HxJSON_Integer_New(value)			HLIB_JSON_New(eHLIB_JSON_Integer, (void *)value)
#define		HxJSON_Integer_Get(json)			(HINT32)HLIB_JSON_Get(json, NULL)
#define		HxJSON_Integer_Set(json,value)		HLIB_JSON_Set(json, NULL, (void *)value)

//1 Real
#define		HxJSON_Real_New(value)				HLIB_JSON_Real_New_fn(value)
#define		HxJSON_Real_Get(json)				(HFLOAT64)HLIB_JSON_Real_Get_fn(json, NULL)
#define		HxJSON_Real_Set(json,value)			HLIB_JSON_Real_Set_fn(json, NULL, (HFLOAT64)value)

//1 TRUE, FALSE
#define		HxJSON_Boolean_New(value)			HLIB_JSON_New((value) ? eHLIB_JSON_TRUE : eHLIB_JSON_FALSE)
#define		HxJSON_TRUE_New()					HLIB_JSON_New(eHLIB_JSON_TRUE)
#define		HxJSON_FALSE_New()					HLIB_JSON_New(eHLIB_JSON_FALSE)
#define		HxJSON_Boolean_Get(json)			(HBOOL)(HxJSON_IsBoolean(json) ? HLIB_JSON_Get(json, NULL) : FALSE)

//1 NULL
#define		HxJSON_NULL_New()					HLIB_JSON_New(eHLIB_JSON_NULL)


/**
 * @see http://tools.ietf.org/pdf/draft-pbryan-zyp-json-pointer-00.pdf
 *
 *	Usage:
 *	{
 *		"foo": 1,
 *		"bar": {"baz": 2},
 *		"qux": [3, 4, 5]
 *	}
 *
 *	HLIB_JSON_Pointer_Get(json, "/");        // return self, the root of the resource itself
 *	HLIB_JSON_Pointer_Get(json, "/foo");     // the object referred to by the foo property
 *	HLIB_JSON_Pointer_Get(json, "/bar/baz");
 *	HLIB_JSON_Pointer_Get(json, "/qux/0");   // the first object in the "qux" array
 *	HLIB_JSON_Pointer_Get(json, "/qux/1");
 *	HLIB_JSON_Pointer_Get(json, "/qux/2");   // the third object in the "qux" array
 */
HxJSON_t	HLIB_JSON_Pointer_Get (HxJSON_t json, const HCHAR *pointer);

/**
 * HLIB_JSON_Pointer_Get() with type checking!!
 */
HxJSON_t	HLIB_JSON_Pointer_SafeGet (HxJSON_t json, HxJSON_Type_e type, const HCHAR *pointer);


HBOOL		HLIB_JSON_IsJSON (const HCHAR *text, HUINT32 n);

#endif /* __HLIB_JSON_H__ */

