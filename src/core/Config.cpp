#include "toml.hpp"

#include "core/Config.h"
#include "core/Log.h"


EngineConfig Config::s_Config;

void Config::Load(const std::string& path) {
    try {
        auto data = toml::parse_file(path);

        s_Config.window.width = data["window"]["width"].value_or(800);
        s_Config.window.height = data["window"]["height"].value_or(800);
        s_Config.window.title = data["window"]["title"].value_or("Engine");

        s_Config.renderer.fov = data["renderer"]["fov"].value_or(90.0f);
        s_Config.renderer.nearClip = data["renderer"]["near"].value_or(0.1f);
        s_Config.renderer.farClip = data["renderer"]["far"].value_or(100.0f);

        LOG_INFO(Config, "Config loaded: {}", path);
    }
    catch (const toml::parse_error& e) {
        LOG_INFO(Config, "Failed to load config: {}", e.what());
    }
}