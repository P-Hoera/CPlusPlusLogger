#pragma once

#include <string>
#include "LogViewerRPCInterface.h"

class RPCServerConnection
{
public:
	RPCServerConnection(const std::string& protocol, const RPC_IF_HANDLE& serverIfSpec);

	~RPCServerConnection();

	bool isEnabled();

	void enable();

	void disable();

private:
	void addProtocol();

	void processBindingVector();

	bool m_isEnabled = false;
	RPC_BINDING_VECTOR* m_bindingVector = nullptr;
	std::string m_protocol;
	RPC_IF_HANDLE m_serverIfSpec;
};