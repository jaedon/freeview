/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include "Grep.h"
#include <assert.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Log.h>

using namespace netflix;

Grep::Grep(const std::vector<std::string> &args, unsigned flags)
{
    if (!args.empty())
        init(args, flags);
}
Grep::Grep(const std::string &pattern, unsigned int flags)
{
    init(pattern, flags);
}

void Grep::clear()
{
    mData.reset();
}

bool Grep::isValid() const
{
    return mData.get();
}

bool Grep::init(const std::vector<std::string> &args, unsigned int flags)
{
    clear();
    if(!Configuration::consoleSinkLogColor())
        flags |= NoColor;
    Configuration::Color color = Configuration::Invalid;
    std::string pattern;
    bool dashDash = false;
    for (size_t i=0; i<args.size(); ++i) {
        const std::string &arg = args.at(i);
        if (!dashDash) {
            bool handled = true;
            if (arg == "-v" || arg == "--invert-match") {
                flags |= Inverted;
            } else if (arg == "-i" || arg == "--ignore-case") {
                flags |= IgnoreCase;
            } else if (arg == "--highlight") {
                flags |= Highlight;
            } else if (arg == "--") {
                dashDash = true;
            } else if (arg == "--no-color") {
                flags |= NoColor;
            } else if (arg == "--color") {
                ++i;
                if(i != arg.size()) {
                    color = Configuration::toColor(args[i]);
                    if(color == Configuration::Invalid)
                        return false;
                }
            } else if (startsWith(arg, "--color=")) {
                color = Configuration::toColor(arg.substr(8));
                if(color == Configuration::Invalid)
                    return false;
            } else {
                handled = false;
            }
            if (handled)
                continue;
        }
        if (pattern.empty()) {
            pattern = arg;
        } else {
            return false;
        }
    }
    return init(pattern, flags, color);
}

bool Grep::init(const std::string &args, unsigned int flags, Configuration::Color color)
{
    if (args.empty())
        return false;
    if(!Configuration::consoleSinkLogColor())
        flags |= NoColor;
    flags |= Valid;
    mData.reset(new Data(args, flags, color));
#ifdef HAVE_REGEX
    if (regcomp(&mData->regexp, args.c_str(), flags & IgnoreCase ? REG_ICASE : 0)) {
        mData->flags = 0;
        clear();
        return false;
    }
#endif
    return true;
}

bool Grep::match(const std::string &line) const
{
    return match(line.c_str());
}

bool Grep::match(const std::vector<Grep> &grep, const std::string &line)
{
    for (std::vector<Grep>::const_iterator it = grep.begin(); it != grep.end(); ++it) {
        if (!it->match(line))
            return false;
    }
    return true;
}

static std::string findColor(const std::string &line, size_t start = std::string::npos)
{
    std::string color;
    size_t idx = start;
    while (true) {
        idx = line.rfind("\x1b[", idx);
        if (idx == std::string::npos)
            break;
        if (idx + 2 >= line.size()) {
            if (idx == 0)
                break;
            --idx;
            continue;
        }

        const char *const start = &line[idx];
        const char *ch = start + 2;
        if (*ch == '3') {
            ++ch;
            if (*ch < '0' || *ch > '7') {
                if (idx == 0)
                    break;
                --idx;
                continue;
            }
            ++ch;
            if (*ch != 'm' && strncmp(";1m", ch, 3) && strncmp(";0m", ch, 3)) {
                if (idx == 0)
                    break;
                --idx;
                continue;
            }
            if (*ch == 'm') {
                ++ch;
            } else {
                ch += 3;
            }
            return line.substr(idx, ch - start);
        } else if (*ch == '0') {
            ++ch;
            if (*ch == 'm') {
                return line.substr(idx, 4);
            } else if (*ch == ';') {
                return line.substr(idx, 6);
            }
        }
        if (idx == 0)
            break;
        --idx;
    }
    return std::string();
}

struct Chunk {
    int start, size;
    bool match;
};

