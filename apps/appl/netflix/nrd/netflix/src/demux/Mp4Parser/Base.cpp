/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Base.h"

#include <string.h>
#include <limits>

#include "Context.h"

#include "Reader.h"
#include "Dumper.h"
#include "macros.h"
#include "SampleGroupEntries.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;
using netflix::containerlib::mp4parser::UUID;

/*******************************************************************************
 * Box
 */

const ullong Box::UNLIMITED_SIZE = numeric_limits<ullong>::max();
const uint32_t Box::UUID_BOX_TYPE = Box::buildFourCC("uuid");

uint32_t Box::buildFourCC(const char* fourCCString)
{
    assert(strlen(fourCCString) == 4);

    return (fourCCString[0] << 24) +
           (fourCCString[1] << 16) +
           (fourCCString[2] <<  8) + fourCCString[3];
}

UUID Box::buildUUID(uint32_t seg1,
                    uint16_t seg2,
                    uint16_t seg3,
                    uint16_t seg4,
                    ullong seg5)
{
    UUID uuid;

    uuid[0]  = seg1;
    uuid[1]  = seg1 >> 8;
    uuid[2]  = seg1 >> 16;
    uuid[3]  = seg1 >> 24;
    uuid[4]  = seg2;
    uuid[5]  = seg2 >> 8;
    uuid[6]  = seg3;
    uuid[7]  = seg3 >> 8;
    uuid[8]  = seg4 >> 8;
    uuid[9]  = seg4;
    uuid[10] = seg5 >> 40;
    uuid[11] = seg5 >> 32;
    uuid[12] = seg5 >> 24;
    uuid[13] = seg5 >> 16;
    uuid[14] = seg5 >> 8;
    uuid[15] = seg5;

    return uuid;
}

bool Box::getNextBoxInfo(Reader& reader, ullong& size, uint32_t& boxType)
{
    uint32_t smallSize;

    if(!reader.good())
        return false;

    reader.read(smallSize, 32);
    reader.read(boxType, 32);

    if(!reader.good())
        return false;

    if(smallSize == 1)
    {
        reader.read(size, 64);
    } else {
        size = smallSize;
    }

    return true;
}

bool Box::readBoxes(
    Context& context,
    const BoxReadHandlerList& handlerList,
    std::list< shared_ptr<Box> >& container,
    Reader& reader)
{
    const std::list< shared_ptr<BoxReadHandler> >& handlers =
        handlerList.handlers();

    list< shared_ptr<BoxReadHandler> >::const_iterator iter;
    uint32_t pos;

    vector<uint32_t> boxCounters(handlers.size());
    boxCounters.assign(boxCounters.size(), 0);

    while(reader.good())
    {
        //assert(context.parserState() == Context::PARSE_CONTINUE);

        if(reader.eos())
        {
            for(iter  = handlers.begin(), pos = 0;
                iter != handlers.end();
                iter ++, pos ++)
            {
                if(boxCounters[pos] == 0 &&
                    ((*iter)->restriction() == BoxReadHandler::MANDATORY ||
                     (*iter)->restriction() == BoxReadHandler::MANDATORY_UNIQUE))
                {
                    context.setParserState(Context::PARSE_ERROR_MISSING_OBJECT);
                    return false;
                }
            }

            return true;
        }

        uint32_t size;
        uint32_t boxType;
        ullong largeSize = 0;
        shared_ptr<const UUID> userType;

        ullong startOffset = reader.tell();

        reader.read(size, 32);
        reader.read(boxType, 32);

        if(!reader.good())
            break;

        uint32_t prologueBytes = 8;

        if(size == 1)
        {
            reader.read(largeSize, 64);
            prologueBytes += 8;
        }

        if(boxType == UUID_BOX_TYPE)
        {
            UUID* uuid = new UUID;
            reader.readBytes(*uuid);
            userType.reset(uuid);

            prologueBytes += 16;
        }

        if(!reader.good())
            break;

        ullong boxSize;

        if(size == 0)
            boxSize = UNLIMITED_SIZE;
        else
            boxSize = (size == 1)? largeSize : size;

        if(boxSize < prologueBytes)
        {
            context.setParserState(Context::PARSE_ERROR_INVALID_OBJECT_SIZE);
            return false;
        }

        Reader subReader(reader, boxSize - prologueBytes);

        for(iter  = handlers.begin(), pos = 0;
            iter != handlers.end();
            iter ++, pos ++)
        {
            if(!(*iter)->isInstanceOf(boxType, userType))
                continue;

            if(boxCounters[pos] > 0 &&
                ((*iter)->restriction() == BoxReadHandler::MANDATORY_UNIQUE ||
                 (*iter)->restriction() == BoxReadHandler::OPTIONAL_UNIQUE))
            {
                context.setParserState(Context::PARSE_ERROR_DUPLICATED_OBJECT);
                return false;
            }

            if(!(*iter)->read(boxType,
                              boxSize,
                              startOffset,
                              userType,
                              context,
                              subReader,
                              container))
            {
                if(context.parserState() == Context::PARSE_CONTINUE)
                    context.setParserState(Context::PARSE_ERROR_DATA);
            }

            if(context.parserState() != Context::PARSE_CONTINUE)
                return false;

            boxCounters[pos] ++;
            break;
        }

        if(iter == handlers.end())
        {
            shared_ptr<Box> unknownBox(new UnknownBox);

            unknownBox->read(boxType,
                             boxSize,
                             startOffset,
                             userType,
                             context,
                             subReader);

            container.push_back(unknownBox);
        }

        subReader.skipToEOS();
    }

    context.setParserState(Context::PARSE_ERROR_DATA);
    return false;
}

