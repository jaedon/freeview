// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __PARENTAL_RATING_CHANGE_EVENT
#define __PARENTAL_RATING_CHANGE_EVENT

#include "Event.h"
#include "NativeParentalControlManager.h"
#include "ParentalRating.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
	class ParentalRatingChangeEvent : public Event
	{
	private:
		char* _contentID;
#if defined(OIPF) || defined(HBBTV_1_2_1)
		ParentalRatingHandle* _ratings;
		int _ratings_count;
#else
		ParentalRatingHandle _rating;
#endif // OIPF || HBBTV_1_2_1
		char* _DRMSystemID;
#if defined(OIPF) || defined(HBBTV_1_2_1)
		OOIFBoolean _blocked;
#endif // OIPF || HBBTV_1_2_2
#if defined(OIPF) || defined(HBBTV_1_2_1)
		ParentalRatingCollection *getParentalRatingCollection(int count, ParentalRatingHandle *ratings);
#endif // OIPF || HBBTV_1_2_1
	protected:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	public:
		ParentalRatingChangeEvent(jsplugin_obj* c
			, const char* contentID
#if defined(OIPF) || defined(HBBTV_1_2_1)
			, ParentalRatingHandle* ratings
			, int ratings_count
#else
			, ParentalRatingHandle rating
#endif // OIPF || HBBTV_1_2_1
			, const char* DRMSystemID
#if defined(OIPF) || defined(HBBTV_1_2_1)
			, OOIFBoolean blocked
#endif // OIPF || HBBTV_1_2_1
			);
		virtual ~ParentalRatingChangeEvent();
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif //__PARENTAL_RATING_CHANGE_EVENT
