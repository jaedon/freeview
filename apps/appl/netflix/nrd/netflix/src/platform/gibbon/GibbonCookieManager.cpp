#include <nrdbase/Base64.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/StringCompressor.h>
#include "GibbonApplication.h"
#include "GibbonConfiguration.h"
#include "GibbonCookieManager.h"
#include "GibbonResourceManager.h"

using namespace netflix;
using namespace netflix::gibbon;

const char *cookie_key = "UI/cks";

GibbonCookieManager::GibbonCookieManager()
    : mMutex(GIBBON_COOKIE_MUTEX, "CookieManagerMutex", Mutex::NonRecursive), mFilters(GibbonConfiguration::cookieFilters())
{
}

void GibbonCookieManager::dump()
{
    ScopedMutex lock(mMutex);
    Variant var;
    var["cookies"] = mCookies;
    var["filters"] = mFilters;
    Log::warn(TRACE_RESOURCEMANAGER, "%s", var.toJSON(true).c_str());
}

void GibbonCookieManager::load()
{
    const std::string compressed = Base64::decode(GibbonApplication::instance()->readSystemConfiguration(SystemKeys::UICookies));
    if (compressed.empty()) {
        NTRACE(TRACE_RESOURCEMANAGER, "didn't load cookies because we didn't read any");
        clear();
        return;
    }

    if (compressed.size() < 3 || compressed[0] != 'C' || compressed[1] != 'K' || compressed[2] != 'S') {
        NTRACE(TRACE_RESOURCEMANAGER, "didn't load cookies because we didn't see CKS: %s", compressed.c_str());
        return;
    }
    std::string cookies;
    StringCompressor::inflate(cookies, compressed.substr(3));
    {
        ScopedMutex lock(mMutex);
        mCookies = Variant::fromJSON(cookies);
        int i = 0;
        while (i < mCookies.size()) {
            Variant &cookie = mCookies[i];
            if (!cookie["expires"].isInteger()) {
                NTRACE(TRACE_RESOURCEMANAGER, "expiration date for stored cookie is not a number");
                mCookies.remove(i);
            } else if (contains(mFilters, cookie.mapValue<std::string>("name"))) {
                NTRACE(TRACE_RESOURCEMANAGER, "Filtered out cookie %s filters: %s",
                       cookie.toJSON().c_str(), StringTokenizer::join(mFilters, ", ").c_str());
                mCookies.remove(i);
            } else {
                ++i;
            }
        }
        NTRACE(TRACE_RESOURCEMANAGER, "I think we read in %d cookies", i);
    }
    GibbonApplication::instance()->getBridge()->cookiesChanged();
}

void GibbonCookieManager::clear()
{
    {
        ScopedMutex lock(mMutex);
        mCookies.clear();
    }
    GibbonApplication::instance()->getBridge()->cookiesChanged();
}

void GibbonCookieManager::processHeaders(const AseUrl &url,
                                         const HttpResponseHeaders &headers)
{
    bool modified = false;
    for (size_t i = 0; i < headers.size(); ++i) {
        if (!strcasecmp(headers[i].header.c_str(), "Set-Cookie") && !headers[i].value.empty()) {
            modified = processCookie(url, headers[i].value) || modified;
        }
    }
    if (modified) {
        if(shared_ptr<GibbonBridge> bridge = GibbonApplication::instance()->getBridge())
            bridge->cookiesChanged();
    }
}

static bool domainSuffix(const std::string &host, const std::string &suffix)
{
    if (host == suffix)
        return true;
    if (suffix[0] == '.') {
        if (endsWith(host, suffix))
            return true;
    } else {
        if (endsWith(host, "." + suffix))
            return true;
    }

    return false;
}