bool Box::readSampleGroupEntries(
    Context& context,
    shared_ptr<SampleGroupDescriptionEntry> sampleGroupDescriptionEntry,
    uint32_t entry,
    Reader& reader)
{
    return sampleGroupDescriptionEntry->readSpecifics(reader, context, entry);
}

void Box::dumpBoxes(
    const std::list< shared_ptr<Box> >& container,
    Dumper& dumper)
{
    dumper.enterSubLevel();

    list< shared_ptr<Box> >::const_iterator iter;
    for(iter = container.begin(); iter != container.end(); iter ++)
        (*iter)->dump(dumper);

    dumper.exitSubLevel();
}

void Box::setBoxBasicInfo(uint32_t boxType,
                          ullong boxSize,
                          ullong startOffset,
                          shared_ptr<const UUID> userType)
{
    boxType_ = boxType;
    boxSize_ = boxSize;
    startOffset_ = startOffset;
    userType_ = userType;
}

void Box::dumpBoxBasicInfo(Dumper& dumper) const
{
    dumper.message("===================");

    DUMP_FOURCC(boxType_);
    DUMP(boxSize_);
    DUMP(startOffset_);

    if(userType_.get())
        DUMP_UUID(*userType_);

    DUMP(valid_);
}

void Box::clear()
{
    valid_ = false;
    userType_.reset();

    clearSpecifics();
}

bool Box::read(uint32_t boxType,
               ullong boxSize,
               ullong startOffset,
               shared_ptr<const UUID> userType,
               Context& context,
               Reader& reader)
{
    clear();
    setBoxBasicInfo(boxType, boxSize, startOffset, userType);

    valid_ = readSpecifics(reader, context);
    return valid_;
}

void Box::dump(Dumper& dumper) const
{
    dumpBoxBasicInfo(dumper);
    dumpSpecifics(dumper);
}

/*******************************************************************************
 * FullBox
 */

bool FullBox::read(uint32_t boxType,
                   ullong boxSize,
                   ullong startOffset,
                   shared_ptr<const UUID> userType,
                   Context& context,
                   Reader& reader)
{
    clear();
    setBoxBasicInfo(boxType, boxSize, startOffset, userType);

    reader.read(version_, 8);
    reader.read(flags_, 24);

    if(!reader.good())
        return false;

    valid_ = readSpecifics(reader, context);
    return valid_;
}

void FullBox::dump(Dumper& dumper) const
{
    dumpBoxBasicInfo(dumper);

    DUMP(version_);
    DUMP_BIN(flags_);

    dumpSpecifics(dumper);
}

