/**
 * Plugin: Interfaces for Humax DRM
*/

/*
   Opera JavaScript plugin use case: Interfaces for Humax DRM

   This plugin can be instantiated by eg
   <OBJECT TYPE="hmxApplication/drm" ID="hmxdrmObject"></OBJECT>
   (only TYPE is mandatory, but ID may be pretty useful)

   bool setVmxCas(string address, string vendername);

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "oplhmxdrm.h"

#include "libplugin.h"

static jsplugin_cap cap;

static const char* global_names[] = { "HmxDrm", NULL }; /* no global names */
static const char* object_types[] = { "hmxApplication/drm", NULL }; /* only one MIME type */

/* Someone may want to compile and link under W*ndows */
# ifdef WIN32
#  define DECLSPEC __declspec(dllexport)
# else
#  define DECLSPEC
# endif

static struct jsplugin_callbacks* opera_callbacks;

static int set_vmx_cas_communication(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
				int argc, jsplugin_value *argv, jsplugin_value *result )
{
	int ret = JSP_CALL_ERROR;

	int err;

	if (argc != 2)
	{
		PLUGIN_Print("Invalid Parameter\n");
		ret = JSP_CALL_ERROR;
		goto exit;
	}

	if (this_obj == NULL)
	{
		/* this can happen under certain circumstances (wrong ecmascript code) */
		PLUGIN_Print("This obj is NULL\n");
		ret = JSP_CALL_ERROR;
		goto exit;
	}

	PLUGIN_Print("hmxdrm: param1=%s, param2=%s\n", argv[0].u.string, argv[1].u.string);

	err = OPL_HMXDRM_Set_VMX_Cas_Info(argv[0].u.string, argv[1].u.string);
	if (err != eOPL_HMXDRM_ERR_OK)
	{
		if (err == eOPL_HMXDRM_ERR_NOT_SUPPORTED)
		{
			PLUGIN_Print("This STB is not support VERIMATRIX\n");

		}
		else
		{
			PLUGIN_Print("Fail to set vmx_cas_info err=%d\n",err);
		}

	}
	
	result->type = JSP_TYPE_NUMBER;
	result->u.number = err;
	ret = JSP_CALL_VALUE;

exit:
	return ret;
}


static int set_playready_drm(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
				int argc, jsplugin_value *argv, jsplugin_value *result )
{

	int ret = JSP_CALL_ERROR;

	int err;

	if (this_obj == NULL)
	{
		/* this can happen under certain circumstances (wrong ecmascript code) */
		PLUGIN_Print("This obj is NULL\n");
		ret = JSP_CALL_ERROR;
		goto exit;
	}

	PLUGIN_Print("hmxdrm: param1=%s,param2=%s, param3=%d\n", argv[0].u.string, argv[1].u.string, argv[2].u.number);
	err = OPL_HMXDRM_Set_Playready_Drm(argv[0].u.string, argv[1].u.string, argv[2].u.number);
//	err = OPL_HMXDRM_Set_VMX_Cas_Info(argv[0].u.string, argv[1].u.string);
/*
	if (err != eOPL_HMXDRM_ERR_OK)
	{
		if (err == eOPL_HMXDRM_ERR_NOT_SUPPORTED)
		{
			PLUGIN_Print("This STB is not support VERIMATRIX\n");

		}
		else
		{
			PLUGIN_Print("Fail to set vmx_cas_info err=%d\n",err);
		}

	}
*/
	result->type = JSP_TYPE_NUMBER;
//	result->u.number = err;
	ret = JSP_CALL_VALUE;

exit:
	return ret;
}


static int drm_system_status(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
				int argc, jsplugin_value *argv, jsplugin_value *result )
{
	int ret = JSP_CALL_ERROR;
	int nStatus = -1;
	int err;

	if (argc > 1)
	{
		PLUGIN_Print("unused Parameter is existed argc -  [%d]\n",argc);
	}

	if (this_obj == NULL)
	{
		/* this can happen under certain circumstances (wrong ecmascript code) */
		PLUGIN_Print("This obj is NULL\n");
		ret = JSP_CALL_ERROR;
		goto exit;
	}

	err = OPL_HMXDRM_Drm_System_Status(&nStatus);
	if (err != eOPL_HMXDRM_ERR_OK)
	{
		PLUGIN_Print("Fail to get drm system status\n");
		result->u.number = eOPL_HMXDRM_SYSTEM_STATUS_ERROR;
	}
	else
	{
		result->u.number = nStatus;
	}

	result->type = JSP_TYPE_NUMBER;
	ret = JSP_CALL_VALUE;

exit:
	return ret;
}

