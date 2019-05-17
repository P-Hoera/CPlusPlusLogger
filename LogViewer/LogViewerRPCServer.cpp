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

	RPC_STATUS status;

	char protocol[] = "ncalrpc";
	char protocol2[] = "ncacn_ip_tcp";

	status = RpcServerUseProtseq(reinterpret_cast<unsigned char*>(protocol), RPC_C_PROTSEQ_MAX_REQS_DEFAULT, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	status = RpcServerUseProtseq(reinterpret_cast<unsigned char*>(protocol2), RPC_C_PROTSEQ_MAX_REQS_DEFAULT, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	RPC_BINDING_VECTOR* bindingVector;

	status = RpcServerInqBindings(&bindingVector);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	status = RpcEpRegisterNoReplace(LogViewerRPCInterface_v1_0_s_ifspec, bindingVector, NULL, NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	status = RpcBindingVectorFree(&bindingVector);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	status = RpcServerRegisterIf2(LogViewerRPCInterface_v1_0_s_ifspec, NULL, NULL, RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, RPC_C_LISTEN_MAX_CALLS_DEFAULT, (unsigned)-1, SecurityCallback);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}
/*
	char protocol[] = "ncalrpc"; // ncacn_ip_tcp
	char port[] = "4747";

	status = RpcServerUseProtseqEp( reinterpret_cast<unsigned char*>(protocol), RPC_C_PROTSEQ_MAX_REQS_DEFAULT, reinterpret_cast<unsigned char*>(port), NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	char protocol2[] = "ncacn_ip_tcp";
	char port2[] = "4748";

	status = RpcServerUseProtseqEp(reinterpret_cast<unsigned char*>(protocol2), RPC_C_PROTSEQ_MAX_REQS_DEFAULT, reinterpret_cast<unsigned char*>(port2), NULL);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	status = RpcServerRegisterIf2(LogViewerRPCInterface_v1_0_s_ifspec, NULL, NULL, RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, RPC_C_LISTEN_MAX_CALLS_DEFAULT, (unsigned)-1, SecurityCallback);

	if (status)
	{
		throw std::runtime_error("Server startup failed");
	}

	*/

	m_thread = std::thread(&LogViewerRPCServer::threadWorkFunction, this);
}

LogViewerRPCServer::~LogViewerRPCServer()
{
	RpcMgmtStopServerListening(NULL);
	m_thread.join();
	sourceProcessorAnonymous.reset();
}

void LogViewerRPCServer::threadWorkFunction()
{
	RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, FALSE);
}

void* __RPC_USER midl_user_allocate(size_t size)
{
	return new char[size];
}

void __RPC_USER midl_user_free(void* p)
{
	delete[] reinterpret_cast<char*>(p);
}

