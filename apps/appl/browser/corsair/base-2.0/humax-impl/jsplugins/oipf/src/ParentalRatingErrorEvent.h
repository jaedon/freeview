// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __PARENTAL_RATING_ERROR_EVENT
#define __PARENTAL_RATING_ERROR_EVENT

#include "Event.h"
#include "NativeParentalControlManager.h"
#include "ParentalRating.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
	class ParentalRatingErrorEvent : public Event
	{
	private:
		char* _contentID;
#if defined(OIPF) || defined(HBBTV_1_2_1)
		ParentalRatingHandle* _ratings;
		int _count;
#else
		ParentalRatingHandle _rating;
#endif
		char* _DRMSystemID;
#if defined(OIPF) || HBBTV_1_2_1
		ParentalRatingCollection *getParentalRatingCollection(int count, ParentalRatingHandle *ratings);
#endif // OIPF || HBBTV_1_2_1
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	public:
		ParentalRatingErrorEvent(jsplugin_obj* c
			, const char* contentID
#if defined(OIPF) || defined(HBBTV_1_2_1)
			, ParentalRatingHandle* ratings
			, int count
#else
			, ParentalRatingHandle rating
#endif // OIPF || HBBTV_1_2_1
			, const char* DRMSystemID /*!< use NULL for JavaScripts's null */
			);
		virtual ~ParentalRatingErrorEvent();
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif //__PARENTAL_RATING_ERROR_EVENT
