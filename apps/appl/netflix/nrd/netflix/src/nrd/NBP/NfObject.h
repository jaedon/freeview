/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NfObject_h
#define NfObject_h

#include <string.h>
#include <string>
#include <set>
#include <vector>

#include <nrdbase/Variant.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Time.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/StringUtils.h>

#include <nrd/ApplicationEvent.h>

namespace netflix {
class NrdApplication;
class Request;
class EventConnection;

class NfObject : public Noncopyable, public enable_shared_from_this<NfObject>
{
public:
    enum PropertyFlag {
        ReadOnly  = (1 << 0),
        WriteOnly = (1 << 1),
        ReadWrite = ReadOnly|WriteOnly
    };

    struct PropertyInfo {
        const char *name;
        uint32_t flags;

        operator const char *() const { return name; }
    };

    NfObject(const std::string &name,
             const PropertyInfo *props,
             int propertyCount,
             const char **methodList = 0,
             int methodCount = 0,
             unsigned int flags = 0);
    NfObject(const std::string &name,
             const char **methodList,
             int methodCount,
             unsigned int flags = 0);
    NfObject(const std::string &name,
             const PropertyInfo *props,
             const char **methodList,
             unsigned int flags = 0);
    NfObject(const std::string &name, unsigned int flags = 0);

    virtual ~NfObject();

    enum Flag {
        Flag_None = 0x0,
        Flag_NoPropertySync = 0x1
    };

    unsigned int flags() const { return mFlags; }

    const PropertyInfo *properties() const;
    int propertyCount() const;
    const char **methods() const;
    int methodCount() const;

    struct ClassInfo {
        ClassInfo()
            : properties(0), methods(0)
        {}

        const PropertyInfo *properties;
        const char **methods;
    };
    static inline const std::map<std::string, ClassInfo> &getRegisteredClasses() { return *sClassInfo; }
    static int registerProperties(const std::string &className, const PropertyInfo *props);
    static int registerMethods(const std::string &className, const char **props);

    const char *methodName(int idx) const { return idx < mMethodCount ? mMethodList[idx] : ""; }
    virtual unsigned requestFlags(const Request &) const { return 0; } // flags from NBP/Request.h
    Variant send(const Request &request);
    Variant invoke(const Request &request);
    virtual Variant invoke(int, const Variant &) { return false; }

    const char *propertyName(int idx) const { return idx < mPropertyCount ? mProperties[idx].name : ""; }
    int methodIndex(const std::string &name) const;
    int propertyIndex(const std::string &name) const;
    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    bool setProperty(const std::string &name, const Variant &value)
    { return setProperty(propertyIndex(name), value); }

    void propertyUpdated(int index, unsigned int flags = 0) { propertiesUpdated(&index, 1, flags); }
    void propertiesUpdated(const int *indexes, int count, unsigned int flags = 0);
    Variant properties(const int *indexes, int count) const;

    void setParent(const shared_ptr<NfObject> &parent);
    inline void addChild(const shared_ptr<NfObject> &object) { assert(object); object->setParent(shared_from_this()); }
    shared_ptr<NfObject> parent() const { ScopedMutex lock(sMutex); return mParent.lock(); }

    int sendEvent(const std::string& eventName,
                  const Variant &eventData = Variant(),
                  unsigned int flags = 0, const Time& time = Time::mono());
    virtual void objectId(std::string &key, Variant &id) const { key = "object"; id = path(); }
    inline const std::string &path() const { ScopedMutex lock(sMutex); return mPath.empty() ? mName : mPath; }
    inline const std::string &name() const { return mName; }
    inline shared_ptr<NfObject> child(const std::string &name)
    {
        ScopedMutex lock(sMutex);
        const ObjectMap::const_iterator it = mChildren.find(name.c_str());
        return it == mChildren.end() ? shared_ptr<NfObject>() : it->second;
    }
    static Mutex &mutex() { return sMutex; }

