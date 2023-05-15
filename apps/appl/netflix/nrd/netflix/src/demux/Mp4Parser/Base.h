/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_Base__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_Base__

#include <nrdbase/StdInt.h>
#include <cassert>
#include <vector>
#include <list>

#include <nrdbase/tr1.h>

namespace netflix {
namespace containerlib {
namespace mp4parser {

class Context;
class Reader;
class Dumper;
class BoxReadHandlerList;
class DescReadHandlerList;
class SampleGroupDescriptionEntry;

/*******************************************************************************
 * UUID
 */

class UUID : public std::vector<uint8_t>
{
public:
    UUID() : std::vector<uint8_t>(16) {}
};

/*******************************************************************************
 * MATRIX
 */

class MATRIX : public std::vector<uint8_t>
{
public:
    MATRIX() : std::vector<uint8_t>(36) {}
};

/*******************************************************************************
 * Box
 */

class Box
{
public:
    static const ullong UNLIMITED_SIZE;
    static const uint32_t UUID_BOX_TYPE;

public:
    static uint32_t buildFourCC(const char* fourCCString);

    static UUID buildUUID(uint32_t seg1,
                          uint16_t seg2,
                          uint16_t seg3,
                          uint16_t seg4,
                          ullong seg5);

    static bool readBoxes(
        Context& context,
        const BoxReadHandlerList& handlerList,
        std::list< shared_ptr<Box> >& container,
        Reader& reader);

    // SampleGroupDescriptionEntry is not a Box. It should be read with below
    // readSampleGroupEntries, instead of readBoxes
    static bool readSampleGroupEntries(
        Context& context,
        shared_ptr<SampleGroupDescriptionEntry> sampleGroupDescriptionEntry,
        uint32_t entry,
        Reader& reader);

    static bool getNextBoxInfo(Reader& reader, ullong& size, uint32_t& boxType);

    static void dumpBoxes(
        const std::list< shared_ptr<Box> >& container,
        Dumper& dumper);

protected:
    uint32_t boxType_;
    ullong boxSize_;
    ullong startOffset_;
    shared_ptr<const UUID> userType_;
    bool valid_;

protected:
    // The protected constructor is intentional since this class is abstract.
    Box() : valid_(false) {}

    virtual void clearSpecifics() = 0;
    virtual bool readSpecifics(Reader& reader, Context& context) = 0;
    virtual void dumpSpecifics(Dumper& dumper) const = 0;

    void setBoxBasicInfo(uint32_t boxType,
                         ullong boxSize,
                         ullong startOffset,
                         shared_ptr<const UUID> userType);

    void dumpBoxBasicInfo(Dumper& dumper) const;

public:
    virtual ~Box() {}

    virtual void clear();

    virtual bool read(uint32_t boxType,
                      ullong boxSize,
                      ullong startOffset,
                      shared_ptr<const UUID> userType,
                      Context& context,
                      Reader& reader);

    virtual void dump(Dumper& dumper) const;

    inline bool valid() { return valid_; }
};

/*******************************************************************************
 * FullBox
 */

class FullBox : public Box
{
protected:
    uint32_t version_;
    uint32_t flags_; // 24 bits

public:
    virtual bool read(uint32_t boxType,
                      ullong boxSize,
                      ullong startOffset,
                      shared_ptr<const UUID> userType,
                      Context& context,
                      Reader& reader);

    virtual void dump(Dumper& dumper) const;

protected:
    bool isFlagBitSet(uint32_t bitIndex) const;
};

/*******************************************************************************
 * UnknownBox
 */

class UnknownBox : public Box
{
protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SampleEntry
 */

class SampleEntry : public Box
{
protected:
    uint16_t data_reference_index_;

public:
    virtual bool read(uint32_t boxType,
                      ullong boxSize,
                      ullong startOffset,
                      shared_ptr<const UUID> userType,
                      Context& context,
                      Reader& reader);

    virtual void dump(Dumper& dumper) const;
};

/*******************************************************************************
 * BoxReadHandler
 */

class BoxReadHandler
{
public:
    enum Restriction
    {
        MANDATORY,
        MANDATORY_UNIQUE,
        OPTIONAL,
        OPTIONAL_UNIQUE
    };

private:
    const Restriction restriction_;

public:
    BoxReadHandler(Restriction restriction) : restriction_(restriction) {}

