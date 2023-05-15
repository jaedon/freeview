/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DolbyBoxes.h"
#include "Context.h"
#include "Reader.h"
#include "Dumper.h"
#include "macros.h"

#include <stdio.h>

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * Table of Box Types
 */
DEFINE_STD_BOXTYPE(DolbyVisionConfigurationBox, "dvcC");

/*******************************************************************************
 * DolbyVisionConfigurationBox
 */

DEFINE_CHILD_BOXES(DolbyVisionConfigurationBox);
DEFINE_CHILD_BOXES_END(DolbyVisionConfigurationBox);

void DolbyVisionConfigurationBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool DolbyVisionConfigurationBox::readSpecifics(Reader& reader, Context& context)
{
    PRINT("DolbyVisionConfigurationBox::%s\n", __func__);

    reader.read(dv_version_major_, 8);
    reader.read(dv_version_minor_, 8);
    reader.read(dv_profile_, 13);
    reader.read(rpu_present_, 1);
    reader.read(el_present_, 1);
    reader.read(bl_present_, 1);

    PRINT("dv_version_major %d\n", dv_version_major_ );
    PRINT("dv_version_minor %d\n", dv_version_minor_ );
    PRINT("dv_profile %d\n", dv_profile_ );
    PRINT("rpu_present_ %d\n", rpu_present_ );
    PRINT("el_present_ %d\n", el_present_ );
    PRINT("bl_present %d\n", bl_present_ );

    for(int i = 0 ; i<5; i++){
        reader.read(reserved_[0], 32);
    }
    // check point
    if(!reader.good())
        return false;

    return readBoxes(context, CHILD_BOXES(DolbyVisionConfigurationBox), childBoxes_, reader);
}

void DolbyVisionConfigurationBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(dv_version_major_);
    DUMP(dv_version_minor_);
    DUMP(dv_profile_);
    DUMP(rpu_present_);
    DUMP(el_present_);
    DUMP(bl_present_);

    dumpBoxes(childBoxes_, dumper);
}

