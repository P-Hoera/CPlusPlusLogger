#include "LogViewerLocalRPCSinkBase.h"

namespace Log
{
	LogViewerLocalRPCSinkBase::~LogViewerLocalRPCSinkBase()
	{
		stopThread();
	}

	LogViewerLocalRPCSinkBase::LogViewerLocalRPCSinkBase(const std::string& sinkName, const SinkType sinkType)
		:LogViewerRPCSinkBase(sinkName, sinkType)
	{}

	void LogViewerLocalRPCSinkBase::initialize()
	{
		LogViewerRPCSinkBase::initialize("ncalrpc", "", "");
	}

	void LogViewerLocalRPCSinkBase::initializeAsDummy()
	{
		LogViewerRPCSinkBase::initializeAsDummy();
	}
}