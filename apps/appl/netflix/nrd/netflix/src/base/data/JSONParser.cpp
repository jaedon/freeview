/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSONParser.h"
#include <nrdbase/Log.h>

using namespace netflix;

void JSONParser::putUtf(unsigned int utf, JSONParser::String *string)
{
    assert(utf < 0x200000);
    if (utf < 0x80) {
        string->addChar(static_cast<unsigned char>(utf));
        return;
    }

    // length of the utf8 representation of the unicode character
    int len = (utf < 0x800) ? 2 : (utf < 0x10000) ? 3 : 4;

    // top len bits of first byte are 1
    // and remaining bytes remainder coded as 6 bits per byte with 10 in top two bits
    string->addChar(static_cast<unsigned char>((((1 << len)-1) << (8-len)) | utf >> ((len - 1)*6)));
    --len;

    while (len)
        string->addChar(static_cast<unsigned char>(0x80 | ((utf >> (--len*6)) & 0x3f)));
}

void JSONParser::putEscapedUtf(unsigned int utf, String *string)
{
    assert(utf < 0x200000);

    if (utf < 0x10000) {
        char buf[6];
        snprintf(buf, sizeof(buf), "\\u%04x", utf);
        string->addData(buf, sizeof(buf) - 1);
    } else {
        char buf[16];
        snprintf(buf, sizeof(buf), "\\u%04x\\u%04x", ((utf >> 10) + 0xd800), (utf & 0x3ff) + 0xdc00);
        string->addData(buf, sizeof(buf) - 1);
    }
}

JSONParser::Value *JSONParser::parse(const char *data, int size)
{
    mData = mCurrent = data;
    mEnd = data + size;
    Value *ret = parseValue();
    if (ret)
        eatWhiteSpace();
    return ret;
}

JSONParser::Value *JSONParser::parseValue()
{
    if (!eatWhiteSpace()) {
        switch (*mCurrent) {
        case '{': {
            // object
            ++mCurrent;
            Object *object = createObject();
            assert(object);
            if (!parseObject(object)) {
                NTRACE(TRACE_JSONPARSER, "failed to parse object");
                delete object;
                break;
            }
            return object; }
        case '[': {
            // array
            ++mCurrent;
            Array *array = createArray();
            assert(array);
            if (!parseArray(array)) {
                NTRACE(TRACE_JSONPARSER, "failed to parse array");
                delete array;
                break;
            }
            return array; }
        case '\"': {
            String *string = createStringValue();
            // string
            if (!parseString(string)) {
                NTRACE(TRACE_JSONPARSER, "failed to parse string");
                delete string;
                break;
            }
            return string; }
        case 't':
            ++mCurrent;
            if (mEnd - mCurrent < 3) {
                mCurrent = mEnd;
                return 0;
            }
            if (!strncmp(mCurrent, "rue", 3)) {
                mCurrent += 3;
                return create(true);
            }
            NTRACE(TRACE_JSONPARSER, "failed to parse true");
            break;
        case 'f':
            ++mCurrent;
            if (mEnd - mCurrent < 4) {
                mCurrent = mEnd;
                return 0;
            }
            if (!strncmp(mCurrent, "alse", 4)) {
                mCurrent += 4;
                return create(false);
            }
            NTRACE(TRACE_JSONPARSER, "failed to parse false");
            break;
        case 'n':
            ++mCurrent;
            if (mEnd - mCurrent < 3) {
                mCurrent = mEnd;
                return 0;
            }
            if (!strncmp(mCurrent, "ull", 3)) {
                mCurrent += 3;
                return createNull();
            }
            NTRACE(TRACE_JSONPARSER, "failed to parse null");
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-': {
            char *endptr;
            const llong i = strtoll(mCurrent, &endptr, 10);
            if (endptr != mCurrent && *endptr != '.') {
                mCurrent = endptr;
                return create(i);
            }

            const double d = strtod(mCurrent, &endptr);
            if (endptr == mCurrent) {
                NTRACE(TRACE_JSONPARSER, "failed to parse number");
                break;
            }
            mCurrent = endptr;
            return create(d); }
        default:
            NTRACE(TRACE_JSONPARSER, "Unexpected character %c", *mCurrent);
            break;
        }
    }
    mError = mCurrent != mEnd;
    return 0;
}

