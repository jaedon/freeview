#include "jlabsCDSRecording.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "Collection.h"
#include "BookmarkCollection.h"
#include "ParentalRating.h"
#include "jlabsCDSMetadata.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"

#include "opldmp.h"

using namespace Ooif;

jlabsCDSRecording::jlabsCDSRecording( jlabsCDSRecordingHandle id, jlabsCDSMetaHandle metaId ) : WrapperObject<jlabsCDSRecording>(id)
{
	ENTRY;
	m_metaDataHandle = metaId;
}

jlabsCDSRecording::jlabsCDSRecording(const jlabsCDSRecording&original ) : WrapperObject<jlabsCDSRecording>(original.identifier), m_metaDataHandle(original.m_metaDataHandle)
{
	ENTRY;
}


jlabsCDSRecording::~jlabsCDSRecording()
{
	ENTRY;
}

int jlabsCDSRecording::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	NATIVE_GET_STRING("objectID", native_jlabsCDSRecordingGetObjectID(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("class", native_jlabsCDSRecordingGetClass(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("restricted", native_jlabsCDSRecordingGetRestricted(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("objectType", native_jlabsCDSRecordingGetObjectType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("importURI", native_jlabsCDSRecordingGetImportURI(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("thumbnail", native_jlabsCDSRecordingGetThumbnailURI(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("title", native_jlabsCDSRecordingGetTitle(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("date", native_jlabsCDSRecordingGetDate(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("duration", native_jlabsCDSRecordingGetDuration(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("resolution", native_jlabsCDSRecordingGetResolution(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("digitalCopyControl", native_jlabsCDSRecordingGetDigitalCopyControl(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("genre", obj, native_jlabsCDSRecordingGetGenre(getIdentifier(), &SIZE, &NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("childCount", native_jlabsCDSRecordingGetChildCount(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("channelName", native_jlabsCDSRecordingGetChannelName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("channelNr", native_jlabsCDSRecordingGetChannelNumber(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("radioBand", native_jlabsCDSRecordingGetRadioBand(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("analogCopyControl", native_jlabsCDSRecordingGetAnalogCopyControl(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("mediaProfile", native_jlabsCDSRecordingGetMediaProfile(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("objectParentId", native_jlabsCDSRecordingGetObjectParentId(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_VOIDP_COLLECTION("parentalRating", obj,
			native_jlabsCDSRecordingGetParentalRatings(getIdentifier(), &SIZE, &NATIVE_ARRAY),
			getParentalRatings(SIZE, NATIVE_ARRAY),
			JSP_GET_VALUE);

	NATIVE_GET_VOIDP_COLLECTION("bookMarks", obj,
			native_jlabsCDSRecordingGetBookmarks(getIdentifier(), &SIZE, &NATIVE_ARRAY),
			new BookmarkCollection(getIdentifier(), SIZE, NATIVE_ARRAY),
			JSP_GET_VALUE);

	GET_OBJECT("cdsMetadata", obj, new jlabsCDSMetaData(m_metaDataHandle), JSP_GET_VALUE );
	return JSP_GET_NOTFOUND;
}

int jlabsCDSRecording::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

void jlabsCDSRecording::releaseNative( jlabsCDSRecordingHandle id)
{
	NATIVE_WARN_NOK(native_jlabsCDSRecordingReleaseHandle(id));
}

ParentalRatingCollection *jlabsCDSRecording::getParentalRatings(int count, ParentalRatingHandle *handles)
{
	ParentalRatingCollection *collection = new ParentalRatingCollection(getIdentifier());
	for (int i=0; i<count; i++) {
		collection->addItem(new ParentalRating(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}
