#pragma once

#include "LogViewerRPCServer.h"

#include <mutex>
#include <thread>

class RPCServerCore
{
public:
	enum class ServerType
	{
		LogViewerRPCServer
	};

	enum class ProtocolType
	{
		Local,
		Network
	};

	~RPCServerCore();

	static void initializeCore(const std::shared_ptr<SourceProcessor>& sourceProcessor);

	static void shutDownCore();

	static void initializeServer(const ServerType server);

	static void shutDownServer(const ServerType server);

	static void enableProtocol(const ServerType server, const ProtocolType protocol);

	static void disableProtocol(const ServerType server, const ProtocolType protocol);

private:
	RPCServerCore(const std::shared_ptr<SourceProcessor>& sourceProcessor);

	void threadWorkFunction();

	void initializeServerInternal(const ServerType server);

	void shutDownServerInternal(const ServerType server);

	void enableProtocolInternal(const ServerType server, const ProtocolType protocol);

	void disableProtocolInternal(const ServerType server, const ProtocolType protocol);

	static std::mutex m_coreInstanceMutex;
	static std::unique_ptr<RPCServerCore> m_coreInstance;

	std::thread m_thread; //!< The thread used for the work function.
	std::shared_ptr<SourceProcessor> m_sourceProcessor;
	std::unique_ptr<LogViewerRPCServer> m_logViewerRPCServer;
};
