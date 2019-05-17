#include "LogViewerNetworkRPCSink.h"

namespace Log
{
	LogViewerNetworkRPCSink::LogViewerNetworkRPCSink(const std::string& sinkName, const std::string& networkAddress)
		:LogViewerNetworkRPCSinkBase(sinkName, SinkType::LogViewerNetworkRPCSink)
	{
		LogViewerNetworkRPCSinkBase::initialize(networkAddress);
	}

	LogViewerNetworkRPCSink::~LogViewerNetworkRPCSink()
	{
		stopThread();
	}
}