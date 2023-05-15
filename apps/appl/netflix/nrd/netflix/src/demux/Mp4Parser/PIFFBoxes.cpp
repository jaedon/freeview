/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdio.h>
#include <nrdbase/Base64.h>

#include "PIFFBoxes.h"

#include "Context.h"

#include "Reader.h"
#include "Dumper.h"
#include "macros.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;
using netflix::containerlib::mp4parser::UUID;

/*******************************************************************************
 * Table of Box Types
 */

DEFINE_DUAL_BOXTYPE(ProtectionSystemSpecificHeaderBox, "pssh",
    0x29701FE4, 0x3CC7, 0x4A34, 0x8C5B, 0xAE90C7439A47LL);

DEFINE_DUAL_BOXTYPE(TrackEncryptionBox, "tenc",
    0xCEDB7489, 0xE77B, 0x514C, 0x84F9, 0x7148F9882554LL);

DEFINE_DUAL_BOXTYPE(SampleEncryptionBox, "senc",
    0x524F39A2, 0x9B5A, 0x144F, 0xA244, 0x6C427C648DF4LL);

/*******************************************************************************
 * ProtectionSystemSpecificHeaderBox
 */

bool ProtectionSystemSpecificHeaderBox::readSpecifics(Reader& reader, Context& context)
{
    reader.readBytes(systemID_);
    reader.read(dataSize_, 32);

    if(!reader.good())
        return false;

    vector<uint8_t> protectionSystemSpecificData(dataSize_);
    reader.readBytes(protectionSystemSpecificData);

    if(!reader.good())
        return false;

    context.protectionSystemHeaderFound(systemID_, protectionSystemSpecificData);
    return true;
}

void ProtectionSystemSpecificHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP_UUID(systemID_);
    DUMP(dataSize_);
}

/*******************************************************************************
 * TrackEncryptionBox
 */

bool TrackEncryptionBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(default_AlgorithmID_, 24);
    reader.read(default_IV_size_, 8);
    reader.readBytes(default_KID_);

    /*
    std::string keyId((char*)&default_KID_[0], default_KID_.size());
    std::string keyIdB64 = Base64::encode(keyId);
    printf("KeyID %s\n", keyIdB64.c_str());
    */

    if(reader.good())
    {
        uint32_t scheme = context.currentTrackContext()->currProtectionScheme();

        if(context.isProtectionSchemeSupported(scheme)){
            context.setDefaultDrmInfo(default_AlgorithmID_, default_IV_size_, default_KID_);
            context.currentTrackContext()->setDefaultDrmInfo(default_AlgorithmID_,
                                                             default_IV_size_,
                                                             default_KID_);
        }
    }

    return reader.good();
}

void TrackEncryptionBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(default_AlgorithmID_);
    DUMP(default_IV_size_);
    DUMP_UUID(default_KID_);
}

/*******************************************************************************
 * SampleEncryptionBox
 */

