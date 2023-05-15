#ifndef MDX_UTIL_H
#define MDX_UTIL_H

/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <ithread.h>
#include <ixml.h> /* for IXML_Document, IXML_Element */
#include <upnp.h> /* for Upnp_EventType */
#include <upnptools.h>

#include <nrdbase/Mutex.h>
#include <nrdbase/Variant.h>

#include <expat.h>
#include <stdlib.h>
#include <string.h>

#define MDX_SUCCESS        0
#define MDX_ERROR        (-1)
#define MDX_WARNING        1

namespace netflix {
namespace mdx {

class HttpHeadersToVariantMap
{
public:
    HttpHeadersToVariantMap();
    virtual ~HttpHeadersToVariantMap();

    virtual Variant format(const std::string &headers);
};

class XmlToVariantMap
{
public:
    XmlToVariantMap();
    virtual ~XmlToVariantMap();

    virtual Variant format(const std::string &xml);

private:
    void reset();
    void static startElementHandler(void *userData, const char *name, const char **attributes);
    void static endElementHandler(void *userData, const char *name);
    void static charDataHandler(void *user, const char *s, int len);

private:
    enum { MaxAllocated = 1024 * 1024 };
    Variant mVariantMap;
    std::vector<Variant> mMapInProgress;
    XML_Parser mParser;
    int mAllocated;
};

}} // netflix::mdx

#endif /* MDX_UTIL_H */

