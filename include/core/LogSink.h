// LogSink.h
#pragma once
#include <spdlog/sinks/base_sink.h>
#include <vector>
#include <string>
#include <mutex>

enum class LogLevel { Trace, Info, Warn, Error };

struct LogEntry {
    LogLevel level;
    std::string message;
};

class LogBuffer {
public:
    static LogBuffer& Get() { static LogBuffer instance; return instance; }

    void Push(LogLevel level, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex);
        entries.push_back({ level, msg });
        if (entries.size() > max_entries)
            entries.erase(entries.begin());
        scrollToBottom = true;
    }

    const std::vector<LogEntry>& GetEntries() const { return entries; }
    bool ShouldScrollToBottom() const { return scrollToBottom; }
    void ClearScroll() { scrollToBottom = false; }
    void Clear() { std::lock_guard<std::mutex> lock(mutex); entries.clear(); }

private:
    std::vector<LogEntry> entries;
    std::mutex mutex;
    bool scrollToBottom = false;
    size_t max_entries = 500;
};

// spdlog sink — 把日志转发到 LogBuffer
template<typename Mutex>
class ImGuiLogSink : public spdlog::sinks::base_sink<Mutex> {
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        std::string str = fmt::to_string(formatted);

        LogLevel level;
        switch (msg.level) {
        case spdlog::level::warn:  level = LogLevel::Warn;  break;
        case spdlog::level::err:   level = LogLevel::Error; break;
        case spdlog::level::info:  level = LogLevel::Info;  break;
        default:                   level = LogLevel::Trace; break;
        }
        LogBuffer::Get().Push(level, str);
    }
    void flush_() override {}
};

using ImGuiLogSink_mt = ImGuiLogSink<std::mutex>;