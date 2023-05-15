/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DUMPER_H
#define DUMPER_H

#include <nrdbase/Variant.h>
#include <assert.h>
#include <map>
#include <string>
#include <vector>

namespace netflix {

class Dumper
{
public:
    enum KeyFlags {
        None = 0x0,
        Verbose = 0x1
    };

    class Key
    {
    public:
        Key(const char* key, unsigned int flags = None);
        Key(const std::string& key, unsigned int flags = None);

        std::string key;
        unsigned int flags;
    };

    class Keys
    {
    public:
        Keys& operator<<(const Key& key);

    private:
        Keys(Dumper* dumper);

        friend class Dumper;

    private:
        Dumper* mDumper;
    };

    Keys keys() { return Keys(this); }

    class Values
    {
    public:
        Values& operator<<(const Variant& value);

    private:
        struct Data
        {
            ~Data();

            Dumper* dumper;
            size_t keyIdx;
            std::string key;
            Variant variant;
        };

        Values(Dumper* dumper, const std::string& key = std::string());

        friend class Dumper;

    private:
        shared_ptr<Data> mData;
    };

    Dumper() : mVariantPtr(&mVariant["values"]) { }
    virtual ~Dumper() { }

    Values values(const std::string& key) { return Values(this, key); }
    Values values() { return Values(this); }
    Variant& operator[](const std::string& key) { assert(key != "values"); return mVariant[key]; }
    unsigned int valuesSize() const { return mVariant["values"].size(); }

    Variant variant() const { return mVariant; }

    void next(const std::string& name);

    std::vector<std::string> toConsole();
    std::vector<std::string> toHTML();

protected:
    virtual std::vector<std::string> consoleEntry(const unsigned int idx, const std::string& section, const Variant& value) = 0;
    virtual std::vector<std::string> consolePostProcess(const Variant& values)
    {
        (void)values;
        return std::vector<std::string>();
    }
    virtual std::vector<std::string> htmlEntry(const unsigned int idx, const std::string& section, const Variant& value)
    {
        (void)idx;
        (void)section;
        (void)value;
        return std::vector<std::string>();
    }
    virtual std::vector<std::string> htmlPostProcess(const Variant& values)
    {
        (void)values;
        return std::vector<std::string>();
    }

    std::vector<std::string> one(const std::string& str)
    {
        std::vector<std::string> ret;
        ret.push_back(str);
        return ret;
    }

private:
    std::vector<Key> mKeys;
    std::map<std::string, unsigned int> mFlags;
    Variant mVariant;
    Variant* mVariantPtr;

    friend class Keys;
    friend class Values;
};

inline Dumper::Key::Key(const std::string& k, unsigned int f)
    : key(k), flags(f)
{
}

inline Dumper::Key::Key(const char* k, unsigned int f)
    : key(k), flags(f)
{
}

} // namespace netflix

#endif
