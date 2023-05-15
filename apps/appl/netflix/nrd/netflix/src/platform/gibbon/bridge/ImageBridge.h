/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __IMAGEBRIDGE_H__
#define __IMAGEBRIDGE_H__

#include <Image.h>
#include <GibbonBaseBridge.h>

#include <nrd/NfObject.h>
#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class ImageBridge : public Image::Listener, public GibbonBaseBridge
{
public:
    ImageBridge(const std::string &name, const Image::SharedPtr &image);
    ~ImageBridge();
    void init();

    inline Image::SharedPtr getImage() const { return mImage; }
    inline int getID() const { return getImage()->getID(); }

    virtual unsigned requestFlags(const Request &request) const
    {
        return GibbonBaseBridge::requestFlags(request) | Request::Queued;
    }

    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &args);

    virtual ClassType classType() const { return ImageType; }
    virtual uint32_t classID() const { return getID(); }

    static Variant imageLoadProperties(const Image::SharedPtr &image);

    virtual void onImageLoaded(const Image::SharedPtr &image, bool loaded);

protected:
    bool mSendImageLoaded;
    Image::SharedPtr mImage;
};

}} // namespace netflix::gibbon

#endif /* __IMAGEBRIDGE_H__ */
