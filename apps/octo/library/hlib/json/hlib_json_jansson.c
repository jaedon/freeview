/*
 * hxjson.c
 *
 *  Created on: 2011. 5. 16.
 */

#include <hlib.h>
#include <_hlib_json.h>

#include <jansson.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxJSON"


static void *hlib_json_malloc(HUINT32 size)
{
	return HLIB_MEM_Malloc(size);
}
static void hlib_json_free(void *p)
{
	//HLIB_MEM_Free(p);
	HLIB_MEM_Free(p);
}

static json_malloc_t	s_malloc = hlib_json_malloc;
static json_free_t		s_free	 = hlib_json_free;

//static HxJSON_Config_e	s_config = eHLIB_JSON_Conf_Default;
static HxJSON_Config_e	s_config = eHLIB_JSON_Conf_Minimum;	// TODO: 일단 미니멈...



#define	____HLIB_JSON_Private_________________________________________________


static const HCHAR *	hlib_json_NextToken (HCHAR *token, HINT32 n, const HCHAR *p)
{
	if (*p == '\0')
		return NULL;

	if (*p == '/')
		p++;

	while (n-- && *p != '\0' && *p != '/')
	{
		*token++ = *p++;
	}

	if (n < 0)
		return NULL;

	*token = '\0';

	return p;
}



#define	____HLIB_JSON_APIs____________________________________________________

void		HLIB_JSON_Init (HxJSON_Config_e conf, void *(*_malloc)(unsigned long), int (*_free)(void *))
{
	HxLOG_Trace();

	s_config = conf;

	if (_malloc && _free)
	{
		s_malloc = (json_malloc_t)_malloc;
		s_free   = (json_free_t)_free;
	}
	json_set_alloc_funcs((json_malloc_t)s_malloc, (json_free_t)s_free);
}


HxJSON_t	HLIB_JSON_New (HxJSON_Type_e type, ...)
{
	json_t	*self;
	va_list	ap;
	void	*value;

	HxLOG_Trace();

	va_start(ap, type);
	switch(type)
	{
	case eHLIB_JSON_Object:
		self  = json_object();
		break;

	case eHLIB_JSON_Array:
		self = json_array();
		break;

	case eHLIB_JSON_String:
		value = (void *)(va_arg(ap, const char *));
		HxLOG_Assert(value);
		if (s_config & eHLIB_JSON_Conf_CheckUTF8)
			self = json_string((const char *)value);
		else
			self = json_string_nocheck((const char *)value);
		break;

	case eHLIB_JSON_Integer:
		value = (void *)(va_arg(ap, HINT32));
		self = json_integer((json_int_t)value);
		break;

	case eHLIB_JSON_Real:
		//self = json_real(va_arg(ap, double));
		HxLOG_Warning("Not supported yet... Real...\n");
		self = NULL;
		break;

	case eHLIB_JSON_TRUE:
		self = json_true();
		break;

	case eHLIB_JSON_FALSE:
		self = json_false();
		break;

	case eHLIB_JSON_NULL:
		self = json_null();
		break;

	default:
		HxLOG_Error("Unknown Type(%d)\n", type);
		self = NULL;
		break;
	}
	va_end(ap);
	return (HxJSON_t)self;
}

HxJSON_t	HLIB_JSON_Real_New_fn ( double value )
{
	json_t	*self;

	HxLOG_Trace();

	self = json_real(value);

	return (HxJSON_t)self;
}

void		HLIB_JSON_Delete (HxJSON_t json)
{
	json_t *self = (json_t *)json;

	HxLOG_Trace();
	HxLOG_Assert(self);

	json_decref(self);
}

HxJSON_t	HLIB_JSON_Clone (HxJSON_t json)
{
	json_t *self = (json_t *)json;

	HxLOG_Trace();
	HxLOG_Assert(self);

	// incref로 할 수 있을 것 같긴 한데...
	// 일단 deep_copy로 두고 나중에 생각하자..
	return (HxJSON_t)json_deep_copy(self);
	//return (HxJSON_t)json_incref(self);
}

