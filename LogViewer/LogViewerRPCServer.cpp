#include "LogViewerRPCServer.h"

#include "LogViewerRPCInterface.h"

#include <sstream>

namespace
{
	std::shared_ptr<SourceProcessor> sourceProcessorAnonymous;
}

LogViewerRPCInterface_ContextHandle LogViewerRPCInterface_connect(handle_t bindingHandle, const unsigned char* processName, unsigned long processID)
{
	std::stringstream stringstream;
	stringstream << reinterpret_cast<const char*>(processName) << " [" << "0x" << std::hex << processID << "]";
	return sourceProcessorAnonymous->openSource(std::shared_ptr<std::string>(new std::string(stringstream.str())), SourceType::RPCConnection);
}

void LogViewerRPCInterface_addEntry(LogViewerRPCInterface_ContextHandle contextHandle, const unsigned char* compiledMessage)
{
	std::shared_ptr<std::string> compiledLogEntry(new std::string(reinterpret_cast<const char*>(compiledMessage)));
	sourceProcessorAnonymous->processCompiledLogEntry(contextHandle, compiledLogEntry);
}

void LogViewerRPCInterface_disconnect(LogViewerRPCInterface_ContextHandle* contextHandle)
{
	if (sourceProcessorAnonymous != nullptr)
	{
		sourceProcessorAnonymous->closeSource(*contextHandle);
	}
	
	*contextHandle = nullptr;
}

void __RPC_USER LogViewerRPCInterface_ContextHandle_rundown(LogViewerRPCInterface_ContextHandle contextHandle)
{
	LogViewerRPCInterface_disconnect(&contextHandle);
}

RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE, void*)
{
	return RPC_S_OK;
}

LogViewerRPCServer::LogViewerRPCServer(const std::shared_ptr<SourceProcessor>& sourceProcessor)
{
	sourceProcessorAnonymous = sourceProcessor;

	m_localRPCConnection.enable();
	m_networkRPCConnection.enable();

	registerServer();

	m_thread = std::thread(&LogViewerRPCServer::threadWorkFunction, this);
}

LogViewerRPCServer::~LogViewerRPCServer()
{
	RpcMgmtStopServerListening(NULL);
	m_thread.join();
	sourceProcessorAnonymous.reset();
}

void LogViewerRPCServer::enableLocalRPCConnection()
{
	m_localRPCConnection.enable();
}

void LogViewerRPCServer::disableLocalRPCConnection()
{
	m_localRPCConnection.disable();
}

bool LogViewerRPCServer::localRPCConnectionIsEnabled()
{
	return m_localRPCConnection.isEnabled();
}

void LogViewerRPCServer::enableNetworkRPCConnection()
{
	m_networkRPCConnection.enable();
}

void LogViewerRPCServer::disableNetworkRPCConnection()
{
	m_networkRPCConnection.disable();
}

bool LogViewerRPCServer::networkRPCConnectionIsEnabled()
{
	return m_networkRPCConnection.isEnabled();
}

void LogViewerRPCServer::threadWorkFunction()
{
	RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, FALSE);
}

void LogViewerRPCServer::registerServer()
{
	RPC_STATUS status = RpcServerRegisterIf2(LogViewerRPCInterface_v1_0_s_ifspec, NULL, NULL, RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, RPC_C_LISTEN_MAX_CALLS_DEFAULT, (unsigned)-1, SecurityCallback);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
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

