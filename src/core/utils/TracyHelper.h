#pragma once

/**
 * @file TracyHelper.h
 * @brief Helper file for Tracy profiler integration
 * 
 * Include this file in your source files where you want to add profiling.
 * Tracy will only be active when compiled with -DENABLE_TRACY=ON
 */

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>

// Frame marking - call once per frame/iteration
#define PROFILE_FRAME_MARK FrameMark

// Zone profiling - measures time spent in a scope
#define PROFILE_ZONE ZoneScoped
#define PROFILE_ZONE_NAMED(name) ZoneScopedN(name)

// Function profiling - automatically uses function name
#define PROFILE_FUNCTION ZoneScoped

// Custom zone with dynamic name
#define PROFILE_ZONE_DYNAMIC(name) ZoneScopedN(name)

// Plot values (useful for monitoring variables)
#define PROFILE_PLOT(name, value) TracyPlot(name, (int64_t)(value))

// Log messages to Tracy
#define PROFILE_MESSAGE(text, size) TracyMessage(text, size)
#define PROFILE_MESSAGE_L(text) TracyMessageL(text)

#else
// No-op macros when Tracy is disabled
#define PROFILE_FRAME_MARK
#define PROFILE_ZONE
#define PROFILE_ZONE_NAMED(name)
#define PROFILE_FUNCTION
#define PROFILE_ZONE_DYNAMIC(name)
#define PROFILE_PLOT(name, value)
#define PROFILE_MESSAGE(text, size)
#define PROFILE_MESSAGE_L(text)
#endif
