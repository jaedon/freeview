#include "NaluHelper.h"
#include <fcntl.h>
#include <assert.h>
#include <nrdbase/Log.h>

using namespace netflix::device::esplayer;
using namespace std;
using namespace netflix;

h264NaluHelper::h264NaluHelper() :
    mState(0),
    mFramesParsed(0)
{
    mNALTable.push_back("Unspecified");
    mNALTable.push_back("Coded slice of a non-IDR picture");
    mNALTable.push_back("Coded slice data partition A");
    mNALTable.push_back("Coded slice data partition B");
    mNALTable.push_back("Coded slice data partition C");
    mNALTable.push_back("Coded slice of an IDR picture");
    mNALTable.push_back("Supplemental enhancement information");
    mNALTable.push_back("Sequence parameter set");
    mNALTable.push_back("Picture parameter set");
    mNALTable.push_back("Access unit delimiter");
    mNALTable.push_back("End of sequence");
    mNALTable.push_back("End of stream");
    mNALTable.push_back("Filler data");
    mNALTable.push_back("Sequence parameter set extension");
    mNALTable.push_back("Prefix NAL unit");
    mNALTable.push_back("Subset sequence parameter set");
    mNALTable.push_back("Reserved");
    mNALTable.push_back("Reserved");
    mNALTable.push_back("Reserved");
    mNALTable.push_back("Coded slice of an auxiliary coded");
    mNALTable.push_back("Coded slice extension");
    mNALTable.push_back("Coded slice extension for depth view components");
    mNALTable.push_back("Reserved");
    mNALTable.push_back("Reserved");
    mNALTable.push_back("Unspecified");
    mNALTable.push_back("Unspecified");
    mNALTable.push_back("Unspecified");
    mNALTable.push_back("Unspecified");
    mNALTable.push_back("Unspecified");
    mNALTable.push_back("Unspecified");
    mNALTable.push_back("Unspecified");
    mNALTable.push_back("Unspecified");
    assert(mNALTable.size() == 32);
}

bool h264NaluHelper::analyze(const uint8_t * auData, uint32_t auDataSize)
{
    bool rc = false;
    bool idrSliceFound = false;
    bool spsFound = false;
    bool ppsFound = false;

    parse(auData, auDataSize, idrSliceFound, spsFound, ppsFound);

    if (mFramesParsed == 1) {
        // Waiting for IDR (post-seek, skipback, start of playback)
        rc = idrSliceFound && spsFound && ppsFound;
    } else {
        // decoding
    }
    return rc;
}

bool h264NaluHelper::print(const uint8_t * auData, uint32_t auDataSize)
{
    bool rc = false;

    for (uint32_t i = 0; (i < auDataSize) && (i < 256) && (auDataSize > 15); i += 16 ) {
        Log::warn(TRACE_MEDIAPLAYBACK, "[0x%02x] 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                i,
                auData[i], auData[i+1],auData[i+2],auData[i+3],
                auData[i+4], auData[i+5],auData[i+6],auData[i+7],
                auData[i+8], auData[i+9],auData[i+10],auData[i+11],
                auData[i+12], auData[i+13],auData[i+14],auData[i+15]);
    }

    for (uint32_t i = 0, c = 0; i < auDataSize; i++) {
        if ((auData[i] == 0) && (auData[i+1] == 0) && (auData[i+2] == 0) && (auData[i+3] == 1)) {
            uint32_t nalu = auData[i+4] & 0x1f;

            Log::warn(TRACE_MEDIAPLAYBACK, "[%d] NAL unit %d at position %d '%s'",c, nalu, i, mNALTable[nalu].c_str());
            c++;
            rc = true;
       }
    }
    return rc;
}

bool h264NaluHelper::parse(const uint8_t * auData, uint32_t auDataSize, bool & idrSliceFound, bool & spsFound, bool & ppsFound)
{
    idrSliceFound = false;
    spsFound = false;
    ppsFound = false;

    for (uint32_t i = 0, c = 0; i < auDataSize; i++) {
        if ((auData[i] == 0) && (auData[i+1] == 0) && (auData[i+2] == 0) && (auData[i+3] == 1)) {
            uint32_t nalu = auData[i+4] & 0x1f;

            switch (nalu) {
            case 5:
                idrSliceFound = true;
                break;
            case 7:
                spsFound = true;
                break;
            case 8:
                ppsFound = true;
                break;
            }
            c++;
       }
    }

    mFramesParsed++;

    return true;
}

bool h264NaluHelper::dump(const uint8_t * auData, uint32_t auDataSize, const char * name)
{
    bool rc = false;

    int fd = open(name, O_CREAT | O_RDWR, S_IRWXU);
    if (fd != -1) {
        ssize_t bytes = write(fd, auData, auDataSize);
        rc = (bytes == (ssize_t)auDataSize);
        fsync(fd);
        close(fd);
    }

    return rc;
}

