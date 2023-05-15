/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Anders Karlsson
 */


#ifndef LOG_LEVELS_H
#define LOG_LEVELS_H


namespace OperaHbbTV
{
	typedef enum
	{
		LOG_LEVEL_DEBUG = 1,	// Debug messages
		LOG_LEVEL_LOG = 2,		// Normal logging
		LOG_LEVEL_WARN = 3,		// Warnings about things
		LOG_LEVEL_ERR = 4		// Real errors
	} LogLevel;
}


#endif // LOG_LEVELS_H
