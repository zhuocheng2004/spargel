#include "samples/vulkan_triangle/logging.h"

#include <stdio.h>

namespace base::logging {

namespace impl {

static char const* LogLevelToName(LogLevel level) {
  switch (level) {
    case LogLevel::Debug:
      return "debug";
    case LogLevel::Info:
      return "info";
    case LogLevel::Warning:
      return "warning";
    case LogLevel::Error:
      return "error";
    case LogLevel::Fatal:
      return "fatal";
  }
}

}  // namespace impl

void Log(LogLevel level, char const* message, meta::SourceLocation loc) {
  printf("[%s:%s:%d:%s] %s\n", impl::LogLevelToName(level), loc.FileName(),
         loc.Line(), loc.FunctionName(), message);
}

class DummyLogMessage final : public LogMessage {
 public:
  void Begin(meta::SourceLocation const& loc, LogLevel level);
  void Write(char const*) override;
  void Write(char) override;
  void Write(uint32_t) override;
  void Write(size_t) override;
  void End() override;
};

class DummyLogger final : public Logger {
 public:
  LogMessage* BeginMessage(meta::SourceLocation const& loc,
                           LogLevel level) override;

 private:
  DummyLogMessage message_;
};

Logger* Logger::Default() {
  static DummyLogger logger;
  return &logger;
}

LogMessage* DummyLogger::BeginMessage(meta::SourceLocation const& loc,
                                      LogLevel level) {
  message_.Begin(loc, level);
  return &message_;
}

void DummyLogMessage::Begin(meta::SourceLocation const& loc, LogLevel level) {
  printf("[%s:%s:%d:%s] ", impl::LogLevelToName(level), loc.FileName(),
         loc.Line(), loc.FunctionName());
}

void DummyLogMessage::Write(char const* s) { printf("%s", s); }

void DummyLogMessage::Write(char c) { printf("%c", c); }

void DummyLogMessage::Write(uint32_t n) { printf("%u", n); }

void DummyLogMessage::Write(size_t n) { printf("%zu", n); }

void DummyLogMessage::End() { putchar('\n'); }

}  // namespace base::logging
