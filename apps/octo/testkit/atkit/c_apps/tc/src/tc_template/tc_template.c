#include "atkit.h"
#include "tc_template.h"

stTc_manifest_t st_defaultManifest = {};

HINT32 TC_TEMPLATE_TestCase(HxJSON_t j_args)
{
    /* Step 1: Import data structure from manifest file to st_defaultManifest */
    /*      + ManifestImporter(pathName, st_manifest); */
    /* Step 2: Pass arguments to Apkit functions */
    /*      + APPKIT_SysInfo_Get(st_defaultManifest->argument); */
    /* Step 3: Verify the return values */

    return 0;
}

void TC_TEMPLATE_Init(HxJSON_t jManifest)
{
    printf("[%s]\n", __FUNCTION__);
    fflush(stdout);

    s_manifest = jManifest;

    /* Initial Functions */
    
    /* Add TC */
    ADD_TC(TC_TEMPLATE_TestCase);
}

void TC_TEMPLATE_DeInit(void)
{
    printf("[%s]\n", __FUNCTION__);
    fflush(stdout);
}