static int drm_rights_error(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
				int argc, jsplugin_value *argv, jsplugin_value *result )
{
	int ret = JSP_CALL_ERROR;
	char *pErrState = NULL;
	int err;

	if (argc >= 1)
	{
		PLUGIN_Print("unused Parameter is existed argc -  [%d]\n",argc);
	}

	if (this_obj == NULL)
	{
		/* this can happen under certain circumstances (wrong ecmascript code) */
		PLUGIN_Print("This obj is NULL\n");
		ret = JSP_CALL_ERROR;
		goto exit;
	}

	err = OPL_HMXDRM_Drm_Rights_Error((char**)&pErrState);
	if (err != eOPL_HMXDRM_ERR_OK)
	{
		PLUGIN_Print("Fail to get drm system status\n");
		//result->u.string = eOPL_HMXDRM_SYSTEM_STATUS_ERROR;
		ret = JSP_CALL_ERROR;
	}
	else
	{
		PLUGIN_Print("hmxdrm:  url error message %s\n", pErrState);
		result->u.string = pErrState;
		if ( result->u.string == NULL )
		{
			ret = JSP_CALL_NO_VALUE;
			goto exit;
		}
		result->type =JSP_TYPE_STRING ;
		ret = JSP_CALL_VALUE;
	}

exit:
	return ret;
}


int hmxdrm_getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	int err = eOPL_HMXDRM_ERR_OK;

	int ret = JSP_GET_NOTFOUND;

	char *pszIdentifier = NULL;

	PLUGIN_Print("%s\n", name);

	if (strcmp(name, "setPlayreadyDrm") == 0)
	{
		jsplugin_obj *hmxdrmObj  = NULL;
		int r = opera_callbacks->create_function(obj, NULL, NULL, set_playready_drm,
						NULL, "", NULL, NULL, &hmxdrmObj);
		if ( r != JSP_CREATE_OK )
		{
			ret = JSP_GET_ERROR;
			goto exit;
		}

		result->type = JSP_TYPE_OBJECT;
		result->u.object = hmxdrmObj;
		ret = JSP_GET_VALUE_CACHE;
		goto exit;
	}
	
	else if (strcmp(name, "setVmxCas") == 0)
	{
		jsplugin_obj *hmxdrmObj  = NULL;
		int r = opera_callbacks->create_function(obj, NULL, NULL, set_vmx_cas_communication,
						NULL, "", NULL, NULL, &hmxdrmObj);

		if ( r != JSP_CREATE_OK )
		{
			ret = JSP_GET_ERROR;
			goto exit;
		}

		result->type = JSP_TYPE_OBJECT;
		result->u.object = hmxdrmObj;
		ret = JSP_GET_VALUE_CACHE;
		goto exit;

	}
	else if (strcmp(name, "getVmxIdentifier") == 0)
	{
		err = OPL_HMXDRM_Get_VMX_Identifier((char**)&pszIdentifier);
		if (err != eOPL_HMXDRM_ERR_OK)
		{
			if (err == eOPL_HMXDRM_ERR_NOT_SUPPORTED)
			{
				PLUGIN_Print("This STB is not support VERIMATRIX\n");
			}
			else
			{
				PLUGIN_Print("Fail to get vmx_identifier err=%d\n", err);
			}
			ret = JSP_GET_ERROR;
			goto exit;
		}

		PLUGIN_Print("pszIdentifier=%s\n", pszIdentifier);

		result->u.string = pszIdentifier;
		if ( result->u.string == NULL )
		{
			ret = JSP_CALL_NO_VALUE;
			goto exit;
		}
		result->type = JSP_TYPE_STRING;
		ret = JSP_GET_VALUE_CACHE;
		goto exit;
	}
	else if (strcmp(name, "DRMSystemStatus") == 0)
	{
		jsplugin_obj *hmxdrmObj  = NULL;
		int r = opera_callbacks->create_function(obj, NULL, NULL, drm_system_status,
						NULL, "", NULL, NULL, &hmxdrmObj);
		if ( r != JSP_CREATE_OK ) return JSP_GET_ERROR;

		result->type = JSP_TYPE_OBJECT;
		result->u.object = hmxdrmObj;
		ret = JSP_GET_VALUE_CACHE;
		goto exit;
	}
	else if (strcmp(name, "DRMRightsError") == 0)
	{
		jsplugin_obj *hmxdrmObj  = NULL;
		int r = opera_callbacks->create_function(obj, NULL, NULL, drm_rights_error,
						NULL, "", NULL, NULL, &hmxdrmObj);

		if ( r != JSP_CREATE_OK ) return JSP_GET_ERROR;

		result->type = JSP_TYPE_OBJECT;
		result->u.object = hmxdrmObj;
		ret = JSP_GET_VALUE_CACHE;
		goto exit;
	}
	else
	{
		/* nothing to do */
	}

	ret = JSP_GET_NOTFOUND;