bool SampleEncryptionBox::readSpecifics(Reader& reader, Context& context)
{

    uint32_t trackId = context.currentTrackFragmentContext()->trackID();

    shared_ptr<TrackContext> trackContext = context.findTrackContext(trackId);

    if(trackContext.get())
    {
        trackContext->getDefaultDrmInfo(AlgorithmID_, IV_size_, &KID_);
    } else {
        context.getDefaultDrmInfo(AlgorithmID_, IV_size_, &KID_);
    }

    if(isFlagBitSet(overrideTrackEncryptionBoxParameters_))
    {
        reader.read(AlgorithmID_, 24);
        reader.read(IV_size_, 8);
        reader.readBytes(KID_);
    }

    reader.read(sample_count_, 32);

    if(!reader.good())
        return false;

    context.currentTrackFragmentContext()->setDrmAlgorithmID(AlgorithmID_);

    vector<TrackFragmentContext::EncryptionMapEntry>& encryptionMapEntries =
        context.currentTrackFragmentContext()->encryptionMapEntries();

    vector<uint8_t>& IVDataPool =
        context.currentTrackFragmentContext()->IVDataPool();

    if(AlgorithmID_ == 0) // no encryption, don't parse further.
        return true;

    if(IV_size_ != 8 && IV_size_ != 16)
        return false;

    uint32_t i;

    /*
     * constructing drmInfoEntry for legacy
     */
    /*
    for(i = 0; i < sample_count_; i ++)
    {
        uint32_t IVOffset = IVDataPool.size();
        IVDataPool.resize(IVOffset + IV_size_);
        reader.readBytes(IVDataPool, IVOffset, IV_size_);

        if(!reader.good())
            return false;

        TrackFragmentContext::DrmInfoEntry drmInfoEntry;

        drmInfoEntry.IVDataOffset_ = IVOffset;
        drmInfoEntry.IVSize_ = IV_size_;
        drmInfoEntry.encryptionMapStartIndex_ = encryptionMapEntries.size();
        drmInfoEntry.encryptionMapEntriesUsed_ = 0;

        if(isFlagBitSet(useSubSampleEncryption_))
        {
            uint32_t numberOfEntries;
            reader.read(numberOfEntries, 16);

            if(!reader.good())
                return false;

            uint32_t j;
            for(j = 0; j < numberOfEntries; j ++)
            {
                TrackFragmentContext::EncryptionMapEntry entry;

                reader.read(entry.bytesOfClearData_, 16);
                reader.read(entry.bytesOfEncryptedData_, 32);

                if(!reader.good())
                    return false;

                encryptionMapEntries.push_back(entry);
            }

            drmInfoEntry.encryptionMapEntriesUsed_ = numberOfEntries;
        }

        context.currentTrackFragmentContext()->drmInfoEntries().
            push_back(drmInfoEntry);
    }
    */

    /*
     * construct newer structure of EncryptionEntry
     */
    /*
    for(i = 0; i < sample_count_; i ++)
    {
        TrackFragmentContext::EncryptionEntry encryptionEntry;
        encryptionEntry.IV_.resize(IV_size_);
        reader.readBytes(encryptionEntry.IV_);

        if(!reader.good())
            return false;

        if(isFlagBitSet(useSubSampleEncryption_))
        {
            uint32_t numberOfEntries;
            reader.read(numberOfEntries, 16);

            if(!reader.good())
                return false;

            uint32_t j;
            for(j = 0; j < numberOfEntries; j ++)
            {
                TrackFragmentContext::EncryptionMapEntry entry;

                reader.read(entry.bytesOfClearData_, 16);
                reader.read(entry.bytesOfEncryptedData_, 32);

                if(!reader.good())
                    return false;

                encryptionEntry.subSampleMap_.push_back(entry);
            }
        }
        context.currentTrackFragmentContext()->encryptionEntries().push_back(encryptionEntry);
    }
    */

    /*
     * combination of both legacy and newer encryption information
     */
    for(i = 0; i < sample_count_; i ++)
    {
        TrackFragmentContext::DrmInfoEntry drmInfoEntry;
        TrackFragmentContext::EncryptionEntry encryptionEntry;

        encryptionEntry.IV_.resize(IV_size_);
        reader.readBytes(encryptionEntry.IV_);

        // legacy compatible
        uint32_t IVOffset = IVDataPool.size();
        IVDataPool.resize(IVOffset + IV_size_);
        memcpy(&IVDataPool[IVOffset], &encryptionEntry.IV_[0], IV_size_);

        drmInfoEntry.IVDataOffset_ = IVOffset;
        drmInfoEntry.IVSize_ = IV_size_;
        drmInfoEntry.encryptionMapStartIndex_ = encryptionMapEntries.size();
        drmInfoEntry.encryptionMapEntriesUsed_ = 0;
        // legacy compatible

        if(!reader.good())
            return false;

        if(isFlagBitSet(useSubSampleEncryption_))
        {
            uint32_t numberOfEntries;
            reader.read(numberOfEntries, 16);

            if(!reader.good())
                return false;

            uint32_t j;
            for(j = 0; j < numberOfEntries; j ++)
            {
                TrackFragmentContext::EncryptionMapEntry entry;

                reader.read(entry.bytesOfClearData_, 16);
                reader.read(entry.bytesOfEncryptedData_, 32);

                if(!reader.good())
                    return false;

                encryptionEntry.subSampleMap_.push_back(entry);

                // legacy compatible
                encryptionMapEntries.push_back(entry);
            }
            // legacy
            drmInfoEntry.encryptionMapEntriesUsed_ = numberOfEntries;
        }
        context.currentTrackFragmentContext()->encryptionEntries().push_back(encryptionEntry);

        // legacy compatible
        context.currentTrackFragmentContext()->drmInfoEntries().push_back(drmInfoEntry);
    }

    return reader.good();
}

void SampleEncryptionBox::dumpSpecifics(Dumper& dumper) const
{
    if(isFlagBitSet(overrideTrackEncryptionBoxParameters_))
    {
        DUMP(AlgorithmID_);
        DUMP(IV_size_);
        DUMP_UUID(KID_);
    }

    DUMP(sample_count_);
}