bool JSONParser::parseString(JSONParser::String *string)
{
    bool bUtf16ByteSwap = false;
    if (*mCurrent != '\"') {
        NTRACE(TRACE_JSONPARSER, "String doesn't start with quote %s", mCurrent);
        return false;
    }
    const char *pending = 0;
    while (!atEnd()) {
        ++mCurrent;
        switch (*mCurrent) {
        case '"':
            if (pending) {
                string->finalize(pending, mCurrent - pending);
            } else {
                string->finalize();
            }
            ++mCurrent;
            return true;
        case '\\':
            if (pending) {
                string->addRange(pending, mCurrent - pending);
                pending = 0;
            }
            switch (*++mCurrent) {
            case '"':
            case '\\':
            case '/':
            case '\'':
                string->addChar(*mCurrent);
                break;
            case 'b':
                string->addChar('\b');
                break;
            case 'f':
                string->addChar('\f');
                break;
            case 'n':
                string->addChar('\n');
                break;
            case 'r':
                string->addChar('\r');
                break;
            case 't':
                string->addChar('\t');
                break;
            case 'u': {
                if (mCurrent + 5 >= mEnd)
                    break;
                char buf[5];
                memcpy(buf, mCurrent + 1, 4);
                buf[4] = '\0';
                char *endptr;
                unsigned int uchar = strtoul(buf, &endptr, 16);
                if (endptr != buf + 4) {
                    NTRACE(TRACE_JSONPARSER, "Invalid unicode 1");
                    return false;
                }

                mCurrent += 4;

                if (uchar == 0xfffe) {
                    // reversed byte order mark - someone picked up these utf16's the wrong way round
                    bUtf16ByteSwap = true;
                } else {
                    if (bUtf16ByteSwap)
                        uchar = ((uchar & 0xff) << 16) + (uchar >> 16);
                    if (uchar < 0xd800 || uchar >= 0xdc00) {
                        putUtf(uchar, string);
                    } else if (atEnd()) {
                        break;
                    } else {
                        ++mCurrent;
                        if (mCurrent + 7 >= mEnd) {
                            NTRACE(TRACE_JSONPARSER, "Invalid unicode 2");
                            return false;
                        } else if (strncmp(mCurrent, "\\u", 2)) {  // utf-16 surrogate pair
                            NTRACE(TRACE_JSONPARSER, "Invalid unicode 3: %s", std::string(mCurrent, 2).c_str());
                            return false;
                        }
                        mCurrent += 2;
                        memcpy(buf, mCurrent, 4);
                        const unsigned int uchar2 = strtoul(buf, &endptr, 16);
                        if (endptr != buf + 4) {
                            NTRACE(TRACE_JSONPARSER, "Invalid unicode 4");
                            return false;
                        }
                        if (uchar2 < 0xdc00 || uchar2 > 0xdfff) {
                            NTRACE(TRACE_JSONPARSER, "Invalid unicode 5");
                            return false;
                        }

                        const unsigned int unicode = (((uchar - 0xD800) << 10) | (uchar2 - 0xDC00)) + 0x10000;
                        putUtf(unicode, string);
                        mCurrent += 3;
                    }
                }
                break; }
            default:
                NTRACE(TRACE_JSONPARSER, "Unexpected escaped character %c", *mCurrent);
                return false;
            }
            break;
        default:
            if (!pending)
                pending = mCurrent;
            break;
        }
    }
    NTRACE(TRACE_JSONPARSER, "Ran out of string before end of string");
    return false;
}

bool JSONParser::parseObject(JSONParser::Object *object)
{
    if (*mCurrent == '}') {
        ++mCurrent;
        return true;
    }

    while (!atEnd()) {
        String *key = createStringKey();
        assert(key);
        if (eatWhiteSpace())
            break;
        if (!parseString(key)) {
            NTRACE(TRACE_JSONPARSER, "Failed to parse key");
            delete key;
            break;
        }
        if (eatWhiteSpace())
            break;
        if (*mCurrent != ':')
            return false;
        ++mCurrent;
        Value *value = parseValue();
        if (!value) {
            NTRACE(TRACE_JSONPARSER, "Failed to parse value");
            delete key;
            return false;
        }
        const bool ok = object->setValue(key, value);
        delete key;
        delete value;
        if (!ok)
            break;
        if (eatWhiteSpace())
            break;
        if (*mCurrent == '}') {
            ++mCurrent;
            return true;
        } else if (*mCurrent != ',') {
            NTRACE(TRACE_JSONPARSER, "Unexpected char %c, expected } or , at %s", *mCurrent, mCurrent);
            break;
        }
        ++mCurrent;
    }
    NTRACE(TRACE_JSONPARSER, "Ran out of string before end of object");
    return false;
}

bool JSONParser::parseArray(JSONParser::Array *array)
{
    if (*mCurrent == ']') {
        ++mCurrent;
        array->finalize();
        return true;
    }

    while (!atEnd()) {
        Value *val = parseValue();
        if (!val) {
            NTRACE(TRACE_JSONPARSER, "Failed to parse value");
            return false;
        }
        if (eatWhiteSpace()) {
            delete val;
            break;
        }
        array->append(val);
        delete val;
        if (*mCurrent == ']') {
            ++mCurrent;
            array->finalize();
            return true;
        } else if (*mCurrent != ',') {
            NTRACE(TRACE_JSONPARSER, "Got unexpected character %c looking for ] or , at:%s\n", *mCurrent, mCurrent);
            break;
        }
        ++mCurrent;
    }
    NTRACE(TRACE_JSONPARSER, "Ran out of string before end of array");
    return false;
}

bool JSONParser::eatWhiteSpace()
{
    while (mCurrent != mEnd) {
        if (!isspace((unsigned char)*mCurrent))
            return false;
        ++mCurrent;
    }
    return true;
}
