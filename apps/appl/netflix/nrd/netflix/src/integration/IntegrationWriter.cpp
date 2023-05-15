/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "IntegrationWriter.h"
#include <integration/Splash.h>
#include <integration/Security.h>
#include <integration/Buffer.h>
#include <integration/Tile.h>
#include <nrdbase/Configuration.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <sstream>

using namespace netflix;

static bool encode(const std::vector<unsigned char> &secret,
                   const std::vector<unsigned char> &data,
                   std::vector<unsigned char> &cipherEnvelope,
                   std::vector<unsigned char> &macEnvelope,
                   IntegrationWriter::ErrorCode *code,
                   std::string *error)
{
    if (code)
        *code = IntegrationWriter::Success;

    // There must be data to be protected.
    if (data.empty()) {
        if (code)
            *code = IntegrationWriter::EmptyData;
        if (error)
            *error = "data empty";
        return false;
    }

    // Derive the key material
    std::vector<unsigned char> cipherKey;
    std::vector<unsigned char> macKey;
    if (!netflix::deriveKeyMaterial(secret, cipherKey, macKey)) {
        if (code)
            *code = IntegrationWriter::InvalidSecret;
        if (error)
            *error = "Couldn't derive key material";
        return false;
    }

    // Prepare the cipher and mac envelope version.
    const unsigned char tmp[netflix::EnvelopeVersionSizeInBytes] = { 0x0, 0x0, 0x0, 0x1 };
    std::vector<unsigned char> cipherEnvelopeVersion(tmp, tmp + sizeof(tmp) / sizeof(unsigned char));
    std::vector<unsigned char> macEnvelopeVersion(tmp, tmp + sizeof(tmp) / sizeof(unsigned char));

    // Prepare a random IV.
    std::vector<unsigned char> iv(netflix::CipherBlockSizeInBytes, 0);
    RAND_bytes(&iv[0], iv.size());
    std::vector<unsigned char> tmpIv(iv);

    // Prepare the space for storing the cipher text
    size_t paddingLen = netflix::CipherBlockSizeInBytes - (data.size() % netflix::CipherBlockSizeInBytes);
    size_t cipherTextLen = data.size() + paddingLen;
    std::vector<unsigned char> cipherText(cipherTextLen, 0);
    int lenFromUpdate = 0;
    int lenFromFinal = 0;

    // Encrypt the data.
    EVP_CIPHER_CTX cipherCtx;
    EVP_CIPHER_CTX_init(&cipherCtx);
    EVP_EncryptInit_ex(&cipherCtx, EVP_aes_128_cbc(), NULL, &cipherKey[0], &tmpIv[0]);
    EVP_EncryptUpdate(&cipherCtx, &cipherText[0], &lenFromUpdate, &data[0], data.size());
    EVP_EncryptFinal_ex(&cipherCtx, &cipherText[lenFromUpdate], &lenFromFinal);
    EVP_CIPHER_CTX_cleanup(&cipherCtx);

    // Computed ciphertext length is same as the length of the pre-computed
    // ciphertext length, otherwise encryption failed.
    if (lenFromUpdate + lenFromFinal != static_cast<int>(cipherTextLen)) {
        if (code)
            *code = IntegrationWriter::EncryptionFailure;
        if (error)
            *error = "Encryption failed";
        return false;
    }

    // Prepare the cipher envelope:
    // -  4 byte version
    // - 16 byte iv
    // -  N byte ciphertext
    cipherEnvelope.clear();
    cipherEnvelope.insert(cipherEnvelope.end(), cipherEnvelopeVersion.begin(), cipherEnvelopeVersion.end());
    cipherEnvelope.insert(cipherEnvelope.end(), iv.begin(), iv.end());
    cipherEnvelope.insert(cipherEnvelope.end(), cipherText.begin(), cipherText.end());

    // Compute the HMAC-SHA-256 value over the cipher envelope.
    std::vector<unsigned char> hmac;
    if (!netflix::computeHmacSha256Value(macKey, cipherEnvelope, hmac)) {
        if (code)
            *code = IntegrationWriter::HmacError;
        if (error)
            *error = "HMAC failed";
        return false;
    }

    // Prepare the mac envelope:
    // - 4 byte version
    // - 32 byte HMAC value
    macEnvelope.clear();
    macEnvelope.insert(macEnvelope.end(), macEnvelopeVersion.begin(), macEnvelopeVersion.end());
    macEnvelope.insert(macEnvelope.end(), hmac.begin(), hmac.end());

    return true;
}

