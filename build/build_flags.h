#pragma once

#define BUILD_FLAG_CAT_INDIRECT(a, b) a ## b
#define BUILD_FLAG_CAT(a, b) BUILD_FLAG_CAT_INDIRECT(a, b)

#define BUILD_FLAG(flag) (BUILD_FLAG_CAT(BUILD_FLAG_INTERNAL_, flag)())
