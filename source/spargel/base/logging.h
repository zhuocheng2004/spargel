#pragma once

#include <spargel/base/compiler.h>
#include <spargel/base/source_location.h>
#include <spargel/base/types.h>

#define LOG_IMPL(level, ...) ::spargel::base::log(level, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define spargel_log_debug(...) LOG_IMPL(::spargel::base::LOG_DEBUG, __VA_ARGS__)
#define spargel_log_info(...) LOG_IMPL(::spargel::base::LOG_INFO, __VA_ARGS__)
#define spargel_log_warn(...) LOG_IMPL(::spargel::base::LOG_WARN, __VA_ARGS__)
#define spargel_log_error(...) LOG_IMPL(::spargel::base::LOG_ERROR, __VA_ARGS__)
#define spargel_log_fatal(...) LOG_IMPL(::spargel::base::LOG_FATAL, __VA_ARGS__);

namespace spargel::base {

    enum log_level {
        /* message for debugging */
        LOG_DEBUG = 0,
        /* general log events */
        LOG_INFO,
        /* warning, not necessarily shown to users */
        LOG_WARN,
        /* error that can recover from */
        LOG_ERROR,
        /* nothing more can be done other than aborting */
        LOG_FATAL,

        _LOG_COUNT,
    };

    SPARGEL_ATTRIBUTE_PRINTF_FORMAT(5, 6)
    void log(int level, char const* file, char const* func, ssize line, char const* format, ...);

    struct logger {};

}  // namespace spargel::base
