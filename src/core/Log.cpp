#include "core/Log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// static variable init
std::shared_ptr<spdlog::logger> Log::s_WindowLogger;
std::shared_ptr<spdlog::logger> Log::s_ShaderLogger;
std::shared_ptr<spdlog::logger> Log::s_SceneLogger;

void Log::Init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_WindowLogger = spdlog::stdout_color_mt("Window");
    s_ShaderLogger = spdlog::stdout_color_mt("Shader");
    s_SceneLogger = spdlog::stdout_color_mt("Scene");

    s_WindowLogger->set_level(spdlog::level::trace);
    s_ShaderLogger->set_level(spdlog::level::trace);
    s_SceneLogger->set_level(spdlog::level::trace);
}

void Log::Shutdown() {
    s_WindowLogger.reset();
    s_ShaderLogger.reset();
    s_SceneLogger.reset();
    spdlog::shutdown();
}