#ifndef GLSL_PREPRO_EXPAND_H
#define GLSL_PREPRO_EXPAND_H

#include "middleware/khronos/glsl/prepro/glsl_prepro_token.h"
#include "middleware/khronos/glsl/prepro/glsl_prepro_macro.h"

extern TokenSeq *glsl_expand(TokenSeq *ts, bool recursive);

#endif