/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef FONTMANAGERVALUES_H_
#define FONTMANAGERVALUES_H_

namespace netflix {
namespace gibbon {

struct FontValues {
    double emboldenFactor;
    double slantFactor;
    FontValues() : emboldenFactor(1), slantFactor(0.375) { }
};

}}

#endif /* FONTMANAGERVALUES_H_ */
