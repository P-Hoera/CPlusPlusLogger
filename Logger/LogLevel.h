#pragma once

#include "LoggerAPI.h"

#include <string>

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief All valid log levels.
	///
	/// \see stringToLogLevel
	/// \see logLevelToString
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	enum class LOGGER_API LogLevel
	{
		Fatal = 0, //!< Fatal log level
		Error = 1, //!< Error log level
		Warning = 2, //!< Warning log level
		Info = 3, //!< Info log level
		Debug = 4, //!< Debug log level
		Trace = 5, //!< Trace log level	
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief Converts a log level to its printable string representation.
	///
	/// \param logLevel The log level to convert to a string.
	///
	/// \returns The string representation of the given log level. If none exists "Unknown" is returned instead.
	///
	/// \see stringToLogLevel
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string LOGGER_API logLevelToString(const LogLevel logLevel);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief Converts a string to the log level it is representing.
	///
	/// \param logLevelString The string to convert into a log level.
	///
	/// \returns The log level.
	///
	/// \exception std::invalid_argument Is thrown if the string does not represent a valid log level.
	///
	/// \see logLevelToString
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LogLevel LOGGER_API stringToLogLevel(const std::string& logLevelString);
}

#pragma warning( pop ) 