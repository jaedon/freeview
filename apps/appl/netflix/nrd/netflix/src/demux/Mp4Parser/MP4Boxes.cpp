/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MP4Boxes.h"

#include "Context.h"

#include "MP4Descriptors.h"
#include "Reader.h"
#include "Dumper.h"
#include "macros.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * Table of Box Types
 */

DEFINE_STD_BOXTYPE(ESDBox, "esds");

/*******************************************************************************
 * ESDBox
 */

DEFINE_CHILD_DESCRIPTORS(ESDBox);
    STD_CHILD_DESCRIPTOR(ES_Descriptor, MANDATORY_UNIQUE);
DEFINE_CHILD_DESCRIPTORS_END(ESDBox);

void ESDBox::clearSpecifics()
{
    childDescriptors_.clear();
}

bool ESDBox::readSpecifics(Reader& reader, Context& context)
{
    return BaseDescriptor::readDescriptors(context,
                                           CHILD_DESCRIPTORS(ESDBox),
                                           childDescriptors_,
                                           reader);
}

void ESDBox::dumpSpecifics(Dumper& dumper) const
{
    BaseDescriptor::dumpDescriptors(childDescriptors_, dumper);
}
