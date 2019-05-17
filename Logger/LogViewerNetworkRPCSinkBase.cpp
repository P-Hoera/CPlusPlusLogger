#include "LogViewerNetworkRPCSinkBase.h"

namespace Log
{
	LogViewerNetworkRPCSinkBase::~LogViewerNetworkRPCSinkBase()
	{
		stopThread();
	}

	LogViewerNetworkRPCSinkBase::LogViewerNetworkRPCSinkBase(const std::string& sinkName, const SinkType sinkType)
		:LogViewerRPCSinkBase(sinkName, sinkType)
	{}

	void LogViewerNetworkRPCSinkBase::initialize(const std::string& networkAddress)
	{
		LogViewerRPCSinkBase::initialize("ncacn_ip_tcp", networkAddress, "");
	}

	void LogViewerNetworkRPCSinkBase::initializeAsDummy()
	{
		LogViewerRPCSinkBase::initializeAsDummy();
	}
}