#pragma once

#include "SourceProcessor.h"
#include "LogViewerRPCServerConnection.h"

class LogViewerRPCServer
{
	friend class RPCServerCore;

public:
	~LogViewerRPCServer();

	void enableLocalRPCConnection();

	void disableLocalRPCConnection();

	bool localRPCConnectionIsEnabled();

	void enableNetworkRPCConnection();

	void disableNetworkRPCConnection();

	bool networkRPCConnectionIsEnabled();

private:
	LogViewerRPCServer(const std::shared_ptr<SourceProcessor>& sourceProcessor);

	void registerServer();

	LogViewerRPCServerConnection m_localRPCConnection{"ncalrpc"};
	LogViewerRPCServerConnection m_networkRPCConnection{"ncacn_ip_tcp"};
};