/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_Dumper__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_Dumper__

#include <nrdbase/StdInt.h>
#include <sstream>

#ifndef NDEBUG
#define NDEBUG
#endif
#ifdef NDEBUG
#define PRINT(fmt, ...)
#else
#include <stdio.h>
#define PRINT(fmt, ...) printf("PARSER DEBUG::" fmt, __VA_ARGS__ )
#endif

namespace netflix {
namespace containerlib {
namespace mp4parser {

class UUID;
class MATRIX;

/*******************************************************************************
 * Dumper
 */

class Dumper
{
private:
    uint32_t level_;

private:
    void addPrefix(std::ostringstream& oss);

    template <typename DATA> void internalDump(
        const char* name, const DATA& data);

protected:
    virtual void output(std::string outputMessage);

public:
    Dumper() : level_(0) {}

    virtual ~Dumper() {}

    void enterSubLevel();
    void exitSubLevel();

    void dump(const char* name, const bool& data);
    void dump(const char* name, const std::string& data);

    void dump(const char* name, const int16_t& data);
    void dump(const char* name, const int32_t& data);
    void dump(const char* name, const long& data);
    void dump(const char* name, const long long& data);
    void dump(const char* name, const uint16_t& data);
    void dump(const char* name, const uint32_t& data);
    void dump(const char* name, const unsigned long& data);
    void dump(const char* name, const unsigned long long& data);

    void dumpBin(const char* name, uint32_t fourCC);

    void dumpFourCC(const char* name, uint32_t fourCC);

    static void dumpFourCC(uint32_t fourCC);

    void dumpUUID(const char* name, const UUID& uuid);

    void dumpMatrix(const char* name, const MATRIX& matrix);

    void message(const char* message);
};

}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_Dumper__
