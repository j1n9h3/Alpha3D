// Recorder.cpp
#include "utils/Recorder.h"
#include "core/Log.h"
#include <vector>
#include <algorithm>

#ifdef _WIN32
    #define popen _popen
    #define pclose _pclose
#endif

#include "stb_image_write.h"

#include "renderer/Camera.h"


void Recorder::Init(int w, int h) {
    width = w; height = h;
    pixel_buffer.resize(width * height * 3);

    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glGenFramebuffers(1, &fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Recorder::Destroy() {
    if (is_recording) StopRecording();
    glDeleteFramebuffers(1, &fbo_id);
    glDeleteTextures(1, &color_texture);
    glDeleteTextures(1, &depth_texture);
}

void Recorder::BeginRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Recorder::EndRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Recorder::StartRecording(const std::string& output_path, int fps, int bitrate_mbps) {
    std::string cmd = "ffmpeg -y -f rawvideo -pixel_format rgb24"
        " -video_size " + std::to_string(width) + "x" + std::to_string(height) +
        " -framerate " + std::to_string(fps) +
        " -i pipe:0"
        " -vf vflip"
        " -c:v libopenh264"
        " -pix_fmt yuv420p"
        " -b:v " + std::to_string(bitrate_mbps) + "M"
        " " + output_path +
        " 2> log/ffmpeg_log.txt";
    ffmpeg = popen(cmd.c_str(), "wb");
    if (!ffmpeg) {
        LOG_ERROR(Recorder, "Failed to open ffmpeg pipe");
        return;
    }
    is_recording = true;
    LOG_INFO(Recorder, "Recording started: {} @ {}fps {}Mbps", output_path, fps, bitrate_mbps);
}

void Recorder::StopRecording() {
    if (ffmpeg) {
        pclose(ffmpeg);
        ffmpeg = nullptr;
    }
    is_recording = false;
    LOG_INFO(Recorder, "Recording stopped");
}

void Recorder::CaptureFrame() {
    if (!is_recording) return;

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixel_buffer.data());
    glPixelStorei(GL_PACK_ALIGNMENT, 4);

    static bool saved = false;
    if (!saved) {
        stbi_write_png("20260624.png", width, height, 3, pixel_buffer.data(), width * 3);
        saved = true;
    }
    fwrite(pixel_buffer.data(), 1, pixel_buffer.size(), ffmpeg);
}