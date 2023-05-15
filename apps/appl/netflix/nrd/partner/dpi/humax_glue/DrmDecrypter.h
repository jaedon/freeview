/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef DRMDECRYPTER_H
#define DRMDECRYPTER_H

/** @file DrmDecrypter.h - Provides PlayReady decryption functionality as a
 * wrapper around calls to the PlayReady library.  ESPlayerNative, the reference
 * implementation of elementary stream player, creates a DrmDecrypter instance
 * to handle decryption of media samples. The file does not implement a DPI
 * interface.
 */

#include <string>
#include <vector>

#if defined(PLAYREADY)
#include <drmmanager.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

//#include <drmstrings.h>
/* Unset conflicting SAL tags. */
#undef __in
#undef __out
#endif

#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>
#include <nrd/IElementaryStreamPlayer.h>
#include <nrdbase/Mutex.h>
#include "DrmError.h"

namespace netflix {
namespace device {
namespace esplayer {

class DrmSession;
class PlaybackGroupNative;

class DrmDecrypter
{
public:

    DrmDecrypter(PlaybackGroupNative *playbackGroup);
    ~DrmDecrypter();

    /**
     * Decrypts the provided data using the license stored for this
     * session. The first time this is called, if a valid license is
     * found, the minimum output protection level values are set.
     *
     * This method should only be called after
     * storeLicense(std::vector<char>&).
     *
     * @param[in] an identifier of the encryption algorithm used to encrypt the
     *         track. 0: not encrypted, 1: AES 128-bit in CTRmode.
     * @param[in] IVData the initialization vector data for the decryption process.
     * @param[in] IVDataSize the size of initialization vector data in bytes.
     * @param[in|out] dataSegments contain the data segments that will be decrypted
     *                in place.
     * @param[in] dataSegmentOffset specifies the number of bytes that were encrypted
     *                 before the first byte in the dataSegment.
     *
     * @return NFErr_OK if decryption was successful or not necessary,
     *         NFErr_DRMFailed if the license could not be found or
     *         decryption failed.
     */
    struct DataSegment
    {
        unsigned char* data;
        uint32_t numBytes;
    };
    NFErr  decrypt(unsigned char* IVData,
                   uint32_t IVDataSize,
                   uint64_t dataSegmentOffset,
                   DataSegment& dataSegment);


    NFErr initDecryptContext();

    void cleanupDecryptContext();

    /**
     * Returns the compressed digital video minimum output protection
     * level. Zero until decrypt(std::vector<char>&) is called.
     *
     * @return compressed digital video protection level.
     */
    uint16_t compressedDigitalVideoLevel() const {
        return levels_.compressedDigitalVideoLevel_;
    }

    /**
     * Returns the uncompressed digital video minimum output
     * protection level. Zero until decrypt(std::vector<char>&) is
     * called.
     *
     * @return uncompressed digital video protection level.
     */
    uint16_t uncompressedDigitalVideoLevel() const {
        return levels_.uncompressedDigitalVideoLevel_;
    }

    /**
     * Returns the analog video minimum output protection level. Zero
     * until decrypt(std::vector<char>&) is called.
     *
     * @return analog video protection level.
     */
    uint16_t analogVideoLevel() const {
        return levels_.analogVideoLevel_;
    }

    /**
     * Returns the compressed digital audio minimum output protection
     * level. Zero until decrypt(std::vector<char>&) is called.
     *
     * @return compressed digital audio protection level.
     */
    uint16_t compressedDigitalAudioLevel() const {
        return levels_.compressedDigitalAudioLevel_;
    }

    /**
     * Returns the uncompressed digital audio minimum output
     * protection level. Zero until decrypt(std::vector<char>&) is
     * called.
     *
     * @return uncompressed digital audio protection level.
     */
    uint16_t uncompressedDigitalAudioLevel() const {
        return levels_.uncompressedDigitalAudioLevel_;
    }

    /** Playback protection levels struct. */
    struct PlayLevels {
        uint16_t compressedDigitalVideoLevel_;   //!< Compressed digital video output protection level.
        uint16_t uncompressedDigitalVideoLevel_; //!< Uncompressed digital video output protection level.
        uint16_t analogVideoLevel_;              //!< Analog video output protection level.
        uint16_t compressedDigitalAudioLevel_;   //!< Compressed digital audio output protection level.
        uint16_t uncompressedDigitalAudioLevel_; //!< Uncompressed digital audio output protection level.
    };

private:
#if defined(PLAYREADY)
    std::auto_ptr<DRM_DECRYPT_CONTEXT> decryptContext_;
#endif
    struct PlayLevels levels_;                   //!< Play protection levels
};

} // namespace esplayer
} // device
} // namespace netflix

#endif

