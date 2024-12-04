#pragma once

#include <spargel/base/panic.h>

#define SPARGEL_ENABLE_CONTRACTS 1

#if SPARGEL_ENABLE_CONTRACTS
#define EXPECTS(cond) ((cond) ? static_cast<void>(0) : spargel::base::Panic())
#define ENSURES(cond) ((cond) ? static_cast<void>(0) : spargel::base::Panic())
#else
#define EXPECTS(cond)
#define ENSURES(cond)
#endif