void *		HLIB_JSON_Get (HxJSON_t json, void *key)
{
	json_t *self = (json_t *)json;

	HxLOG_Assert(self);

	switch(HLIB_JSON_TypeOf(json))
	{
	case eHLIB_JSON_Object:
		HxLOG_Assert(key);
		return (void *)json_object_get(self, (const char *)key);

	case eHLIB_JSON_Array:
		return (void *)json_array_get(self, (size_t)key);

	case eHLIB_JSON_String:
		return (void *)json_string_value(self);

	case eHLIB_JSON_Integer:
		return (void *)json_integer_value(self);

	case eHLIB_JSON_Real:
		//return (void *)json_real_value(self);
		HxLOG_Warning("Not support yet... Real... ~(__~)\n");
		return NULL;

	case eHLIB_JSON_TRUE:
		return (void *)TRUE;

	case eHLIB_JSON_FALSE:
		return (void *)FALSE;

	case eHLIB_JSON_NULL:
		return (void *)NULL;

	default:
		HxLOG_Error("Unknown Type(%d)\n", HLIB_JSON_TypeOf(json));
		break;
	}
	return (void *)NULL;
}

double		HLIB_JSON_Real_Get_fn (HxJSON_t json, void *key)
{
	json_t *self = (json_t *)json;

	(void)key;

	HxLOG_Trace();
	HxLOG_Assert(self);

	return json_real_value(self);
}

HERROR		HLIB_JSON_Real_Set_fn (HxJSON_t json, void *key, HFLOAT64 value)
{
	json_t	*self = (json_t *)json;
	int		ret;

	HxLOG_Trace();
	HxLOG_Assert(self);

	ret = json_real_set(self, (double)value);

	return (ret < 0) ? ERR_FAIL : ERR_OK;
}

HERROR		HLIB_JSON_Set (HxJSON_t json, void *key, void *value)
{
	json_t	*self = (json_t *)json;
	int		ret;

	HxLOG_Trace();
	HxLOG_Assert(self);

	switch(HLIB_JSON_TypeOf(json))
	{
	case eHLIB_JSON_Object:
		HxLOG_Assert(key && value);
		if (s_config & eHLIB_JSON_Conf_CheckUTF8)
			ret = json_object_set_new(self, (const char *)key, (json_t *)value);
		else
			ret = json_object_set_new_nocheck(self, (const char *)key, (json_t *)value);
		break;

	case eHLIB_JSON_Array:
		HxLOG_Assert(value);
		ret = json_array_set_new(self, (size_t)key, (json_t *)value);
		break;

	case eHLIB_JSON_String:
		HxLOG_Assert(value);
		if (s_config & eHLIB_JSON_Conf_CheckUTF8)
			ret = json_string_set(self, (const char *)value);
		else
			ret = json_string_set_nocheck(self, (const char *)value);
		break;

	case eHLIB_JSON_Integer:
		ret = json_integer_set(self, (json_int_t)value);
		break;

	case eHLIB_JSON_Real:
		HxLOG_Warning("Not supported yet... Real...\n");
		//ret = json_real_set(self, (double)value);
		ret = -1;
		break;

	case eHLIB_JSON_TRUE:
	case eHLIB_JSON_FALSE:
	case eHLIB_JSON_NULL:
	default:
		ret = -1;
		break;
	}
	return (ret < 0) ? ERR_FAIL : ERR_OK;
}

HERROR		HLIB_JSON_Remove (HxJSON_t json, void *key)
{
	json_t	*self = (json_t *)json;
	int		ret;

	HxLOG_Trace();
	HxLOG_Assert(self);

	switch(HLIB_JSON_TypeOf(json))
	{
	case eHLIB_JSON_Object:
		HxLOG_Assert(key);
		ret = json_object_del(self, (const char *)key);
		break;

	case eHLIB_JSON_Array:
		ret = json_array_remove(self, (size_t)key);
		break;

	case eHLIB_JSON_String:
	case eHLIB_JSON_Integer:
	case eHLIB_JSON_Real:
	case eHLIB_JSON_TRUE:
	case eHLIB_JSON_FALSE:
	case eHLIB_JSON_NULL:
	default:
		ret = -1;
		break;
	}
	return (ret < 0) ? ERR_FAIL : ERR_OK;
}

