/*************************************************************************************************************
	File 		: maxdome.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: maxdome plugin main
	date    	: 2012/11/07
	attention 	: 

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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
*************************************************************************************************************/

#include "jsplugin.h"
#include "macro.h"

#include "systeminfo.h"
#include "lanconfig.h"
#include "wlanconfig.h"

#include <cstring>
#include <cstdio>


namespace Maxdome
{
	static jsplugin_cap cap;

	static const char* global_names[] = {
		"systeminfoobject",
		"lanconfigobject",
		"wlanconfigobject",
		NULL };

	static const char* object_types[] = { NULL }; /* only one MIME type */

	jsplugin_callbacks* g_opera_callbacks;

	/******************************************************************************************
	 * jsplugin
	 ******************************************************************************************/
	static int maxdome_allow_access(const char *protocol, const char *hostname, int port, const char *url)
	{
		return 1;
	}

	static int maxdome_global_getter(jsplugin_obj *ctx, const char *name, jsplugin_value *result)
	{
		ENTRY;

		GLOBAL_GET_OBJECT("systeminfoobject", ctx, SystemInfoObject::constructor,
				result, JSP_GET_VALUE_CACHE);

		GLOBAL_GET_OBJECT("lanconfigobject", ctx, LanConfigObject::constructor,
				result, JSP_GET_VALUE_CACHE);

		GLOBAL_GET_OBJECT("wlanconfigobject", ctx, WlanConfigObject::constructor,
				result, JSP_GET_VALUE_CACHE);

		GET_FAIL;
	}
}	/* namespace */

using namespace Maxdome;

extern "C" {

int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
{
	PLUGIN_FPrint("====== Maxdome plugin loaded ======\n");
	cap.global_names = global_names;
	cap.object_types = object_types;

	//cap.handle_object = maxdome_handle_object;
	cap.allow_access = maxdome_allow_access;
	cap.global_getter = maxdome_global_getter;
	cap.global_setter = NULL;
	cap.default_permission = NULL;

	*result = &cap;

	g_opera_callbacks = c;

	return 0;
}

}	/* extern C */

