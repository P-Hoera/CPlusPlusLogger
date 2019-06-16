#include "LogViewerRPCServerConnection.h"

#include <stdexcept>
#include <memory>

LogViewerRPCServerConnection::LogViewerRPCServerConnection(const std::string& protocol)
	:m_protocol(protocol)
{
	addProtocol();
	processBindingVector();
}

LogViewerRPCServerConnection::~LogViewerRPCServerConnection()
{
	RpcBindingVectorFree(&m_bindingVector);
}

bool LogViewerRPCServerConnection::isEnabled()
{
	return m_isEnabled;
}

void LogViewerRPCServerConnection::enable()
{
	if (m_isEnabled)
	{
		return;
	}

	RPC_STATUS status = RpcEpRegisterNoReplace(LogViewerRPCInterface_v1_0_s_ifspec, m_bindingVector, NULL, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	m_isEnabled = true;
}

void LogViewerRPCServerConnection::disable()
{
	if (!m_isEnabled)
	{
		return;
	}

	RPC_STATUS status = RpcEpUnregister(LogViewerRPCInterface_v1_0_s_ifspec, m_bindingVector, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	m_isEnabled = false;
}

void LogViewerRPCServerConnection::addProtocol()
{
	std::unique_ptr<unsigned char> protocol((new unsigned char[m_protocol.length() + 1]));

	memcpy(protocol.get(), m_protocol.c_str(), m_protocol.length() + 1);

	RPC_STATUS status = RpcServerUseProtseq(protocol.get(), RPC_C_PROTSEQ_MAX_REQS_DEFAULT, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}
}

void LogViewerRPCServerConnection::processBindingVector()
{
	RPC_STATUS status = RpcServerInqBindings(&m_bindingVector);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	bool found = false;

	for (int i = 0; i < m_bindingVector->Count; ++i)
	{
		unsigned char* bindingCString;

		status = RpcBindingToStringBinding(m_bindingVector->BindingH[i], &bindingCString);

		if (status)
		{
			throw std::runtime_error("Server startup failed");
		}

		std::string bindingString(reinterpret_cast<char*>(bindingCString));

		RpcStringFree(&bindingCString);

		if (bindingString.find(m_protocol) != std::string::npos)
		{
			found = true;
		}
		else
		{
			status = RpcBindingFree(&m_bindingVector->BindingH[i]);

			if (status)
			{
				throw std::runtime_error("Server startup failed");
			}

			m_bindingVector->BindingH[i] = nullptr;
		}
	}

	if (!found)
	{
		throw std::runtime_error("Server startup failed");
	}
}