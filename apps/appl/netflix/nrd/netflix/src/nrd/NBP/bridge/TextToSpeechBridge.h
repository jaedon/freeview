/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TextToSpeechBridge_h__
#define TextToSpeechBridge_h__

#include <nrd/ITextToSpeech.h>
#include <nrd/NfObject.h>
#include <nrd/Request.h>

namespace netflix {

class TextToSpeechBridge : public NfObject
{
public:
    TextToSpeechBridge();
    ~TextToSpeechBridge();

    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &data);

    virtual unsigned requestFlags(const Request &) const { return Request::Synchronous; }

private:
    struct Listener : public netflix::device::ITextToSpeech::Listener
    {
        Listener(TextToSpeechBridge* b) : bridge(b) { }

        virtual void saying(unsigned int handle);
        virtual void said(unsigned int handle);
        virtual void stopped(unsigned int handle);
        virtual void capabilitiesChanged();
        virtual void verbosityChanged();
        virtual void propertiesChanged();

        TextToSpeechBridge* bridge;
    };

private:
    shared_ptr<netflix::device::ITextToSpeech> mTextToSpeech;
    shared_ptr<Listener> mListener;

    friend struct Listener;
};

} // namespace netflix

#endif // TextToSpeechBridge_h__
