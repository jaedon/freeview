/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SecureStore.h"

#include <set>
#include <iterator>
#include <map>
#include <openssl/md5.h>

#include <nrdbase/Base64.h>
#include <nrdbase/LexicalCast.h>
#include <nrdbase/ScopedMutex.h>

#include "ISystem.h"
#include "NrdApplication.h"
#include "ConfigurationKeys.h"

using namespace netflix;

#define CONFIG_VERSION 1

// some system keys
static const std::string s_version("ver");
static const std::string s_writeCount("wc");

static std::string s_separator = "/";
static std::string s_systemKey = "s";


namespace {

//////////
//
// CHECKSUM
//
//////////

// This is the name of the attribute that identifies the checksum.
static const std::string CHECKSUM_ATTRIBUTE("CHECKSUM");

// Calculates the MD5 of a map, and returns a base-64 encoding of
// the digest.
static std::string calculateChecksum(const std::map<std::string, std::string> &m)
{
    MD5_CTX md5;
    MD5_Init(&md5);

    std::map<std::string, std::string>::const_iterator i;
    std::map<std::string, std::string>::const_iterator const end = m.end();
    for (i = m.begin(); i != end; ++i)
    {
        MD5_Update(&md5, i->first.data(), i->first.length());
        MD5_Update(&md5, i->second.data(), i->second.length());
    }

    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_Final(digest, &md5);
    std::string value;
    Base64::encode(digest, digest + sizeof digest, back_inserter(value));
    return value;
}

//////////
//
// helpers
//
//////////

static std::string makeKey(const std::string &account, const std::string &name)
{
    if (account.empty())
        return name;
    return account + s_separator + name;
}

static bool mapHas(const std::map<std::string, std::string> &values, const std::string &key)
{
    std::map<std::string, std::string>::const_iterator it = values.find(key);
    return (it != values.end());
}

static bool mapUnset(/*out*/std::map<std::string, std::string> &values, const std::string &key)
{
    std::map<std::string, std::string>::iterator it = values.find(key);
    if (it != values.end())
    {
        values.erase(it);
        return true; // std::map changed
    }

    return false; // unchanged
}

} // namespace anonymous

//////////
//
// SecureStore
//
//////////
SecureStore::SecureStore() :
    m_mutex(SECURESTORE_MUTEX, "SecureStore"),
    m_writeCount(0),
    m_flushCount(0),
    m_dirty(false)
{
    // load the values from the system
    load();
}

void SecureStore::shutdown()
{
    Log::debug(TRACE_SECURE_STORE, "SecureStore shutdown called");

}
//////////
//
// calculateUsedSpace
//
//////////
uint32_t SecureStore::calculateUsedSpace() const
{
    uint32_t usedSpace = 0;

    ScopedMutex lock(m_mutex);

    std::map<std::string, std::string>::const_iterator it;
    for (it = m_values.begin(); it != m_values.end(); ++it)
    {
        usedSpace += it->first.size() + it->second.size();
    }

    return usedSpace;
}

//////////
//
// clear
//
//////////
void SecureStore::clear()
{
    NTRACE(TRACE_SECURE_STORE, "SecureStore::clear() called");
    // else clear our values and commit
    ScopedMutex lock(m_mutex);
    m_values.clear();
    commit();
}

//////////
//
// get
//
//////////

template <>
std::string SecureStore::get(SystemKeys::Key key) const
{
   return getByAccount(s_systemKey, SystemKeys::toString(key));
}

std::string SecureStore::getByAccount(const std::string &accountKeyParam, const std::string &key) const
{
    // if no current account, use the "0" account
    std::string accountKey = !accountKeyParam.empty() ? accountKeyParam : "0";
    std::string secureKey(makeKey(accountKey, key));

    // else get directly from our store
    ScopedMutex lock(m_mutex);
    std::map<std::string, std::string>::const_iterator it = m_values.find(secureKey);
    if (it != m_values.end())
        return it->second;

    return std::string();
}

//////////
//
// set
//
//////////

template<>
void SecureStore::set(SystemKeys::Key key, const std::string &value)
{
    setByAccount(s_systemKey, SystemKeys::toString(key), value);
}

void SecureStore::set(SystemKeys::Key key, const char *value)
{
    set(key, std::string(value));
}

void SecureStore::setByAccount(const std::string &accountKeyParam, const std::string &key,
                               const std::string &value)
{
    // if no current account, use the "0" account
    std::string accountKey = !accountKeyParam.empty() ? accountKeyParam : "0";
    std::string secureKey(makeKey(accountKey, key));

    NTRACE(TRACE_SECURE_STORE, "setByAccount %s: %s = %s", accountKey.c_str(), key.c_str(), value.c_str());

    // else store directly into our store and commit
    ScopedMutex lock(m_mutex);

    // Don't commit the std::map unless absolutely necessary
    // It is necessary only if:
    // (1) the name doesn't yet exist, or
    // (2) it exists, but is not currently set to the correct value.
    std::map<std::string, std::string>::const_iterator it = m_values.find(secureKey);
    if ((it == m_values.end()) || (it->second != value))
    {
        m_values[secureKey] = value;

        commit();
    }
}

