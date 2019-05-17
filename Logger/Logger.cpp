#include "Logger.h"

namespace Log
{
	Logger::Logger(const std::string& loggerName, const std::string& moduleName, const LogLevel logLevel, const std::shared_ptr<SinkSynchronizer> sinkSynchronizer)
		:m_loggerName(loggerName), m_moduleName(moduleName), m_logLevel(logLevel), m_connectedSinks(new SinkVector()), m_sinkSynchronizer(sinkSynchronizer)
	{
		if (sinkSynchronizer == nullptr)
		{
			throw std::invalid_argument("SinkSynchronizer is nullptr.");
		}
	}

	void Logger::logTrace(const std::string& message)
	{
		log(LogLevel::Trace, message);
	}

	void Logger::logTrace(const std::function<std::string()>& messageFunction)
	{
		log(LogLevel::Trace, messageFunction);
	}

	void Logger::logTraceAlways(const std::string& message)
	{
		logAlways(LogLevel::Trace, message);
	}

	void Logger::logTraceAlways(const std::function<std::string()>& messageFunction)
	{
		logAlways(LogLevel::Trace, messageFunction());
	}

	void Logger::logDebug(const std::string& message)
	{
		log(LogLevel::Debug, message);
	}

	void Logger::logDebug(const std::function<std::string()>& messageFunction)
	{
		log(LogLevel::Debug, messageFunction);
	}

	void Logger::logDebugAlways(const std::string& message)
	{
		logAlways(LogLevel::Debug, message);
	}

	void Logger::logDebugAlways(const std::function<std::string()>& messageFunction)
	{
		logAlways(LogLevel::Debug, messageFunction());
	}

	void Logger::logInfo(const std::string& message)
	{
		log(LogLevel::Info, message);
	}

	void Logger::logInfo(const std::function<std::string()>& messageFunction)
	{
		log(LogLevel::Info, messageFunction);
	}

	void Logger::logInfoAlways(const std::string& message)
	{
		logAlways(LogLevel::Info, message);
	}

	void Logger::logInfoAlways(const std::function<std::string()>& messageFunction)
	{
		logAlways(LogLevel::Info, messageFunction());
	}

	void Logger::logWarning(const std::string& message)
	{
		log(LogLevel::Warning, message);
	}

	void Logger::logWarning(const std::function<std::string()>& messageFunction)
	{
		log(LogLevel::Warning, messageFunction);
	}

	void Logger::logWarningAlways(const std::string& message)
	{
		logAlways(LogLevel::Warning, message);
	}

	void Logger::logWarningAlways(const std::function<std::string()>& messageFunction)
	{
		logAlways(LogLevel::Warning, messageFunction());
	}

	void Logger::logError(const std::string& message)
	{
		log(LogLevel::Error, message);
	}

	void Logger::logError(const std::function<std::string()>& messageFunction)
	{
		log(LogLevel::Error, messageFunction);
	}

	void Logger::logErrorAlways(const std::string& message)
	{
		logAlways(LogLevel::Error, message);
	}

	void Logger::logErrorAlways(const std::function<std::string()>& messageFunction)
	{
		logAlways(LogLevel::Error, messageFunction());
	}

	void Logger::logFatal(const std::string& message)
	{
		log(LogLevel::Fatal, message);
	}

	void Logger::logFatal(const std::function<std::string()>& messageFunction)
	{
		log(LogLevel::Fatal, messageFunction);
	}

	void Logger::logFatalAlways(const std::string& message)
	{
		logAlways(LogLevel::Fatal, message);
	}

	void Logger::logFatalAlways(const std::function<std::string()>& messageFunction)
	{
		logAlways(LogLevel::Fatal, messageFunction());
	}

	void Logger::log(const LogLevel logLevel, const std::string& message)
	{
		if (shouldBeLogged(logLevel))
		{
			logToSinks(logLevel, message);
		}
	}

	void Logger::log(const LogLevel logLevel, const std::function<std::string()>& messageFunction)
	{
		if (shouldBeLogged(logLevel))
		{
			logToSinks(logLevel, messageFunction());
		}
	}

	void Logger::logAlways(const LogLevel logLevel, const std::string& message)
	{
		logToSinks(logLevel, message);
	}

	void Logger::logAlways(const LogLevel logLevel, const std::function<std::string()>& messageFunction)
	{
		logToSinks(logLevel, messageFunction());
	}

	bool Logger::shouldBeLogged(const LogLevel logLevel)
	{
		using LogLevelType = std::underlying_type_t<LogLevel>;

		if (static_cast<LogLevelType>(logLevel) <= static_cast<LogLevelType>(m_logLevel.load(std::memory_order_relaxed)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	LogLevel Logger::getLogLevel() const
	{
		return m_logLevel.load(std::memory_order_relaxed);
	}

	void Logger::setLogLevel(const LogLevel newLogLevel)
	{
		m_logLevel.store(newLogLevel, std::memory_order_relaxed);
	}

	std::string Logger::getLoggerName() const
	{
		return m_loggerName;
	}

	std::string Logger::getModuleName() const
	{
		return m_moduleName;
	}

	void Logger::connectToSink(std::shared_ptr<SinkBase> sink)
	{
		if (sink == nullptr)
		{
			return;
		}

		std::unique_lock<std::mutex> lock(m_connectedSinksMutex);

		std::shared_ptr<SinkVector> connectedSinks = std::shared_ptr<SinkVector>(new SinkVector(*m_connectedSinks));
		auto foundSink = std::find(connectedSinks->begin(), connectedSinks->end(), sink);

		if (foundSink != connectedSinks->end())
		{
			return;
		}

		connectedSinks->push_back(sink);
		m_connectedSinks = connectedSinks;
	}

	void Logger::disconnectFromSink(const std::shared_ptr<SinkBase> sink)
	{
		if (sink == nullptr)
		{
			return;
		}

		std::unique_lock<std::mutex> lock(m_connectedSinksMutex);

		std::shared_ptr<SinkVector> connectedSinks = std::shared_ptr<SinkVector>(new SinkVector(*m_connectedSinks));

		auto foundSink = std::find(connectedSinks->begin(), connectedSinks->end(), sink);

		if (foundSink != connectedSinks->end())
		{
			connectedSinks->erase(foundSink);
			m_connectedSinks = connectedSinks;
		}
	}

	void Logger::disconnectAllSinks()
	{
		std::unique_lock<std::mutex> lock(m_connectedSinksMutex);

		m_connectedSinks = std::shared_ptr<SinkVector>(new SinkVector());
	}

	bool Logger::isConnectedToSink(const std::shared_ptr<SinkBase> sink)
	{
		if (sink == nullptr)
		{
			return false;
		}

		std::unique_lock<std::mutex> lock(m_connectedSinksMutex);

		auto foundSink = std::find(m_connectedSinks->begin(), m_connectedSinks->end(), sink);

		if (foundSink != m_connectedSinks->end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Logger::logToSinks(const LogLevel logLevel, const std::string& message)
	{
		std::shared_ptr<Log::RawLogEntry> rawLogEntry = std::make_shared<Log::RawLogEntry>(std::chrono::system_clock::now(), logLevel, m_moduleName, std::this_thread::get_id(), message);

		std::unique_lock<std::mutex> lock(m_connectedSinksMutex);
		m_sinkSynchronizer->logToSinks(rawLogEntry, m_connectedSinks);
	}
}