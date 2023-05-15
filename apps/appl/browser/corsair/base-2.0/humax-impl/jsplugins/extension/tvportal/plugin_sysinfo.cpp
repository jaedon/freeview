/**
 * Plugin: Humax Sysinfo
*/

/*
   Opera JavaScript plugin use case: Humax Sysinfo OBJECT

   This plugin can be instantiated by eg
   <OBJECT TYPE="valups/system" ID="systemObject"></OBJECT>
   (only TYPE is mandatory, but ID may be pretty useful)

   modelName
   modelVariant
   serialNumber
   macAddress
   softwareVersion
   screenLanguage
   systemId

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hlib.h>
#include "jsplugin.h"

#include "opllocalsystem.h"
#include "libplugin.h"

typedef struct _sysinfo_t {
	char *model_name;
	char *model_variant;
	char *serial_number;
	char *macaddr;
	char *software_version;
	char *screen_language;
	char *system_id;
	eOplNetType used_netiface;
	char *chip_id;
} sysinfo_t;

static jsplugin_cap cap;

static const char* global_names[] = { NULL }; /* no global names */
static const char* object_types[] = { "valups/system", NULL }; /* only one MIME type */

/* Someone may want to compile and link under W*ndows */
# ifdef WIN32
#  define DECLSPEC __declspec(dllexport)
# else
#  define DECLSPEC
# endif

static struct jsplugin_callbacks* opera_callbacks;

