/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Anders Karlsson
 */


#ifndef LOGGING_LOG_ENTRY_H
#define LOGGING_LOG_ENTRY_H

#include <ostream>
#include <iostream>
#include <sstream>

#include "log_levels.h"


namespace OperaHbbTV
{
	typedef std::basic_ostream<char, std::char_traits<char> > cout_t;
	typedef cout_t& (*endl_t)(cout_t&);


	// LogEntry represents one single log line entry. It accepts stream input and outputs itself on destruction.
	class LogEntry
	{
	public:
		LogEntry(LogLevel log_level, const std::string module = "");
		~LogEntry();

		template <class T>
		LogEntry& operator<<(T msg)
		{
			os << msg;
			return *this;
		}

		// Do nothing on std::endl;
		LogEntry& operator<<(endl_t e)
		{
			return *this;
		}

		LogLevel getLogLevel() { return log_level; }
		std::string getString() { return os.str(); }

		void output();

	protected:
		LogEntry(const LogEntry &);
		LogEntry& operator=(const LogEntry &);

		LogLevel log_level;
		std::string module;

		std::ostringstream os;
		bool outputed;
	};


	// Empty log entry that does nothing. Used to turn off logging at compile time.
	class EmptyEntry
	{
	public:
		template <class T>
		EmptyEntry& operator<<(T msg)
		{
			return *this;
		}

		// Handle std::endl.
		EmptyEntry& operator<<(endl_t e)
		{
			return *this;
		}
	};

}

#endif // LOGGING_LOG_ENTRY_H