HxJSON_t	HLIB_JSON_Detach (HxJSON_t json, void *key)
{
	json_t	*self  = (json_t *)json;
	json_t	*value = NULL;

	HxLOG_Trace();
	HxLOG_Assert(self);

	switch(HLIB_JSON_TypeOf(json))
	{
	case eHLIB_JSON_Object:
		HxLOG_Assert(key);
		value = json_object_get(self, (const char *)key);
		break;

	case eHLIB_JSON_Array:
		value = json_array_get(self, (size_t)key);
		break;

	default:
		HxLOG_Error("Unknown Type(%d)\n", HLIB_JSON_TypeOf(json));
		break;
	}
	if (value == NULL)
		return (HxJSON_t)NULL;

	// ++ reference count
	// value = json_incref(value);
	value = json_deep_copy(value);

	(void)HLIB_JSON_Remove(json, key);

	return (HxJSON_t)value;


}

HERROR		HLIB_JSON_Clear (HxJSON_t json)
{
	json_t	*self = (json_t *)json;
	int		ret;

	HxLOG_Trace();
	HxLOG_Assert(self);

	switch(HLIB_JSON_TypeOf(json))
	{
	case eHLIB_JSON_Object:
		ret = json_object_clear(self);
		break;

	case eHLIB_JSON_Array:
		ret = json_array_clear(self);
		break;

	case eHLIB_JSON_String:
		if (s_config & eHLIB_JSON_Conf_CheckUTF8)
			ret = json_string_set(self, "");
		else
			ret = json_string_set_nocheck(self, "");
		break;

	case eHLIB_JSON_Integer:
	case eHLIB_JSON_Real:
	case eHLIB_JSON_TRUE:
	case eHLIB_JSON_FALSE:
	case eHLIB_JSON_NULL:
	default:
		ret = -1;
		break;
	}
	return (ret < 0) ? ERR_FAIL : ERR_OK;
}

HINT32		HLIB_JSON_GetSize (HxJSON_t json)
{
	json_t	*self = (json_t *)json;
	HINT32	size;

	HxLOG_Assert(self);

	switch(HLIB_JSON_TypeOf(json))
	{
	case eHLIB_JSON_Object:
		size = (HINT32)json_object_size(self);
		break;

	case eHLIB_JSON_Array:
		size = (HINT32)json_array_size(self);
		break;

	case eHLIB_JSON_String:
	case eHLIB_JSON_Integer:
	case eHLIB_JSON_Real:
	case eHLIB_JSON_TRUE:
	case eHLIB_JSON_FALSE:
	case eHLIB_JSON_NULL:
	default:
		size = 0;
		break;
	}
	return size;
}

HERROR		HLIB_JSON_ForEach (HxJSON_t json, HBOOL (* doIt)(void *key, void *value, void *user), void *user)
{
	json_t	*self = (json_t *)json;

	HxLOG_Trace();
	HxLOG_Assert(self && doIt && user);

	if (HxJSON_IsArray(json))
	{
		HINT32	i;
		HINT32	size = HLIB_JSON_GetSize(json);
		void	*value;

		for (i = 0 ; i < size ; i++)
		{
			value = HLIB_JSON_Get(json, (void *)i);
			if (doIt((void *)i, value, user))
				break;
		}
		return ERR_OK;
	}

	if (HxJSON_IsObject(json))
	{
		void *iter;

		iter = json_object_iter(self);
		while (iter)
		{
			if (doIt((void *)json_object_iter_key(iter), (void *)json_object_iter_value(iter), user))
				break;
			iter = json_object_iter_next(self, iter);
		}
		return ERR_OK;
	}
	return ERR_FAIL;
}


