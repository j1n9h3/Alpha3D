// Log.h
#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define LOG_MODULES \
    X(Window) \
    X(Shader) \
    X(Texture) \
    X(Scene) \
    X(Mesh) \
    X(Model) \
    X(Config) \
    X(Camera) \
    X(Editor)

#define LOG_TRACE(name, ...)  Log::Get##name##Logger()->trace(__VA_ARGS__)
#define LOG_INFO(name, ...)  Log::Get##name##Logger()->info(__VA_ARGS__)
#define LOG_WARN(name, ...) Log::Get##name##Logger()->warn(__VA_ARGS__)
#define LOG_ERROR(name, ...) Log::Get##name##Logger()->error(__VA_ARGS__)

class Log {
public:
    static void Init();
    static void Shutdown();

    #define X(name) \
        static std::shared_ptr<spdlog::logger>& Get##name##Logger() { return s_##name##Logger; }
        LOG_MODULES
    #undef X

private:
    #define X(name) static std::shared_ptr<spdlog::logger> s_##name##Logger;
        LOG_MODULES
    #undef X
};