bool GibbonCookieManager::processCookie(const AseUrl &url,
                                        const std::string &cookie)
{
    NTRACE(TRACE_RESOURCEMANAGER, "processing cookie from %s: %s", url.str().c_str(), cookie.c_str());

    std::vector<std::string> parts;
    StringTokenizer::tokenize(cookie, parts, ";");
    if (parts.size() < 1)
        return false;
    Variant newCookie;
    std::vector<std::string> nameValue;
    StringTokenizer::tokenize(parts[0], nameValue, "=");
    bool removeMissingValue = nameValue.size() < 2 || nameValue[1].empty();
    newCookie["name"] = nameValue[0];
    newCookie["value"] = removeMissingValue ? "" : nameValue[1];
    const Time now = Time::serverTime();

    bool seenMaxAge = false, seenExpiration = false;
    Time expiration = now;

    enum {
        DomainLength = 7,
        PathLength = 5,
        ExpiresLength = 8,
        MaxAgeLength = 8,
        SecureLength = 6,
        HttpOnlyLength = 8
    };
    for (size_t i = 1; i < parts.size(); i++) {
        trim(parts[i]);
        if (startsWithIgnoreCase(parts[i], "Domain=", DomainLength)) {
            std::string domain = parts[i].substr(DomainLength);
            if (domain.find('.') != domain.rfind('.') // supercookie, e.g. .com; must have two or more dots.
                && domainSuffix(url.GetHost(), domain)) {
                newCookie["domain"] = domain;
            } else {
                Log::warn(TRACE_RESOURCEMANAGER,
                          "blocking attempt to set cookie from domain %s for domain %s",
                          url.GetHost().c_str(), domain.c_str());
                return false;
            }
        } else if (startsWithIgnoreCase(parts[i], "Path=", PathLength)) {
            newCookie["path"] = parts[i].substr(PathLength);
        } else if (startsWithIgnoreCase(parts[i], "Expires=", ExpiresLength)) {
            if (seenMaxAge) //max-age takes precedence over expires
                continue;
            const std::string expirationString = parts[i].substr(ExpiresLength);
            expiration = Time::fromRFC1123(expirationString);
            seenExpiration = true;
        } else if (startsWithIgnoreCase(parts[i], "Max-Age=", MaxAgeLength)) {
            const unsigned long seconds = strtoul(parts[i].substr(MaxAgeLength).c_str(), 0, 10);
            expiration = now + Time::fromSeconds(seconds);
            seenMaxAge = true;
        } else if (startsWithIgnoreCase(parts[i], "Secure", SecureLength)) {
            newCookie["secure"] = true;
        } else if (startsWithIgnoreCase(parts[i], "HttpOnly", HttpOnlyLength)) {
            newCookie["http"] = true;
        }
    }

    bool expired = false;
    if (seenMaxAge || seenExpiration) {
        expired = now >= expiration;
        newCookie["expires"] = expiration.seconds();
        NTRACE(TRACE_RESOURCEMANAGER, "expiration(%s) date for new cookie is %llu %llu (%s)",
               seenMaxAge ? "max-age" : "expiration", expiration.seconds(), now.seconds(), expired ? "expired" : "fresh");
    }

    if (!newCookie.contains("domain"))
        newCookie["domain"] = url.GetHost();
    if (!newCookie.contains("path"))
        newCookie["path"] = "/"; //url.path();

    Variant cookies;
    {
        ScopedMutex lock(mMutex);
        if (contains(mFilters, nameValue[0])) {
            NTRACE(TRACE_RESOURCEMANAGER, "Filtered out cookie \"%s\" for url: %s filters: %s", nameValue[0].c_str(),
                   url.c_str(), StringTokenizer::join(mFilters, ", ").c_str());
            return false;
        }

        // replace any matching cookie
        bool found = false;
        const bool remove = removeMissingValue || expired;
        for (int j = 0; j < mCookies.size(); j++) {
            Variant &cookie = mCookies[j];
            if (cookie["domain"] == newCookie["domain"] &&
                cookie["path"] == newCookie["path"] &&
                cookie["name"] == newCookie["name"]) {
                NTRACE(TRACE_RESOURCEMANAGER,
                       "%s cookie %s for domain %s path %s",
                       remove ? "removing" : "replacing",
                       cookie["name"].string().c_str(),
                       cookie["domain"].string().c_str(),
                       cookie["path"].string().c_str());
                found = true;
                if (remove) {
                    mCookies.remove(j);
                    break;
                }
                if (cookie["value"] == newCookie["value"] &&
                    cookie["secure"] == newCookie["secure"] &&
                    cookie["http"] == newCookie["http"]) {
                    // nothing to do, it hasn't changed
                    return false;
                }
                cookie["value"] = newCookie["value"];
                cookie["secure"] = newCookie["secure"];
                cookie["http"] = newCookie["http"];
                break;
            }
        }
        if (found) {
            cookies = mCookies;
        } else if (!remove) {
            NTRACE(TRACE_RESOURCEMANAGER, "adding cookie %s for domain %s path %s",
                   newCookie["name"].string().c_str(),
                   newCookie["domain"].string().c_str(),
                   newCookie["path"].string().c_str());
            mCookies.push_back(newCookie);
            cookies = mCookies;
            // write new cookies immediately; some platforms don't shut down
            // cleanly and we need to guarantee that new cookies are written
        } else {
            return false;
        }
    }
    const std::string json = cookies.toJSON();
    std::string compressed;
    StringCompressor::deflate(compressed, json);
    GibbonApplication::instance()->writeSystemConfiguration(SystemKeys::UICookies, Base64::encode("CKS" + compressed));
    return true;
}

std::string GibbonCookieManager::cookies(const AseUrl &url, unsigned int flags)
{
    ScopedMutex lock(mMutex);

    std::ostringstream os;
    bool first = true;

    for (int i = 0; i < mCookies.size(); i++) {
        Variant &cookie = mCookies[i];
        // make sure domain matches
        if (!domainSuffix(url.GetHost(), cookie["domain"].string()))
            continue;
        // make sure path matches
        if (url.GetPath().empty()) {
            if (cookie["path"].string() != "/")
                continue;
        } else {
            if (!startsWith(url.GetPath(), cookie["path"].string()))
                continue;
        }
        // make sure secure matches
        if (!(flags & Trusted)
            && !url.IsHttps()
            && !GibbonResourceManager::isNBP(url)
            && !GibbonConfiguration::sendSecureCookies()
            && cookie.mapValue<bool>("secure")) {
            continue;
        }
        // make sure HttpOnly only gets sent to http
        if (!(flags & HttpOnly) && cookie["http"] == true)
            continue;

        // if it's expired, remove it
        if (cookie["expires"].isInteger()) {
            const Time now = Time::serverTime();
            Time expiration = Time::fromSeconds(cookie["expires"].integer());
            if (now > expiration) {
                NTRACE(TRACE_RESOURCEMANAGER, "expiration date for stored cookie is %llu %llu", expiration.seconds(), now.seconds());
                mCookies.remove(i);
                i--; // need to decrement since we've modified mCookies
                continue;
            }
        }

        if (!first) {
            os << "; ";
        }
        first = false;

        os << cookie["name"].string() << "=" << cookie["value"].string();
    }

    return os.str();
}