HCHAR *		HLIB_JSON_Encode (const HxJSON_t json, HCHAR *buf, HUINT32 len)
{
	const json_t *self = (const json_t *)json;
	HCHAR	*ret;
	size_t	flags;

	HxLOG_Trace();
	HxLOG_Assert(self);

	if (s_config & eHLIB_JSON_Conf_EncodeIndent)
		flags = JSON_INDENT(4);
	else
		flags = JSON_INDENT(0);

	if (s_config & eHLIB_JSON_Conf_EncodeASCII)
		flags |= JSON_ENSURE_ASCII;

	if (s_config & eHLIB_JSON_Conf_EncodeSortKey)
		flags |= JSON_SORT_KEYS;
	else
		flags |= JSON_PRESERVE_ORDER;

	if (s_config & eHLIB_JSON_Conf_EncodeCompact)
		flags |= JSON_COMPACT;

	ret = (HCHAR *)json_dumps(self, flags);
	if (ret && buf && len)
	{
		HLIB_STD_StrNCpySafe(buf, ret, len);
		s_free(ret);
		return buf;
	}
	return ret;
}

void		HLIB_JSON_FreeMemory (void *data)
{
	s_free(data);
}


HxJSON_t	HLIB_JSON_Decode (const HCHAR *jsontext)
{
	json_error_t	error;
	json_t *self;

	HxLOG_Trace();
	if (HxSTD_StrEmpty(jsontext) == TRUE)
		return NULL;

	self = json_loads((const char *)jsontext, 0, &error);
	if (self == NULL)
	{
		HxLOG_Critical("\n\n[%d:%d:%d]\n\t%s\n\t%s\n\n\n", error.line, error.column, error.position, error.source, error.text);
	}
	return (HxJSON_t)self;
}


HxJSON_t	HLIB_JSON_DecodeFile (const HCHAR *path)
{
	json_error_t	error;
	json_t *self;

	HxLOG_Trace();
	HxLOG_Assert(path);

	self = json_load_file(path, 0, &error);
	if (self == NULL)
	{
		HxLOG_Critical("\n\n[%d:%d:%d]\n\t%s\n\t%s\n\n\n", error.line, error.column, error.position, error.source, error.text);
	}
	return (HxJSON_t)self;
}


HxJSON_t	HLIB_JSON_Build (const HCHAR *fmt, ...)
{
	json_t	*self;
	va_list	ap;

	HxLOG_Trace();
	HxLOG_Assert(fmt);

	va_start(ap, fmt);
	self = json_vpack_ex(NULL, 0, fmt, ap);
	va_end(ap);

	return (HxJSON_t)self;
}

HERROR		HLIB_JSON_Scan (HxJSON_t json, const HCHAR *fmt, ...)
{
	json_t	*self = (json_t *)json;
	va_list ap;
	HINT32	ret;

	HxLOG_Trace();
	HxLOG_Assert(self && fmt);

	va_start(ap, fmt);
	ret = (HINT32)json_vunpack_ex(self, NULL, 0, fmt, ap);
	va_end(ap);

	return (ret < 0) ? ERR_FAIL : ERR_OK;
}

HxJSON_Type_e	HLIB_JSON_TypeOf (HxJSON_t json)
{
	json_t	*self = (json_t *)json;

	HxLOG_Assert(self);

#if 1
	return (HxJSON_Type_e)json_typeof(self);
#else
	switch(json_typeof(self))
	{
	case JSON_OBJECT:	return eHLIB_JSON_Object;
    case JSON_ARRAY:	return eHLIB_JSON_Array;
    case JSON_STRING:	return eHLIB_JSON_String;
    case JSON_INTEGER:	return eHLIB_JSON_Integer;
    case JSON_REAL:		return eHLIB_JSON_Real;
    case JSON_TRUE:		return eHLIB_JSON_TRUE;
    case JSON_FALSE:	return eHLIB_JSON_FALSE;
    case JSON_NULL:		return eHLIB_JSON_NULL;
	default:
		break;
	}
	return eHLIB_JSON_NULL;
#endif
}

// some type specific APIs
HERROR		HLIB_JSON_Object_Update (HxJSON_t json, HxJSON_t otherobj)
{
	HxLOG_Trace();
	HxLOG_Assert(json && otherobj);

	if (!(HxJSON_IsObject(json) && HxJSON_IsObject(otherobj)))
		return ERR_FAIL;

	if (json_object_update((json_t *)json, (json_t *)otherobj) < 0)
		return ERR_FAIL;
	return ERR_OK;
}

