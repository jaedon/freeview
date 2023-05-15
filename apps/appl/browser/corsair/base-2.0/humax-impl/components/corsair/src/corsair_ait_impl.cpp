/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#include "ait_api.h"
#include "corsair_log.h"
#include "corsair_ait_manager.h"

void hbbtv_ait_start()
{
	LOG_DEBUG(" (+)\n" );
	CorsairAITManager::init();
	LOG_DEBUG(" (-)\n" );
}

void hbbtv_ait_stop()
{ 
	LOG_DEBUG(" (+)\n" );
	CorsairAITManager::shutdown();
	LOG_DEBUG(" (-)\n" );
}

void hbbtv_ait_invalidate()
{ 
	LOG_DEBUG(" (+)\n" );
	CorsairAITManager::removeInvalidAIT();
	LOG_DEBUG(" (-)\n" );
}

void hbbtv_ait_completed(unsigned char version_number)
{
	LOG_DEBUG(" (+) %d\n", version_number );
	CorsairAITManager::notifyCompletedToLoadAIT(version_number);
	LOG_DEBUG(" (-)\n" );
}

