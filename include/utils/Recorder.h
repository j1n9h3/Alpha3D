// Recorder.h
#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

class Camera;
class Recorder {
public:
    void Init(int width, int height);
    void Destroy();

    void BeginRender();
    void EndRender();
    void CaptureFrame();

    void StartRecording(const std::string& output_path, int fps = 60, int bitrate_mbps = 20);
    void StopRecording();

    bool IsRecording() const { return is_recording; }
    GLuint GetColorTexture() const { return color_texture; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    bool GetShowGuide() const { return show_guide; }
    void SetShowGuide(bool v) { show_guide = v; }
private:
    bool show_guide = false;
    GLuint fbo_id = 0;
    GLuint color_texture = 0;
    GLuint depth_texture = 0;
    int width = 0, height = 0;

    FILE* ffmpeg = nullptr;
    bool is_recording = false;
    std::vector<uint8_t> pixel_buffer;
};