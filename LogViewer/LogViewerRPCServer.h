#pragma once

#include "SourceProcessor.h"
#include "RPCServerConnection.h"

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

	RPCServerConnection m_localRPCConnection{"ncalrpc", LogViewerRPCInterface_v1_0_s_ifspec };
	RPCServerConnection m_networkRPCConnection{"ncacn_ip_tcp", LogViewerRPCInterface_v1_0_s_ifspec };
};