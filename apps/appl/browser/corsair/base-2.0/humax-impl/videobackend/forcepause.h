/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software ASA.  All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Opera's
 * separate written consent.
 */

#ifndef FORCEPAUSE_H
#define FORCEPAUSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "include/operavideobackend_api.h"

void add_instance(OVB instance);
void remove_instance(OVB instance);

void print_instances();
void pause_all_other_instances(OperaVideoFuncs* ovf, OVB instance);

#endif // FORCEPAUSE_H

