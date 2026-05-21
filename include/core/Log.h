// Log.h
#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define LOGGER(name) \
    static std::shared_ptr<spdlog::logger>& Get##name##Logger() { return s_##name##Logger; } \

#define INFO(name, ...)  Log::Get##name##Logger()->info(__VA_ARGS__)
#define WARN(name, ...) Log::Get##name##Logger()->warn(__VA_ARGS__)
#define ERROR(name, ...) Log::Get##name##Logger()->error(__VA_ARGS__)

class Log {
public:
    static void Init();
    static void Shutdown();
    LOGGER(GLAD)
    LOGGER(Window)
    LOGGER(Shader)
    LOGGER(Scene)
private:
    static std::shared_ptr<spdlog::logger> s_GLADLogger;
    static std::shared_ptr<spdlog::logger> s_WindowLogger;
    static std::shared_ptr<spdlog::logger> s_ShaderLogger;
    static std::shared_ptr<spdlog::logger> s_SceneLogger;
};