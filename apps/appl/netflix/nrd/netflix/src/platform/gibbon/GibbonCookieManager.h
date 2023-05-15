#ifndef GibbonCookieManager_h
#define GibbonCookieManager_h

#include <nrdbase/Mutex.h>
#include <nrdbase/Variant.h>
#include <nrdnet/AseUrl.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <string>

namespace netflix {
namespace gibbon {

class GibbonCookieManager
{
public:
    GibbonCookieManager();

    /* once secure store is available */
    void load();

    /* clear the cookies */
    void clear();

    /* headers coming from HTTP */
    void processHeaders(const AseUrl &url, const HttpResponseHeaders &headers);

    /* set from JS */
    bool processCookie(const AseUrl &url, const std::string &cookie);

    /* reading the values */
    enum Flag {
        None = 0x0,
        HttpOnly = 0x1,
        Trusted = 0x2
    };
    std::string cookies(const AseUrl &location, unsigned int flags);

    /* for gibbon console */
    void dump();

    void setFilters(const std::vector<std::string> &filters)
    {
        ScopedMutex lock(mMutex);
        mFilters = filters;
    }
    std::vector<std::string> filters() const
    {
        ScopedMutex lock(mMutex);
        return mFilters;
    }

private:

    mutable Mutex mMutex;
    Variant mCookies;
    std::vector<std::string> mFilters;
};
}
}

#endif
