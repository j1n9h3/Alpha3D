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

    // photo
    void SaveImage();
    void RequestPhoto(const std::string& output_path) { 
        photo_requested = true, photo_output_path = output_path;
    }
    bool ConsumePhotoRequest();

    // video
    void BeginRender();
    void EndRender();
    void StartRecording(const std::string& output_path, int fps = 60, int bitrate_mbps = 20);
    void StopRecording();
    bool IsRecording() const { return is_recording; }
    void CaptureFrame();
    GLuint GetColorTexture() const { return color_texture; }

    // guiding lines
    bool GetShowGuide() const { return show_guide; }
    void SetShowGuide(bool v) { show_guide = v; }

    // getting properties
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    bool show_guide = false;
    GLuint fbo_id = 0;
    GLuint color_texture = 0;
    GLuint depth_texture = 0;
    int width = 0, height = 0;

    FILE* ffmpeg = nullptr;
    bool is_recording = false;
    bool photo_requested = false;
    std::string photo_output_path;
    std::vector<uint8_t> pixel_buffer;
};