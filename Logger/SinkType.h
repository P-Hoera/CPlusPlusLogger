#pragma once

#include "LoggerAPI.h"

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief All available sink types.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	enum class LOGGER_API SinkType
	{
		ConsoleSink, //!< A console sink.
		DelayedConsoleSink, //!< A console sink with delayed initialization.
		FileSink, //!< A file sink.
		DelayedFileSink, //!< A file sink with delayed initialization.
		LogViewerLocalRPCSink, //!< A RPC sink connecting to an external logger.
		DelayedLogViewerLocalRPCSink, //!< A delayed RPC sink connecting to an external logger.
		LogViewerNetworkRPCSink, //!< A RPC sink connecting to an external logger.
		DelayedLogViewerNetworkRPCSink //!< A delayed RPC sink connecting to an external logger.
	};
}

#pragma warning( pop ) 