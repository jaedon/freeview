/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __GIBBONBASEBRIDGE_H__
#define __GIBBONBASEBRIDGE_H__

#include <nrd/Request.h>
#include <nrd/NfObject.h>

namespace netflix {
namespace gibbon {

enum {
    DumpProperty = 1U << 31
};

class WidgetBridge;
class GibbonBridgeTimer;
class GibbonBridgeRequest;

class GibbonBaseBridge : public NfObject
{
public:
    enum ClassType {
        UnknownType = 0,
        WidgetType = 1,
        EffectType = 2,
        ImageType  = 3,
        TextType   = 4
    };

    inline GibbonBaseBridge(const std::string &name, const PropertyInfo *props, int propertyCount,
                            const char **methodList, int methodCount, unsigned int flags = Flag_None)
        : NfObject(name, props, propertyCount, methodList, methodCount, flags),
          mSetProperties(0), mPendingPropertiesSync(0)
    {
    }

    bool haveProperty(int index) const;
    void setHaveProperty(int index);

    void syncPendingProperties();
    void propertiesChanged(unsigned int properties, bool autoSync=true);

    virtual void setProperties(int index, const Variant &value) {
        assert(value.isArray());
        for(Variant::ArrayIterator it = value.arrayBegin(); it != value.arrayEnd(); ++it)
            setProperty(index, (*it));
    }

    virtual unsigned requestFlags(const Request &/*request*/) const { return Request::Synchronous; }

    virtual ClassType classType() const { return UnknownType; }
    virtual uint32_t classID() const { return 0; }
    virtual void objectId(std::string &key, Variant &id) const;

protected:
    virtual void syncPendingProperties(unsigned int) { }

    uint64_t mSetProperties;
    mutable unsigned int mPendingPropertiesSync;
};

}} // namespace netflix::gibbon

#endif /* __GIBBONBASEBRIDGE_H__ */
