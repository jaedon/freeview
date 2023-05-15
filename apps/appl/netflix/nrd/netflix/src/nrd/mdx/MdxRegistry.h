/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MDXREGISTRY_H
#define MDXREGISTRY_H

#include <nrdbase/tr1.h>

namespace netflix {
namespace mdx {

class IMdx;

/**
 * @class MdxRegistry MdxRegistry.h
 * @brief Global registry for instances of IMdx.
 */
class MdxRegistry
{
public:
    /**
     * Registers the IMdx instance.
     *
     * @param[in] mediaControl the IMdx instance.
     */
    static void setMdx(shared_ptr<IMdx> mdx);

    /**
     * Returns the registered IMdx instance.
     *
     * @return the registered IMdx instance.
     */
    static shared_ptr<IMdx> getMdx();

private:
    /** Private constructor. */
    MdxRegistry() {}

    static shared_ptr<IMdx> sMdx; //!< IMdx pointer.
};

}} // namespace netflix::mdx

#endif // MDXREGISTRY_H
