/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Anders Karlsson
 */


#ifndef LOGGING_OUTPUT_H
#define LOGGING_OUTPUT_H

#include <ostream>
#include <iostream>
#include <fstream>
#include <vector>

#include "log_entry.h"


namespace OperaHbbTV
{
	typedef std::basic_ostream<char, std::char_traits<char> > cout_t;
	typedef cout_t& (*endl_t)(cout_t&);


	// Base class for log outputs.
	class LoggingOutput
	{
	public:
		LoggingOutput(LogLevel _report_level) : report_level(_report_level) {}
		virtual ~LoggingOutput() {}

		virtual void output(class LogEntry& log_entry) = 0;

	protected:
		LogLevel report_level;
	};


	// Log out put that outputs logging to stdout.
	class LoggingOutputStdout : public LoggingOutput
	{
	public:
		LoggingOutputStdout(LogLevel _report_level) : LoggingOutput(_report_level) {}
		virtual void output(class LogEntry& log_entry);
	};


	// Log out put that outputs logging to a file.
	class LoggingOutputFile : public LoggingOutput
	{
	public:
		LoggingOutputFile(LogLevel _report_level) : LoggingOutput(_report_level) {}
		~LoggingOutputFile();

		bool openFile(const char* path);
		virtual void output(class LogEntry& log_entry);

	protected:
		std::ofstream* outfile;
	};


	typedef std::vector<LoggingOutput*> LoggingOutputVector;
}

#endif // LOGGING_OUTPUT_H
