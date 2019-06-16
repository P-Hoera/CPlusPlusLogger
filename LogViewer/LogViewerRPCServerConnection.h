#pragma once

#include <string>
#include "LogViewerRPCInterface.h"

class LogViewerRPCServerConnection
{
public:
	LogViewerRPCServerConnection(const std::string& protocol);

	~LogViewerRPCServerConnection();

	bool isEnabled();

	void enable();

	void disable();

private:
	void addProtocol();

	void processBindingVector();

	bool m_isEnabled = false;
	RPC_BINDING_VECTOR* m_bindingVector = nullptr;
	std::string m_protocol;
};