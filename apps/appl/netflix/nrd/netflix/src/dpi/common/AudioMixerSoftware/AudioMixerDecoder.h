/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRDP_AUDIOMIXER_DECODER_H
#define NRDP_AUDIOMIXER_DECODER_H

#include <nrdbase/Endian.h>
#include <cstddef>
#include <stdint.h>

namespace netflix {
namespace device {

class AudioMixerDecoder
{
public:
    /* Free all mixer decoder resources */
    virtual ~AudioMixerDecoder() {};

    /* Read X decoded bytes from the device. */
    virtual uint32_t read(void * destinationBuffer, uint32_t bytesToRead) = 0;

    /* Move back to the start */
    virtual void reset() = 0;

    /* Information about the file */
    virtual void info(uint32_t & channels, uint32_t & samplerate, uint64_t & samples) = 0;

    /* Check if EOS*/
    virtual bool isEOS() = 0;
};

}}

#endif
