/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 * @author Anders Karlsson
 */


#ifndef LOGGING_H
#define LOGGING_H

#include <ostream>
#include <iostream>
#include <fstream>
#include <vector>

#include "logging_output.h"
#include "log_levels.h"


// Main use of this logging class should be through the DEBUG, LOG, WARN and ERR macros below.
// Those macros take a module name as input that will be reflected in the log message.
//
// Using any of those macros results in a local LogEntry object that can take input stream data.
// When the LogEntry object goes out of scope it will output the data.
//
// It is not necessary to add std::endl at end of stream. Nothing bad will happen if that is
// made though.
//
// Typical use can be:
//
//   ERR("TCPServer") << "failed opening socket";
//   LOG("TCPServer") << "got connection from: " << inet_ntoa(client_address.sin_addr) << " : " << new_socket_fd;


namespace OperaHbbTV
{
	// Logging main class.
	class Logging
	{
	public:
		Logging();
		static Logging& getInstance();

		// Add a LoggingOutput object to output logs to.
		void addOutput(LoggingOutput* output) { outputs.push_back(output); }

		// Output a finished LogEntry.
		void output(LogEntry& entry);

		LoggingOutputVector outputs;
		static Logging* instance;

	private:
		~Logging();
		Logging(const Logging &);
		Logging& operator=(const Logging &);

		pthread_mutex_t output_mutex;
	};

#if 0
#ifdef LOGGING_ENABLED
	#define DEBUG(module) OperaHbbTV::LogEntry(OperaHbbTV::LOG_LEVEL_DEBUG, module)
	#define LOG(module) OperaHbbTV::LogEntry(OperaHbbTV::LOG_LEVEL_LOG, module)
	#define WARN(module) OperaHbbTV::LogEntry(OperaHbbTV::LOG_LEVEL_WARN, module)
	#define ERR(module) OperaHbbTV::LogEntry(OperaHbbTV::LOG_LEVEL_ERR, module)
#else
	#define DEBUG(module) OperaHbbTV::EmptyEntry()
	#define LOG(module) OperaHbbTV::EmptyEntry()
	#define WARN(module) OperaHbbTV::EmptyEntry()
	#define ERR(module) OperaHbbTV::EmptyEntry()
#endif
#endif
}
#endif // LOGGING_H


