#include "core/Log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


#define X(name) std::shared_ptr<spdlog::logger> Log::s_##name##Logger;
LOG_MODULES
#undef X

#include "core/LogSink.h"

void Log::Init() {
    spdlog::set_pattern("%^[%T] [%n] %v%$");
    auto imgui_sink = std::make_shared<ImGuiLogSink_mt>();
    imgui_sink->set_pattern("[%T] [%n] %v"); // ImGui 里不需要颜色转义符

#define X(name) \
        s_##name##Logger = spdlog::stdout_color_mt(#name); \
        s_##name##Logger->set_level(spdlog::level::trace); \
        s_##name##Logger->sinks().push_back(imgui_sink);
    LOG_MODULES
#undef X
}

void Log::Shutdown() {
    #define X(name) s_##name##Logger.reset();
        LOG_MODULES
    #undef X
    spdlog::shutdown();
}