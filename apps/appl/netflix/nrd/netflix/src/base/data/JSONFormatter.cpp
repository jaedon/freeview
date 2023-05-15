/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include "JSONFormatter.h"
#include <nrdbase/Variant.h>
#include <nrdbase/Log.h>
#include <nrdbase/LinkedList.h>
#include <sstream>
#include <limits>
#include <stdio.h>
#include <nrdbase/tr1.h>
#include <list>
#include <ctype.h>

using namespace netflix;

JSONFormatter::JSONFormatter(uint32_t flags)
    : mFlags(flags), mMaxStringLength(INT_MAX), mMaxLength(0)
{
}

struct Node
{
    enum Flag {
        None = 0x00,
        ArrayStart = 0x01,
        ArrayEnd = 0x02,
        ObjectStart = 0x04,
        ObjectEnd = 0x08,
        Comma = 0x10,
        Key = 0x20,
        InArray = 0x40
    };

    explicit Node(unsigned f)
        : flags(f), next(0), prev(0)
    {}
    explicit Node(const Variant &var, unsigned f)
        : flags(f), variant(var), next(0), prev(0)
    {}
    explicit Node(const std::string &k, const Variant &var, unsigned f)
        : flags(f), variant(var), key(k), next(0), prev(0)
    {}

    unsigned flags;
    const Variant variant;
    std::string key;
    std::string json;
    Node *next, *prev;
};

std::string JSONFormatter::escape(const std::string & string, uint32_t flags)
{
    std::ostringstream os;
    escape(os, string.c_str(), string.size(), flags);
    return os.str();
}


void JSONFormatter::escape(std::basic_ostream<char> &os, const char *string, int length, uint32_t flags)
{
#define NEED_ESCAPE()                           \
    do {                                        \
        if (!hasEscape) {                       \
            hasEscape = true;                   \
            if (i)                              \
                os.write(string, i);            \
        }                                       \
    } while (false)


    //     \b  Backspace (ascii code 08)
    //     \f  Form feed (ascii code 0C)
    //     \n  New line
    //     \r  Carriage return
    //     \t  Tab
    //     \v  Vertical tab
    //     \'  Apostrophe or single quote
    //     \"  Double quote
    //     \\  Backslash caracter

    bool hasEscape = false;

    for(int i = 0; i < length; ++i) {
        switch (unsigned char ch = string[i]) {
        case 8: // backspace
            NEED_ESCAPE();
            os << "\\b";
            break;
        case 12: // Form feed
            NEED_ESCAPE();
            os << "\\f";
            break;
        case '\n': // newline
            NEED_ESCAPE();
            os << "\\n";
            break;
        case '\t': // tab
            NEED_ESCAPE();
            os << "\\t";
            break;
        case '\r': // carriage return
            NEED_ESCAPE();
            os << "\\r";
            break;
        case '"': // quote
            NEED_ESCAPE();
            os << "\\\"";
            break;
        case '\\': // backslash
            NEED_ESCAPE();
            os << "\\\\";
            break;
        case '\'': // single qoute
            if (flags & EscapeSingleQuote) {
                NEED_ESCAPE();
                os << "\\'";
            } else if (hasEscape) {
                os << static_cast<char>(ch);
            }
            break;

        default:
            if (ch < 0x20 && !isspace(ch)) { // escape non printable characters
                NEED_ESCAPE();
                char buffer[7];
                snprintf(buffer, 7, "\\u%04x", ch);
                os << buffer;
            } else if (hasEscape) {
                os << static_cast<char>(ch);
            }
            break;
        }
    }

    if (!hasEscape)
        os.write(string, length);
}

void JSONFormatter::formatString(std::basic_ostream<char> &os, const char *string, int length) const
{
    os << '"';
    if (length > mMaxStringLength) {
        escape(os, string, mMaxStringLength, mFlags);
        os << "...[TRUNCATED@" << length << "]";
    } else {
        escape(os, string, length, mFlags);
    }
    os << '"';
}