bool FullBox::isFlagBitSet(uint32_t bitIndex) const
{
    assert(bitIndex < 24);
    return (flags_ & (1 << bitIndex)) != 0;
}

/*******************************************************************************
 * UnknownBox
 */

bool UnknownBox::readSpecifics(Reader& /*reader*/, Context& /*context*/)
{
    return true;
}

void UnknownBox::dumpSpecifics(Dumper& dumper) const
{
    dumper.message("*** Unknown Box ***");
}

/*******************************************************************************
 * SampleEntry
 */

bool SampleEntry::read(uint32_t boxType,
                       ullong boxSize,
                       ullong startOffset,
                       shared_ptr<const UUID> userType,
                       Context& context,
                       Reader& reader)
{
    clear();
    setBoxBasicInfo(boxType, boxSize, startOffset, userType);

    reader.skipBytes(6); // const unsigned int(8)[6] reserved = 0;
    reader.read(data_reference_index_, 16);

    if(!reader.good())
        return false;

    valid_ = readSpecifics(reader, context);
    return valid_;
}

void SampleEntry::dump(Dumper& dumper) const
{
    dumpBoxBasicInfo(dumper);

    DUMP(data_reference_index_);

    dumpSpecifics(dumper);
}

/*******************************************************************************
 * BoxReadHandler
 */

bool BoxReadHandler::readAddBox(
    uint32_t boxType,
    ullong boxSize,
    ullong startOffset,
    const shared_ptr<const UUID>& userType,
    Context& context,
    Reader& reader,
    std::list< shared_ptr<Box> >& container,
    Box* newBox)
{
    shared_ptr<Box> p(newBox);

    if(!newBox->read(boxType, boxSize, startOffset, userType, context, reader))
        return false;

    container.push_back(p);
    return true;
}

/*******************************************************************************
 * BoxReadHandlerList
 */

void BoxReadHandlerList::addHandler(BoxReadHandler* handler)
{
    handlers_.push_back(shared_ptr<BoxReadHandler>(handler));
}

const std::list< shared_ptr<BoxReadHandler> >& BoxReadHandlerList::handlers() const
{
    return handlers_;
}

/*******************************************************************************
 * BaseDescriptor
 */

bool BaseDescriptor::readDescriptors(
    Context& context,
    const DescReadHandlerList& handlerList,
    std::list< shared_ptr<BaseDescriptor> >& container,
    Reader& reader)
{
    const std::list< shared_ptr<DescReadHandler> >& handlers =
        handlerList.handlers();

    list< shared_ptr<DescReadHandler> >::const_iterator iter;
    uint32_t pos;

    vector<uint32_t> descCounters(handlers.size());
    descCounters.assign(descCounters.size(), 0);

    while(reader.good())
    {
        assert(context.parserState() == Context::PARSE_CONTINUE);

        if(reader.eos())
        {
            for(iter  = handlers.begin(), pos = 0;
                iter != handlers.end();
                iter ++, pos ++)
            {
                if(descCounters[pos] == 0 &&
                    ((*iter)->restriction() == DescReadHandler::MANDATORY ||
                     (*iter)->restriction() == DescReadHandler::MANDATORY_UNIQUE))
                {
                    context.setParserState(Context::PARSE_ERROR_MISSING_OBJECT);
                    return false;
                }
            }

            return true;
        }

        uint32_t descriptorTag;
        uint32_t descriptorSize = 0;

        ullong startOffset = reader.tell();

        reader.read(descriptorTag, 8);

        uint32_t prologueBytes = 1;

        uint32_t i;
        for(i = 0; i < 4; i ++)
        {
            uint32_t hasNextSizeToken;
            uint32_t sizeToken;

            reader.read(hasNextSizeToken, 1);
            reader.read(sizeToken, 7);

            if(!reader.good())
                break;

            prologueBytes ++;

            descriptorSize <<= 7;
            descriptorSize += sizeToken;

            if(hasNextSizeToken == 0)
                break;
        }

        if(!reader.good())
            break;

        Reader subReader(reader, descriptorSize);
        descriptorSize += prologueBytes;

        for(iter  = handlers.begin(), pos = 0;
            iter != handlers.end();
            iter ++, pos ++)
        {
            if(!(*iter)->isInstanceOf(descriptorTag))
                continue;

            if(descCounters[pos] > 0 &&
                ((*iter)->restriction() == DescReadHandler::MANDATORY_UNIQUE ||
                 (*iter)->restriction() == DescReadHandler::OPTIONAL_UNIQUE))
            {
                context.setParserState(Context::PARSE_ERROR_DUPLICATED_OBJECT);
                return false;
            }

            if(!(*iter)->read(descriptorTag,
                              descriptorSize,
                              startOffset,
                              context,
                              subReader,
                              container))
            {
                if(context.parserState() == Context::PARSE_CONTINUE)
                    context.setParserState(Context::PARSE_ERROR_DATA);
            }

            if(context.parserState() != Context::PARSE_CONTINUE)
                return false;

            descCounters[pos] ++;
            break;
        }

        if(iter == handlers.end())
        {
            shared_ptr<BaseDescriptor> unknownDescriptor(new UnknownDescriptor);

            unknownDescriptor->read(descriptorTag,
                                    descriptorSize,
                                    startOffset,
                                    context,
                                    subReader);

            container.push_back(unknownDescriptor);
        }

        subReader.skipToEOS();
    }

    context.setParserState(Context::PARSE_ERROR_DATA);
    return false;
}

