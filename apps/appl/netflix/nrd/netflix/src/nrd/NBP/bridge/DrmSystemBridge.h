/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DRMSYSTEMBRIDGE_H
#define DRMSYSTEMBRIDGE_H

#include <nrd/NfObject.h>
#include <nrdbase/Variant.h>
#include <DrmManager.h>

namespace netflix {

namespace device {
class IDrm;
}


class DrmSystemBridge : public NfObject
{
public:
    DrmSystemBridge();
    virtual ~DrmSystemBridge();

    virtual unsigned requestFlags(const Request &request) const;
    virtual bool getProperty(int index, Variant *result) const;
    virtual Variant invoke(int method, const Variant &args);

private:

    // drm system
    bool openDrmSystem(Variant &result, int method, const Variant& var);
    bool closeDrmSystem(Variant &result, int method, const Variant& var);
    std::string getDrmType(Variant &result, int method, const Variant& var);

    // secure stop invoke handlers
    bool commitSecureStop(Variant &result, int method, const Variant &var);
    bool getSecureStop(Variant &result, int method, const Variant &var);
    bool getSecureStopIds(Variant &result, int method, const Variant &var);
    bool resetSecureStops(Variant &result, int method, const Variant &var);
    bool enableSecureStop(Variant &result, int method, const Variant &var);
    bool isSecureStopEnabled(Variant &result, int method, const Variant &var);

    // drm deleteion invoke handlers
    bool deleteDrmStore(Variant &result, int method, const Variant &var);
    bool deleteKeyStore(Variant &result, int method, const Variant &var);
    bool getDrmStoreHash(Variant &result, int method, const Variant &var);
    bool getKeyStoreHash(Variant &result, int method, const Variant &var);
    time_t getDrmTime(Variant &result, int method, const Variant &var);

    // license handling bridge functions
    // below two calls are for JS to call.
    bool generateChallenge(Variant &result, int method, const Variant& var);
    bool provideLicense(Variant &result, int method, const Variant& var);
    bool deleteDrmSession(Variant &result, int method, const Variant& var);
    bool flushDrmSessions(Variant &result, int method, const Variant& var);
    bool getLdlSessionsLimit(Variant &result, int method, const Variant& var);
    bool getDrmSessionIds(Variant &result, int method, const Variant & var);

private:
    DrmManager* m_drm;
    bool m_drmSystemOpened;
    Mutex m_drmSystemBridgeMutex;
};

} // namespace netflix

#endif // DRMSYSTEMBRIDGE_H