std::string JSONFormatter::jsonObject(const Variant &value) const
{
    if ((mFlags & (Pretty|UnprettySmallObjects)) == (Pretty|UnprettySmallObjects)
        && (value.isArray() || value.isStringMap())) {
        JSONFormatter formatter(HalfPretty);
        formatter.setMaxLength(40);
        bool truncated = false;
        const std::string json = formatter.format(value, &truncated);
        // printf("TRYING here %d %d\n", truncated, json.size());

        if (!truncated)
            return json;
    }
    return std::string();
}

void JSONFormatter::format(const Variant &value, std::basic_ostream<char> &stream, bool &truncated) const
{
    const bool pretty = (mFlags & Pretty);
    const bool halfPretty = (mFlags & HalfPretty);
    LinkedList<Node*> nodes;
    Node *node = new Node(value, Node::None);

    nodes.append(node);
    int depth = 0;
    truncated = false;
    while (!nodes.isEmpty()) {
        Node *node = nodes.pop_front();
        const Variant::Type type = node->variant.type();
        switch (type) {
        case Variant::Type_Array: {
            unsigned int flags = (node->flags & (Node::Key|Node::InArray)) | Node::ArrayStart;
            const std::string json = jsonObject(node->variant);

            const int count = json.empty() ? node->variant.size() : 0;
            if (!count)
                flags |= Node::ArrayEnd | (node->flags & Node::Comma);
            Node *n = new Node(node->key, Variant(), flags);
            n->json = json;
            nodes.prepend(n);

            if (count) {
                for (int i=0; i<count; ++i) {
                    flags = Node::InArray;
                    if (i + 1 < count)
                        flags |= Node::Comma;
                    Node *tmp = new Node(node->variant[i], flags);
                    nodes.insertAfter(tmp, n);
                    n = tmp;
                }
                nodes.insertAfter(new Node(Node::ArrayEnd|(node->flags & (Node::Comma|Node::InArray))), n);
            }
            break; }
        case Variant::Type_Custom:
            nodes.prepend(new Node(node->variant.custom()->toVariant(), Node::None));
            break;
        case Variant::Type_StringMap: {
            Variant::StringMapIterator it = node->variant.stringMapBegin();
            const std::string json = jsonObject(node->variant);
            const Variant::StringMapIterator end = json.empty() ? node->variant.stringMapEnd() : it;
            unsigned int flags = (node->flags & (Node::Key|Node::InArray)) | Node::ObjectStart;
            if (it == end)
                flags |= Node::ObjectEnd | (node->flags & Node::Comma);
            Node *n = new Node(node->key, Variant(), flags);
            n->json = json;
            nodes.prepend(n);
            if (it != end) {
                while (it != end) {
                    const std::string &key = it->first;
                    const Variant &val = it->second;
                    Node *tmp = new Node(key, val, (++it == end ? Node::None : Node::Comma) | Node::Key);
                    nodes.insertAfter(tmp, n);
                    n = tmp;
                }
                nodes.insertAfter(new Node(Node::ObjectEnd|(node->flags & (Node::Comma|Node::InArray))), n);
            }
            break; }
        case Variant::Type_IntegerMap: {
            assert(false);
            break; }
        default:
            if (node->flags & (Node::ArrayStart|Node::ObjectStart)) {
                assert(type == Variant::Type_Null);

                if (node->flags & Node::Key) {
                    formatString(stream, node->key.c_str(), node->key.size());
                    if (pretty || halfPretty)
                        stream << ": ";
                    else
                        stream << ':';
                }
                if (!node->json.empty()) {
                    stream << node->json;
                } else if (node->flags & (Node::ArrayEnd|Node::ObjectEnd)) {
                    stream << (node->flags & Node::ArrayStart ? "[]" : "{}");
                } else {
                    stream << (node->flags & Node::ArrayStart ? '[' : '{');
                    if (pretty) {
                        stream << '\n';
                    } else if (halfPretty) {
                        stream << ' ';
                    }
                    ++depth;
                }
            } else if (node->flags & (Node::ArrayEnd|Node::ObjectEnd)) {
                assert(type == Variant::Type_Null);
                --depth;
                if (pretty) {
                    stream << '\n';
                    if (depth)
                        stream << formatIndent(depth);
                } else if (halfPretty) {
                    stream << ' ';
                }
                stream << (node->flags & Node::ArrayEnd ? ']' : '}');
            } else {
                if (node->flags & Node::Key) {
                    assert(!(node->flags & Node::InArray));
                    formatString(stream, node->key.c_str(), node->key.size());
                    if (pretty || halfPretty) {
                        stream << ": ";
                    } else {
                        stream << ':';
                    }
                }
                switch (node->variant.type()) {
                case Variant::Type_Null:
                    if (mFlags & NullVariantToUndefined) {
                        stream << "undefined";
                    } else if (!(mFlags & NullVariantToEmptyString)) {
                        stream << "null";
                    }
                    break;
                case Variant::Type_Custom:
                    assert(0); // custom variants should return a non-custom
                               // variant or a null variant if that is
                               // impossible in their toVariant() function. This
                               // one clearly didn't
                    break;
                case Variant::Type_Integer:
                    stream << node->variant.integer();
                    break;
                case Variant::Type_Pointer:
                    stream << '"' << node->variant.pointer() << '"';
                    break;
                case Variant::Type_Double: {
                    const double val = node->variant.dbl();
                    if (isnan(val) || isinf(val)) {
                        stream << "null";
                    } else {
                        stream.precision(std::numeric_limits<double>::digits10);
                        stream << val;
                    }
                    break; }
                case Variant::Type_Boolean: {
                    static const std::string sTrue("true"), sFalse("false");
                    stream << (node->variant.boolean() ? sTrue : sFalse);
                    break; }
                case Variant::Type_String: {
                    const std::string string = node->variant.string();
                    formatString(stream, string.c_str(), string.size());
                    break; }
                case Variant::Type_DataBuffer: {
                    const DataBuffer dataBuffer = node->variant.dataBuffer();
                    const int size = dataBuffer.size();
                    if (mFlags & DataBuffersAsArrays && dataBuffer.isBinary()) {
                        const unsigned char *data = dataBuffer.data();
                        stream << "[ ";
                        for (int i=0; i<size; ++i) {
                            if (i)
                                stream << ", ";
                            stream << static_cast<unsigned int>(data[i]);
                        }
                        stream << " ]";
                    } else {
                        formatString(stream, dataBuffer.data<const char>(), dataBuffer.size());
                    }
                    break; }
                default:
                    assert(0);
                    break;
                }
            }

            if (node->flags & Node::Comma) {
                if (pretty) {
                    bool nextNeedsNewLine = true;
                    assert(!nodes.isEmpty());
                    if (node->flags & Node::InArray) {
                        nextNeedsNewLine = false;
                        Node *next = nodes.first();
                        switch (next->flags & (Node::ObjectStart|Node::ObjectEnd|Node::ArrayStart|Node::ArrayEnd)) {
                        case Node::ObjectStart:
                        case Node::ArrayStart:
                        case Node::ObjectEnd:
                        case Node::ArrayEnd:
                            nextNeedsNewLine = true;
                            break;
                        default:
                            if ((next->variant.isArray() || next->variant.isStringMap()) && next->variant.size()) {
                                nextNeedsNewLine = true;
                            } else if ((node->flags & (Node::ObjectStart|Node::ObjectEnd|Node::ArrayStart|Node::ArrayEnd))
                                       && !(next->flags & (Node::ObjectStart|Node::ObjectEnd|Node::ArrayStart|Node::ArrayEnd))) {
                                nextNeedsNewLine = true;
                            }
                            break;
                        }
                    }
                    if (!nextNeedsNewLine) {
                        stream << ", ";
                    } else {
                        stream << ",\n";
                        if (depth)
                            stream << formatIndent(depth);
                    }
                } else if (halfPretty) {
                    stream << ", ";
                } else {
                    stream << ',';
                }
            } else if (depth && pretty) {
                stream << formatIndent(depth);
            }
            break;
        }
        delete node;
        if (mMaxLength && stream.tellp() > mMaxLength) {
            truncated = true;
            nodes.deleteAll();
            break;
        }
    }
    assert(nodes.isEmpty());
}

const char *JSONFormatter::mimeType() const
{
    return "application/json";
}

std::string JSONFormatter::formatIndent(int indent)
{
    std::string result;
    for(int i = 0; i < indent; ++i)
        result += "   ";
    return result;
}
