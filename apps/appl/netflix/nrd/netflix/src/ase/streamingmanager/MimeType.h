/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MIME_TYPE_H
#define MIME_TYPE_H

#include <string>
#include <map>

#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace ase
{

class MimeType
{
public:
    enum Type
    {
        MIME_TYPE_UNKNOWN = 0,
        MIME_TYPE_APPLICATION,
        MIME_TYPE_AUDIO,
        MIME_TYPE_VIDEO,
        MIME_TYPE_TEXT,
        MIME_TYPE_IMAGE
    };

public:
    MimeType();

    MimeType( std::string const& mime );

    MimeType& operator=( std::string const& mime );

    bool valid() const;

    Type getType() const;

    MediaType getMediaType() const;

    std::string getSubtype() const;

    bool getParameter( std::string const& name, std::string& value ) const;

    bool contains( MimeType const& other );

protected:
    bool parse( std::string const& mime );

    typedef std::map<std::string,Type> MimeTypesMap;

    static MimeTypesMap* initializeTypesMap();

protected:
    static std::auto_ptr<const MimeTypesMap>  sMimeTypesMapPtr;

protected:
    typedef std::map<std::string,std::string> ParameterMap;

    bool            mValid;

    Type            mType;

    std::string     mSubtype;

    ParameterMap    mParameters;
};

inline MimeType::MimeType()
    : mValid( false )
{
}

inline MimeType::MimeType( std::string const& mime )
    : mValid( false )
{
    mValid = parse( mime );
}

inline MimeType& MimeType::operator=( std::string const& mime )
{
    mValid = parse( mime );
    return *this;
}

inline bool MimeType::valid() const
{
    return mValid;
}

inline MimeType::Type MimeType::getType() const
{
    return mType;
}

inline std::string MimeType::getSubtype() const
{
    return mSubtype;
}

}}   // namespace ase

#endif
