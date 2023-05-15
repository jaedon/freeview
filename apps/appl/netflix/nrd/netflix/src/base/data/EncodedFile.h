/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef EncodedFile_h
#define EncodedFile_h

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

class EncodedFile
{
public:
    EncodedFile()
        : mPos(-1), mSize(-1), mFD(-1), mError(false), mMode(None)
    {}

    enum Mode {
        None,
        Read,
        Write
    };
    bool open(const std::string &fileName, Mode mode)
    {
        assert(mFD == -1);
        if (mode == Write) {
            const size_t slash = fileName.rfind('/');
            assert(slash != std::string::npos);
            memcpy(mTempBuf, fileName.c_str(), slash + 1);
            strcpy(mTempBuf + slash + 1, "XXXXXX");
            mFD = mkstemp(mTempBuf);
            if (mFD == -1) {
                mError = true;
                return false;
            }
        } else {
            mFD = ::open(fileName.c_str(), O_RDONLY);
            if (mFD == -1) {
                mError = true;
                return false;
            }
            struct stat st;
            if (fstat(mFD, &st)) {
                ::close(mFD);
                mFD = -1;
                mError = true;
                return false;
            }
            mSize = st.st_size;
        }

        mFileName = fileName;
        mError = false;
        mMode = mode;
        mPos = 0;
        return true;
    }

    ~EncodedFile()
    {
        flush();
    }

    bool flush()
    {
        if (mFD != -1) {
            ::close(mFD);
            mFD = -1;
            if (mMode == Write) {
                if (!mError) {
                    const int ret = rename(mTempBuf, mFileName.c_str()) == 0;
                    return ret;
                } else {
                    unlink(mTempBuf);
                }
            }
        }
        return false;
    }

    Mode mode() const { return mMode; }
    bool isValid() const { return !mError; }
    bool hasError() const { return mError; }
    size_t pos() const { return mPos; }
    size_t size() const { return mSize; }
    bool atEnd() const { return mPos == mSize; }

    template <typename T>
    EncodedFile &operator<<(const T &t)
    {
        assert(mMode == Write);
        const int size = static_cast<int>(t.size());
        write(&size, sizeof(size)) && write(&t[0], size);

        return *this;
    }

    template <typename T>
    EncodedFile &operator>>(T &t)
    {
        int size = 0;
        if (read(&size, sizeof(size)) && size >= 0 && size <= 1024 * 1024) {
            t.resize(size);
            read(&t[0], size);
        }
        return *this;
    }

    bool write(const void *buf, size_t size)
    {
        if (mError)
            return false;
        if (::write(mFD, buf, size) != static_cast<int>(size)) {
            mError = true;
            return false;
        }
        mPos += size;
        mSize += size;
        return true;
    }
    bool read(void *buf, size_t size)
    {
        if (mError)
            return false;
        if (::read(mFD, buf, size) != static_cast<int>(size)) {
            mError = true;
            return false;
        }
        mPos += size;
        return true;
    }

    void fsync()
    {
        assert(mMode == Write);
        ::fsync(mFD);
    }
private:
    size_t mPos, mSize;
    std::string mFileName;
    int mFD;
    bool mError;
    Mode mMode;
    char mTempBuf[PATH_MAX];
};

#endif
