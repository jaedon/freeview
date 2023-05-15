/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef QALOCATIONMONITORBRIDGE_H
#define QALOCATIONMONITORBRIDGE_H

#include "NfObject.h"

#include "nrdase/INetworkMonitor.h"

namespace netflix {

class QALocationMonitorBridge : public NfObject
{
public:
    QALocationMonitorBridge();
    ~QALocationMonitorBridge();

    //virtual bool getProperty(int index, Variant *result) const;
    //virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &data);

    Variant start( Variant const& data );
    Variant stop( Variant const& data );
    Variant getHistory();
    Variant setHistory( Variant const& data );

    class Listener : public netflix::ase::INetworkMonitorListener
    {
    public:
        Listener(QALocationMonitorBridge *bridge);
        ~Listener();

        virtual void operator()( Variant const& event );
    private:
        QALocationMonitorBridge *mBridge;
    };

protected:
    shared_ptr<Listener> mListener;
};
}

#endif // QALOCATIONMONITORBRIDGE_H
