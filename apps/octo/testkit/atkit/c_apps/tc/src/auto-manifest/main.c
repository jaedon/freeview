#include "tc_manifest_manager.h"

#define TC_TEST_MANIFEST "/home/atkit/manifest/auto-manifest.manifest"

int main(void)
{
//    stTc_parameter_t tc_test0_param[] = {{STRING /* param data type */
  //                                      , "input_string" /* param name*/
    //                                    , "hello" /* param value*/
      //                                  , "none" /* param description*/}};

    /** stTc_parameter_t tc_test1_param[] = {{INTEGER, "input_integer", "25", "none"}, {STRING, "output_value", "100", "none"}}; */

//    stTc_item_t tc[] = {
  //                              {"tc_test0" /* tc_name */
    //                            , 1 /* tc_runCount */
      //                          , 1 /* category_count */
        //                        , {{"string"}} /* category */
          //                      , 1 /* number of parameter */
            //                    , tc_test0_param  
              //                  , STRING /* expected result type */, "result" /* result name */, "0" /* result value */}, 

                //                {"tc_test1", 1, 1, {{"integer"}}, 2, tc_test1_param, STRING, "result", "0"}};

    stTc_manifest_t test = {true /* RUN_ALL*/
                            , 2 /* RUN_CATEGORY_COUNT */
                            , {{"string"}, {"integer"}} /* RUN_CATETORY */
                            , 3 /* RUN_COUNT */
                            , 2 /* NUMBER OF TC */
                            , {
                                {"tc_test0" /* tc_name */
                                , 1 /* tc_runCount */
                                , 1 /* category_count */
                                , {{"string"}} /* category */
                                , 1 /* number of parameter */
                                , {{STRING /* param data type */
                                        , "input_string" /* param name */
                                        , "hello" /* param value */
                                        , "none" /* param description */}}  
                                , STRING /* expected result type */, "result" /* result name */, "0" /* result value */}, 

                                {"tc_test1", 1, 1, {{"integer"}}, 2, {{INTEGER, "input_integer", "25", "none"}, {STRING, "output_value", "100", "none"}}, STRING, "result", "0"}}};

    st_Manifest = &test;

    if (!IsManifestExist(TC_TEST_MANIFEST)) 
        ManifestExporter(TC_TEST_MANIFEST, st_Manifest);

    if (IsManifestExist(TC_TEST_MANIFEST)) 
        ManifestImporter(TC_TEST_MANIFEST, st_Manifest);

    return 0;
}
