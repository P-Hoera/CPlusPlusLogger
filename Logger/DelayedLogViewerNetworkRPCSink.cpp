#include "DelayedLogViewerNetworkRPCSink.h"

namespace Log
{
	DelayedLogViewerNetworkRPCSink::DelayedLogViewerNetworkRPCSink(const std::string& sinkName)
		:LogViewerNetworkRPCSinkBase(sinkName, SinkType::DelayedLogViewerNetworkRPCSink)
	{}

	DelayedLogViewerNetworkRPCSink::~DelayedLogViewerNetworkRPCSink()
	{
		stopThread();
	}

	void DelayedLogViewerNetworkRPCSink::initialize(const std::string& networkAddress)
	{
		LogViewerNetworkRPCSinkBase::initialize(networkAddress);
	}

	void DelayedLogViewerNetworkRPCSink::initializeAsDummy()
	{
		LogViewerNetworkRPCSinkBase::initializeAsDummy();
	}
}