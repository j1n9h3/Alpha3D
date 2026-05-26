#include "core/Log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


#define X(name) std::shared_ptr<spdlog::logger> Log::s_##name##Logger;
LOG_MODULES
#undef X

void Log::Init() {
    spdlog::set_pattern("%^[%T] [%n] %v%$");
    #define X(name) \
        s_##name##Logger = spdlog::stdout_color_mt(#name); \
        s_##name##Logger->set_level(spdlog::level::trace);
        LOG_MODULES
    #undef X
}

void Log::Shutdown() {
    #define X(name) s_##name##Logger.reset();
        LOG_MODULES
    #undef X
    spdlog::shutdown();
}