HxJSON_t	HLIB_JSON_Object_GetObject (HxJSON_t json, const HCHAR *key)
{
	HxJSON_t value = HxJSON_Object_Get(json, key);

	if (value && HxJSON_IsObject(value))
	{
		return value;
	}
	return (HxJSON_t)NULL;
}

HxJSON_t	HLIB_JSON_Object_GetArray (HxJSON_t json, const HCHAR *key)
{
	HxJSON_t value = HxJSON_Object_Get(json, key);

	if (value && HxJSON_IsArray(value))
	{
		return value;
	}
	return (HxJSON_t)NULL;
}

HINT32	HLIB_JSON_Object_GetInteger (HxJSON_t json, const HCHAR *key)
{
	HxJSON_t value = HxJSON_Object_Get(json, key);

	if (value && HxJSON_IsInteger(value))
	{
		return HxJSON_Integer_Get(value);
	}
	return 0;
}

HINT32	HLIB_JSON_Object_GetIntegerWithDefaultValue (HxJSON_t json, const HCHAR *key, HINT32 defvalue)
{
	HxJSON_t value = HxJSON_Object_Get(json, key);

	if (value && HxJSON_IsInteger(value))
	{
		return HxJSON_Integer_Get(value);
	}
	return defvalue;
}

const HCHAR *HLIB_JSON_Object_GetString  (HxJSON_t json, const HCHAR *key)
{
	HxJSON_t value = HxJSON_Object_Get(json, key);

	if (value && HxJSON_IsString(value))
	{
		return HxJSON_String_Get(value);
	}
	return NULL;
}

HBOOL	HLIB_JSON_Object_GetBoolean (HxJSON_t json, const HCHAR *key)
{
	HxJSON_t value = HxJSON_Object_Get(json, key);

	if (value && HxJSON_IsBoolean(value))
	{
		return HxJSON_Boolean_Get(value);
	}
//	return (HBOOL)-1;
	return FALSE;
}

HUINT32		HLIB_JSON_Object_GetBinary (HxJSON_t json, const HCHAR *key, void *value, HUINT32 n)
{
	const HCHAR *base64;
	HUINT32		ret;
	HUINT32		len;

	HxLOG_Assert(json && key && value && n);

	base64 = HLIB_JSON_Object_GetString(json, key);
	if (base64 == NULL)
		return 0;

	len = HxSTD_StrLen(base64);
	ret = HLIB_MATH_DecodeBase64(NULL, base64, len);
	if (ret == 0 || n < ret)
		return 0;

	return HLIB_MATH_DecodeBase64(value, base64, len);
}

void *		HLIB_JSON_Object_GetBinaryDup (HxJSON_t json, const HCHAR *key, HUINT32 *size)
{
	const HCHAR *base64;

	HxLOG_Assert(json && key);

	base64 = HLIB_JSON_Object_GetString(json, key);
	if (base64 == NULL)
		return NULL;

	return HLIB_MATH_DecodeBase64Easy(base64, size);
}

void *		HLIB_JSON_Object_Iterator (HxJSON_t json)
{
	if (HxJSON_IsObject(json))
	{
		return json_object_iter((json_t *)json);
	}
	return NULL;
}
#if	defined(CONFIG_SUPPORT_REMA)
void *		HLIB_JSON_Object_IteratorAt (HxJSON_t json, const HCHAR *key)
{
	if (HxJSON_IsObject(json))
	{
		return json_object_iter_at((json_t *)json, key);
	}
	return NULL;
}
#endif
const HCHAR *HLIB_JSON_Object_IteratorKey (void *iter)
{
	return (const HCHAR *)json_object_iter_key(iter);
}

HxJSON_t	HLIB_JSON_Object_IteratorValue (void *iter)
{
	return (HxJSON_t)json_object_iter_value(iter);
}

void *		HLIB_JSON_Object_NextIterator (HxJSON_t json, void *iter)
{
	return json_object_iter_next((json_t *)json, iter);
}


