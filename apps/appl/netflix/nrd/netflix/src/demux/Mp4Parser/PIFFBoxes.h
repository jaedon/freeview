/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_PIFFBoxes__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_PIFFBoxes__

#include "Base.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * ProtectionSystemSpecificHeaderBox
 */

class ProtectionSystemSpecificHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;
    static const UUID ExpectedBoxUserType_;

private:
    UUID systemID_;
    uint32_t dataSize_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackEncryptionBox
 */

class TrackEncryptionBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;
    static const UUID ExpectedBoxUserType_;

private:
    uint32_t default_AlgorithmID_;
    uint32_t default_IV_size_;
    UUID default_KID_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SampleEncryptionBox
 */

class SampleEncryptionBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;
    static const UUID ExpectedBoxUserType_;

private:
    static const uint32_t overrideTrackEncryptionBoxParameters_ = 0;
    static const uint32_t useSubSampleEncryption_ = 1;

private:
    uint32_t AlgorithmID_;
    uint32_t IV_size_;
    UUID KID_;
    uint32_t sample_count_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_PIFFBoxes__
