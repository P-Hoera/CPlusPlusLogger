#pragma once

#include "LoggerAPI.h"

#include "Logger.h"

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief Factory used to construct logger.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class LOGGER_API LoggerFactory
	{
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a logger.
		///
		/// \param loggerName A name for the logger that can be used to identify it.
		/// \param moduleName The module name used by log entries of this logger.
		/// \param logLevel The log level to use for initialization.
		///
		/// \returns The constructed logger.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<Logger> createLogger(const std::string& loggerName, const std::string& moduleName, const LogLevel logLevel);

	private:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructor.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		LoggerFactory();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Holds and returns the instance of the factory.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static LoggerFactory& getInstance();

		std::shared_ptr<SinkSynchronizer> m_sinkSynchronizer; //!< The sink synchronizer used by all logger.
	};
}

#pragma warning( pop ) 