#include "RPCServerCore.h"

std::mutex RPCServerCore::m_coreInstanceMutex;
std::unique_ptr<RPCServerCore> RPCServerCore::m_coreInstance;

RPCServerCore::~RPCServerCore()
{
	m_logViewerRPCServer.reset();
	RpcMgmtStopServerListening(NULL);
	m_thread.join();
}

void RPCServerCore::initializeCore(const std::shared_ptr<SourceProcessor>& sourceProcessor)
{
	std::unique_lock<std::mutex> lock(m_coreInstanceMutex);

	if(m_coreInstance == nullptr)
	{
		m_coreInstance.reset(new RPCServerCore(sourceProcessor));
	}
}

void RPCServerCore::shutDownCore()
{
	std::unique_lock<std::mutex> lock(m_coreInstanceMutex);

	m_coreInstance.reset();
}

void RPCServerCore::initializeServer(const ServerType server)
{
	std::unique_lock<std::mutex> lock(m_coreInstanceMutex);

	if (m_coreInstance == nullptr)
	{
		throw std::runtime_error("Core not initialized.");
	}

	m_coreInstance->initializeServerInternal(server);
}

void RPCServerCore::shutDownServer(const ServerType server)
{
	std::unique_lock<std::mutex> lock(m_coreInstanceMutex);

	if (m_coreInstance == nullptr)
	{
		throw std::runtime_error("Core not initialized.");
	}

	m_coreInstance->shutDownServerInternal(server);
}

void RPCServerCore::enableProtocol(const ServerType server, const ProtocolType protocol)
{
	std::unique_lock<std::mutex> lock(m_coreInstanceMutex);

	if (m_coreInstance == nullptr)
	{
		throw std::runtime_error("Core not initialized.");
	}

	m_coreInstance->enableProtocolInternal(server, protocol);
}

void RPCServerCore::disableProtocol(const ServerType server, const ProtocolType protocol)
{
	std::unique_lock<std::mutex> lock(m_coreInstanceMutex);

	if (m_coreInstance == nullptr)
	{
		throw std::runtime_error("Core not initialized.");
	}

	m_coreInstance->disableProtocolInternal(server, protocol);
}

RPCServerCore::RPCServerCore(const std::shared_ptr<SourceProcessor>& sourceProcessor)
	:m_sourceProcessor(sourceProcessor)
{
	// the following block is needed to start the listening process without a server interface being registered
	// the listening done in the thread will stop immediately if no protocol sequence is added
	// this might require reviewing if other server interfaces are added at some point and 2 or more servers are run by the core
	{
		std::string protocol = "ncalrpc";
		std::unique_ptr<unsigned char> protocolCString((new unsigned char[protocol.length() + 1]));

		memcpy(protocolCString.get(), protocol.c_str(), protocol.length() + 1);

		RPC_STATUS status = RpcServerUseProtseq(protocolCString.get(), RPC_C_PROTSEQ_MAX_REQS_DEFAULT, NULL);

		if (status)
		{
			throw std::runtime_error("Core startup failed");
		}
	}

	m_thread = std::thread(&RPCServerCore::threadWorkFunction, this);
}

void RPCServerCore::threadWorkFunction()
{
	RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, FALSE);
}

void RPCServerCore::initializeServerInternal(const ServerType server)
{
	switch (server)
	{
		case ServerType::LogViewerRPCServer:
		{
			if (m_logViewerRPCServer != nullptr)
			{
				return;
			}

			m_logViewerRPCServer.reset(new LogViewerRPCServer(m_sourceProcessor));
			return;
		}
		default:
		{
			throw std::invalid_argument("Unsupported Server");
		}
	}
}

void RPCServerCore::shutDownServerInternal(const ServerType server)
{
	switch (server)
	{
		case ServerType::LogViewerRPCServer:
		{
			m_logViewerRPCServer.reset();
			return;
		}
	}
}

void RPCServerCore::enableProtocolInternal(const ServerType server, const ProtocolType protocol)
{
	switch(server)
	{
		case ServerType::LogViewerRPCServer:
		{
			if(m_logViewerRPCServer == nullptr)
			{
				throw std::runtime_error("Server not initialized.");
			}

			switch(protocol)
			{
				case ProtocolType::Local:
				{
					m_logViewerRPCServer->enableLocalRPCConnection();
					return;
				}
				case ProtocolType::Network:
				{
					m_logViewerRPCServer->enableNetworkRPCConnection();
					return;
				}
				default:
				{
					throw std::invalid_argument("Unsupported Protocol");
				}
			}
		}
		default:
		{
			throw std::invalid_argument("Unsupported Server");
		}
	}
}

void RPCServerCore::disableProtocolInternal(const ServerType server, const ProtocolType protocol)
{
	switch (server)
	{
		case ServerType::LogViewerRPCServer:
		{
			if (m_logViewerRPCServer == nullptr)
			{
				throw std::runtime_error("Server not initialized.");
			}

			switch (protocol)
			{
				case ProtocolType::Local:
				{
					m_logViewerRPCServer->disableLocalRPCConnection();
					return;
				}
				case ProtocolType::Network:
				{
					m_logViewerRPCServer->disableNetworkRPCConnection();
					return;
				}
			}
		}
	}
}

void* __RPC_USER midl_user_allocate(size_t size)
{
	return new char[size];
}

void __RPC_USER midl_user_free(void* p)
{
	delete[] reinterpret_cast<char*>(p);
}