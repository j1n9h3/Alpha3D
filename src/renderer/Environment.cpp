#include "renderer/Environment.h"

void Environment::Scan(const std::string& directory) {
    paths.clear();
    names.clear();
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        auto ext = entry.path().extension().string();
        if (ext == ".hdr" || ext == ".exr") {
            paths.push_back(entry.path().string());
            names.push_back(entry.path().stem().string());
        }
    }
    changed = false;
}

bool Environment::Select(int index) {
    if (index < 0 || index >= (int)paths.size()) return false;
    if (index == selected) return false;
    selected = index;
    changed = true;
    return true;
}