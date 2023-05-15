/* Template for creating TCs */

#include "atkit.h"
#include "tc_manifest_manager.h"

#define MAX_LENGTH 128

static HxJSON_t s_manifest = NULL;

HINT32 TC_TEMPLATE_TestCase(HxJSON_t j_args);
void TC_TEMPLATE_Init(HxJSON_t jManifest);
void TC_TEMPLATE_DeInit(void);
