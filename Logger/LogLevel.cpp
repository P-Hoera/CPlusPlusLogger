#include "LogLevel.h"

#include <vector>
#include <stdexcept>

namespace Log
{
	using LogLevelStringPair = std::pair<LogLevel, std::string>;
	const std::vector<LogLevelStringPair> logLevelStringVector = { {LogLevel::Fatal, "Fatal"},
																   {LogLevel::Error, "Error"},
																   {LogLevel::Warning, "Warning"},
																   {LogLevel::Info, "Info"},
																   {LogLevel::Debug, "Debug"},
																   {LogLevel::Trace, "Trace"} };

	std::string logLevelToString(const LogLevel logLevel)
	{
		for (const LogLevelStringPair& entry : logLevelStringVector)
		{
			if (entry.first == logLevel)
			{
				return entry.second;
			}
		}

		return "Unknown";
	}

	LogLevel stringToLogLevel(const std::string& logLevelString)
	{
		for (const LogLevelStringPair& entry : logLevelStringVector)
		{
			if (entry.second == logLevelString)
			{
				return entry.first;
			}
		}

		throw std::invalid_argument("Invalid log level string.");
	}
}