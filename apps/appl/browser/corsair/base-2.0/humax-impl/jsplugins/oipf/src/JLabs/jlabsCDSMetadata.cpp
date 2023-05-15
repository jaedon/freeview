#include "jlabsCDSMetadata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"

using namespace Ooif;


jlabsCDSMetaData::jlabsCDSMetaData( jlabsCDSMetaHandle id ) : WrapperObject<jlabsCDSMetaData>(id)
{
	ENTRY;
}

jlabsCDSMetaData::~jlabsCDSMetaData()
{
	ENTRY;
}

int jlabsCDSMetaData::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	NATIVE_GET_STRING("album", native_jlabsCDSMetaGetAlbum(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("artist", native_jlabsCDSMetaGetArtist(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("description", native_jlabsCDSMetaGetDescription(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longDescription", native_jlabsCDSMetaGetLongDescription(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("author", native_jlabsCDSMetaGetAuthor(identifier, &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int jlabsCDSMetaData::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

void jlabsCDSMetaData::releaseNative( jlabsCDSMetaHandle id)
{
	ENTRY;
}
