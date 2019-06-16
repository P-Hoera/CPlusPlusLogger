#pragma once

#include "SourceProcessor.h"
#include "LogViewerRPCServerConnection.h"

#include <thread>

class LogViewerRPCServer
{
public:
	LogViewerRPCServer(const std::shared_ptr<SourceProcessor>& sourceProcessor);

	~LogViewerRPCServer();

	void enableLocalRPCConnection();

	void disableLocalRPCConnection();

	bool localRPCConnectionIsEnabled();

	void enableNetworkRPCConnection();

	void disableNetworkRPCConnection();

	bool networkRPCConnectionIsEnabled();

private:
	void threadWorkFunction();

	void registerServer();

	LogViewerRPCServerConnection m_localRPCConnection{"ncalrpc"};
	LogViewerRPCServerConnection m_networkRPCConnection{"ncacn_ip_tcp"};
	std::thread m_thread; //!< The thread used for the work function.
};