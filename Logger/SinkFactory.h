#pragma once

#include "LoggerAPI.h"

#include "ConsoleSink.h"
#include "DelayedConsoleSink.h"
#include "FileSink.h"
#include "DelayedFileSink.h"
#include "LogViewerLocalRPCSink.h"
#include "DelayedLogViewerLocalRPCSink.h"
#include "LogViewerNetworkRPCSink.h"
#include "DelayedLogViewerNetworkRPCSink.h"

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief Factory used to construct all sink types.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class LOGGER_API SinkFactory
	{
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a sink with default console target. Its fully initialized upon construction.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		///
		/// \returns The constructed console sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<ConsoleSink> createConsoleSink(const std::string& sinkName);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a delayed sink targeting the default console. Must be initialized after construction in order 
		///        to sink incomming messages. Can be initialized as a dummy in case errors prevent a normal initialization. 
		///        Initialization of any kind should only be delayed as much as possible to prevent high memory consumtion.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		///
		/// \returns The constructed delayed console sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<DelayedConsoleSink> createDelayedConsoleSink(const std::string& sinkName);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a sink targeting a single log file. Its fully initialized upon construction.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		/// \param fileName The file to be used as sink target.
		///
		/// \returns The constructed file sink.
		///
		/// \exception std::invalid_argument Thrown if the file cannot be opened for writing.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<FileSink> createFileSink(const std::string& sinkName, const std::string& fileName);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a delayed sink targeting a single log file. Must be initialized after construction in order 
		///        to sink incomming messages. Can be initialized as a dummy in case errors prevent a normal initialization. 
		///        Initialization of any kind should only be delayed as much as possible to prevent high memory consumtion.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		///
		/// \returns The constructed delayed file sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<DelayedFileSink> createDelayedFileSink(const std::string& sinkName);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a sink targeting a RPC connection. Its fully initialized upon construction.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		///
		/// \returns The constructed RPC sink.
		///
		/// \exception std::runtime_error Thrown if the connection to the server could not be established.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<LogViewerLocalRPCSink> createLogViewerLocalRPCSink(const std::string& sinkName);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a delayed sink targeting a RPC connection. Must be initialized after construction in order 
		///        to sink incomming messages. Can be initialized as a dummy in case errors prevent a normal initialization. 
		///        Initialization of any kind should only be delayed as much as possible to prevent high memory consumtion.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		///
		/// \returns The constructed RPC sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<DelayedLogViewerLocalRPCSink> createDelayedLogViewerLocalRPCSink(const std::string& sinkName);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a sink targeting a RPC connection. Its fully initialized upon construction.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		/// \param networkAddress A network address to connect to. Using "" is valid and will cause a connection to a server
		///						  in the network or locally. Use a network address to connect to a specific server.
		///
		/// \returns The constructed RPC sink.
		///
		/// \exception std::runtime_error Thrown if the connection to the server could not be established.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<LogViewerNetworkRPCSink> createLogViewerNetworkRPCSink(const std::string& sinkName, const std::string& networkAddress);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructs a delayed sink targeting a RPC connection. Must be initialized after construction in order 
		///        to sink incomming messages. Can be initialized as a dummy in case errors prevent a normal initialization. 
		///        Initialization of any kind should only be delayed as much as possible to prevent high memory consumtion.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		///
		/// \returns The constructed RPC sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static std::shared_ptr<DelayedLogViewerNetworkRPCSink> createDelayedLogViewerNetworkRPCSink(const std::string& sinkName);

	private:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Unused Constructor.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		SinkFactory() = delete;
	};
}

#pragma warning( pop ) 