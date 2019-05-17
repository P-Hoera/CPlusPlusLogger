#include "DelayedConsoleSink.h"

namespace Log
{
	DelayedConsoleSink::DelayedConsoleSink(const std::string& sinkName)
		:ConsoleSinkBase(sinkName, SinkType::DelayedConsoleSink)
	{}

	DelayedConsoleSink::~DelayedConsoleSink()
	{
		stopThread();
	}

	void DelayedConsoleSink::initialize()
	{
		ConsoleSinkBase::initialize();
	}

	void DelayedConsoleSink::initializeAsDummy()
	{
		ConsoleSinkBase::initializeAsDummy();
	}
}