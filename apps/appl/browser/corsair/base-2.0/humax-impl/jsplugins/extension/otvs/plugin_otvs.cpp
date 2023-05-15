/** **********************************************************************************************************
	@file 		plugin_otvs.cpp
	
	@date		2012/09/07
	@author		Sangsoo Yoo (ssyoo@humaxdigital.com)
	@breif		
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR 
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR 
	INABILITY TO USE THE SOFTWARE.
	
	This License is effective until terminated. You may terminate this License at any time 
	by destroying all copies of the Software including all documentation. This License will terminate 
	immediately without notice from Humax to you if you fail to comply with any provision of this License. 
	Upon termination, you must destroy all copies of the Software and all documentation.  
	
	The laws of the Republic of Korea will apply to any disputes arising out of or relating to 
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>

#include "jsplugin.h"
#include "libplugin.h"

#include "plugin_otvs.hpp"

static jsplugin_cap cap;

static const char* global_names[] = { "tvstoreapi", NULL }; /* no global names */
static const char* object_types[] = { NULL }; /* only one MIME type */

/* Someone may want to compile and link under W*ndows */
# ifdef WIN32
#  define DECLSPEC __declspec(dllexport)
# else
#  define DECLSPEC
# endif

static struct jsplugin_callbacks* opera_callbacks;

OTVSAppManager::OTVSAppManager()
{
	apps.clear();
	loadOTVSApps();
}

OTVSAppManager::~OTVSAppManager()
{
	storeOTVSApps();
	apps.clear();
	filename.clear();
}

void OTVSAppManager::loadOTVSApps()
{
	if ( !filename.empty() )
		return;
		
	std::ifstream ifile;
	std::string applist_path, tmp;
	const char* data_filename = "otvs_apps.dat";
	char* opera_home = getenv("OPERA_HOME");
	char app_id[256];
	
	if ( opera_home == NULL )
	{
		return;
	}	
	
	applist_path = opera_home;
	applist_path.append("/");
	applist_path.append(data_filename);
	filename = applist_path;
	ifile.open(applist_path.c_str(), std::ifstream::binary );
	if( !ifile.is_open())
		return;
	while(ifile)
	{
		memset(app_id, 0x00, sizeof(app_id));
		ifile.getline(app_id, sizeof(app_id));
		tmp = app_id;
		if( tmp.empty())
			break;
		PLUGIN_Print(" loaded %s\n", tmp.c_str());
		apps.push_back(tmp);
	}
	ifile.close();
	
	PLUGIN_Print(" %d apps are loaded\n", apps.size());
}

void OTVSAppManager::storeOTVSApps()
{
	std::ofstream ofile;
	std::vector<std::string>::iterator	it;
	
	if( apps.size() == 0 || filename.size() == 0 )
		return;
	
	ofile.open(filename.c_str(), std::ofstream::binary | std::ofstream::trunc);
	if( !ofile.is_open())
		return;
	
	for ( it = apps.begin(); it != apps.end(); it++ )
	{
		ofile.write((*it).c_str(), (*it).size());
		ofile.write("\n", 1);
	}
	ofile.close();
}

void OTVSAppManager::add(const char* app_id)
{
	std::vector<std::string>::iterator	it;
	for ( it = apps.begin(); it != apps.end(); it++ )
	{
		if ( (*it).compare( app_id ) == 0 )
		{
			break;
		}
	}

	if ( it == apps.end() )
	{
		apps.push_back(app_id);
	}
}

void OTVSAppManager::remove(const char* app_id)
{
	std::vector<std::string>::iterator	it;
	
	for ( it = apps.begin(); it != apps.end(); it++ )
	{
		if ( (*it).compare( app_id ) == 0 )
		{
			apps.erase(it);
			break;
		}
	}
}

const char* OTVSAppManager::item(int index)
{
	if( index >= apps.size() )
		return NULL;
	return apps.at(index).c_str();
}

static int local_installApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	OTVSAppManager* manager = static_cast<OTVSAppManager*>(this_obj->plugin_private);

	if ( !manager )
		return JSP_CALL_ERROR;
	
	if (!( argc == 1 && argv[0].type == JSP_TYPE_STRING ))
		return JSP_CALL_ERROR;
	
	manager->add( argv[0].u.string );
	result->type = JSP_TYPE_NUMBER;
	result->u.number = 0;
	return JSP_CALL_VALUE;
}	

static int local_uninstallApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	OTVSAppManager* manager = static_cast<OTVSAppManager*>(this_obj->plugin_private);

	if ( !manager )
		return JSP_CALL_ERROR;
	
	if (!( argc == 1 && argv[0].type == JSP_TYPE_STRING ))
		return JSP_CALL_ERROR;
	
	manager->remove( argv[0].u.string );
	result->type = JSP_TYPE_NUMBER;
	result->u.number = 0;
	return JSP_CALL_VALUE;
}

