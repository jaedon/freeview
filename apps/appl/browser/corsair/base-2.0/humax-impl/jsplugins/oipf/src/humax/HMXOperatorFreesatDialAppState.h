// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef	CONFIG_OP_FREESAT
#ifdef OIPF
#ifndef _HMXOPERATORFREESAT_DIAL_APP_STATE_H_
#define _HMXOPERATORFREESAT_DIAL_APP_STATE_H_

#include "Event.h"

namespace Ooif
{
	class HMXOperatorFreesatDialAppState : public Event
	{
	private:
		OOIFNumber	m_Type;
		int			m_appState;
		int			m_userData;

	public:
		HMXOperatorFreesatDialAppState (jsplugin_obj *context, OOIFNumber eType, int appState, int userData);
		~HMXOperatorFreesatDialAppState ();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif //
#endif // OIPF
#endif




