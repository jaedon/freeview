/*
   Opera JavaScript plugin use case: Freesat JS extension
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jsplugin.h"
#include "libplugin.h"

/* Someone may want to compile and link under W*ndows */
# ifdef WIN32
#  define DECLSPEC __declspec(dllexport)
# else
#  define DECLSPEC
# endif

#define CHECK_ARGUMENT_COUNT(count)	\
	do { \
		if (argc != count) { \
			return JSP_CALL_ERROR;	\
		} \
	} while (0)

#define CHECK_ARGUMENT_MIN_MAX(min, max)	\
	do { \
		if (argc < min) { \
			return JSP_CALL_ERROR;	\
		} \
		else if (argc > max) { \
			return JSP_CALL_ERROR;	\
		} \
	} while (0)


#define CHECK_OBJECT(obj)	\
	do { \
		if (obj != NULL) { \
			return JSP_CALL_ERROR;	\
		} \
	} while (0)


#define MAX_URL_LEN			(2048)


typedef enum {
	MHEG_FUNCTION_TYPE_STORE,
	MHEG_FUNCTION_TYPE_READ,
} eMhegFuncType;


typedef struct _mheg_userdata_t {
	eMhegFuncType func_type;
	char *filename;
	char *mheg_array;
	unsigned int expiry_date;
	unsigned int priority;

	jsplugin_obj *global_context;
	jsplugin_obj *this_obj;
	jsplugin_obj *arg_obj;
} mheg_userdata_t;



static jsplugin_callbacks *g_opera_callbacks;
static jsplugin_obj *g_global_context;
static jsplugin_cap cap;

static const char* global_names[] = {
	"fsat_getSubtitleMode",
	"fsat_getAudioDescriptionMode",
	"fsat_setSubtitleMode",
	"fsat_setAudioDescriptionMode",
	"fsat_promptForPin",
	"Mheg_StorePersistent",
	"Mheg_ReadPersistent",
	"fsat_tmpHMX_getUITuneURL",
	"fsat_tmpHMX_setUITuneURL",
	"fsat_tmpHMX_promptForPin",
	"fsat_tmpHMX_validatePin",
	NULL };

static char s_latest_app_url[MAX_URL_LEN];

static int create_method( jsplugin_obj* this_obj, jsplugin_function *method, const char *sign, jsplugin_value *result )
{
	int r;

	jsplugin_obj *thefunction;
	r = g_opera_callbacks->create_function( this_obj, NULL, NULL, method, NULL, sign, NULL, NULL, &thefunction );
	if (r < 0)
	{
		return JSP_GET_ERROR;
	}
	else
	{
		result->type = JSP_TYPE_OBJECT;
		result->u.object = thefunction;
		return JSP_GET_VALUE_CACHE;
	}
}

static int fsat_get_subtitle( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	unsigned char enabled = 0;

	CHECK_ARGUMENT_COUNT(0);
	CHECK_OBJECT(this_obj);

	// TODO: enabled = opl_get_subtitle();

	result->type = JSP_TYPE_BOOLEAN;
	result->u.boolean = enabled;

	return JSP_CALL_VALUE;
}

static int fsat_get_audiodescription( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	unsigned char enabled = 0;

	CHECK_ARGUMENT_COUNT(0);
	CHECK_OBJECT(this_obj);

	// TODO: enabled = opl_get_audio_description();

	result->type = JSP_TYPE_BOOLEAN;
	result->u.boolean = enabled;

	return JSP_CALL_VALUE;
}

static int fsat_set_subtitle( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_OBJECT(this_obj);

	// TODO: opl_set_subtitle(argv[0].u.boolean);

	return JSP_CALL_NO_VALUE;
}

static int fsat_set_audiodescription( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_OBJECT(this_obj);

	// TODO: opl_set_audio_description(argv[0].u.boolean);

	return JSP_CALL_NO_VALUE;
}

static unsigned char __mheg_store_persistent(mheg_userdata_t *ud)
{
	unsigned char ret = 0;

	// mheg_array: OctetString,Some text to Store,Integer,123456,Boolean,true
	// TODO: ret = opl_mheg_store_persistent(ud->filename, ud->mheg_array,
	// 				ud->expiry_date, ud->priority);
	return ret;
}

static unsigned char __mheg_read_persistent(mheg_userdata_t *ud)
{
	unsigned char ret = 0;

	// mheg_array: OctetString,Integer,Boolean
	// TODO: ret = opl_mheg_store_persistent(ud->filename, ud->mheg_array);

	// you SHOULD make a return value as string type by GEN2 specification.
	/* ex: [['OctetString', 'Some text to store'],['Integer', '123456'],['Boolean', 'true']] */
	// And you SHOULD return this string in fsat_mheg_read_persistent.
	return ret;
}

