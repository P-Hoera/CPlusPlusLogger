#include "LogEntry.h"

#include <sstream>
#include <iomanip>

namespace Log
{
	LogEntry::LogEntry(const RawLogEntry& rawLogEntry)
	{
		m_timeStamp = TimeStamp(rawLogEntry.getTimeStamp());
		m_timeStampString = m_timeStamp.convertToString();
		m_logLevel = rawLogEntry.getLogLevel();
		m_logLevelString = logLevelToString(rawLogEntry.getLogLevel());
		m_moduleName = rawLogEntry.getModuleName();
		createThreadIDString(rawLogEntry.getThreadID());
		m_message = rawLogEntry.getMessage();

		compileLogEntry();
	}

	LogEntry::LogEntry(const std::string& compiledLogEntry)
	{
		decompileLogEntry(compiledLogEntry);
	}

	const TimeStamp& LogEntry::getTimeStamp() const
	{
		return m_timeStamp;
	}

	const std::string& LogEntry::getTimeStampString() const
	{
		return m_timeStampString;
	}

	const LogLevel LogEntry::getLogLevel() const
	{
		return m_logLevel;
	}

	const std::string& LogEntry::getLogLevelString() const
	{
		return m_logLevelString;
	}

	const std::string& LogEntry::getModuleName() const
	{
		return m_moduleName;
	}

	const std::string& LogEntry::getThreadIDString() const
	{
		return m_threadIDString;
	}

	const std::string& LogEntry::getMessage() const
	{
		return m_message;
	}

	const std::string& LogEntry::getCompiledLogEntry() const
	{
		return m_compiledLogEntry;
	}

	void LogEntry::compileLogEntry()
	{
		std::ostringstream stream;
		stream << "[" << m_timeStampString << "]" << " ";
		stream << m_logLevelString << " ";
		stream << "[" << m_moduleName << "]" << " ";
		stream << m_threadIDString << " ";
		stream << m_message << "\n";
		m_compiledLogEntry = stream.str();
	}

	void LogEntry::decompileLogEntry(const std::string& compiledLogEntry)
	{
		if (compiledLogEntry.empty())
		{
			throw std::invalid_argument("String cannot be empty.");
		}

		m_compiledLogEntry = compiledLogEntry;

		size_t currentPos = 0;
		size_t foundPos = 0;

		// find timestamp
		foundPos = m_compiledLogEntry.find(']');
		if (foundPos == std::string::npos)
		{
			throw std::invalid_argument("Timestamp not found.");
		}
		m_timeStampString = m_compiledLogEntry.substr(currentPos + 1, foundPos - 1);
		m_timeStamp = TimeStamp(m_timeStampString);
		currentPos = foundPos + 2;

		// find log level
		foundPos = m_compiledLogEntry.find(' ', currentPos);
		if (foundPos == std::string::npos)
		{
			throw std::invalid_argument("Log level not found.");
		}
		m_logLevelString = m_compiledLogEntry.substr(currentPos, foundPos - currentPos);
		m_logLevel = stringToLogLevel(m_logLevelString);
		currentPos = foundPos + 1;

		// find module name
		foundPos = m_compiledLogEntry.find("] 0x", currentPos);
		if (foundPos == std::string::npos)
		{
			throw std::invalid_argument("Module name not found.");
		}
		m_moduleName = m_compiledLogEntry.substr(currentPos + 1, foundPos - currentPos - 1);
		currentPos = foundPos + 2;

		// find thread id
		foundPos = m_compiledLogEntry.find(' ', currentPos);
		if (foundPos == std::string::npos)
		{
			throw std::invalid_argument("Thread id not found.");
		}
		m_threadIDString = m_compiledLogEntry.substr(currentPos, foundPos - currentPos);
		currentPos = foundPos + 1;

		// find message
		size_t messageLength = m_compiledLogEntry.length() - currentPos - 1;
		m_message = m_compiledLogEntry.substr(currentPos, messageLength);
	}

	void LogEntry::createThreadIDString(const std::thread::id threadID)
	{
		std::ostringstream stream;
		stream << "0x" << std::hex << threadID;
		m_threadIDString = stream.str();
	}
}