IntegrationWriter::IntegrationWriter(const std::string &directory,
                                     const std::vector<unsigned char> &secret)
    : mDirectory(directory), mSecret(secret)
{
    if (!mDirectory.empty() && mDirectory[mDirectory.size() - 1] != '/')
        mDirectory += '/';
    mTileFileName = mDirectory + tileCatalogFileName();
    mSplashFileName = mDirectory + splashCatalogFileName();
}

template <typename T>
static inline void write(std::vector<unsigned char> &data, const T *bytes, int count = 1)
{
    const unsigned char *start = reinterpret_cast<const unsigned char*>(bytes);
    const unsigned char *end = reinterpret_cast<const unsigned char*>(bytes + count);
    data.insert(data.end(), start, end);
}

static inline void writeData(std::vector<unsigned char> &data, const std::vector<unsigned char> &d)
{
    const int size = d.size();
    write(data, &size);
    write(data, &d[0], size);
}

static inline void writeString(std::vector<unsigned char> &data, const std::string &str)
{
    const int size = str.size();
    write(data, &size);
    write(data, str.c_str(), size);
}

static inline bool serialize(FILE *f, const std::vector<unsigned char> &data)
{
    const int size = data.size();
    return fwrite(&size, sizeof(size), 1, f) && fwrite(&data[0], data.size(), 1, f);
}

bool IntegrationWriter::write(const std::string &fileName,
                              const std::vector<unsigned char> &data,
                              ErrorCode *code,
                              std::string *error)
{
    if (code)
        *code = Success;
    std::vector<unsigned char> encrypted, hmac;
    if (!encode(mSecret, data, encrypted, hmac, code, error)) {
        return false;
    }
    const std::string tmpFile = fileName + ".tmp";

    if (!recursiveMakeDirectory(mDirectory))
        return false;

    FILE *f = fopen(tmpFile.c_str(), "w");
    if (!f) {
        if (code)
            *code = TempFileOpenFailure;
        if (error) {
            std::ostringstream stream;
            stream << "Failed to open temp file: " << tmpFile << " " << errno;
            *error = stream.str();
        }
        return false;
    }

    const uint32_t version = Buffer::Version;
    if (!::fwrite(&version, sizeof(version), 1, f) || !serialize(f, hmac) || !serialize(f, encrypted)) {
        if (code)
            *code = TempFileWriteFailure;
        if (error) {
            std::ostringstream stream;
            stream << "Failed to write to temp file: " << tmpFile << " " << errno;
            *error = stream.str();
        }
        fclose(f);
        unlink(tmpFile.c_str());
        return false;
    }
    fclose(f);

    if (!validateFile(tmpFile, code, error)) {
        unlink(tmpFile.c_str());
        return false;
    }

    if (rename(tmpFile.c_str(), fileName.c_str())) {
        if (code)
            *code = TempFileRenameFailure;
        if (error) {
            std::ostringstream stream;
            stream << "Failed to rename temp file: " << tmpFile << " to " << fileName << " " << errno;
            *error = stream.str();
        }
        return false;
    }

    return true;
}

bool IntegrationWriter::write(const std::vector<Splash> &splashes, ErrorCode *code, std::string *error)
{
    if (code)
        *code = Success;
    if (!isValid()) {
        if (code)
            *code = IntegrationWriterUninitialized;
        if (error)
            *error = "IntegrationWriter not initialized";
        return false;
    }
    std::vector<unsigned char> data;
    const int size = splashes.size();
    ::write(data, &size);
    for (std::vector<Splash>::const_iterator it = splashes.begin(); it != splashes.end(); ++it) {
        const Splash &splash = *it;
        ::write(data, &splash.width);
        ::write(data, &splash.height);
        ::writeData(data, splash.data);
    }

    return write(mSplashFileName, data, code, error);
}

