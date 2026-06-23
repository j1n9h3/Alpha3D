#pragma once
#include <string>
#include <vector>
#include <filesystem>

class Environment {
public:
    void Scan(const std::string& directory);

    const std::vector<std::string>& GetPaths() const { return paths; }
    const std::vector<std::string>& GetNames() const { return names; }
    int GetSelected() const { return selected; }
    const std::string& GetSelectedPath() const { return paths[selected]; }
    bool Select(int index);
    bool HasChanged() const { return changed; }
    void ClearChanged() { changed = false; }

private:
    std::vector<std::string> paths;
    std::vector<std::string> names;
    int selected = 1;
    bool changed = false;
};