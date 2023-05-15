/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "HttpResponseHeaders.h"

using namespace netflix;

NRDP_DEFINE_OBJECTCOUNT_RECORD(HttpResponseHeadersDataCount, HttpResponseHeadersData);

std::string HttpResponseHeadersDataCount::describe(void *object) const
{
    HttpResponseHeaders headers;
    assert(object);
    return '\n' + static_cast<HttpResponseHeaders::Data*>(object)->toString();
}

static std::vector<HttpResponseHeaders::Header> sNull;

std::vector<HttpResponseHeaders::Header> &HttpResponseHeaders::null()
{
    return sNull;
}

