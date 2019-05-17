#include "LoggerFactory.h"

namespace Log
{
	std::shared_ptr<Logger> LoggerFactory::createLogger(const std::string& loggerName, const std::string& moduleName, const LogLevel logLevel)
	{
		std::shared_ptr<Logger> logger(new Logger(loggerName, moduleName, logLevel, getInstance().m_sinkSynchronizer));
		return logger;
	}

	LoggerFactory::LoggerFactory()
		:m_sinkSynchronizer(new SinkSynchronizer())
	{
	}

	LoggerFactory& LoggerFactory::getInstance()
	{
		static LoggerFactory factoryInstance;
		return factoryInstance;
	}
}