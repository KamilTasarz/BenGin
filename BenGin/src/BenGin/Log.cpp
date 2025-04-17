#include "Log.h"

std::shared_ptr<spdlog::logger> Log::s_engine_logger;
std::shared_ptr<spdlog::logger> Log::s_application_logger;

void Log::initialize() {

	spdlog::set_pattern("[%T] {%n}: %^%v%$");

	s_engine_logger = spdlog::stdout_color_mt("BENGIN");
	s_engine_logger->set_level(spdlog::level::trace);
	
	s_application_logger = spdlog::stdout_color_mt("APP");
	s_application_logger->set_level(spdlog::level::trace);

}
