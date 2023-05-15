#include "jlabsContentSearch.h"
#include "jlabsContentResults.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "Query.h"

using namespace Ooif;


jlabsContentSearch::jlabsContentSearch(jlabsContentSearchHandle i) : WrapperObject<jlabsContentSearch>(i)
{
	classType = CLASS_JLABS_CONTENTSEARCH;
}

jlabsContentSearch::~jlabsContentSearch()
{
	ENTRY;
}

int jlabsContentSearch::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_OBJECT("query", 	obj, native_jlabsContentSearchGetQuery	(getIdentifier(), &RETVAL), new Query(RETVAL), 					JSP_GET_VALUE);
	NATIVE_GET_OBJECT("result", obj, native_jlabsContentSearchGetResult	(getIdentifier(), &RETVAL), new jlabsContentResults(RETVAL), 	JSP_GET_VALUE);
	GET_FUNCTION(setQuery, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(orderBy, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createQuery, obj, "sns", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsContentSearch::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

int jlabsContentSearch::setQuery(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	//Dummy Implementation TODO
#if 0
	jlabsContentSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsContentSearch);
	Query *q = OBJECT_VALUE_OR_THROW(q, argv[0], Query);

	NATIVE_CALL(native_jlabsContentSearchSetQuery(instance->getIdentifier(), q->getIdentifier()));
#endif

	return JSP_CALL_NO_VALUE;
}

int jlabsContentSearch::orderBy(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	//Dummy Implementation TODO
#if 0
	jlabsContentSearch *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsContentSearch);
	NATIVE_CALL(native_jlabsContentSearchOrderBy(instance->getIdentifier(), STRING_VALUE(argv[0]), BOOLEAN_VALUE(argv[1])));
#endif

	return JSP_CALL_NO_VALUE;
}

int jlabsContentSearch::createQuery(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(3);

	//Dummy Implementation TODO
#if 0
	QueryHandle id = NULL;
	NATIVE_CALL(native_jlabsContentSearchCreateQuery(STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]), STRING_VALUE(argv[2]), &id));

	RETURN_OBJECT(this_obj, new Query(id), result, JSP_CALL_VALUE);
#endif
	RETURN_NULL(0);

}

void jlabsContentSearch::releaseNative(jlabsContentSearchHandle id)
{
	//NATIVE_WARN_NOK(native_jlabsContentSearchReleaseHandle(id));
}