void BaseDescriptor::dumpDescriptors(
    const std::list< shared_ptr<BaseDescriptor> >& container,
    Dumper& dumper)
{
    dumper.enterSubLevel();

    list< shared_ptr<BaseDescriptor> >::const_iterator iter;
    for(iter = container.begin(); iter != container.end(); iter ++)
        (*iter)->dump(dumper);

    dumper.exitSubLevel();
}

void BaseDescriptor::setDescriptorBasicInfo(uint32_t descriptorTag,
                                            uint32_t descriptorSize,
                                            ullong startOffset)
{
    descriptorTag_ = descriptorTag;
    descriptorSize_ = descriptorSize;
    startOffset_ = startOffset;
}

void BaseDescriptor::dumpDescriptorBasicInfo(Dumper& dumper) const
{
    dumper.message("===================");

    DUMP(descriptorTag_);
    DUMP(descriptorSize_);
    DUMP(startOffset_);
    DUMP(valid_);
}

void BaseDescriptor::clear()
{
    valid_ = false;
    clearSpecifics();
}

bool BaseDescriptor::read(uint32_t descriptorTag,
                          uint32_t descriptorSize,
                          ullong startOffset,
                          Context& context,
                          Reader& reader)
{
    clear();
    setDescriptorBasicInfo(descriptorTag, descriptorSize, startOffset);

    valid_ = readSpecifics(reader, context);
    return valid_;
}

void BaseDescriptor::dump(Dumper& dumper) const
{
    dumpDescriptorBasicInfo(dumper);
    dumpSpecifics(dumper);
}

/*******************************************************************************
 * UnknownDescriptor
 */

bool UnknownDescriptor::readSpecifics(Reader& /*reader*/, Context& /*context*/)
{
    return true;
}

void UnknownDescriptor::dumpSpecifics(Dumper& dumper) const
{
    dumper.message("*** Unknown Descriptor ***");
}

/*******************************************************************************
 * DescReadHandler
 */

bool DescReadHandler::readAddDescriptor(
    uint32_t descriptorTag,
    uint32_t descriptorSize,
    ullong startOffset,
    Context& context,
    Reader& reader,
    std::list< shared_ptr<BaseDescriptor> >& container,
    BaseDescriptor* newDescriptor)
{
    shared_ptr<BaseDescriptor> p(newDescriptor);

    if(!newDescriptor->read(
        descriptorTag, descriptorSize, startOffset, context, reader))
    {
        return false;
    }

    container.push_back(p);
    return true;
}

/*******************************************************************************
 * DescReadHandlerList
 */

void DescReadHandlerList::addHandler(DescReadHandler* handler)
{
    handlers_.push_back(shared_ptr<DescReadHandler>(handler));
}

const std::list< shared_ptr<DescReadHandler> >& DescReadHandlerList::handlers() const
{
    return handlers_;
}


