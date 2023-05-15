/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_MP4Descriptors__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_MP4Descriptors__

#include "Base.h"

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * ES_Descriptor
 */

class ES_Descriptor : public BaseDescriptor
{
public:
    static const uint32_t ExpectedDescriptorTag_;

private:
    uint16_t ES_ID_;
    uint16_t streamDependenceFlag_;
    uint16_t URL_Flag_;
    uint16_t OCRstreamFlag_;
    uint16_t streamPriority_;
    uint16_t dependsOn_ES_ID_;
    uint16_t URLlength_;
    uint16_t OCR_ES_Id_;

    std::list< shared_ptr<BaseDescriptor> > childDescriptors_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * DecoderConfigDescriptor
 */

class DecoderConfigDescriptor : public BaseDescriptor
{
public:
    static const uint32_t ExpectedDescriptorTag_;

private:
    uint16_t objectTypeIndication_;
    uint16_t streamType_;
    uint16_t upStream_;
    uint32_t bufferSizeDB_;
    uint32_t maxBitrate_;
    uint32_t avgBitrate_;

    std::list< shared_ptr<BaseDescriptor> > childDescriptors_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * DecoderSpecificInfo
 */

class DecoderSpecificInfo : public BaseDescriptor
{
public:
    static const uint32_t ExpectedDescriptorTag_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const {}
};

}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_MP4Descriptors__
