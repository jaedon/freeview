/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef QATextToSpeechBridge_h__
#define QATextToSpeechBridge_h__

#include <nrd/ITextToSpeech.h>
#include <nrd/NfObject.h>
#include <nrd/Request.h>

namespace netflix {

class QATextToSpeechBridge : public NfObject
{
public:
    QATextToSpeechBridge();
    ~QATextToSpeechBridge();

    virtual Variant invoke(int method, const Variant &data);
    virtual unsigned requestFlags(const Request &) const { return Request::Synchronous; }

private:
    shared_ptr<netflix::device::ITextToSpeech> mTextToSpeech;
};

} // namespace netflix

#endif // QATextToSpeechBridge_h__
