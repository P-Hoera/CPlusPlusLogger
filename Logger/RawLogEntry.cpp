#include "RawLogEntry.h"

namespace Log
{
	RawLogEntry::RawLogEntry(const std::chrono::time_point<std::chrono::system_clock>& timeStamp, const LogLevel logLevel, const std::string& moduleName, const std::thread::id threadID, const std::string& message)
		:m_timeStamp(timeStamp), m_logLevel(logLevel), m_moduleName(moduleName), m_threadID(threadID), m_message(message)
	{}

	const std::chrono::time_point<std::chrono::system_clock>& RawLogEntry::getTimeStamp() const
	{
		return m_timeStamp;
	}

	const LogLevel RawLogEntry::getLogLevel() const
	{
		return m_logLevel;
	}

	const std::string& RawLogEntry::getModuleName() const
	{
		return m_moduleName;
	}

	const std::thread::id RawLogEntry::getThreadID() const
	{
		return m_threadID;
	}

	const std::string& RawLogEntry::getMessage() const
	{
		return m_message;
	}
}