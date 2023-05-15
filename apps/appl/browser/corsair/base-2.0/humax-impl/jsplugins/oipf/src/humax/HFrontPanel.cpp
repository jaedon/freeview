#ifdef OIPF
#include "HFrontPanel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdarg.h>


#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

//##########################################
HFrontPanel::HFrontPanel() : EventTarget()
{
	ENTRY;
}


HFrontPanel::~HFrontPanel()
{
	ENTRY;
}

int HFrontPanel::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	//GET_FUNCTION(setFPString, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setString, 	obj, "snnb", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setLed,		obj, "nbbb", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setAnimation,	obj, "nnnn",	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setCurrentMediaInform,obj, "",	JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HFrontPanel::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

int HFrontPanel::setString(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	native_HFrontPanel_SetString(STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]), NUMBER_VALUE(argv[2]), BOOLEAN_VALUE(argv[3]));
	return 0;
}

int HFrontPanel::setLed(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	// param  : led id, 	led on or off
	native_HFrontPanel_SetLedOnOff(NUMBER_VALUE(argv[0]), BOOLEAN_VALUE(argv[1]), BOOLEAN_VALUE(argv[2]), BOOLEAN_VALUE(argv[3]));
	return 0;
}

// animation function
int HFrontPanel::setAnimation(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	// vfd :	param : F/P item type (string) 	,	string animation type,
	// led : 	param : F/P item type (led)		, 	led id,					led blink on or off,    blink delay ( ms )
	native_HFrontPanel_SetAnimation(NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), NUMBER_VALUE(argv[2]), NUMBER_VALUE(argv[3]));

	return 0;
}

// category set

int HFrontPanel::setCurrentMediaInform(jsplugin_obj * this_obj,jsplugin_obj * function_obj,int argc,jsplugin_value * argv,jsplugin_value * result)
{
	native_HFrontPanel_SetCategoryInform(0x00000002);
	return 0;
}


#endif // OIPF