//////////
//
// unset
//
//////////

void SecureStore::unset(SystemKeys::Key key)
{
    unsetByAccount(s_systemKey, SystemKeys::toString(key));
}

void SecureStore::unsetByAccount(const std::string &accountKey, const std::string &key)
{
    std::string secureKey(makeKey(accountKey, key));

    NTRACE(TRACE_SECURE_STORE, "unsetByAccount %s: %s", accountKey.c_str(), key.c_str());

    // else get directly from our store
    ScopedMutex lock(m_mutex);
    if (mapUnset(m_values, secureKey))
    {
        // std::map changed, write out
        commit();
    }
}

//////////
//
// has
//
//////////

bool SecureStore::has(SystemKeys::Key key) const
{
    return hasByAccount(s_systemKey, SystemKeys::toString(key));
}

bool SecureStore::hasByAccount(const std::string &accountKey, const std::string &key) const
{
    std::string secureKey(makeKey(accountKey, key));

    // else get directly from our store
    ScopedMutex lock(m_mutex);
    return mapHas(m_values, secureKey);
}

//////////
//
// load
//
//////////
void SecureStore::load()
{
    shared_ptr<netflix::device::ISystem> system = nrdApp()->getSystem();
    ASSERT(system,
           "SecureStore::initialize called before system control was initialized. "
           "This is a serious problem, and it likely means that a static "
           "constructor is attempting to use the Configuration instance.");

    // read in values
    ScopedMutex lock(m_mutex);
    m_values = system->loadEncrypted();

    // Ensure that the checksum exists and is correct.
    // If it does not exist or is not correct, then the std::map is cleared of its invalid
    // information.
    if (!m_values.empty()) {
        std::map<std::string, std::string>::iterator it = m_values.find(CHECKSUM_ATTRIBUTE);
        if (it != m_values.end()) {
            const std::string checksum = it->second;
            m_values.erase(it); // remove checksum when calculating
            if (
#ifndef BUILD_PRODUCTION
                (checksum != CHECKSUM_ATTRIBUTE) && // back-door for hand edits
#endif
                (checksum != calculateChecksum(m_values))) {
                Log::error(TRACE_SECURE_STORE, "Invalid checksum, clearing secure store");
                m_values.clear(); // invalid checksum
            }
        } else {
            Log::error(TRACE_SECURE_STORE, "No checksum found in secure store, clearing secure store");
            m_values.clear(); // no checksum
        }
    } else {
        Log::Message m(TRACE_SECURE_STORE, Log::Info, "securestore");
        m.m_message = "No secure store found";
        m.send();
    }

    // remember the write count
    m_writeCount = lexical_cast<uint32_t>(m_values[s_writeCount]);

    // crash counter
    if (Configuration::crashReportShutdowns()) {
        std::string nrdStarted = makeKey(s_systemKey, SystemKeys::toString(SystemKeys::nrdStarted));
        std::map<std::string, std::string>::iterator it1 = m_values.find(nrdStarted);
        if (it1 != m_values.end()) {
            std::string crashCountKey = makeKey(s_systemKey, SystemKeys::toString(SystemKeys::crashCount));
            uint32_t crashCount = lexical_cast<uint32_t>(m_values[crashCountKey]);
            m_values[crashCountKey] = lexical_cast<std::string>(++crashCount);
        }
    }

    // Regenerate the persistent information.  This is done in case the settings were
    // missing or created from backup. (Also, the checksum is recalculated in case the
    // checksum back-door was used.)
    commit();
    m_dirty = false;
}

//////////
//
// commit
//
//////////
void SecureStore::commit()
{
    ASSERT(MutexStack::locked(m_mutex.name()), "SecureStore lock must be acquired before commit");

    // bump the write count
    m_values[s_writeCount] = lexical_cast<std::string>(++m_writeCount);

    // add the version
    m_values[s_version] = lexical_cast<std::string>(CONFIG_VERSION);

    // calculate checksum
    std::string checksum(calculateChecksum(m_values));
    m_values[CHECKSUM_ATTRIBUTE] = checksum;

    shared_ptr<netflix::device::ISystem> system = nrdApp()->getSystem();
    ASSERT(system,
           "SecureStore::commit called before system control was initialized. "
           "This is a serious problem, and it likely means that a static "
           "constructor is attempting to use the Configuration instance.");

    system->storeEncrypted(m_values);

    // remove the checksum from our local std::map
    mapUnset(m_values, CHECKSUM_ATTRIBUTE);

    m_dirty = true;
}

//////////
//
// flush
//
//////////
void SecureStore::flushEncrypted()
{
    ScopedMutex lock(m_mutex);
    if (m_dirty) {
        shared_ptr<netflix::device::ISystem> system = nrdApp()->getSystem();
        system->flushEncrypted();
        m_flushCount++;
        m_dirty = false;
    }
}

