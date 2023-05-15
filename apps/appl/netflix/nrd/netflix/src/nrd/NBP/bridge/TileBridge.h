/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TILEBRIDGE_H
#define TILEBRIDGE_H

#include <nrd/NfObject.h>
#include <nrd/IntegrationWriter.h>
#include <nrd/NrdWriteLimiter.h>
#include <nrdbase/Mutex.h>

namespace netflix {
class TileBridge : public NfObject
{
public:
    TileBridge();
    void init();

    virtual Variant invoke(int method, const Variant &data);
    virtual bool getProperty(int property, Variant *data) const;
    shared_ptr<IntegrationWriter> integrationWriter() const { return mIntegrationWriter; }
protected:
    virtual void event(ApplicationEvent::Type, const Variant &);
private:
    bool setSplashes(const Variant &data, IntegrationWriter::ErrorCode *code, std::string *error);
    bool setTiles(const Variant &data, IntegrationWriter::ErrorCode *code, std::string *error);

    shared_ptr<IntegrationWriter> mIntegrationWriter;
    Mutex mIntegrationWriterMutex;

};
}
#endif
