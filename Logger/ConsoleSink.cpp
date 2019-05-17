#include "ConsoleSink.h"

namespace Log
{
	ConsoleSink::ConsoleSink(const std::string& sinkName)
		:ConsoleSinkBase(sinkName, SinkType::ConsoleSink)
	{
		ConsoleSinkBase::initialize();
	}

	ConsoleSink::~ConsoleSink()
	{
		stopThread();
	}
}