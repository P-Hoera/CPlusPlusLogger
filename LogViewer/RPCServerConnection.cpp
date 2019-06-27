#include "RPCServerConnection.h"

#include <stdexcept>
#include <memory>

RPCServerConnection::RPCServerConnection(const std::string& protocol, const RPC_IF_HANDLE& serverIfSpec)
	:m_protocol(protocol), m_serverIfSpec(serverIfSpec)
{
	addProtocol();
	processBindingVector();
}

RPCServerConnection::~RPCServerConnection()
{
	RpcBindingVectorFree(&m_bindingVector);
}

bool RPCServerConnection::isEnabled()
{
	return m_isEnabled;
}

void RPCServerConnection::enable()
{
	if (m_isEnabled)
	{
		return;
	}

	RPC_STATUS status = RpcEpRegisterNoReplace(m_serverIfSpec, m_bindingVector, NULL, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	m_isEnabled = true;
}

void RPCServerConnection::disable()
{
	if (!m_isEnabled)
	{
		return;
	}

	RPC_STATUS status = RpcEpUnregister(m_serverIfSpec, m_bindingVector, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	m_isEnabled = false;
}

void RPCServerConnection::addProtocol()
{
	std::unique_ptr<unsigned char> protocol((new unsigned char[m_protocol.length() + 1]));

	memcpy(protocol.get(), m_protocol.c_str(), m_protocol.length() + 1);

	RPC_STATUS status = RpcServerUseProtseq(protocol.get(), RPC_C_PROTSEQ_MAX_REQS_DEFAULT, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}
}

void RPCServerConnection::processBindingVector()
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