#include "DelayedLogViewerLocalRPCSink.h"

namespace Log
{
	DelayedLogViewerLocalRPCSink::DelayedLogViewerLocalRPCSink(const std::string& sinkName)
		:LogViewerLocalRPCSinkBase(sinkName, SinkType::DelayedLogViewerLocalRPCSink)
	{}

	DelayedLogViewerLocalRPCSink::~DelayedLogViewerLocalRPCSink()
	{
		stopThread();
	}

	void DelayedLogViewerLocalRPCSink::initialize()
	{
		LogViewerLocalRPCSinkBase::initialize();
	}

	void DelayedLogViewerLocalRPCSink::initializeAsDummy()
	{
		LogViewerLocalRPCSinkBase::initializeAsDummy();
	}
}