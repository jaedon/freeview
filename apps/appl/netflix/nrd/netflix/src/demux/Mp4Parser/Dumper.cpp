/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Dumper.h"
#include "Base.h"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace netflix::containerlib::mp4parser;
using netflix::containerlib::mp4parser::UUID;

/*******************************************************************************
 * Dumper
 */

void Dumper::addPrefix(std::ostringstream& oss)
{
    uint32_t i;
    for(i = 0; i < level_; i ++)
        oss << "    ";
}

template <typename DATA> void Dumper::internalDump(
    const char* name, const DATA& data)
{
    std::ostringstream oss;
    addPrefix(oss);
    oss << name << ": " << data;
    output(oss.str());
}

void Dumper::output(std::string /*outputMessage*/)
{
}

void Dumper::enterSubLevel()
{
    level_ ++;
}

void Dumper::exitSubLevel()
{
    if(level_ > 0)
        level_ --;
}

void Dumper::dump(const char* name, const bool& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const std::string& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const int16_t& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const int32_t& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const long& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const long long& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const uint16_t& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const uint32_t& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const unsigned long& data)
{
    internalDump(name, data);
}

void Dumper::dump(const char* name, const unsigned long long& data)
{
    internalDump(name, data);
}

void Dumper::dumpBin(const char* name, uint32_t fourCC)
{
    ostringstream oss;
    addPrefix(oss);
    oss << name << ": '";

    uint32_t i;
    for(i = 0; i < 32; i ++)
    {
        if(i != 0 && (i & 0x7) == 0)
            oss << " ";

        if(((fourCC << i) & 0x80000000) != 0)
            oss << "1";
        else
            oss << "0";
    }

    oss << "'b";
    output(oss.str());
}

void Dumper::dumpFourCC(const char* name, uint32_t fourCC)
{
    ostringstream oss;
    addPrefix(oss);
    oss << name << ": '";
    oss << static_cast<char>(fourCC >> 24);
    oss << static_cast<char>((fourCC >> 16) & 0xFF);
    oss << static_cast<char>((fourCC >> 8) & 0xFF);
    oss << static_cast<char>(fourCC & 0xFF);
    oss << "'";
    output(oss.str());
}

void Dumper::dumpUUID(const char* name, const UUID& uuid)
{
    ostringstream oss;

    oss.setf(ios::hex, ios::basefield);
    oss.setf(ios::uppercase);
    oss.fill('0');

    addPrefix(oss);
    oss << name << ": '";

    int32_t i;

    for(i = 3; i >= 0; i --)
        oss << setw(2) << static_cast<uint32_t>(uuid[i]);

    oss << "-";

    for(i = 5; i >= 4; i --)
        oss << setw(2) << static_cast<uint32_t>(uuid[i]);

    oss << "-";

    for(i = 7; i >= 6; i --)
        oss << setw(2) << static_cast<uint32_t>(uuid[i]);

    oss << "-";

    for(i = 8; i <= 9; i ++)
        oss << setw(2) << static_cast<uint32_t>(uuid[i]);

    oss << "-";

    for(i = 10; i <= 15; i ++)
        oss << setw(2) << static_cast<uint32_t>(uuid[i]);

    oss << "'";
    output(oss.str());
}

void Dumper::dumpMatrix(const char* name, const MATRIX& matrix)
{
    assert(matrix.size() == 36); // 9 32-bits integers

    ostringstream oss;
    oss.setf(ios::hex, ios::basefield);

    addPrefix(oss);
    oss << name << ": { ";

    uint32_t wordPos;
    for(wordPos = 0; wordPos < 9; wordPos ++)
    {
        uint32_t value = 0;
        uint32_t bytePosInWord;

        for(bytePosInWord = 0; bytePosInWord < 4; bytePosInWord ++)
            value = (value << 8) | matrix[wordPos * 4 + bytePosInWord];

        oss << "0x" << value << ",";
    }

    oss << "}";
    output(oss.str());
}

void Dumper::message(const char* message)
{
    ostringstream oss;
    addPrefix(oss);
    oss << message;
    output(oss.str());
}


void Dumper::dumpFourCC(uint32_t fourCC)
{
#ifndef NDEBUG
    ostringstream oss;
    oss << "'";
    oss << static_cast<char>(fourCC >> 24);
    oss << static_cast<char>((fourCC >> 16) & 0xFF);
    oss << static_cast<char>((fourCC >> 8) & 0xFF);
    oss << static_cast<char>(fourCC & 0xFF);
    oss << "'";
    cout << oss.str() << endl;
#endif
    (void)fourCC;
}