static inline std::vector<Chunk> chunks(const std::string &string, const Grep &grep)
{
    const char *str = string.c_str();
    int index = 0;
    std::vector<Chunk> ret;
    int start = -1, size = -1;
    while (index < static_cast<int>(string.size()) && grep.match(str + index, &start, &size)) {
        if (index != start) {
            const Chunk c = { index, start, false };
            ret.push_back(c);
        }
        const Chunk c = { start + index, size, true };
        ret.push_back(c);
        index += start + size;
    }
    if (index < static_cast<int>(string.size())) {
        const Chunk c = { index, (int)string.size() - index, false };
        ret.push_back(c);
    }
    // printf("%s => %d\n", string.c_str(), ret.size());
    // for (std::vector<Chunk>::const_iterator it = ret.begin(); it != ret.end(); ++it) {
    //     printf("   %d-%d %s\n", it->start, it->size, it->match ? "true" : "false");
    // }
    // printf("\n");

    return ret;
}

bool Grep::filter(std::string &text, const Configuration::ColorScheme &colorScheme) const
{
    if (!isValid() || text.empty())
        return true;

    const bool color = !(mData->flags & NoColor) && (!colorScheme.isNull() || mData->color != Configuration::Invalid);
    std::string currentColor = color ? Configuration::colorToEscapeCode(Configuration::Default) : std::string();
    const char *resetColor = Configuration::colorToEscapeCode(Configuration::Default);
    const char *grepColor = color ? Configuration::colorToEscapeCode(mData->color == Configuration::Invalid ? colorScheme.grep : mData->color) : 0;

    const std::string temp = text;
    const std::vector<Chunk> chunks = ::chunks(temp, *this);
    assert(!chunks.empty());
    if (chunks.size() > 1 || chunks[0].match) {
        if (color) {
            std::ostringstream output;
            for (std::vector<Chunk>::const_iterator chunk = chunks.begin(); chunk != chunks.end(); ++chunk) {
                if (chunk->match) {
                    const std::string col = findColor(text, chunk->start);
                    if (!col.empty())
                        currentColor = col;
                    output << resetColor << grepColor;
                }
                output.write(text.c_str() + chunk->start, chunk->size);
                if (chunk->match) {
                    output << resetColor << currentColor;
                }
            }
            const std::string col = findColor(text);
            if (!col.empty())
                currentColor = col;
            text = output.str();
            return true;
        } else {
            return true;
        }
    }
    return mData->flags & Highlight;
}

bool Grep::match(const char *line, int *start, int *length) const
{
    bool match = true;
    if (mData) {
        assert(mData->flags);
#ifdef HAVE_REGEX
        regmatch_t matches[1];
        match = !regexec(&mData->regexp, line, 1, matches, 0);
        if (match && !(mData->flags & Inverted)) {
            if (start)
                *start = matches[0].rm_so;
            if (length)
                *length = matches[0].rm_eo - matches[0].rm_so;
        }
#else
        const char *found;
        if (mData->flags & IgnoreCase) {
            found = strcasestr(line, mData->pattern.c_str());
        } else {
            found = strstr(line, mData->pattern.c_str());
        }
        match = found;
        if (!(mData->flags & Inverted) && found) {
            if(start)
                *start = found - line;
            if (length)
                *length = mData->pattern.size();
        }
#endif
        if (mData->flags & Inverted) {
            match = !match;
            if(match) {
                if(start)
                    *start = 0;
                if (length)
                    *length = strlen(line);
            }
        }
    } else {
        match = false;
    }
    if (!match) {
        if (start)
            *start = -1;
        if (length)
            *length = -1;
    }

    return match;
}

std::string Grep::describe() const
{
    if (!mData)
        return "grep(invalid)";
    std::ostringstream str;
    if(mData->flags & Highlight)
        str << "highlite";
    else
        str << "grep";
    str << "(";
    if (mData->flags & Inverted)
        str << "-v ";
    if (mData->flags & IgnoreCase)
        str << "-i ";
    if (mData->flags & NoColor) {
        str << "--no-color ";
    } else if (mData->color != Configuration::Invalid) {
        str << "--" << Configuration::colorToEscapeCode(mData->color) << "color " << Configuration::colorToEscapeCode(Configuration::Default);
    }
    str << '\'' << mData->pattern << "')";
    return str.str();
}

