#include "DelayedFileSink.h"

namespace Log
{
	DelayedFileSink::DelayedFileSink(const std::string& sinkName)
		:FileSinkBase(sinkName, SinkType::DelayedFileSink)
	{}

	DelayedFileSink::~DelayedFileSink()
	{
		stopThread();
	}

	void DelayedFileSink::initialize(const std::string& fileName)
	{
		FileSinkBase::initialize(fileName);
	}

	void DelayedFileSink::initializeAsDummy()
	{
		FileSinkBase::initializeAsDummy();
	}
}