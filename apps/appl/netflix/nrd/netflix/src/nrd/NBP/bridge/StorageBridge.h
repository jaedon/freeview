/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef StorageBridge_h
#define StorageBridge_h

#include <nrd/ApplicationEvent.h>
#include <nrd/NfObject.h>
#include <nrd/config.h>
#include <nrd/Request.h>
#include <nrdbase/Application.h>
#include <nrdbase/LinkedList.h>

namespace netflix {
class StorageBridgePrivate;
class DiskStore;

class StorageJob;
class WriteTimer;
class StorageRequest;

class StorageBridge : public NfObject
{
public:
    StorageBridge();
    void init();
    void shutdown();
    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &data);

    virtual unsigned requestFlags(const Request &) const { return Request::Synchronous; }
    shared_ptr<DiskStore> diskStore() const;
protected:
    virtual void event(ApplicationEvent::Type, const Variant &);
private:
    void addRequest(StorageRequest *request);
    void executeRequest(StorageRequest *request);

    void queueWrite();
    void addWriteTask();
    void writeNow();
    void startJob();

    shared_ptr<StorageBridgePrivate> mPrivate;
    static Mutex sMutex;

    friend class WriteTimer;
    friend class StorageJob;
    friend class StorageBridgePrivate;
};
}

#endif
