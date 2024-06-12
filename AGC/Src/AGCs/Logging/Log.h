#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <stdexcept>

namespace AGC {
	class Log {
	public:
		static void init();

		static std::shared_ptr<spdlog::logger> getLogger() { return m_logger; }

	private:
		static std::shared_ptr<spdlog::logger> m_logger;

	};

#define	AGC_TRACE(...)	::AGC::Log::getLogger()->trace(__VA_ARGS__);
#define	AGC_INFO(...)	::AGC::Log::getLogger()->info(__VA_ARGS__);
#define	AGC_WARN(...)	::AGC::Log::getLogger()->warn(__VA_ARGS__);
#define	AGC_ERROR(...)	::AGC::Log::getLogger()->error(__VA_ARGS__);
#define	AGC_CRIT(...)	::AGC::Log::getLogger()->critical(__VA_ARGS__);

#define RUNTIME_ERROR(message)		\
AGC_CRIT(message)					\
throw std::runtime_error(message)

#define AGC_ASSERT(condition, message) {		\
		if(condition) {							\
			throw std::runtime_error(message);	\
		}										\
	}

}
