/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __SECURE_STORE_H__
#define __SECURE_STORE_H__

#include <nrdbase/LexicalCast.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Noncopyable.h>

#include <map>

#include "ConfigurationKeys.h"

namespace netflix {


//////////
//
// SecureStore
//
//////////
class SecureStore : private Noncopyable, public enable_shared_from_this<SecureStore>
{
public:
    SecureStore();
    virtual ~SecureStore() {}
    void shutdown();
    /**
     * Returns the amount of characters stored.
     * Note that this is just the sum of all the keys and values and does not account
     * for any overhead of the internal structures.
     */
    uint32_t calculateUsedSpace() const;

    uint32_t getWriteCount() { return m_writeCount; }

    uint32_t getFlushCount() { return m_flushCount; }

    /**
     * Clears the configuration of all key, value pairs
     */
    void clear();

    /**
     * Get the value of the specified attribute from the configuration.
     *
     * @param[in] key the key of the attribute.
     * @return the value associated with the specified attribute.
     */
    template <typename valuetype_>
    valuetype_ get(SystemKeys::Key key) const
    {
        return lexical_cast<valuetype_>(get<std::string>(key));
    }

    /**
     * Set the value of the specified attribute in the configuration.
     *
     * @param[in] name the name of the attribute.
     * @param[in] value the value of the attribute.
     */
    template <typename valuetype_>
    void set(SystemKeys::Key key, const valuetype_ &value)
    {
        set(key, lexical_cast<std::string>(value));
    }

    /**
     * This is a "transparent overload" in the sense that its existence
     * does not modify the behavior of this class.  But it does reduce
     * the amount of generated code.  If this wasn't provided, the
     * compiler would instantiate the templated set function for each
     * unique literal character array length.  (There would exist one
     * for char[2], char[3], char[4], etc.)
     *
     * @param[in] name the name of the attribute.
     * @param[in] value the value of the attribute.
     */
    void set(SystemKeys::Key key, const char *value);

    /**
     * Unsets or removes a setting from the configuration entirely.
     * Calling this function on a non-existent attribute has no
     * effect.
     *
     * @param[in] name the name of the attribute to unset.
     */
    void unset(SystemKeys::Key key);

    /**
     * Tests for existence of an attribute name in the current
     * configuration.
     *
     * @param[in] name the name of the attribute.
     * @return true if the attribute exists in the configuration.
     */
    bool has(SystemKeys::Key key) const;

/*private: logically private */
    /**
     * The following are a duplicate of all the get/set/has functions but with an
     * accountKey specified. The accountKey for the previous functions is extracted
     * from SystemData and that is the preferred method of calling.
     * However, should you need direct access to these methods for specific account,
     * use the following versions.
     * Note, be sure this is really what you want
     */
    std::string getByAccount(const std::string &accountKey,
                             const std::string &name) const;

    void setByAccount(const std::string &accountKey, const std::string &name,
                      const std::string &value);

    void unsetByAccount(const std::string &accountKey, const std::string &name);

    bool hasByAccount(const std::string &accountKey, const std::string &name) const;
    void flushEncrypted();
private:
    // Accessing values in the secure store
    std::string get(const std::string &key) const;
    void set(const std::string &key, const std::string &value);
    void unset(const std::string &key);
    bool has(const std::string &key) const;

private:
    void load();
    void commit();

private:
    mutable Mutex m_mutex;
    std::map<std::string, std::string> m_values;

    uint32_t m_writeCount;
    uint32_t m_flushCount;

    bool m_dirty;
};

// Declare explicit specializations for std::string.  All other instantiations
// of the respective templated functions forward to these:
template <>
std::string SecureStore::get(SystemKeys::Key key) const;

template <>
void SecureStore::set(SystemKeys::Key key, const std::string &value);


}

#endif // __SECURE_STORE_H__