    virtual ~BoxReadHandler() {}

    inline Restriction restriction() { return restriction_; }

    virtual bool isInstanceOf(uint32_t boxType,
                              const shared_ptr<const UUID>& userType) = 0;

    virtual bool read(uint32_t boxType,
                      ullong boxSize,
                      ullong startOffset,
                      const shared_ptr<const UUID>& userType,
                      Context& context,
                      Reader& reader,
                      std::list< shared_ptr<Box> >& container) = 0;

protected:
    bool readAddBox(uint32_t boxType,
                    ullong boxSize,
                    ullong startOffset,
                    const shared_ptr<const UUID>& userType,
                    Context& context,
                    Reader& reader,
                    std::list< shared_ptr<Box> >& container,
                    Box* newBox);
};

/*******************************************************************************
 * StandardBoxReadHandler
 */

template <typename T>
class StandardBoxReadHandler : public BoxReadHandler
{
public:
    StandardBoxReadHandler(Restriction restriction)
        : BoxReadHandler(restriction) {}

    virtual bool isInstanceOf(uint32_t boxType,
                              const shared_ptr<const UUID>& /*userType*/)
    {
        return (boxType == T::ExpectedBoxType_);
    }

    virtual bool read(uint32_t boxType,
                      ullong boxSize,
                      ullong startOffset,
                      const shared_ptr<const UUID>& userType,
                      Context& context,
                      Reader& reader,
                      std::list< shared_ptr<Box> >& container)
    {
        return readAddBox(boxType,
                          boxSize,
                          startOffset,
                          userType,
                          context,
                          reader,
                          container,
                          new T);
    }
};

/*******************************************************************************
 * ExtendedBoxReadHandler
 */

template <typename T>
class ExtendedBoxReadHandler : public BoxReadHandler
{
public:
    ExtendedBoxReadHandler(Restriction restriction)
        : BoxReadHandler(restriction) {}

    virtual bool isInstanceOf(uint32_t boxType,
                              const shared_ptr<const UUID>& userType)
    {
        if(boxType != Box::UUID_BOX_TYPE)
            return false;

        return (*userType == T::ExpectedBoxUserType_);
    }

    virtual bool read(uint32_t boxType,
                      ullong boxSize,
                      ullong startOffset,
                      const shared_ptr<const UUID>& userType,
                      Context& context,
                      Reader& reader,
                      std::list< shared_ptr<Box> >& container)
    {
        return readAddBox(boxType,
                          boxSize,
                          startOffset,
                          userType,
                          context,
                          reader,
                          container,
                          new T);
    }
};

/*******************************************************************************
 * DualBoxReadHandler
 */

template <typename T>
class DualBoxReadHandler : public BoxReadHandler
{
public:
    DualBoxReadHandler(Restriction restriction)
        : BoxReadHandler(restriction) {}

    virtual bool isInstanceOf(uint32_t boxType,
                              const shared_ptr<const UUID>& userType)
    {
        if(boxType == T::ExpectedBoxType_)
            return true;

        if(boxType != Box::UUID_BOX_TYPE)
            return false;

        return (*userType == T::ExpectedBoxUserType_);
    }

    virtual bool read(uint32_t boxType,
                      ullong boxSize,
                      ullong startOffset,
                      const shared_ptr<const UUID>& userType,
                      Context& context,
                      Reader& reader,
                      std::list< shared_ptr<Box> >& container)
    {
        return readAddBox(boxType,
                          boxSize,
                          startOffset,
                          userType,
                          context,
                          reader,
                          container,
                          new T);
    }
};

/*******************************************************************************
 * SampleEntryReadHandler
 */

template <typename T>
class SampleEntryReadHandler : public BoxReadHandler
{
public:
    SampleEntryReadHandler(Restriction restriction)
        : BoxReadHandler(restriction) {}

    virtual bool isInstanceOf(uint32_t /*boxType*/,
                              const shared_ptr<const UUID>& /*userType*/)
    {
        return true;
    }

