/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <string.h>
#include <stdlib.h>

#include <vector>
#include <string>

namespace netflix {

class Buffer
{
public:
    enum { Version = 1 };

    Buffer(const std::vector<unsigned char> &data=std::vector<unsigned char>())
        : mData(data), mPosition(0), mHasError(false)
    {}

    enum Status {
        Ok,
        NoFileError,
        ReadError,
        IntegrityError,
        VersionError
    };

    Status open(const std::string &filename, const std::vector<unsigned char> &secret);

    bool hasError() const { return mHasError; }
    bool isEmpty() const { return mData.empty(); }

    template <typename T>
    bool read(T *data, int count = 1)
    {
        if (mPosition + count > static_cast<int>(mData.size())) {
            mHasError = true;
            return false;
        }

        memcpy(data, &mData[mPosition], sizeof(T) * count);
        mPosition += sizeof(T) * count;
        return true;
    }


    template <typename T> Buffer &operator>>(T &t)
    {
        if (!read(&t)) {
            t = T();
        }
        return *this;
    }

    Buffer &operator>>(std::string &string)
    {
        int size;
        if (read(&size) && size > 0) {
            string.resize(size);
            if (!read(&string[0], size))
                string.clear();
        }

        return *this;

    }

    Buffer &operator>>(std::vector<unsigned char> &data)
    {
        int size;
        if (read(&size) && size > 0) {
            data.resize(size);
            if (!read(&data[0], size))
                data.clear();
        }

        return *this;
    }

private:
    Buffer(const Buffer &);
    Buffer &operator=(const Buffer &);

    std::vector<unsigned char> mData;
    int mPosition;
    bool mHasError;
};

}

#endif /* __BUFFER_H__ */
