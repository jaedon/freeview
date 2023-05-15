/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TextScriptItemizer_H
#define TextScriptItemizer_H

#include "TextItemizerCommon.h"
#include <string>

#include <unicode/utypes.h>
#include <unicode/uobject.h>
#include <unicode/uscript.h>

namespace netflix {
namespace gibbon {

class TextScriptRun : public BaseTextRun {
public:
    typedef shared_ptr<TextScriptRun> SharedPtr;
    TextScriptRun(uint32_t _start, uint32_t _end, const std::string &_script) : BaseTextRun(_start, _end), script(_script) {}
    virtual ~TextScriptRun() {}

    std::string script;
};

class TextScriptRuns : public BaseTextRuns<TextScriptRun::SharedPtr> {
public:
    typedef std::vector<TextScriptRun::SharedPtr> TextScriptRunsList;
    typedef BaseTextRuns<TextScriptRun::SharedPtr> Base;
    void addRun(uint32_t start, uint32_t end, const std::string &script);

    TextScriptRuns() : Base() {}
#if CAPTURE_COPY_CONSTRUCTORS
    TextScriptRuns(const TextScriptRuns&) : Base() {
        assert(false);
    }
#endif
};

class TextScriptItemizer {
public:
    static void processScripts(TextScriptRuns& textRuns, icu::UnicodeString &text);
};

//-----------------------------------------------------------------------------

inline std::ostream &operator<< (std::ostream &out, const TextScriptRun& run)
{
    out << BASE_ITEMIZER_STREAM(run) << ": " << run.script;
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextScriptRun* run)
{
    out << (*run);
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextScriptRuns& runs)
{
    TextScriptRuns::TextScriptRunsList::const_iterator it = runs.get().begin();
    TextScriptRuns::TextScriptRunsList::const_iterator end = runs.get().end();

    out << "Text script runs" << std::endl;
    int i = 0;
    for (; it != end; it++) {
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << it->get() << std::endl;
        ++i;
        out.flush();
    }

    return out;
}


// scrptrun.h -----------------------------------------------------------------
/*
 *******************************************************************************
 *
 *   Copyright (C) 1999-2003, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *******************************************************************************
 *   file name:  scrptrun.h
 *
 *   created on: 10/17/2001
 *   created by: Eric R. Mader
 */

struct ScriptRecord
{
    UChar32 startChar;
    UChar32 endChar;
    UScriptCode scriptCode;
};

struct ParenStackEntry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

class ScriptRun : public icu::UObject {
public:
    ScriptRun();

    ScriptRun(const UChar chars[], int32_t length);

    ScriptRun(const UChar chars[], int32_t start, int32_t length);

    void reset();

    void reset(int32_t start, int32_t count);

    void reset(const UChar chars[], int32_t start, int32_t length);

    int32_t getScriptStart();

    int32_t getScriptEnd();

    UScriptCode getScriptCode();

    UBool next();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    virtual inline UClassID getDynamicClassID() const { return getStaticClassID(); }

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    static inline UClassID getStaticClassID() { return (UClassID)&fgClassID; }

private:

    static UBool sameScript(int32_t scriptOne, int32_t scriptTwo);

    int32_t charStart;
    int32_t charLimit;
    const UChar *charArray;

    int32_t scriptStart;
    int32_t scriptEnd;
    UScriptCode scriptCode;

    ParenStackEntry parenStack[128];
    int32_t parenSP;

    static int8_t highBit(int32_t value);
    static int32_t getPairIndex(UChar32 ch);

    static UChar32 pairedChars[];
    static const int32_t pairedCharCount;
    static const int32_t pairedCharPower;
    static const int32_t pairedCharExtra;

    /**
     * The address of this static class variable serves as this class's ID
     * for ICU "poor man's RTTI".
     */
    static const char fgClassID;
};

inline ScriptRun::ScriptRun()
{
    reset(NULL, 0, 0);
}

inline ScriptRun::ScriptRun(const UChar chars[], int32_t length)
{
    reset(chars, 0, length);
}

inline ScriptRun::ScriptRun(const UChar chars[], int32_t start, int32_t length)
{
    reset(chars, start, length);
}

inline int32_t ScriptRun::getScriptStart()
{
    return scriptStart;
}

inline int32_t ScriptRun::getScriptEnd()
{
    return scriptEnd;
}

inline UScriptCode ScriptRun::getScriptCode()
{
    return scriptCode;
}

inline void ScriptRun::reset()
{
    scriptStart = charStart;
    scriptEnd   = charStart;
    scriptCode  = USCRIPT_INVALID_CODE;
    parenSP     = -1;
}

inline void ScriptRun::reset(int32_t start, int32_t length)
{
    charStart = start;
    charLimit = start + length;

    reset();
}

inline void ScriptRun::reset(const UChar chars[], int32_t start, int32_t length)
{
    charArray = chars;

    reset(start, length);
}

// scrptrun.h -----------------------------------------------------------------


} /* namespace gibbon */
} /* namespace netflix */

#endif /* TextScriptItemizer_H */
