#pragma once

#include <memory.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "Core.h"

class BENGIN_API Log {

public:

	static void initialize();

	inline static std::shared_ptr<spdlog::logger>& getEngineLogger() { return s_engine_logger; }
	inline static std::shared_ptr<spdlog::logger>& getApplicationLogger() { return s_application_logger; }

private:

	static std::shared_ptr<spdlog::logger> s_engine_logger;
	static std::shared_ptr<spdlog::logger> s_application_logger;

};

// trace -> severity 0 (white color), use for comments or variable values
// debug -> severity 1 (blue color), use if we enter/reach a block of code
// info -> severity 2 (green color), use if successful
// warn -> severity 3 (yellow color), use if something didn't go as planned but didn't affect runtime
// error -> severity 4 (red color), use if something behaves not as intended and affects runtime
// critical -> severity 5 (red highlight), use in the worst cases

// Macros for the engine logging

#define LOG_BENGIN_TRACE(...)            ::Log::getEngineLogger()->trace(__VA_ARGS__)
#define LOG_BENGIN_DEBUG(...)            ::Log::getEngineLogger()->debug(__VA_ARGS__)
#define LOG_BENGIN_INFORMATION(...)      ::Log::getEngineLogger()->info(__VA_ARGS__)
#define LOG_BENGIN_WARNING(...)          ::Log::getEngineLogger()->warn(__VA_ARGS__)
#define LOG_BENGIN_ERROR(...)            ::Log::getEngineLogger()->error(__VA_ARGS__)
#define LOG_BENGIN_CRITICAL(...)         ::Log::getEngineLogger()->critical(__VA_ARGS__)

// Macros for the application (game/editor) logging

#define LOG_APPLICATION_TRACE(...)       ::Log::getApplicationLogger()->trace(__VA_ARGS__)
#define LOG_APPLICATION_DEBUG(...)       ::Log::getApplicationLogger()->debug(__VA_ARGS__)
#define LOG_APPLICATION_INFORMATION(...) ::Log::getApplicationLogger()->info(__VA_ARGS__)
#define LOG_APPLICATION_WARNING(...)     ::Log::getApplicationLogger()->warn(__VA_ARGS__)
#define LOG_APPLICATION_ERROR(...)       ::Log::getApplicationLogger()->error(__VA_ARGS__)
#define LOG_APPLICATION_CRITICAL(...)    ::Log::getApplicationLogger()->critical(__VA_ARGS__)
