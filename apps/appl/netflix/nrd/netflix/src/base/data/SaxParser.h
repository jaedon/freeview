/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef SaxParser_h
#define SaxParser_h

#include <new>    // for std::bad_alloc
#include <string>
#include <expat.h>
#include <assert.h>
#include <nrdbase/Noncopyable.h>

namespace netflix {

// C++ RIAA wrapper for expat
class SaxParser : public Noncopyable
{
public:
    SaxParser()
        : parser_(XML_ParserCreate(0))
    {
        assert(parser_ && "Could not create XML parser");
    }
    SaxParser(char nssep)
        : parser_(XML_ParserCreateNS(0, nssep))
    {
    }
    ~SaxParser()
    {
        if (parser_)
            XML_ParserFree(parser_);
    }
    void setUserData(void* handler)
    {
        XML_SetUserData(parser_, handler);
    }
    void setElementHandler(void(*startElement)(void *userData, const char *name,
                           const char **atts),
                           void(*endElement)(void *userData, const char *name))
    {
        XML_SetElementHandler(parser_, startElement, endElement);
    }
    void setCharDataHandler(void(*charData)(void *user, const char *s, int len))
    {
        XML_SetCharacterDataHandler(parser_, charData);
    }
    void setDefaultHandler(void(*defaultData)(void *user, const char *s, int len))
    {
        XML_SetDefaultHandler(parser_, defaultData);
    }
    void setNamespaceDeclHandler(void(*startNamespace)(void *user, const char *prefix,
                                 const char *uri),
                                 void(*endNamespace)(void *user, const char *prefix))
    {
        XML_SetNamespaceDeclHandler(parser_, startNamespace, endNamespace);
    }
    bool parse(const std::string& xmlDoc)
    {
        return XML_Parse(parser_, xmlDoc.c_str(), xmlDoc.size(), 1) == XML_STATUS_OK;
    }
    bool parse(const char *xml, int size)
    {
        return XML_Parse(parser_, xml, size, 1) == XML_STATUS_OK;
    }

    void *getBuffer(int len)
    {
        return XML_GetBuffer(parser_, len);
    }
    bool parseBuffer(int len)
    {
        return XML_ParseBuffer(parser_, len, (len == 0)) == XML_STATUS_OK;
    }
    void stop()
    {
        XML_StopParser(parser_, XML_FALSE);
    }
    void reset()
    {
        XML_ParserReset(parser_, 0);
    }
    int getAttributeCount()
    {
        return XML_GetSpecifiedAttributeCount(parser_);
    }
    std::string errorString() const
    {
        const char *err = XML_ErrorString(XML_GetErrorCode(parser_));
        return (err ? std::string(err) : std::string());
    }
    int curByteOffset() const
    {
        return XML_GetCurrentByteIndex(parser_);
    }
private:
    XML_Parser parser_; // *XMLParserStruct
};
} // namespace netflix

#endif
