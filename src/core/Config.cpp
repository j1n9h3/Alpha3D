#include "toml.hpp"

#include "core/Config.h"
#include "core/Log.h"


EngineConfig Config::s_Config;

void Config::Load(const std::string& path) {
    try {
        auto data = toml::parse_file(path);

        s_Config.window.width = data["window"]["width"].value_or(800);
        s_Config.window.height = data["window"]["height"].value_or(800);
        s_Config.window.x = data["window"]["x"].value_or(50);
        s_Config.window.y = data["window"]["y"].value_or(50);
        s_Config.window.title = data["window"]["title"].value_or("Engine");

        s_Config.renderer.fov = data["renderer"]["fov"].value_or(90.0f);
        s_Config.renderer.nearClip = data["renderer"]["near"].value_or(0.1f);
        s_Config.renderer.farClip = data["renderer"]["far"].value_or(100.0f);

        LOG_INFO(Config, "Config loaded: {}.", path);
    }
    catch (const toml::parse_error& e) {
        LOG_INFO(Config, "Failed to load config: {}.", e.what());
    }
}


void Config::Save(const std::string& path) {
    toml::table doc;
    doc.insert_or_assign("window", toml::table{
        {"width",  s_Config.window.width},
        {"height", s_Config.window.height},
        {"x",      s_Config.window.x},
        {"y",      s_Config.window.y},
        {"title",  s_Config.window.title},
        });
    doc.insert_or_assign("renderer", toml::table{
        {"fov",      s_Config.renderer.fov},
        {"nearClip", s_Config.renderer.nearClip},
        {"farClip",  s_Config.renderer.farClip},
    });
    std::ofstream file(path);
    file << doc;
}