/* getter for the properties that the OBJECT has */
static int sysinfo_getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	char buf[1024] = {0,};
	HERROR err = ERR_OK;
	sysinfo_t *sysinfo = (sysinfo_t *)obj->plugin_private;
	int nCount;
	int nIndex;

	PLUGIN_Print("sysinfo_getter: %s\n", name);

	if ( sysinfo == NULL )
	{
		PLUGIN_Print(("libplugin_sysinfo: sysinfo is NULL.\n"));
		return JSP_GET_NOTFOUND;
	}

	memset(buf, 0, sizeof(buf));

	if (strcmp(name, "modelName") == 0)
	{
		err = OPL_LocalSystem_GetModelName(buf);
		if ( err == ERR_OK )
		{
			if ( sysinfo->model_name ) HLIB_STD_MemFree(sysinfo->model_name);
			sysinfo->model_name = HLIB_STD_StrDup(buf);
			if ( sysinfo->model_name == NULL )
			{
				return JSP_OBJECT_NOMEM;
			}

			result->type = JSP_TYPE_STRING;
			result->u.string = sysinfo->model_name;
			PLUGIN_Print("sysinfo: modelName:%s\n", sysinfo->model_name);
		}

		return JSP_GET_VALUE;
	}
	else if (strcmp(name, "modelVariant") == 0)
	{
		if ( sysinfo->model_variant ) HLIB_STD_MemFree(sysinfo->model_variant);
		sysinfo->model_variant = HLIB_STD_StrDup(buf);
		if ( sysinfo->model_variant == NULL )
		{
			return JSP_OBJECT_NOMEM;
		}
		result->type = JSP_TYPE_STRING;
		result->u.string = sysinfo->model_variant;
		PLUGIN_Print("sysinfo: modelVariant:%s\n", sysinfo->model_variant);

		return JSP_GET_VALUE;
	}
	else if (strcmp(name, "serialNumber") == 0)
	{
		err = OPL_LocalSystem_GetSerialNumber(buf);
		if ( err == ERR_OK )
		{
			if ( sysinfo->serial_number ) HLIB_STD_MemFree(sysinfo->serial_number);
			sysinfo->serial_number = HLIB_STD_StrDup(buf);
			if ( sysinfo->serial_number == NULL )
			{
				return JSP_OBJECT_NOMEM;
			}

			result->type = JSP_TYPE_STRING;
			result->u.string = sysinfo->serial_number;
			PLUGIN_Print("sysinfo: serialNumber:%s\n", sysinfo->serial_number);
		}

		return JSP_GET_VALUE;
	}
	else if (strcmp(name, "softwareVersion") == 0)
	{
		err = OPL_LocalSystem_GetSoftwareVersion(buf);
		if ( err == ERR_OK )
		{
			if ( sysinfo->software_version ) HLIB_STD_MemFree(sysinfo->software_version);
			sysinfo->software_version = HLIB_STD_StrDup(buf+7);
			if ( sysinfo->software_version == NULL )
			{
				return JSP_OBJECT_NOMEM;
			}

			result->type = JSP_TYPE_STRING;
			result->u.string = sysinfo->software_version;
			PLUGIN_Print("sysinfo: softwareVersion:%s\n", sysinfo->software_version);
		}

		return JSP_GET_VALUE;
	}
	else if (strcmp(name, "screenLanguage") == 0)
	{
		err = OPL_LocalSystem_GetScreenLanguage(buf);
		if ( err == ERR_OK )
		{
			if ( sysinfo->screen_language ) HLIB_STD_MemFree(sysinfo->screen_language);
			sysinfo->screen_language = HLIB_STD_StrDup(buf);
			if ( sysinfo->screen_language == NULL )
			{
				return JSP_OBJECT_NOMEM;
			}

			result->type = JSP_TYPE_STRING;
			result->u.string = sysinfo->screen_language;
			PLUGIN_Print("sysinfo: screenLanguage:%s\n", sysinfo->screen_language);
		}

		return JSP_GET_VALUE;
	}
	else if (strcmp(name, "systemId") == 0)
	{
		err = OPL_LocalSystem_GetSystemID(buf);
		if ( err == ERR_OK )
		{
			if ( sysinfo->system_id ) HLIB_STD_MemFree(sysinfo->system_id);
			sysinfo->system_id = HLIB_STD_StrDup(buf);
			if ( sysinfo->system_id == NULL )
			{
				return JSP_OBJECT_NOMEM;
			}

			result->type = JSP_TYPE_STRING;
			result->u.string = sysinfo->system_id;
			PLUGIN_Print("sysinfo: systemId:%s\n", sysinfo->system_id);
		}

		return JSP_GET_VALUE;
	}
	else if (strcmp(name, "usedNetInterface") == 0)
	{
		OplNetworkInterface_t *networkinterfaceObj = NULL;

		err = OPL_LocalSystem_GetNetInterfacesCount(&nCount);
		if ( err != ERR_OK)
		{
			return JSP_GET_VALUE;
		}
		err = OPL_LocalSystem_GetDefaultNetInterface(&nIndex);
		if ( err != ERR_OK)
		{
			return JSP_GET_VALUE;
		}

		networkinterfaceObj = OPL_LocalSystem_GetNetInterfaceByIndex(nIndex);
		if ( networkinterfaceObj == NULL )
		{
			PLUGIN_Print("libplugin_sysinfo: networkinterfaceObj is NULL.\n");
			return JSP_GET_NOTFOUND;
		}

		sysinfo->used_netiface = networkinterfaceObj->eNetType;

		OPL_LocalSystem_FreeNetInterface(networkinterfaceObj);

		result->type = JSP_TYPE_NUMBER;

		switch (sysinfo->used_netiface)
		{
			case OPLNET_TYPE_LAN:
				result->u.number = 1;			// LAN = 1 ; based on Humax_JavaScript_Extension_API.doc
				break;
			case OPLNET_TYPE_WLAN:
				result->u.number = 2;			// WLAN = 2 ; based on Humax_JavaScript_Extension_API.doc
				break;
			case OPLNET_TYPE_NONE:
				result->u.number = 0;			// NONE = 0 ; based on Humax_JavaScript_Extension_API.doc
				break;
			default:
				result->u.number = 1;			// Default is LAN
				break;
		}

		return JSP_GET_VALUE;
	}
	else if (strcmp(name, "macAddress") == 0)
	{
		OplNetworkInterface_t *networkinterfaceObj = NULL;

		err = OPL_LocalSystem_GetNetInterfacesCount(&nCount);
		if ( err != ERR_OK)
		{
			return JSP_GET_VALUE;
		}

		for ( int i=0; i<nCount; ++i )
		{
			networkinterfaceObj = OPL_LocalSystem_GetNetInterfaceByIndex(i);
			if ( networkinterfaceObj == NULL )
			{
				PLUGIN_Print("libplugin_sysinfo: networkinterfaceObj is NULL.\n");
				return JSP_GET_NOTFOUND;
			}

			if ( networkinterfaceObj->eNetType != OPLNET_TYPE_LAN )
			{
				OPL_LocalSystem_FreeNetInterface(networkinterfaceObj);
			}
			else
			{
				if ( networkinterfaceObj->szMacAddress )
				{
					if ( sysinfo->macaddr ) HLIB_STD_MemFree(sysinfo->macaddr);
					sysinfo->macaddr = HLIB_STD_StrDup(networkinterfaceObj->szMacAddress);
					if ( sysinfo->macaddr == NULL )
					{
						OPL_LocalSystem_FreeNetInterface(networkinterfaceObj);
						return JSP_OBJECT_NOMEM;
					}
				}

				OPL_LocalSystem_FreeNetInterface(networkinterfaceObj);

				result->type = JSP_TYPE_STRING;
				result->u.string = sysinfo->macaddr;
				PLUGIN_Print("sysinfo: macaddr:%s\n", sysinfo->macaddr);
				break;
			}
		}

		return JSP_GET_VALUE;
	}
    else if (strcmp(name, "deviceUniqueId") == 0)
	{
		err = OPL_LocalSystem_GetChipId(buf);
		if ( err == ERR_OK )
		{
			if ( sysinfo->chip_id ) HLIB_STD_MemFree(sysinfo->chip_id);
			sysinfo->chip_id = HLIB_STD_StrDup(buf);
			if ( sysinfo->chip_id == NULL )
			{
				return JSP_OBJECT_NOMEM;
			}

			result->type = JSP_TYPE_STRING;
			result->u.string = sysinfo->chip_id;
			PLUGIN_Print("sysinfo: chipid:%s\n", sysinfo->chip_id);
		}

		return JSP_GET_VALUE;
	}

	return JSP_GET_NOTFOUND;
}