    Variant syncProperties() const;
    void sync(const shared_ptr<EventConnection> &connection = shared_ptr<EventConnection>()) const;
    void invalidArgumentError(const std::string &method, const char *argument);
    void invalidArgumentError(int index, const char *argument) { invalidArgumentError(methodName(index), argument); }
    struct NfObjectComparator {
        inline bool operator()(const char *l, const char *r) const { return strcmp(l, r) < 0; }
    };
    typedef std::map<const char *, shared_ptr<NfObject>, NfObjectComparator > ObjectMap;
    ObjectMap children() const { ScopedMutex lock(sMutex); return mChildren; }

    // SystemData listener
    void startEventListener();
    void stopEventListener();
    virtual void event(ApplicationEvent::Type, const Variant &) {}
protected:
    void clearChildren();

private:
    void updateHierarchy(const std::string &parentPath);

    weak_ptr<NfObject> mParent;
    const std::string mName;
    std::string mPath;
    static Mutex sMutex;
    ObjectMap mChildren;
    const PropertyInfo *mProperties;
    int mPropertyCount;
    const char **mMethodList;
    int mMethodCount;
    const unsigned int mFlags;
    shared_ptr<ApplicationEventListener> mListener;
    static std::map<std::string, ClassInfo> *sClassInfo;
    friend class netflix::NrdApplication;
};


#define DeclareMethodEnum(METHODNAME, ...) METHODNAME,
#define DeclareMethodTableEntry(METHODNAME, ...) #METHODNAME,

#define MethodTable(OBJECT, METHODLIST)                                 \
    namespace Methods {                                                 \
    namespace OBJECT {                                                  \
    enum Type {                                                         \
        METHODLIST(DeclareMethodEnum)                                   \
        MethodCount                                                     \
    };                                                                  \
    static const char *methods[] = {                                    \
        METHODLIST(DeclareMethodTableEntry)                             \
        0                                                               \
    };                                                                  \
    }}                                                                  \
    static int registered_methods ## METHODLIST = netflix::NfObject::registerMethods(#OBJECT, Methods::OBJECT::methods); \
    static void call_registered_methods_a ## METHODLIST();              \
    static inline void call_registered_methods_b ## METHODLIST()        \
    {                                                                   \
        NRDP_UNUSED(registered_methods ## METHODLIST);                  \
        call_registered_methods_a ## METHODLIST();                      \
    }                                                                   \
    static inline void call_registered_methods_a ## METHODLIST()        \
    {                                                                   \
        call_registered_methods_b ## METHODLIST();                      \
    }

// ---------------------------------------------------------------------------
// property table declarations

/*
 * Example:
 *
 *  #define DevicePropertyList(Property) \
 *          Property(FOO, ReadOnly) \
 *          Property(BAR, ReadWrite)
 *
 *  PropertyTable(DevicePropertyList, properties)
 */

#define DeclareEnum(PROPNAME, READONLY)         \
    PROPNAME,

#define DeclareEnums(PROPLIST)                  \
    enum Type {                                 \
        PROPLIST(DeclareEnum)                   \
        PropertyCount };

#define DeclarePropTableEntry(PROPNAME, FLAGS)  \
    { #PROPNAME, FLAGS },

#define DeclarePropTable(PROPLIST, STRINGLISTVAR)               \
    static netflix::NfObject::PropertyInfo STRINGLISTVAR [] = { \
        PROPLIST(DeclarePropTableEntry)                         \
        {0, 0} };

#define PropertyTable(OBJECT, PROPLIST, STRINGLISTVAR)                  \
    namespace Properties {                                              \
    namespace OBJECT {                                                  \
    DeclareEnums(PROPLIST)                                              \
    DeclarePropTable(PROPLIST, STRINGLISTVAR)                           \
    }}                                                                  \
    static int registered_properties ## PROPLIST = netflix::NfObject::registerProperties(#OBJECT, Properties::OBJECT::properties); \
    static inline void call_registered_properties_a ## PROPLIST();      \
    static inline void call_registered_properties_b ## PROPLIST()       \
    {                                                                   \
        NRDP_UNUSED(registered_properties ## PROPLIST);                      \
        call_registered_properties_a ## PROPLIST();                     \
    }                                                                   \
    static inline void call_registered_properties_a ## PROPLIST()       \
    {                                                                   \
        call_registered_properties_b ## PROPLIST();                     \
    }
}

#endif