exit:
	return ret;
}

int hmxdrm_destructor(jsplugin_obj *obj)
{
	int ret = JSP_DESTROY_OK;

	PLUGIN_Print(" + \n");
	OPL_HMXDRM_Unset_VMX_Cas_Info();
	PLUGIN_Print(" - \n");
	return ret;
}

int hmxdrm_constructor( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	int 			r;
	int				ret = JSP_CALL_ERROR;
	jsplugin_obj 	*hmxdrmObj = NULL;

	PLUGIN_Print("This:%08X function:%08X\n", this_obj, function_obj);

    r = opera_callbacks->create_object( function_obj,
    									hmxdrm_getter,
    									NULL,
  	 									hmxdrm_destructor,
    									NULL,
    									&hmxdrmObj);
    if (r < 0)
    {
		PLUGIN_Print("Fail to create object! JSP_CALL_NOMEM !! \n");
		ret = JSP_CALL_NOMEM;
		goto exit;
    }

	result->type = JSP_TYPE_OBJECT;
	result->u.object = hmxdrmObj;

	ret = JSP_CALL_VALUE;

exit:
    return ret;

}

int hmxdrm_global_getter( jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	int				r = 0;
   	jsplugin_obj 	*hmxdrmObj;

	PLUGIN_Print("This:%08X :%s\n", obj, name);

	if (strcmp( name,  global_names[0]) == 0)
    {
		r = opera_callbacks->create_function(obj, /* reference object, used for environment etc */
						     NULL, NULL, /* getter/setter, not needed for constructor */
						     NULL, /* not a regular function call... */
						     hmxdrm_constructor, /* ... but a constructor */
						     "n", /* signature is empty */
						     NULL, /* no destructor necessary */
						     NULL, /* no garbage collection necessary (really?) */
						     &hmxdrmObj); /* here we get the function object */
		if ( r != JSP_CREATE_OK )
		{
		    return JSP_GET_ERROR; /* FIXME, too generic */
		}
		else
		{
		    result->type = JSP_TYPE_OBJECT;
		    result->u.object = hmxdrmObj;
		    return JSP_GET_VALUE_CACHE;
		}
    }

	return JSP_GET_NOTFOUND;

}

static int hmxdrm_handle_object(jsplugin_obj *global_obj, jsplugin_obj *object_obj, int attrs_count,
			jsplugin_attr *attrs, jsplugin_getter **getter, jsplugin_setter **setter,
			jsplugin_destructor **destructor, jsplugin_gc_trace **trace, jsplugin_notify **inserted, jsplugin_notify **removed)
{
	int nRet = 0;

	*getter = hmxdrm_getter;
	*setter = NULL;
	*inserted = NULL;
	*destructor = hmxdrm_destructor;
	*removed = NULL;

	nRet = JSP_OBJECT_OK;

exit:
	return nRet;
}

void hmxdrm_init(jsplugin_obj *global_context)
{
	PLUGIN_Print("This:%08X\n", global_context);
}

void hmxdrm_destroy(jsplugin_obj *global_context)
{
	PLUGIN_Print("This:%08X\n", global_context);
}


static int hmxdrm_allow_access(const char *protocol, const char *hostname, int port, const char *url)
{
	PLUGIN_Print("proto=%s, name=%s, port=%d\n", protocol, hostname, port);
	return 1;
}

extern "C" {
DECLSPEC int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
{
	PLUGIN_FPrint("====== HmxDrm plugin loaded ======\n");
	cap.global_names = global_names;
	cap.object_types = object_types;

	cap.global_getter = hmxdrm_global_getter;
	cap.global_setter = NULL;

	cap.init = hmxdrm_init;
	cap.destroy = hmxdrm_destroy;
	cap.gc_trace = NULL;

	cap.handle_object = hmxdrm_handle_object;
	cap.allow_access = hmxdrm_allow_access;

	*result = &cap;

	opera_callbacks = c;

	return 0;
}
} /* extern "C" */