/* destructor */
static int sysinfo_destructor(jsplugin_obj *obj)
{
	PLUGIN_Print("sysinfo destructor\n");

	sysinfo_t *priv_data = (sysinfo_t *)obj->plugin_private;
	if ( priv_data == NULL )
	{
		return JSP_DESTROY_OK;
	}

	if ( priv_data->model_name )
		HLIB_STD_MemFree(priv_data->model_name);

	if ( priv_data->model_variant )
		HLIB_STD_MemFree(priv_data->model_variant);

	if ( priv_data->serial_number )
		HLIB_STD_MemFree(priv_data->serial_number);

	if ( priv_data->macaddr )
		HLIB_STD_MemFree(priv_data->macaddr);

	if ( priv_data->software_version )
		HLIB_STD_MemFree(priv_data->software_version);

	if ( priv_data->screen_language )
		HLIB_STD_MemFree(priv_data->screen_language);

	if ( priv_data->system_id )
		HLIB_STD_MemFree(priv_data->system_id);

	if ( priv_data->chip_id )
		HLIB_STD_MemFree(priv_data->chip_id);

	HLIB_STD_MemFree(priv_data);
	priv_data = NULL;

	return JSP_DESTROY_OK;
}

/* called back when an OBJECT that the plugin claims to support is
   created. note that the plugin also needs to have the appropriate
   access policy set up, or it won't be taken into account at all. */
static int sysinfo_handle_object(jsplugin_obj *global_obj, jsplugin_obj *object_obj, int attrs_count, jsplugin_attr *attrs,
				  jsplugin_getter **getter, jsplugin_setter **setter, jsplugin_destructor **destructor,
				  jsplugin_gc_trace **trace, jsplugin_notify **inserted, jsplugin_notify **removed)
{
	sysinfo_t *priv_data = NULL;

	PLUGIN_Print("libplugin_sysinfo: handle_object.\n");
	priv_data = (sysinfo_t *)HLIB_STD_MemAlloc(sizeof(sysinfo_t));
	if ( priv_data == NULL )
	{
		PLUGIN_Print("libplugin_sysinfo: priv_data is NULL.\n");
		return JSP_OBJECT_NOMEM;
	}

	memset(priv_data, 0, sizeof(*priv_data));

	object_obj->plugin_private = (void *)priv_data;

	/* set callbacks */
	*getter = &sysinfo_getter;
	*setter = NULL;
	*inserted = NULL;
	*destructor = &sysinfo_destructor;
	*removed = NULL;

	return JSP_OBJECT_OK;
}

static void sysinfo_init(jsplugin_obj *global_context)
{
	PLUGIN_Print("libplugin_sysinfo: init.\n");
}

static void sysinfo_destroy(jsplugin_obj *global_context)
{
	PLUGIN_Print("libplugin_sysinfo: destroy.\n");
}


static int sysinfo_allow_access(const char *protocol, const char *hostname, int port, const char *url)
{
	PLUGIN_Print("allow_access: proto=%s, name=%s, port=%d\n", protocol, hostname, port);

	return 1;
}

extern "C" {

/* the global plugin hook. */
DECLSPEC int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
{
	PLUGIN_FPrint("====== sysinfo plugin loaded ======\n");
	cap.global_names = global_names;
	cap.object_types = object_types;

	cap.global_getter = NULL;
	cap.global_setter = NULL;

	cap.init = sysinfo_init;
	cap.destroy = sysinfo_destroy;
	cap.gc_trace = NULL;

	cap.handle_object = sysinfo_handle_object;
	cap.allow_access = sysinfo_allow_access;
	cap.default_permission = NULL;

	*result = &cap;

	opera_callbacks = c;

	PLUGIN_Print("##########################################################################\n");
	PLUGIN_Print("SYSINFO jsplugin_capabilities\n");
	PLUGIN_Print("##########################################################################\n");

	return 0;
}

}

