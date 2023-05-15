/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_macros__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_macros__

#define DUMP(_field_)\
    do { dumper.dump(#_field_, _field_); } while(0)

#define DUMP_BIN(_field_)\
    do { dumper.dumpBin(#_field_, _field_); } while(0)

#define DUMP_FOURCC(_field_)\
    do { dumper.dumpFourCC(#_field_, _field_); } while(0)

#define DUMP_UUID(_field_)\
    do { dumper.dumpUUID(#_field_, _field_); } while(0)

#define DUMP_MATRIX(_field_)\
    do { dumper.dumpMatrix(#_field_, _field_); } while(0)

#define DEFINE_STD_BOXTYPE(_class_, _fourCC_)\
    const uint32_t _class_::ExpectedBoxType_ = Box::buildFourCC(_fourCC_)

#define DEFINE_EXT_BOXTYPE(_class_, _seg1_, _seg2_, _seg3_, _seg4_, _seg5_)\
    const UUID _class_::ExpectedBoxUserType_ =\
        Box::buildUUID(_seg1_, _seg2_, _seg3_, _seg4_, _seg5_)

#define DEFINE_DUAL_BOXTYPE(_class_, _fourCC_, _seg1_, _seg2_, _seg3_, _seg4_, _seg5_)\
    const uint32_t _class_::ExpectedBoxType_ = Box::buildFourCC(_fourCC_);\
    const UUID _class_::ExpectedBoxUserType_ =\
        Box::buildUUID(_seg1_, _seg2_, _seg3_, _seg4_, _seg5_)

#define DEFINE_DESCRIPTOR(_class_, _tag_)\
    const uint32_t _class_::ExpectedDescriptorTag_ = _tag_

#define CHILD_BOXES(_class_)\
    childBoxHandlers_##_class_

#define DEFINE_CHILD_BOXES(_class_)\
    static BoxReadHandlerList& addChildBoxHandlers_##_class_()\
    {\
        static BoxReadHandlerList list

#define STD_CHILD_BOX(_class_, _restriction_)\
        list.addHandler(\
            new StandardBoxReadHandler<_class_>(BoxReadHandler::_restriction_))

#define EXT_CHILD_BOX(_class_, _restriction_)\
        list.addHandler(\
            new ExtendedBoxReadHandler<_class_>(BoxReadHandler::_restriction_))

#define DUAL_CHILD_BOX(_class_, _restriction_)\
        list.addHandler(\
            new DualBoxReadHandler<_class_>(BoxReadHandler::_restriction_))

#define DEFINE_CHILD_BOXES_END(_class_)\
        return list;\
    }\
    static BoxReadHandlerList& CHILD_BOXES(_class_) =\
        addChildBoxHandlers_##_class_()

#define SAMPLE_ENTRY_TYPE(_class_)\
    sampleEntryType_##_class_

#define DEFINE_SAMPLE_ENTRY_TYPE(_class_, _restriction_)\
    static BoxReadHandlerList& addSampleEntryType_##_class_()\
    {\
        static BoxReadHandlerList list;\
        list.addHandler(\
            new SampleEntryReadHandler<_class_>(BoxReadHandler::_restriction_));\
        return list;\
    }\
    static BoxReadHandlerList& SAMPLE_ENTRY_TYPE(_class_) =\
        addSampleEntryType_##_class_()

#define CHILD_DESCRIPTORS(_class_)\
    childDescriptorHandlers_##_class_

#define DEFINE_CHILD_DESCRIPTORS(_class_)\
    static DescReadHandlerList& addChildDescriptorHandlers_##_class_()\
    {\
        static DescReadHandlerList list

#define STD_CHILD_DESCRIPTOR(_class_, _restriction_)\
        list.addHandler(\
            new StandardDescReadHandler<_class_>(DescReadHandler::_restriction_))

#define DEFINE_CHILD_DESCRIPTORS_END(_class_)\
        return list;\
    }\
    static DescReadHandlerList& CHILD_DESCRIPTORS(_class_) =\
        addChildDescriptorHandlers_##_class_()

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_macros__
