/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <arpa/inet.h>

#include "RPCUtils.h"

void nfPutWord32(uint8_t *dst, uint32_t inValue)
{
    uint32_t value = htonl(inValue);

    dst[0] = (value & 0xFF000000) >> 24;
    dst[1] = (value & 0x00FF0000) >> 16;
    dst[2] = (value & 0x0000FF00) >> 8;
    dst[3] = (value & 0x000000FF);
}

void nfGetWord32(uint8_t *src, uint32_t *outValue)
{
    uint32_t value = 0;

    value |= src[0] << 24;
    value |= src[1] << 16;
    value |= src[2] << 8;
    value |= src[3];

    *outValue = ntohl(value);
}

