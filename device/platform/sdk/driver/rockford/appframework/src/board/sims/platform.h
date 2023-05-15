/**************************************************************************
 * Chip specific functions
 **************************************************************************/

#if BCHP_CHIP == 7043
#include "bchp_7043.h"
#include "bint_7043.h"

#define BCHP_OPEN          BCHP_Open7043
#define BINT_GET_SETTINGS  BINT_7043_GetSettings

#elif BCHP_CHIP == 7400

#include "bchp_7400.h"
#include "bint_7400.h"

#define BCHP_OPEN          BCHP_Open7400
#define BINT_GET_SETTINGS  BINT_7400_GetSettings

#endif
