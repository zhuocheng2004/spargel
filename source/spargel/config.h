#pragma once

/**
 * trace every allocation in Spargel Engine
 */
#define SPARGEL_TRACE_ALLOCATION 1

/**
 * panic if CHECH() fails; in general this should always be enabled
 */
#define SPARGEL_ENABLE_CHECK 1

/**
 * panic if DCHECK() fails; this should be disabled in release build
 */
#define SPARGEL_ENABLE_DCHECK 1
