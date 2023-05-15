/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BENCHMARKS_H
#define BENCHMARKS_H

#include <string>
#include <vector>

#include <nrdbase/Console.h>

namespace netflix {
namespace gibbon {

class Benchmarks
{
public:
    static bool isRunning();
    static std::vector<Console::Command::Help> help();
    static void run(const std::string &name, const std::vector<std::string> &arguments=std::vector<std::string>());
};

}} // netflix::gibbon

#endif // BENCHMARKS_H
