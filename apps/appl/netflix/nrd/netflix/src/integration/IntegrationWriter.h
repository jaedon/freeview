/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IntegrationWriter_h
#define IntegrationWriter_h

#include <nrd/NrdWriteLimiter.h>
#include <nrdbase/Variant.h>

#include <stdint.h>
#include <vector>
#include <string>

namespace netflix {
struct Splash;
struct TileGroup;
class IntegrationWriter
{
public:
    enum ErrorCode {
#define DEFINE_ERROR_CODE(code) code,
#include <nrd/IntegrationWriterErrorCodesInternal.h>
        Count
    };
#undef DEFINE_ERROR_CODE

    IntegrationWriter(const std::string &directory, const std::vector<unsigned char> &secret);
    virtual ~IntegrationWriter() {}
    bool isValid() const;
    uint32_t version() const;

    bool write(const std::vector<Splash> &splashes, ErrorCode *code = 0, std::string *error = 0);
    bool write(const std::vector<TileGroup> &groups, ErrorCode *code = 0, std::string *error = 0);
    bool clearTiles(ErrorCode *code = 0, std::string *error = 0);
    bool clearSplashes(ErrorCode *code = 0, std::string *error = 0);
    bool validateSplashes(ErrorCode *code, std::string *error) const { return validateFile(mSplashFileName, code, error); }
    bool validateTiles(ErrorCode *code, std::string *error) const { return validateFile(mTileFileName, code, error); }
private:
    bool write(const std::string &filename, const std::vector<unsigned char> &data, ErrorCode *code, std::string *error);
    bool validateFile(const std::string &file, ErrorCode *code, std::string *error) const;

    std::string mDirectory;
    std::string mTileFileName, mSplashFileName;
    const std::vector<unsigned char> mSecret;
};
};

#endif