static int local_getInstalledApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	OTVSAppManager* manager = static_cast<OTVSAppManager*>(this_obj->plugin_private);
	
	if ( !manager )
		return JSP_CALL_ERROR;
		
	if (!( argc == 1 && argv[0].type == JSP_TYPE_NUMBER ))
		return JSP_CALL_ERROR;
		
	result->type = JSP_TYPE_STRING;
	result->u.string = strdup(manager->item(argv[0].u.number));
	return JSP_CALL_VALUE;
}

int plugin_otvs_getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	jsplugin_obj *created_obj  = NULL;
	int getter_retval = JSP_GET_NOTFOUND;

	if (strcmp(name, "installApp") == 0)
	{
		int r = opera_callbacks->create_function(obj, 				//refobj
												 NULL, 				//getter
												 NULL, 				//setter
												 local_installApp, 	//f_call
												 NULL, 				//f_construct
												 "s", 				//f_signature
												 NULL, 				//desturctor
												 NULL, 				//trace
												 &created_obj);		//resutl
		if ( r != JSP_CREATE_OK ) 
			return JSP_GET_ERROR;
		result->type = JSP_TYPE_OBJECT;
		result->u.object = created_obj;
		getter_retval = JSP_GET_VALUE_CACHE;
	}
	if (strcmp(name, "uninstallApp") == 0)
	{
		int r = opera_callbacks->create_function(obj, 				//refobj
												 NULL, 				//getter
												 NULL, 				//setter
												 local_uninstallApp, 	//f_call
												 NULL, 				//f_construct
												 "s", 				//f_signature
												 NULL, 				//desturctor
												 NULL, 				//trace
												 &created_obj);		//resutl
		if ( r != JSP_CREATE_OK ) 
			return JSP_GET_ERROR;
		result->type = JSP_TYPE_OBJECT;
		result->u.object = created_obj;
		getter_retval = JSP_GET_VALUE_CACHE;
	}
	if (strcmp(name, "getInstalledApp") == 0)
	{
		int r = opera_callbacks->create_function(obj, 				//refobj
												 NULL, 				//getter
												 NULL, 				//setter
												 local_getInstalledApp, 	//f_call
												 NULL, 				//f_construct
												 "n", 				//f_signature
												 NULL, 				//desturctor
												 NULL, 				//trace
												 &created_obj);		//resutl
		if ( r != JSP_CREATE_OK ) 
			return JSP_GET_ERROR;
		result->type = JSP_TYPE_OBJECT;
		result->u.object = created_obj;
		getter_retval = JSP_GET_VALUE_CACHE;
	}
	if (strcmp(name, "length") == 0)
	{
		result->type = JSP_TYPE_NUMBER;
		result->u.number = static_cast<OTVSAppManager *>(obj->plugin_private)->size();
		getter_retval = JSP_GET_VALUE;
	}
	return getter_retval;
}

int plugin_otvs_destructor(jsplugin_obj *obj)
{
	PLUGIN_Print(" +- %p\n", obj);
	
	if(obj->plugin_private)
	{
		OTVSAppManager* manager = static_cast<OTVSAppManager*>(obj->plugin_private);
		delete manager;
	}
	
	return JSP_DESTROY_OK;
}

int plugin_otvs_global_getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	jsplugin_obj *created_obj  = NULL;
	int getter_retval = JSP_GET_NOTFOUND;

	if (strcmp(name, "tvstoreapi") == 0)
	{
		OTVSAppManager* manager = NULL;
   		int r = opera_callbacks->create_object( obj,					//refobj
		    									plugin_otvs_getter,		//getter
		    									NULL,					//setter
		  	 									plugin_otvs_destructor,	//destructor
		    									NULL,					//trace
		    									&created_obj);			//result
		if ( r != JSP_CREATE_OK ) 
			return JSP_GET_ERROR;
		result->type = JSP_TYPE_OBJECT;
		result->u.object = created_obj;
		manager = new OTVSAppManager();
		if ( !manager )
			return JSP_GET_NOMEM;
		created_obj->plugin_private = manager;
		getter_retval = JSP_GET_VALUE_CACHE;
	}
	return getter_retval;
}

void plugin_otvs_init(jsplugin_obj *global_context)
{
	PLUGIN_Print(" +- \n");
}

void plugin_otvs_destroy(jsplugin_obj *global_context)
{
	PLUGIN_Print(" +- \n");
}

int plugin_otvs_allow_access(const char *protocol, const char *hostname, int port, const char *url)
{
	PLUGIN_Print("proto=%s, name=%s, port=%d\n", protocol, hostname, port);
	return 1;
}

extern "C" {
DECLSPEC int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
{
	PLUGIN_FPrint("====== OTVS plugin loaded ======\n");
	
	cap.global_names = global_names;
	cap.object_types = object_types;

	cap.global_getter = plugin_otvs_global_getter;
	cap.global_setter = NULL;

	cap.init = plugin_otvs_init;
	cap.destroy = plugin_otvs_destroy;
	cap.gc_trace = NULL;

	cap.handle_object = NULL;
	cap.allow_access = plugin_otvs_allow_access;

	*result = &cap;

	opera_callbacks = c;

	return 0;
}
} /* extern "C" */
