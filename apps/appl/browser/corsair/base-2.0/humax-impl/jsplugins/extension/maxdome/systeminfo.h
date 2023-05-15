/*************************************************************************************************************
	File 		: systeminfo.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
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

#ifndef __SYSTEM_INFO_OBJECT_H__
#define __SYSTEM_INFO_OBJECT_H__

#include "jsplugin.h"
#include "plugtype.h"

namespace Maxdome {

#define MODEL_NAME_LEN	64
#define SW_VERSION_LEN	64
#define DEBUG_INFO_LEN	512	
#define MAX_SERIAL_NUMBER_LEN		1024

typedef enum {
	SYSINFO_RESOLUTION_NTSC_SD	= 0,	/* 640 x 480 */
	SYSINFO_RESOLUTION_PAL_SD,			/* 720 x 576 */
	SYSINFO_RESOLUTION_720P,			/* 1280 x720 */
	SYSINFO_RESOLUTION_1080P,			/* 1920 x 1080 */
	SYSINFO_RESOLUTION_WXGA,			/* 1366 x 1024 */
} eSysInfoResolution;


/******************************************************************************************
 * SystemInfoObject
 ******************************************************************************************/
	class SystemInfoObject {
	public:
		SystemInfoObject();
		virtual ~SystemInfoObject();

		static int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		static int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static int constructor( jsplugin_obj *this_obj, jsplugin_obj *function_obj,
				int argc, jsplugin_value *argv, jsplugin_value *result);
		static int destructor(jsplugin_obj *obj);

	private:
		static PlugReturnType getProductName(jsplugin_obj *obj, const char **retval);
		static PlugReturnType getSerialNumber(jsplugin_obj *obj, const char **retval);
		static PlugReturnType getSecureSerialNumber(jsplugin_obj *obj, const char **retval);
		static PlugReturnType getFirmwareVersion(jsplugin_obj *obj, const char **retval);
		static PlugReturnType getResolution(jsplugin_obj *obj, double *retval);
		static PlugReturnType getNetworkType(jsplugin_obj *obj, double *retval);
		static PlugReturnType getVenderName(jsplugin_obj *obj, const char **retval);

		static int debug_free(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
				jsplugin_value *argv, jsplugin_value *result);

	public:
		char m_model_name[MODEL_NAME_LEN];
		char m_serial_no[MAX_SERIAL_NUMBER_LEN];
		char m_secure_serial_no[MAX_SERIAL_NUMBER_LEN];
		char m_sw_version[SW_VERSION_LEN];
		char m_debug_info[DEBUG_INFO_LEN];
		char m_vender_name[MODEL_NAME_LEN];
	};
} /* namespace */

#endif