HERROR		HLIB_JSON_Array_Append (HxJSON_t json, HxJSON_t value)
{
	json_t *self = (json_t *)json;
	json_t *val  = (json_t *)value;

	HxLOG_Trace();
	HxLOG_Assert(self && val);

	if (!HxJSON_IsArray(json))
		return ERR_FAIL;

	if (json_array_append_new(self, val) < 0)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR		HLIB_JSON_Array_Insert (HxJSON_t json, HINT32 index, HxJSON_t value)
{
	json_t *self = (json_t *)json;
	json_t *val  = (json_t *)value;

	HxLOG_Trace();
	HxLOG_Assert(self && val);

	if (!HxJSON_IsArray(json))
		return ERR_FAIL;

	if (json_array_insert_new(self, (size_t)index, val) < 0)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR		HLIB_JSON_Array_Extend (HxJSON_t json, HxJSON_t otherarray)
{
	json_t *self = (json_t *)json;
	json_t *other  = (json_t *)otherarray;

	HxLOG_Trace();
	HxLOG_Assert(self && other);

	if (!HxJSON_IsArray(json) || !HxJSON_IsArray(otherarray))
		return ERR_FAIL;

	if (json_array_extend(self, other) < 0)
		return ERR_FAIL;

	return ERR_OK;
}


///< @see http://tools.ietf.org/pdf/draft-pbryan-zyp-json-pointer-00.pdf
HxJSON_t	HLIB_JSON_Pointer_Get(HxJSON_t json, const HCHAR *pointer)
{
	HCHAR	token[128];

	HxLOG_Trace();
	HxLOG_Assert(json && pointer);

	if (HxSTD_StrCmp(pointer, "/") == 0)
	{
		return json;	// return this!
	}

	if (!HLIB_STD_StrStartWith(pointer, "/"))
	{
		HxLOG_Error("[%s:%d] Syntax Error[pointer have to start with '/']: %s\n", __HxFILE__, __HxLINE__, pointer);
		return (HxJSON_t)NULL;
	}

	if (HLIB_STD_StrEndWith(pointer, "/"))
	{
		HxLOG_Error("[%s:%d] Syntax Error[pointer have to end with 'token']: %s\n", __HxFILE__, __HxLINE__, pointer);
		return (HxJSON_t)NULL;
	}

	while ((pointer = hlib_json_NextToken(token, 128, pointer)) != NULL)
	{
		if (HxJSON_IsObject(json))
		{
			json  = HxJSON_Object_Get(json, token);
		}
		else if (HxJSON_IsArray(json))
		{
			if (!HLIB_STD_IsInteger(token))
			{
				HxLOG_Error("[%s:%d] Syntax Error[array index MUST BE integer:\"%s\"]: %s\n", __HxFILE__, __HxLINE__, token, pointer);
				return (HxJSON_t)NULL;
			}
			json  = HxJSON_Array_Get(json, HLIB_STD_StrToINT32(token));
		}
		else
		{
			HxLOG_Error("[%s:%d] Syntax Error[Only 'Object' & 'Array' are available!]: %s\n", __HxFILE__, __HxLINE__, pointer);
			return (HxJSON_t)NULL;
		}

		if (json == (HxJSON_t)NULL)
		{
			break;
		}
	}
	return (HxJSON_t)json;
}

HxJSON_t	HLIB_JSON_Pointer_SafeGet (HxJSON_t json, HxJSON_Type_e type, const HCHAR *pointer)
{
	HxJSON_t	value = HLIB_JSON_Pointer_Get(json, pointer);

	if (value && HLIB_JSON_TypeOf(value) == type)
	{
		return value;
	}

	return (HxJSON_t)NULL;
}

HBOOL		HLIB_JSON_IsJSON (const HCHAR *text, HUINT32 n)
{
	HUINT32	i;

	if (n == 0)
		n = strlen(text);

	for (i = 0 ; i < n ; i++)
	{
		if (isspace(text[i]))
			continue;
		if (text[i] != '{')
			return FALSE;
		break;
	}

	while (n--)
	{
		if (isspace(text[n]) || text[n] == '\0')
			continue;
		if (text[n] != '}')
			return FALSE;
		break;
	}
	return TRUE;
}