    virtual bool read(uint32_t boxType,
                      ullong boxSize,
                      ullong startOffset,
                      const shared_ptr<const UUID>& userType,
                      Context& context,
                      Reader& reader,
                      std::list< shared_ptr<Box> >& container)
    {
        return readAddBox(boxType,
                          boxSize,
                          startOffset,
                          userType,
                          context,
                          reader,
                          container,
                          new T);
    }
};

/*******************************************************************************
 * BoxReadHandlerList
 */

class BoxReadHandlerList
{
private:
    std::list< shared_ptr<BoxReadHandler> > handlers_;

public:
    void addHandler(BoxReadHandler* handler);
    const std::list< shared_ptr<BoxReadHandler> >& handlers() const;
};

/*******************************************************************************
 * BaseDescriptor
 */

class BaseDescriptor
{
public:
    static bool readDescriptors(
        Context& context,
        const DescReadHandlerList& handlerList,
        std::list< shared_ptr<BaseDescriptor> >& container,
        Reader& reader);

    static void dumpDescriptors(
        const std::list< shared_ptr<BaseDescriptor> >& container,
        Dumper& dumper);

protected:
    uint32_t descriptorTag_;
    uint32_t descriptorSize_;
    ullong startOffset_;
    bool valid_;

protected:
    // The protected constructor is intentional since this class is abstract.
    BaseDescriptor() : valid_(false) {}

    virtual void clearSpecifics() = 0;
    virtual bool readSpecifics(Reader& reader, Context& context) = 0;
    virtual void dumpSpecifics(Dumper& dumper) const = 0;

    void setDescriptorBasicInfo(uint32_t descriptorTag,
                                uint32_t descriptorSize,
                                ullong startOffset);

    void dumpDescriptorBasicInfo(Dumper& dumper) const;

public:
    virtual ~BaseDescriptor() {}

    virtual void clear();

    virtual bool read(uint32_t descriptorTag,
                      uint32_t descriptorSize,
                      ullong startOffset,
                      Context& context,
                      Reader& reader);

    virtual void dump(Dumper& dumper) const;

    inline bool valid() { return valid_; }
};

/*******************************************************************************
 * UnknownDescriptor
 */

class UnknownDescriptor : public BaseDescriptor
{
protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * DescReadHandler
 */

class DescReadHandler
{
public:
    enum Restriction
    {
        MANDATORY,
        MANDATORY_UNIQUE,
        OPTIONAL,
        OPTIONAL_UNIQUE
    };

private:
    const Restriction restriction_;

public:
    DescReadHandler(Restriction restriction) : restriction_(restriction) {}

    virtual ~DescReadHandler() {}

    inline Restriction restriction() { return restriction_; }

    virtual bool isInstanceOf(uint32_t descriptorTag) = 0;

    virtual bool read(
            uint32_t descriptorTag,
            uint32_t descriptorSize,
            ullong startOffset,
            Context& context,
            Reader& reader,
            std::list< shared_ptr<BaseDescriptor> >& container) = 0;

protected:
    bool readAddDescriptor(
            uint32_t descriptorTag,
            uint32_t descriptorSize,
            ullong startOffset,
            Context& context,
            Reader& reader,
            std::list< shared_ptr<BaseDescriptor> >& container,
            BaseDescriptor* newDescriptor);
};

/*******************************************************************************
 * StandardDescReadHandler
 */

template <typename T>
class StandardDescReadHandler : public DescReadHandler
{
public:
    StandardDescReadHandler(Restriction restriction)
        : DescReadHandler(restriction) {}

    virtual bool isInstanceOf(uint32_t descriptorTag)
    {
        return (descriptorTag == T::ExpectedDescriptorTag_);
    }

    virtual bool read(
            uint32_t descriptorTag,
            uint32_t descriptorSize,
            ullong startOffset,
            Context& context,
            Reader& reader,
            std::list< shared_ptr<BaseDescriptor> >& container)
    {
        return readAddDescriptor(descriptorTag,
                                 descriptorSize,
                                 startOffset,
                                 context,
                                 reader,
                                 container,
                                 new T);
    }
};

/*******************************************************************************
 * DescReadHandlerList
 */

class DescReadHandlerList
{
private:
    std::list< shared_ptr<DescReadHandler> > handlers_;

public:
    void addHandler(DescReadHandler* handler);
    const std::list< shared_ptr<DescReadHandler> >& handlers() const;
};

}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_Base__
