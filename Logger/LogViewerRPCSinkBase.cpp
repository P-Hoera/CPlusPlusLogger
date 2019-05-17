#include "LogViewerRPCSinkBase.h"
#include "LogViewerRPCInterface.h"

namespace Log
{
	const ProcessInfo LogViewerRPCSinkBase::m_currentProcessInfo;

	LogViewerRPCSinkBase::~LogViewerRPCSinkBase()
	{
		stopThread();
		disconnectRPC();
	}

	LogViewerRPCSinkBase::LogViewerRPCSinkBase(const std::string& sinkName, const SinkType sinkType)
		:SinkBase(sinkName, sinkType)
	{}

	void LogViewerRPCSinkBase::initialize(const std::string& protocol, const std::string& networkAddress, const std::string& endpoint)
	{
		auto initializeFunction = [&]
		{
			RPC_STATUS status;
			unsigned char* stringBinding = nullptr;

			status = RpcStringBindingCompose(NULL, reinterpret_cast<unsigned char*>(const_cast<char*>(protocol.c_str())), reinterpret_cast<unsigned char*>(const_cast<char*>(networkAddress.c_str())), reinterpret_cast<unsigned char*>(const_cast<char*>(endpoint.c_str())), NULL, &stringBinding);

			if (status)
			{
				throw std::runtime_error("Connection to Server failed.");
			}

			status = RpcBindingFromStringBinding(stringBinding, &m_rpcBindingHandle);

			RpcStringFree(&stringBinding);

			if (status)
			{
				throw std::runtime_error("Connection to Server failed.");
			}

			status = RpcEpResolveBinding(m_rpcBindingHandle, LogViewerRPCInterface_v1_0_c_ifspec);

			if (status)
			{
				throw std::runtime_error("Connection to Server failed.");
			}

			RpcTryExcept
			{
				m_rpcContextHandle = LogViewerRPCInterface_connect(m_rpcBindingHandle, reinterpret_cast<const unsigned char*>(m_currentProcessInfo.getProcessFileName().c_str()), m_currentProcessInfo.getProcessID());
			}
			RpcExcept(1)
			{
				RpcBindingFree(&m_rpcBindingHandle);
				throw std::runtime_error("Connection to Server failed.");
			}
			RpcEndExcept

			m_isConnected = true;
		};

		SinkBase::initialize(initializeFunction);
	}

	void LogViewerRPCSinkBase::initializeAsDummy()
	{
		auto initializeAsDummyFunction = [&] {};
		SinkBase::initializeAsDummy(initializeAsDummyFunction);
	}

	void LogViewerRPCSinkBase::write(const std::shared_ptr<LogEntry> logEntry)
	{
		// This lambda is required to be able to use the try block. Otherwise it wouldnt be possible because of logEntry.
		auto rpcWriteFunction = [&](const char* compiledMessage)
		{
			if (m_isConnected)
			{
				RpcTryExcept
				{
					LogViewerRPCInterface_addEntry(m_rpcContextHandle, reinterpret_cast<const unsigned char*>(compiledMessage));
				}
				RpcExcept(1)
				{
					disconnectRPC();
				}
				RpcEndExcept
			}
		};

		rpcWriteFunction(logEntry->getCompiledLogEntry().c_str());
	}

	void LogViewerRPCSinkBase::disconnectRPC()
	{
		if (m_isConnected)
		{
			RpcTryExcept
			{
				LogViewerRPCInterface_disconnect(&m_rpcContextHandle);
			}
			RpcExcept(1)
			{
				RpcSmDestroyClientContext(&m_rpcContextHandle);
			}
			RpcEndExcept

			RpcBindingFree(&m_rpcBindingHandle);

			m_isConnected = false;
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