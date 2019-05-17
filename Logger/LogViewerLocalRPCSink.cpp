#include "LogViewerLocalRPCSink.h"

namespace Log
{
	LogViewerLocalRPCSink::LogViewerLocalRPCSink(const std::string& sinkName)
		:LogViewerLocalRPCSinkBase(sinkName, SinkType::LogViewerLocalRPCSink)
	{
		LogViewerLocalRPCSinkBase::initialize();
	}

	LogViewerLocalRPCSink::~LogViewerLocalRPCSink()
	{
		stopThread();
	}
}