static void __mheg_get_result(int status, jsplugin_value *return_value, void *user_data)
{
	unsigned char ret = 0;

	mheg_userdata_t *mheg_userdata = (mheg_userdata_t *)(user_data);

	if (status == JSP_CALLBACK_OK && return_value->type == JSP_TYPE_STRING)
	{
		mheg_userdata->mheg_array = (char *)return_value->u.string;
		ret = (mheg_userdata->func_type == MHEG_FUNCTION_TYPE_STORE) ?
			__mheg_store_persistent(mheg_userdata) : __mheg_read_persistent(mheg_userdata);
	}

	HLIB_STD_MemFree(mheg_userdata);
	g_opera_callbacks->resume(g_global_context);
}


static void __mheg_get_data(int status, jsplugin_value *return_value, void *user_data)
{
	mheg_userdata_t *mheg_userdata = (mheg_userdata_t *)(user_data);

	if (status == JSP_CALLBACK_OK && return_value->type == JSP_TYPE_NATIVE_OBJECT)
	{
		g_opera_callbacks->call_function(mheg_userdata->global_context, mheg_userdata->arg_obj,
							return_value->u.object, 0, 0, user_data, &__mheg_get_result);
	}
	else
	{
		HLIB_STD_MemFree(mheg_userdata);
	}
}

static int fsat_mheg_store_persistent( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	int res = 0;

	if ( argc < 0 )
	{
		result->type = JSP_TYPE_BOOLEAN;
		result->u.boolean = 0;
		return JSP_GET_VALUE;
	}

	CHECK_ARGUMENT_MIN_MAX(2, 4);
	CHECK_OBJECT(this_obj);

	mheg_userdata_t *mheg_userdata = (mheg_userdata_t *)HLIB_STD_MemAlloc(sizeof(mheg_userdata_t));
	memset(mheg_userdata, 0, sizeof(mheg_userdata_t));

	mheg_userdata->func_type = MHEG_FUNCTION_TYPE_STORE;

	mheg_userdata->filename = (char *)(argv[0].u.string);
	if ( argc > 3 ) mheg_userdata->expiry_date = argv[2].u.number;
	if ( argc > 4 ) mheg_userdata->priority = argv[3].u.number;

	mheg_userdata->global_context = g_global_context;
	mheg_userdata->this_obj = this_obj;
	mheg_userdata->arg_obj = argv[1].u.object;

	res = g_opera_callbacks->get_property(g_global_context, argv[1].u.object, "toString",
				mheg_userdata, &__mheg_get_data);
	if (res != JSP_GET_VALUE)
	{
		HLIB_STD_MemFree(mheg_userdata);
		result->type = JSP_TYPE_BOOLEAN;
		result->u.boolean = 0;
		return JSP_GET_VALUE;
	}

	result->type = JSP_TYPE_NULL;

	return JSP_CALL_DELAYED;
}

static int fsat_mheg_read_persistent( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	int res = 0;

	if ( argc < 0 )
	{
		result->type = JSP_TYPE_NULL;

		/* str = [['OctetString', 'Some text to store'],['Integer', '123456'],['Boolean', 'true']] */
		//result->type = JSP_TYPE_STRING;
		//result->u.string = HLIB_STD_StrDup(str);
		return JSP_GET_VALUE;
	}

	CHECK_ARGUMENT_MIN_MAX(2, 2);
	CHECK_OBJECT(this_obj);

	mheg_userdata_t *mheg_userdata = (mheg_userdata_t *)HLIB_STD_MemAlloc(sizeof(mheg_userdata_t));
	memset(mheg_userdata, 0, sizeof(mheg_userdata_t));

	mheg_userdata->func_type = MHEG_FUNCTION_TYPE_READ;

	mheg_userdata->filename = (char *)(argv[0].u.string);

	mheg_userdata->global_context = g_global_context;
	mheg_userdata->this_obj = this_obj;
	mheg_userdata->arg_obj = argv[1].u.object;
	res = g_opera_callbacks->get_property(g_global_context, argv[1].u.object, "toString",
				mheg_userdata, &__mheg_get_data);
	if (res != JSP_GET_VALUE)
	{
		HLIB_STD_MemFree(mheg_userdata);
		result->type = JSP_TYPE_NULL;
		return JSP_GET_VALUE;
	}

	result->type = JSP_TYPE_NULL;

	return JSP_CALL_DELAYED;
}

static int fsat_mheg_get_tune_url( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	CHECK_ARGUMENT_COUNT(0);
	CHECK_OBJECT(this_obj);

	result->type = JSP_TYPE_STRING;
	result->u.string = HLIB_STD_StrDup(s_latest_app_url);	// this will be freed by opera garbage collector.

	return JSP_CALL_VALUE;
}

static int fsat_mheg_set_tune_url( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_OBJECT(this_obj);

	memset(s_latest_app_url, 0, MAX_URL_LEN);

	if ( argv[0].u.string )
	{
		strncpy(s_latest_app_url, argv[0].u.string, MAX_URL_LEN-1);
	}
	else
	{
		strncpy(s_latest_app_url, "", 1);
	}

	result->type = JSP_TYPE_BOOLEAN;
	result->u.boolean = 1;

	return JSP_CALL_VALUE;
}

