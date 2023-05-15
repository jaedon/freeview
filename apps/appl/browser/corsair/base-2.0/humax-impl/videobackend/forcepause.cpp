/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software ASA.  All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Opera's
 * separate written consent.
 */


//
// Simple test code for using the ForcePause event to allow only one video
// to be in the play state at any time.
//
// In MyNew, add:
//   add_instance(instance);
// In MyDestroy, add:
//   remove_instance(instance);
// In MyPlay, add:
// 	 pause_all_other_instances(&gOperaVideoFuncs, instance);
//
// This will cause the last video in the page to be allowed to play when the page loads.
// When the user presses play on any video, all other videos will be paused.
//

#ifdef FORCEPAUSE_SUPPORT

#include "forcepause.h"

#define MAX_INSTANCES 100

// instance info
struct instance_info_t
{
	OVB instance;
};

static instance_info_t* s_instances[MAX_INSTANCES];

void add_instance(OVB instance)
{
	instance_info_t* ins = new instance_info_t();
	if (!ins) return;

	ins->instance = instance;

	for (int i = 0; i < MAX_INSTANCES; i++) {
		if (s_instances[i] == NULL) { // place in first open slot
			s_instances[i] = ins;
			return;
		}
	}
}

void remove_instance(OVB instance)
{
	for (int i = 0; i < MAX_INSTANCES; i++) {
		if (s_instances[i] && s_instances[i]->instance == instance) {
			delete s_instances[i];
			s_instances[i] = NULL;
		}
	}
}

void pause_all_other_instances(OperaVideoFuncs* ovf, OVB instance)
{
#ifdef DEBUG
	fprintf(stderr, "Pausing other instances...\n");
#endif

	for (int i = 0; i < MAX_INSTANCES; i++) {
		if (s_instances[i]) {
			if (s_instances[i]->instance != instance) {
#ifdef DEBUG
				fprintf(stderr, "\tForcePause for instance 0x%x\n", (int)s_instances[i]->instance);
#endif

				OVB_Event event;
				event.type = ForcePause;
				if (ovf) ovf->event(s_instances[i]->instance, event);
			}
#ifdef DEBUG
			else
				fprintf(stderr, "\tAllow MyPlay for instance 0x%x\n", (int)instance);
#endif
		}
	}
}

#endif // FORCEPAUSE_SUPPORT

