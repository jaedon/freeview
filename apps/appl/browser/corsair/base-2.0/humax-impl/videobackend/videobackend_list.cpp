/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software ASA.  All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Opera's
 * separate written consent.
 */


#include "localdata.h"
#include "videobackend_list.h"
#include "videobackend_utils.h"

#include <iostream>
#include <cstdio>
#include <pthread.h>

#define MAX_INSTANCES 100

// instance info
struct instance_info_t
{
	OVB instance;
	unsigned long ticks;
};

static instance_info_t* s_instances[MAX_INSTANCES];

static void print_instances(void)
{
	for ( int i = 0; i < MAX_INSTANCES; i++ )
	{
		if ( s_instances[i] )
		{
			printf("############### INSTACE [%d] ################\n", i);
			printf("INS Handle = %p\n", s_instances[i]->instance);
			printf("TICK = %p\n", s_instances[i]->ticks);
			LocalData *ptr = (LocalData*)(s_instances[i]->instance->bdata);
			printf("playState = %d\n", ptr->is_started); 
			printf("##############################################\n");
		}
	}
}

void vblist_add_instance(OVB instance)
{
	instance_info_t* ins = new instance_info_t();
	if ( !ins ) return;

	ins->instance = instance;
	ins->ticks = vbutils_get_ticks();

	for ( int i = 0; i < MAX_INSTANCES; i++ )
	{
		if ( s_instances[i] == NULL )
		{
			s_instances[i] = ins;
			print_instances();
			return;
		}
	}

	delete ins;
}

void vblist_remove_instance(OVB instance)
{
	for ( int i = 0; i < MAX_INSTANCES; i++ )
	{
		if ( s_instances[i] && s_instances[i]->instance == instance )
		{
			delete s_instances[i];
			s_instances[i] = NULL;
		}
	}
}

OVB vblist_get_latest_instance(void)
{
	unsigned long max = 0;
	int max_index = -1;

	for ( int i = 0; i < MAX_INSTANCES; i++ )
	{
		if ( s_instances[i] && s_instances[i]->ticks > max )
		{
			max = s_instances[i]->ticks;
			max_index = i;
		}
	}

	if ( max_index == -1 )
		return NULL;

	return s_instances[max_index]->instance;
}

bool vblist_empty()
{
	for ( int i = 0; i < MAX_INSTANCES; i++ )
	{
		if ( s_instances[i] ) return false;
	}

	return true;
}

OVB vblist_get_playing_instance(void)
{
	for ( int i = 0; i < MAX_INSTANCES; i++ )
	{
		if ( s_instances[i] )
		{
			if ( !(s_instances[i]->instance) ) continue;

			LocalData *ptr = (LocalData*)(s_instances[i]->instance->bdata);
			if ( !ptr ) continue;

			if ( ptr->is_started == 1 )
				return s_instances[i]->instance;
		}
	}

	return NULL;
}