bool IntegrationWriter::write(const std::vector<TileGroup> &groups, ErrorCode *code, std::string *error)
{
    if (code)
        *code = Success;
    if (!isValid()) {
        if (code)
            *code = IntegrationWriterUninitialized;
        if (error)
            *error = "IntegrationWriter not initialized";
        return false;
    }
    std::vector<unsigned char> data;
    const int size = groups.size();
    ::write(data, &size);
    for (std::vector<TileGroup>::const_iterator it = groups.begin(); it != groups.end(); ++it) {
        const TileGroup &group = *it;
        ::writeString(data, group.title);
        const int tileCount = group.tiles.size();
        ::write(data, &tileCount);
        for (int i=0; i<tileCount; ++i) {
            const Tile &tile = group.tiles[i];
            const std::string *strings[] = { &tile.url, &tile.title, &tile.description, &tile.shortText, &tile.deepLink };
            for (size_t s=0; s<sizeof(strings) / sizeof(strings[0]); ++s) {
                ::writeString(data, *strings[s]);
            }
            ::write(data, &tile.width);
            ::write(data, &tile.height);
            ::write(data, &tile.expiry);
        }
    }

    return write(mTileFileName, data, code, error);
}

bool IntegrationWriter::clearSplashes(ErrorCode *code, std::string *error)
{
    if (code)
        *code = Success;
    if (!isValid()) {
        if (code)
            *code = IntegrationWriterUninitialized;
        if (error)
            *error = "IntegrationWriter not initialized";
        return false;
    }
    struct stat statBuf;
    if (::stat(mSplashFileName.c_str(), &statBuf)) // file doesn't exist
        return true;

    if (unlink(mSplashFileName.c_str())) {
        if (code)
            *code = UnlinkFailure;
        if (error) {
            std::ostringstream stream;
            stream << "Failed to unlink splash catalog file: " << mSplashFileName << " " << errno;
            *error = stream.str();
        }
        return false;
    }
    return true;

}

bool IntegrationWriter::clearTiles(ErrorCode *code, std::string *error)
{
    if (code)
        *code = Success;
    if (!isValid()) {
        if (code)
            *code = IntegrationWriterUninitialized;
        if (error)
            *error = "IntegrationWriter not initialized";
        return false;
    }
    struct stat statBuf;
    if (::stat(mTileFileName.c_str(), &statBuf)) // file doesn't exist
        return true;

    if (unlink(mTileFileName.c_str())) {
        if (code)
            *code = IntegrationWriterUninitialized;
        if (error) {
            std::ostringstream stream;
            stream << "Failed to unlink tile catalog file: " << mTileFileName << " " << errno;
            *error = stream.str();
        }
        return false;
    }
    return true;
}

bool IntegrationWriter::isValid() const
{
    return !mDirectory.empty() && !mSecret.empty();
}

uint32_t IntegrationWriter::version() const
{
    return Buffer::Version;
}

bool IntegrationWriter::validateFile(const std::string &file, ErrorCode *code, std::string *error) const
{
    if (code)
        *code = Success;
    Buffer buffer;
    const Buffer::Status status = buffer.open(file, mSecret);
    switch (status) {
    case Buffer::Ok:
        return true;
    case Buffer::NoFileError:
        if (code)
            *code = ValidationErrorNoFile;
        break;
    case Buffer::ReadError:
        if (code)
            *code = ValidationErrorRead;
        break;
    case Buffer::IntegrityError:
        if (code)
            *code = ValidationErrorIntegrity;
        break;
    case Buffer::VersionError:
        if (code)
            *code = ValidationErrorVersion;
        break;
    }

    if (error) {
        std::ostringstream stream;
        stream << "Failed to validate file: " << file;
        *error = stream.str();
    }
    return false;
}