static int fsat_prompt_for_pin( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	unsigned char ret = 0;
	unsigned char adult = 0;
	unsigned char ip = 0;
	unsigned char pay = 0;

	CHECK_ARGUMENT_COUNT(3);
	CHECK_OBJECT(this_obj);

	adult = argv[0].u.boolean;
	ip = argv[1].u.boolean;
	pay = argv[2].u.boolean;


	// TODO: ret = opl_prompt_for_pin(adult, ip, pay);

	result->type = JSP_TYPE_BOOLEAN;
	result->u.boolean = ret;

	return JSP_CALL_VALUE;
}

static int fsat_validate_pin( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
#if 0	// TODO: remove for HDR-1000S Phase 2 - mhoh
	unsigned char ret = 0;
	char *pin = NULL;
	unsigned char adult = 0;
	unsigned char ip = 0;
	unsigned char pay = 0;

	CHECK_ARGUMENT_COUNT(4);
	CHECK_OBJECT(this_obj);

	pin = argv[0].u.string;
	adult = argv[1].u.boolean;
	ip = argv[2].u.boolean;
	pay = argv[3].u.boolean;


	// TODO: ret = opl_validate_pin(pin, adult, ip, pay);

	result->type = JSP_TYPE_BOOLEAN;
	result->u.boolean = ret;
#endif

	return JSP_CALL_VALUE;
}


/* Answers to requests in the global namespace */
static int fsat_global_getter( jsplugin_obj *ctx, const char *name, jsplugin_value *result )
{
	int r;

	if ( strcmp( name, "fsat_getSubtitleMode" ) == 0 )
	{
		return create_method(ctx, fsat_get_subtitle, "", result);
	}
	else if ( strcmp( name, "fsat_getAudioDescriptionMode" ) == 0 )
	{
		return create_method(ctx, fsat_get_audiodescription, "", result);
	}
	else if ( strcmp( name, "fsat_setSubtitleMode" ) == 0 )
	{
		return create_method(ctx, fsat_set_subtitle, "b", result);
	}
	else if ( strcmp( name, "fsat_setAudioDescriptionMode" ) == 0 )
	{
		return create_method(ctx, fsat_set_audiodescription, "b", result);
	}
	/*
	else if ( strcmp( name, "fsat_promptForPin" ) == 0 )
	{
		return create_method(ctx, fsat_prompt_for_pin, "bbbs", result);
	}
	*/
	else if ( strcmp( name, "Mheg_StorePersistent" ) == 0 )
	{
		return create_method(ctx, fsat_mheg_store_persistent, "s-nn", result);
	}
	else if ( strcmp( name, "Mheg_ReadPersistent" ) == 0 )
	{
		return create_method(ctx, fsat_mheg_read_persistent, "s-", result);
	}
	else if ( strcmp( name, "fsat_tmpHMX_getUITuneURL" ) == 0 )
	{
		return create_method(ctx, fsat_mheg_get_tune_url, "", result);
	}
	else if ( strcmp( name, "fsat_tmpHMX_setUITuneURL" ) == 0 )
	{
		return create_method(ctx, fsat_mheg_set_tune_url, "s", result);
	}
	else if ( strcmp( name, "fsat_tmpHMX_promptForPin" ) == 0 )
	{
		return create_method(ctx, fsat_prompt_for_pin, "bbb", result);
	}
	else if ( strcmp( name, "fsat_tmpHMX_validatePin" ) == 0 )
	{
		return create_method(ctx, fsat_validate_pin, "sbbb", result);
	}

	return JSP_GET_NOTFOUND;
}

static int fsat_allow_access(const char *protocol, const char *hostname, int port, const char *url)
{
    PLUGIN_Print("allow_access: proto=%s, name=%s, port=%d\n", protocol, hostname, port);
    return 1;
}

static void fsat_init(jsplugin_obj *global_context)
{
	g_global_context = global_context;
}

static void fsat_destroy(jsplugin_obj *global_context)
{
	//memset(s_latest_app_url, 0, MAX_URL_LEN);
}

extern "C" {

/* Exported capabilities function */
DECLSPEC int jsplugin_capabilities( jsplugin_cap **result, jsplugin_callbacks *cbs )
{
	memset(&cap, 0, sizeof(jsplugin_cap));
	PLUGIN_FPrint("====== freesat plugin loaded ======\n");

    cap.global_names = global_names;
    cap.object_types = NULL;
    cap.global_getter = fsat_global_getter;
    cap.global_setter = NULL;
    cap.init = fsat_init;
    cap.destroy = fsat_destroy;
    cap.handle_object = NULL;
    cap.gc_trace = NULL;
	cap.allow_access = fsat_allow_access;
	cap.default_permission = NULL;

    *result = &cap;

    g_opera_callbacks = cbs;

    return 0;
}

}

/* eof */
